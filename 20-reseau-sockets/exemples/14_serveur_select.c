/* ============================================================================
   Section 20.8 : Serveur concurrent
   Description : Serveur avec multiplexage I/O via select() et fd_set
   Fichier source : 08-serveur-concurrent.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 30

int main(void) {
    int server_fd, client_fds[MAX_CLIENTS];
    fd_set readfds;
    int max_fd, activity, new_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];

    /* Initialiser tous les clients a 0 (non utilises) */
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_fds[i] = 0;
    }

    /* Creer le socket serveur */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* SO_REUSEADDR */
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Bind */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /* Listen */
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Serveur select() en ecoute sur le port %d\n", PORT);
    printf("Clients max : %d\n", MAX_CLIENTS);

    /* Boucle principale */
    while (1) {
        /* Vider et preparer l'ensemble des descripteurs */
        FD_ZERO(&readfds);

        /* Ajouter le socket serveur */
        FD_SET(server_fd, &readfds);
        max_fd = server_fd;

        /* Ajouter tous les sockets clients actifs */
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = client_fds[i];

            if (fd > 0) {
                FD_SET(fd, &readfds);
            }

            if (fd > max_fd) {
                max_fd = fd;
            }
        }

        /* Attendre une activite sur un des sockets */
        activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("select");
            break;
        }

        /* Si activite sur le socket serveur = nouvelle connexion */
        if (FD_ISSET(server_fd, &readfds)) {
            client_len = sizeof(client_addr);
            new_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

            if (new_socket < 0) {
                perror("accept");
                continue;
            }

            /* Afficher infos client */
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
            printf("Nouvelle connexion : %s:%d (socket %d)\n",
                   client_ip, ntohs(client_addr.sin_port), new_socket);

            /* Ajouter a la liste des clients */
            int added = 0;
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_fds[i] == 0) {
                    client_fds[i] = new_socket;
                    printf("Client ajoute a la position %d\n", i);
                    added = 1;
                    break;
                }
            }

            if (!added) {
                printf("Trop de clients, connexion refusee\n");
                close(new_socket);
            }
        }

        /* Verifier tous les clients pour des donnees entrantes */
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = client_fds[i];

            if (fd > 0 && FD_ISSET(fd, &readfds)) {
                /* Lire les donnees */
                ssize_t bytes_read = recv(fd, buffer, BUFFER_SIZE - 1, 0);

                if (bytes_read == 0) {
                    /* Client deconnecte */
                    client_len = sizeof(client_addr);
                    getpeername(fd, (struct sockaddr*)&client_addr, &client_len);
                    char client_ip[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                    printf("Client deconnecte : %s:%d (socket %d)\n",
                           client_ip, ntohs(client_addr.sin_port), fd);

                    close(fd);
                    client_fds[i] = 0;
                } else if (bytes_read < 0) {
                    perror("recv");
                    close(fd);
                    client_fds[i] = 0;
                } else {
                    /* Donnees recues */
                    buffer[bytes_read] = '\0';
                    printf("Socket %d : %s", fd, buffer);

                    /* Echo */
                    send(fd, buffer, (size_t)bytes_read, 0);
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
