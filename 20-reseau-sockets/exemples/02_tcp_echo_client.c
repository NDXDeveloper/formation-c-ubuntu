/* ============================================================================
   Section 20.3 : Sockets TCP
   Description : Client echo TCP - se connecte au serveur et envoie des messages
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
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    /* 1. Creer le socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* 2. Configurer l'adresse du serveur */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    /* Convertir l'adresse IP du serveur */
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    /* 3. Se connecter au serveur */
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connecte au serveur\n");
    printf("Tapez un message (ou 'quit' pour quitter) :\n");

    /* 4. Boucle d'interaction */
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
        if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
            perror("send");
            break;
        }

        /* Recevoir la reponse (echo) */
        bytes_received = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received < 0) {
            perror("recv");
            break;
        } else if (bytes_received == 0) {
            printf("Serveur deconnecte\n");
            break;
        }

        buffer[bytes_received] = '\0';
        printf("Echo : %s", buffer);
    }

    /* 5. Fermer le socket */
    close(sockfd);
    printf("Deconnecte\n");
    return 0;
}
