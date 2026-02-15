/* ============================================================================
   Section 20.7 : Resolution de noms
   Description : Client DNS avec getaddrinfo - connexion HTTP a www.example.com
   Fichier source : 07-resolution-noms.md
   ============================================================================ */

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define HOSTNAME "www.example.com"
#define PORT "80"

int main(void) {
    struct addrinfo hints, *result, *rp;
    int sockfd, status;

    /* 1. Preparer les hints */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;      /* IPv4 ou IPv6 */
    hints.ai_socktype = SOCK_STREAM;  /* TCP */

    /* 2. Resoudre le nom */
    printf("Resolution de %s...\n", HOSTNAME);
    status = getaddrinfo(HOSTNAME, PORT, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    /* 3. Essayer de se connecter a chaque adresse */
    sockfd = -1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        /* Afficher l'adresse tentee */
        char host[NI_MAXHOST];
        getnameinfo(rp->ai_addr, rp->ai_addrlen,
                    host, sizeof(host), NULL, 0, NI_NUMERICHOST);
        printf("Tentative de connexion a %s...\n", host);

        /* Creer le socket */
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd == -1) {
            perror("socket");
            continue;
        }

        /* Tenter la connexion */
        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            printf("Connecte avec succes !\n");
            break;  /* Succes */
        }

        /* Echec */
        perror("connect");
        close(sockfd);
        sockfd = -1;
    }

    /* 4. Verifier si connexion reussie */
    if (sockfd == -1) {
        fprintf(stderr, "Impossible de se connecter a %s\n", HOSTNAME);
        freeaddrinfo(result);
        exit(EXIT_FAILURE);
    }

    /* 5. Liberer la memoire */
    freeaddrinfo(result);

    /* 6. Utiliser la connexion */
    const char *request = "GET / HTTP/1.0\r\nHost: www.example.com\r\n\r\n";
    send(sockfd, request, strlen(request), 0);

    char buffer[4096];
    ssize_t bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("\nReponse du serveur :\n%s\n", buffer);
    }

    /* 7. Fermer */
    close(sockfd);
    return 0;
}
