/* ============================================================================
   Section 20.4 : Sockets UDP
   Description : Client echo UDP - envoie des messages au serveur UDP
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
    struct sockaddr_in server_addr;
    socklen_t server_len;
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
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Client UDP pret (serveur : 127.0.0.1:%d)\n", PORT);
    printf("Tapez un message (ou 'quit' pour quitter) :\n");

    /* 3. Boucle d'interaction */
    while (1) {
        printf("> ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        /* Verifier si l'utilisateur veut quitter */
        if (strcmp(buffer, "quit\n") == 0) {
            break;
        }

        /* Envoyer le message au serveur */
        ssize_t bytes_sent = sendto(sockfd, buffer, strlen(buffer), 0,
                                     (struct sockaddr*)&server_addr, sizeof(server_addr));

        if (bytes_sent < 0) {
            perror("sendto");
            continue;
        }

        printf("Envoye : %zd octets\n", bytes_sent);

        /* Attendre la reponse (echo) */
        server_len = sizeof(server_addr);
        bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                                  (struct sockaddr*)&server_addr, &server_len);

        if (bytes_received < 0) {
            perror("recvfrom");
            continue;
        }

        buffer[bytes_received] = '\0';
        printf("Echo recu (%zd octets) : %s", bytes_received, buffer);
    }

    /* 4. Fermer le socket */
    close(sockfd);
    printf("Deconnecte\n");
    return 0;
}
