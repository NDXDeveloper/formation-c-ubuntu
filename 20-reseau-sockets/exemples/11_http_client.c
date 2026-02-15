/* ============================================================================
   Section 20.7 : Resolution de noms
   Description : Client HTTP simple avec resolution DNS et arguments CLI
   Fichier source : 07-resolution-noms.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int http_get(const char *hostname, const char *path) {
    struct addrinfo hints, *result, *rp;
    int sockfd, status;
    char request[2048], response[4096];

    /* 1. Resolution DNS */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    printf("Resolution de %s...\n", hostname);
    status = getaddrinfo(hostname, "80", &hints, &result);
    if (status != 0) {
        fprintf(stderr, "Erreur DNS : %s\n", gai_strerror(status));
        return -1;
    }

    /* 2. Connexion */
    sockfd = -1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd == -1) continue;

        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;  /* Succes */
        }

        close(sockfd);
        sockfd = -1;
    }

    freeaddrinfo(result);

    if (sockfd == -1) {
        fprintf(stderr, "Impossible de se connecter\n");
        return -1;
    }

    printf("Connecte a %s\n", hostname);

    /* 3. Envoyer requete HTTP */
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n"
             "\r\n",
             path, hostname);

    if (send(sockfd, request, strlen(request), 0) < 0) {
        perror("send");
        close(sockfd);
        return -1;
    }

    /* 4. Recevoir reponse */
    printf("\n=== Reponse ===\n");
    ssize_t bytes;
    while ((bytes = recv(sockfd, response, sizeof(response) - 1, 0)) > 0) {
        response[bytes] = '\0';
        printf("%s", response);
    }
    printf("\n=============\n");

    /* 5. Fermer */
    close(sockfd);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <hostname> <path>\n", argv[0]);
        fprintf(stderr, "Exemple: %s www.example.com /index.html\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    http_get(argv[1], argv[2]);
    return 0;
}
