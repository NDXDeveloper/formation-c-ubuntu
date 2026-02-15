/* ============================================================================
   Section 20.4 : Sockets UDP
   Description : Client UDP avec retry et timeout en cas de non-reponse
   Fichier source : 04-sockets-udp.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define TIMEOUT_SEC 2
#define MAX_RETRIES 3

int main(void) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int retry_count;

    /* Creer le socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Definir un timeout */
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    /* Configurer l'adresse du serveur */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    const char *message = "Hello, UDP with retry!";

    /* Boucle de retry */
    for (retry_count = 0; retry_count < MAX_RETRIES; retry_count++) {
        printf("Tentative %d/%d...\n", retry_count + 1, MAX_RETRIES);

        /* Envoyer le message */
        if (sendto(sockfd, message, strlen(message), 0,
                   (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("sendto");
            continue;
        }

        /* Attendre la reponse */
        ssize_t bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, NULL, NULL);

        if (bytes_received < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("Timeout : pas de reponse\n");
                continue;
            } else {
                perror("recvfrom");
                break;
            }
        }

        /* Reponse recue avec succes */
        buffer[bytes_received] = '\0';
        printf("Reponse recue : %s\n", buffer);
        break;
    }

    if (retry_count == MAX_RETRIES) {
        printf("Echec apres %d tentatives\n", MAX_RETRIES);
    }

    close(sockfd);
    return 0;
}
