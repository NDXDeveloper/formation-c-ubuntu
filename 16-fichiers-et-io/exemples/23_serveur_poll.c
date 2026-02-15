/* ============================================================================
   Section 16.7 : I/O multiplexing
   Description : Serveur TCP echo avec poll() gerant plusieurs clients
   Fichier source : 07-io-multiplexing.md
   ============================================================================ */

/* Tester : ./23_serveur_poll &
   Puis : echo "hello" | nc -q0 localhost 8080 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 10
#define PORT 8080

int main(void) {
    /* Creer le socket serveur */
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    /* Option pour reutiliser l'adresse */
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Bind */
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    /* Listen */
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    printf("Serveur poll() en ecoute sur le port %d\n", PORT);
    printf("Tester avec : echo \"hello\" | nc -q0 localhost %d\n", PORT);

    /* Preparer le tableau pour poll() */
    struct pollfd fds[MAX_CLIENTS + 1];

    /* Initialiser tous les fd a -1 (non utilise) */
    for (int i = 0; i < MAX_CLIENTS + 1; i++) {
        fds[i].fd = -1;
    }

    /* Le premier slot est pour le socket serveur */
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    int nfds = 1;  /* Nombre de slots utilises */
    int total_messages = 0;

    /* Boucle principale (s'arrete apres 5 messages ou 10s sans activite) */
    while (total_messages < 5) {
        int ret = poll(fds, (nfds_t)nfds, 10000);  /* Timeout 10s */

        if (ret == -1) {
            perror("poll");
            break;
        }

        if (ret == 0) {
            printf("Timeout, arret du serveur.\n");
            break;
        }

        /* Parcourir tous les descripteurs */
        for (int i = 0; i < nfds; i++) {
            if (fds[i].revents == 0) {
                continue;  /* Rien sur ce descripteur */
            }

            if (fds[i].fd == server_fd) {
                /* Nouvelle connexion */
                int client_fd = accept(server_fd, NULL, NULL);
                if (client_fd == -1) {
                    perror("accept");
                    continue;
                }

                printf("Nouvelle connexion : fd %d\n", client_fd);

                /* Ajouter au tableau */
                if (nfds < MAX_CLIENTS + 1) {
                    fds[nfds].fd = client_fd;
                    fds[nfds].events = POLLIN;
                    nfds++;
                } else {
                    printf("Trop de clients, connexion refusee\n");
                    close(client_fd);
                }
            } else {
                /* Donnees d'un client */
                char buffer[1024];
                ssize_t n = read(fds[i].fd, buffer, sizeof(buffer) - 1);

                if (n <= 0) {
                    /* Client deconnecte */
                    printf("Client fd %d deconnecte\n", fds[i].fd);
                    close(fds[i].fd);

                    /* Compacter le tableau */
                    fds[i] = fds[nfds - 1];
                    fds[nfds - 1].fd = -1;
                    nfds--;
                    i--;  /* Reverifier ce slot */
                } else {
                    /* Echo : renvoyer les donnees */
                    buffer[n] = '\0';
                    printf("Recu de fd %d : %s", fds[i].fd, buffer);
                    write(fds[i].fd, buffer, (size_t)n);
                    total_messages++;
                }
            }
        }
    }

    /* Fermer tous les descripteurs */
    for (int i = 0; i < nfds; i++) {
        if (fds[i].fd != -1) {
            close(fds[i].fd);
        }
    }

    printf("Serveur arrete.\n");
    return 0;
}
