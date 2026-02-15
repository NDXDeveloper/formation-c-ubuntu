/* ============================================================================
   Section 20.4 : Sockets UDP
   Description : Serveur echo UDP - renvoie en echo les datagrammes recus
   Fichier source : 04-sockets-udp.md
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
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    /* 1. Creer le socket UDP */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* 2. Configurer l'adresse du serveur */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    /* 3. Lier le socket a l'adresse */
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Serveur UDP en ecoute sur le port %d...\n", PORT);

    /* 4. Boucle principale : recevoir et repondre */
    while (1) {
        client_len = sizeof(client_addr);

        /* Recevoir un datagramme */
        bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                                  (struct sockaddr*)&client_addr, &client_len);

        if (bytes_received < 0) {
            perror("recvfrom");
            continue;
        }

        buffer[bytes_received] = '\0';

        /* Afficher les informations */
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Recu de %s:%d (%zd octets) : %s",
               client_ip, ntohs(client_addr.sin_port), bytes_received, buffer);

        /* Renvoyer en echo au client */
        ssize_t bytes_sent = sendto(sockfd, buffer, (size_t)bytes_received, 0,
                                     (struct sockaddr*)&client_addr, client_len);

        if (bytes_sent < 0) {
            perror("sendto");
        } else {
            printf("Echo envoye (%zd octets)\n", bytes_sent);
        }
    }

    close(sockfd);
    return 0;
}
