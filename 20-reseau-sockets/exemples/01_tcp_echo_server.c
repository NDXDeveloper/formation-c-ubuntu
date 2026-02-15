/* ============================================================================
   Section 20.3 : Sockets TCP
   Description : Serveur echo TCP iteratif - renvoie en echo tout ce qu'il recoit
   Fichier source : 03-sockets-tcp.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(void) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    /* 1. Creer le socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Option pour reutiliser l'adresse immediatement */
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /* 2. Configurer l'adresse du serveur */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    /* 3. Lier le socket a l'adresse */
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /* 4. Passer en mode ecoute */
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Serveur en ecoute sur le port %d...\n", PORT);

    /* Boucle principale : accepter et traiter les clients */
    while (1) {
        client_len = sizeof(client_addr);

        /* 5. Accepter une connexion */
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        /* Afficher les informations du client */
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Client connecte : %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        /* 6. Communiquer avec le client (echo) */
        while ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
            buffer[bytes_received] = '\0';
            printf("Recu : %s", buffer);

            /* Renvoyer en echo */
            if (send(client_fd, buffer, (size_t)bytes_received, 0) < 0) {
                perror("send");
                break;
            }
        }

        if (bytes_received == 0) {
            printf("Client deconnecte\n");
        } else if (bytes_received < 0) {
            perror("recv");
        }

        /* 7. Fermer la connexion avec ce client */
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
