/* ============================================================================
   Section 20.8 : Serveur concurrent
   Description : Serveur multi-processus avec fork et gestion SIGCHLD
   Fichier source : 08-serveur-concurrent.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

/* Gestionnaire pour nettoyer les processus zombies */
void sigchld_handler(int sig) {
    (void)sig;
    /* Attendre tous les fils termines (sans bloquer) */
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        /* vide */
    }
}

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    printf("[PID %d] Client connecte\n", getpid());

    while ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("[PID %d] Recu : %s", getpid(), buffer);

        /* Echo */
        if (send(client_fd, buffer, (size_t)bytes_received, 0) < 0) {
            perror("send");
            break;
        }
    }

    if (bytes_received == 0) {
        printf("[PID %d] Client deconnecte\n", getpid());
    } else {
        perror("recv");
    }

    close(client_fd);
}

int main(void) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;

    /* Installer le gestionnaire de signal pour SIGCHLD */
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;  /* Redemarrer accept() si interrompu */
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
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
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /* Listen */
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Serveur multi-processus en ecoute sur le port %d\n", PORT);

    /* Boucle principale */
    while (1) {
        client_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        /* Afficher l'IP du client */
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Nouvelle connexion de %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        /* Creer un processus fils pour ce client */
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            close(client_fd);
            continue;
        }

        if (pid == 0) {
            /* PROCESSUS FILS */
            close(server_fd);
            handle_client(client_fd);
            exit(EXIT_SUCCESS);
        } else {
            /* PROCESSUS PARENT */
            close(client_fd);
        }
    }

    close(server_fd);
    return 0;
}
