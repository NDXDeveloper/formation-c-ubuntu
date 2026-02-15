/* ============================================================================
   Section 20.7 : Resolution de noms
   Description : Serveur TCP avec getaddrinfo et AI_PASSIVE (IPv4/IPv6)
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

#define PORT "8080"

int main(void) {
    struct addrinfo hints, *result, *rp;
    int server_fd, status;

    /* 1. Preparer les hints pour un serveur */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;      /* IPv4 ou IPv6 */
    hints.ai_socktype = SOCK_STREAM;  /* TCP */
    hints.ai_flags = AI_PASSIVE;      /* Pour bind() (ecoute) */

    /* 2. Resoudre (node=NULL pour INADDR_ANY) */
    status = getaddrinfo(NULL, PORT, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    /* 3. Essayer de creer et bind le socket */
    server_fd = -1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        /* Creer socket */
        server_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (server_fd == -1) {
            continue;
        }

        /* Reutiliser l'adresse */
        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        /* Bind */
        if (bind(server_fd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;  /* Succes */
        }

        close(server_fd);
        server_fd = -1;
    }

    freeaddrinfo(result);

    if (server_fd == -1) {
        fprintf(stderr, "Impossible de bind\n");
        exit(EXIT_FAILURE);
    }

    /* 4. Listen */
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Serveur en ecoute sur le port %s\n", PORT);

    /* 5. Accept des clients */
    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        printf("Client connecte\n");
        /* Traiter le client (echo simple) */
        char buffer[1024];
        ssize_t n;
        while ((n = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[n] = '\0';
            printf("Recu : %s", buffer);
            send(client_fd, buffer, (size_t)n, 0);
        }
        printf("Client deconnecte\n");
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
