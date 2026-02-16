/* ============================================================================
   Section 34.4.1 : Architecture event-driven
   Description : Serveur HTTP event-driven avec epoll (interactif)
   Fichier source : 04.1-architecture-event-driven.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 8080
#define MAX_EVENTS 64
#define BUFFER_SIZE 4096

static volatile int keep_running = 1;

void sig_handler(int sig) { (void)sig; keep_running = 0; }

int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) { perror("fcntl F_GETFL"); return -1; }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL"); return -1;
    }
    return 0;
}

int create_server_socket(int port) {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) { perror("socket"); return -1; }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt"); close(server_fd); return -1;
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons((uint16_t)port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind"); close(server_fd); return -1;
    }

    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("listen"); close(server_fd); return -1;
    }

    if (set_nonblocking(server_fd) < 0) { close(server_fd); return -1; }

    printf("Serveur demarre sur le port %d\n", port);
    return server_fd;
}

void handle_new_connection(int epoll_fd, int server_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (1) {
        int client_fd = accept(server_fd,
                               (struct sockaddr *)&client_addr, &client_len);

        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            else { perror("accept"); break; }
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("Nouvelle connexion de %s:%d (fd=%d)\n",
               client_ip, ntohs(client_addr.sin_port), client_fd);

        if (set_nonblocking(client_fd) < 0) { close(client_fd); continue; }

        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = client_fd;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0) {
            perror("epoll_ctl: client_fd");
            close(client_fd);
        }
    }
}

void handle_client_data(int epoll_fd, int client_fd) {
    char buffer[BUFFER_SIZE];

    while (1) {
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);

        if (bytes_read < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            else {
                perror("read");
                close(client_fd);
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                return;
            }
        } else if (bytes_read == 0) {
            printf("Client deconnecte (fd=%d)\n", client_fd);
            close(client_fd);
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
            return;
        }

        buffer[bytes_read] = '\0';
        printf("Recu de fd=%d: %zd bytes\n", client_fd, bytes_read);

        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
            "Hello, World!";

        ssize_t bytes_written = write(client_fd, response, strlen(response));
        if (bytes_written < 0) perror("write");

        close(client_fd);
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
        return;
    }
}

void event_loop(int server_fd) {
    struct epoll_event ev, events[MAX_EVENTS];

    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) { perror("epoll_create1"); return; }

    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) < 0) {
        perror("epoll_ctl: server_fd");
        close(epoll_fd);
        return;
    }

    printf("Boucle d'evenements demarree (Ctrl+C pour arreter)\n");

    while (keep_running) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000);
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == server_fd) {
                handle_new_connection(epoll_fd, server_fd);
            } else {
                handle_client_data(epoll_fd, events[i].data.fd);
            }
        }
    }

    close(epoll_fd);
}

int main(void) {
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    int server_fd = create_server_socket(PORT);
    if (server_fd < 0) return EXIT_FAILURE;

    event_loop(server_fd);

    close(server_fd);
    printf("\nServeur arrete\n");
    return EXIT_SUCCESS;
}
