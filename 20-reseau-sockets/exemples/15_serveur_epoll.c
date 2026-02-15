/* ============================================================================
   Section 20.8 : Serveur concurrent
   Description : Serveur avec epoll (Level-Triggered) haute performance
   Fichier source : 08-serveur-concurrent.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define PORT 8080
#define MAX_EVENTS 100
#define BUFFER_SIZE 1024

int main(void) {
    int server_fd, epoll_fd;
    struct sockaddr_in server_addr;
    struct epoll_event event, events[MAX_EVENTS];

    /* Creer socket serveur */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    /* Creer instance epoll */
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    /* Ajouter le socket serveur a epoll */
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) < 0) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    printf("Serveur epoll en ecoute sur le port %d\n", PORT);

    char buffer[BUFFER_SIZE];

    /* Boucle evenementielle */
    while (1) {
        int n_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        if (n_events < 0) {
            perror("epoll_wait");
            break;
        }

        /* Traiter tous les evenements */
        for (int i = 0; i < n_events; i++) {
            int fd = events[i].data.fd;

            if (fd == server_fd) {
                /* Nouvelle connexion */
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

                if (client_fd < 0) {
                    perror("accept");
                    continue;
                }

                char client_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                printf("Nouvelle connexion : %s:%d (socket %d)\n",
                       client_ip, ntohs(client_addr.sin_port), client_fd);

                /* Ajouter le client a epoll */
                event.events = EPOLLIN;
                event.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) < 0) {
                    perror("epoll_ctl ADD client");
                    close(client_fd);
                }
            } else {
                /* Donnees d'un client */
                ssize_t bytes_read = recv(fd, buffer, BUFFER_SIZE - 1, 0);

                if (bytes_read <= 0) {
                    /* Deconnexion */
                    if (bytes_read == 0) {
                        printf("Client deconnecte (socket %d)\n", fd);
                    } else {
                        perror("recv");
                    }

                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                } else {
                    /* Echo */
                    buffer[bytes_read] = '\0';
                    printf("Socket %d : %s", fd, buffer);
                    send(fd, buffer, (size_t)bytes_read, 0);
                }
            }
        }
    }

    close(server_fd);
    close(epoll_fd);
    return 0;
}
