/* ============================================================================
   Section 16.7 : I/O multiplexing
   Description : Serveur TCP echo avec epoll() (Linux specifique)
   Fichier source : 07-io-multiplexing.md
   ============================================================================ */

/* Tester : ./24_serveur_epoll &
   Puis : echo "hello" | nc -q0 localhost 8081 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_EVENTS 64
#define PORT 8081

/* Rendre un socket non-bloquant */
static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(void) {
    /* Creer le socket serveur */
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    set_nonblocking(server_fd);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, SOMAXCONN) == -1) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    printf("Serveur epoll() en ecoute sur le port %d\n", PORT);
    printf("Tester avec : echo \"hello\" | nc -q0 localhost %d\n", PORT);

    /* Creer l'instance epoll */
    int epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create1");
        close(server_fd);
        return 1;
    }

    /* Ajouter le socket serveur a epoll */
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        perror("epoll_ctl");
        close(epfd);
        close(server_fd);
        return 1;
    }

    /* Tableau pour recevoir les evenements */
    struct epoll_event events[MAX_EVENTS];
    int total_messages = 0;

    /* Boucle principale (s'arrete apres 5 messages ou 10s sans activite) */
    while (total_messages < 5) {
        int n = epoll_wait(epfd, events, MAX_EVENTS, 10000);

        if (n == -1) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }

        if (n == 0) {
            printf("Timeout, arret du serveur.\n");
            break;
        }

        /* Traiter chaque evenement */
        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;

            if (fd == server_fd) {
                /* Nouvelle connexion */
                while (1) {
                    int client_fd = accept(server_fd, NULL, NULL);
                    if (client_fd == -1) {
                        break;  /* Plus de connexions en attente */
                    }

                    set_nonblocking(client_fd);

                    printf("Nouvelle connexion : fd %d\n", client_fd);

                    /* Ajouter le client a epoll */
                    ev.events = EPOLLIN | EPOLLET;  /* Edge-triggered */
                    ev.data.fd = client_fd;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
                }
            } else {
                /* Donnees d'un client */
                char buffer[1024];
                ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);

                if (bytes <= 0) {
                    /* Client deconnecte */
                    printf("Client fd %d deconnecte\n", fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                } else {
                    /* Echo */
                    buffer[bytes] = '\0';
                    printf("Recu de fd %d : %s", fd, buffer);
                    write(fd, buffer, (size_t)bytes);
                    total_messages++;
                }
            }
        }
    }

    close(epfd);
    close(server_fd);

    printf("Serveur arrete.\n");
    return 0;
}
