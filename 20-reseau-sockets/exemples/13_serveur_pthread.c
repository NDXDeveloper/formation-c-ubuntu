/* ============================================================================
   Section 20.8 : Serveur concurrent
   Description : Serveur multi-threads avec pthreads et threads detaches
   Fichier source : 08-serveur-concurrent.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

/* Structure pour passer des arguments au thread */
typedef struct {
    int client_fd;
    struct sockaddr_in client_addr;
} client_info_t;

/* Fonction executee par chaque thread */
void* handle_client_thread(void *arg) {
    client_info_t *info = (client_info_t*)arg;
    int client_fd = info->client_fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    /* Afficher l'IP du client */
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &info->client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    printf("[Thread %lu] Client connecte : %s:%d\n",
           (unsigned long)pthread_self(), client_ip, ntohs(info->client_addr.sin_port));

    /* Liberer la memoire allouee pour info */
    free(info);

    /* Gerer le client (echo) */
    while ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("[Thread %lu] Recu : %s", (unsigned long)pthread_self(), buffer);

        if (send(client_fd, buffer, (size_t)bytes_received, 0) < 0) {
            perror("send");
            break;
        }
    }

    if (bytes_received == 0) {
        printf("[Thread %lu] Client deconnecte\n", (unsigned long)pthread_self());
    } else {
        perror("recv");
    }

    close(client_fd);
    return NULL;
}

int main(void) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    pthread_t thread;

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

    printf("Serveur multi-threads en ecoute sur le port %d\n", PORT);

    /* Boucle principale */
    while (1) {
        client_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        /* Allouer une structure pour passer les infos au thread */
        client_info_t *info = malloc(sizeof(client_info_t));
        if (info == NULL) {
            perror("malloc");
            close(client_fd);
            continue;
        }

        info->client_fd = client_fd;
        info->client_addr = client_addr;

        /* Creer un thread pour ce client */
        if (pthread_create(&thread, NULL, handle_client_thread, info) != 0) {
            perror("pthread_create");
            free(info);
            close(client_fd);
            continue;
        }

        /* Detacher le thread (pas besoin de join) */
        pthread_detach(thread);
    }

    close(server_fd);
    return 0;
}
