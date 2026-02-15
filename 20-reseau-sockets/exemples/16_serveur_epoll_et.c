/* ============================================================================
   Section 20.9 : Non-blocking I/O et epoll
   Description : Serveur echo epoll Edge-Triggered avec machine a etats
   Fichier source : 09-non-blocking-io-epoll.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define PORT 8080
#define MAX_EVENTS 64
#define BUFFER_SIZE 4096

typedef enum {
    STATE_READING,
    STATE_WRITING,
    STATE_CLOSING
} state_t;

typedef struct {
    int fd;
    state_t state;
    char read_buf[BUFFER_SIZE];
    size_t read_pos;
    char write_buf[BUFFER_SIZE];
    size_t write_pos;
    size_t write_len;
} connection_t;

/* Rendre un socket non-bloquant */
int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/* Gerer la lecture (Edge-Triggered) */
void handle_read(int epfd, connection_t *conn) {
    while (1) {
        ssize_t n = recv(conn->fd,
                        conn->read_buf + conn->read_pos,
                        BUFFER_SIZE - conn->read_pos - 1,
                        0);

        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;  /* Tout lu */
            } else {
                perror("recv");
                conn->state = STATE_CLOSING;
                return;
            }
        } else if (n == 0) {
            printf("[FD %d] Client deconnecte\n", conn->fd);
            conn->state = STATE_CLOSING;
            return;
        }

        conn->read_pos += (size_t)n;

        if (conn->read_pos >= BUFFER_SIZE - 1) {
            fprintf(stderr, "[FD %d] Buffer plein\n", conn->fd);
            break;
        }
    }

    /* Preparer l'echo : copier read_buf -> write_buf */
    if (conn->read_pos > 0) {
        memcpy(conn->write_buf, conn->read_buf, conn->read_pos);
        conn->write_len = conn->read_pos;
        conn->write_pos = 0;
        conn->read_pos = 0;

        /* Passer en mode ecriture */
        conn->state = STATE_WRITING;

        /* Activer EPOLLOUT */
        struct epoll_event event;
        event.events = EPOLLIN | EPOLLOUT | EPOLLET;
        event.data.ptr = conn;
        epoll_ctl(epfd, EPOLL_CTL_MOD, conn->fd, &event);
    }
}

/* Gerer l'ecriture (Edge-Triggered) */
void handle_write(int epfd, connection_t *conn) {
    while (conn->write_pos < conn->write_len) {
        ssize_t n = send(conn->fd,
                        conn->write_buf + conn->write_pos,
                        conn->write_len - conn->write_pos,
                        0);

        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                /* Buffer plein, on reessaiera plus tard */
                return;
            } else {
                perror("send");
                conn->state = STATE_CLOSING;
                return;
            }
        }

        conn->write_pos += (size_t)n;
    }

    /* Tout envoye, repasser en mode lecture */
    conn->write_pos = 0;
    conn->write_len = 0;
    conn->state = STATE_READING;

    /* Desactiver EPOLLOUT */
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.ptr = conn;
    epoll_ctl(epfd, EPOLL_CTL_MOD, conn->fd, &event);
}

int main(void) {
    int server_fd, epfd;
    struct sockaddr_in addr;
    struct epoll_event event, events[MAX_EVENTS];

    /* Creer socket serveur */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Rendre non-bloquant */
    if (set_nonblocking(server_fd) < 0) {
        perror("set_nonblocking");
        exit(EXIT_FAILURE);
    }

    /* Bind */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    /* Creer epoll */
    epfd = epoll_create1(0);
    if (epfd < 0) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    /* Ajouter serveur en Edge-Triggered */
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = server_fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &event) < 0) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    printf("Serveur epoll ET sur port %d\n", PORT);

    /* Boucle evenementielle */
    while (1) {
        int n = epoll_wait(epfd, events, MAX_EVENTS, -1);

        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == server_fd) {
                /* Nouvelles connexions (Edge-Triggered) */
                while (1) {
                    struct sockaddr_in client_addr;
                    socklen_t len = sizeof(client_addr);
                    int client_fd = accept(server_fd,
                                          (struct sockaddr*)&client_addr,
                                          &len);

                    if (client_fd < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            break;  /* Plus de connexions en attente */
                        } else {
                            perror("accept");
                            break;
                        }
                    }

                    char ip[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
                    printf("[FD %d] Connexion de %s:%d\n",
                           client_fd, ip, ntohs(client_addr.sin_port));

                    /* Rendre non-bloquant */
                    set_nonblocking(client_fd);

                    /* Creer la connexion */
                    connection_t *conn = malloc(sizeof(connection_t));
                    if (!conn) {
                        perror("malloc");
                        close(client_fd);
                        continue;
                    }
                    memset(conn, 0, sizeof(connection_t));
                    conn->fd = client_fd;
                    conn->state = STATE_READING;

                    /* Ajouter a epoll en Edge-Triggered */
                    event.events = EPOLLIN | EPOLLET;
                    event.data.ptr = conn;
                    if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &event) < 0) {
                        perror("epoll_ctl");
                        free(conn);
                        close(client_fd);
                    }
                }
            } else {
                /* Evenement client */
                connection_t *conn = events[i].data.ptr;

                if (events[i].events & EPOLLIN) {
                    handle_read(epfd, conn);
                }

                if (events[i].events & EPOLLOUT) {
                    if (conn->state == STATE_WRITING) {
                        handle_write(epfd, conn);
                    }
                }

                /* Fermer si necessaire */
                if (conn->state == STATE_CLOSING) {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, conn->fd, NULL);
                    close(conn->fd);
                    free(conn);
                }
            }
        }
    }

    close(server_fd);
    close(epfd);
    return 0;
}
