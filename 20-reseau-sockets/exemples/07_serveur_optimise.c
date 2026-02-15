/* ============================================================================
   Section 20.6 : Options des sockets
   Description : Serveur TCP optimise avec SO_REUSEADDR/PORT, keepalive, buffers
   Fichier source : 06-options-sockets.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#define PORT 8080

int create_optimized_server(void) {
    int server_fd;
    struct sockaddr_in addr;

    /* 1. Creer socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return -1;
    }

    /* 2. SO_REUSEADDR : Redemarrage rapide */
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                   &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt SO_REUSEADDR");
    }

    /* 3. SO_REUSEPORT : Permettre plusieurs instances (Linux 3.9+) */
    #ifdef SO_REUSEPORT
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT,
                   &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt SO_REUSEPORT");
    }
    #endif

    /* 4. SO_KEEPALIVE : Detecter connexions mortes */
    int keepalive = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_KEEPALIVE,
                   &keepalive, sizeof(keepalive)) < 0) {
        perror("setsockopt SO_KEEPALIVE");
    }

    /* 5. TCP_KEEPIDLE : Attendre 60s avant premier probe */
    int keepidle = 60;
    if (setsockopt(server_fd, IPPROTO_TCP, TCP_KEEPIDLE,
                   &keepidle, sizeof(keepidle)) < 0) {
        perror("setsockopt TCP_KEEPIDLE");
    }

    /* 6. TCP_KEEPINTVL : 10s entre probes */
    int keepintvl = 10;
    if (setsockopt(server_fd, IPPROTO_TCP, TCP_KEEPINTVL,
                   &keepintvl, sizeof(keepintvl)) < 0) {
        perror("setsockopt TCP_KEEPINTVL");
    }

    /* 7. TCP_KEEPCNT : 3 probes avant declaration morte */
    int keepcnt = 3;
    if (setsockopt(server_fd, IPPROTO_TCP, TCP_KEEPCNT,
                   &keepcnt, sizeof(keepcnt)) < 0) {
        perror("setsockopt TCP_KEEPCNT");
    }

    /* 8. SO_RCVBUF : Buffer reception 256 KB */
    int rcvbuf = 256 * 1024;
    if (setsockopt(server_fd, SOL_SOCKET, SO_RCVBUF,
                   &rcvbuf, sizeof(rcvbuf)) < 0) {
        perror("setsockopt SO_RCVBUF");
    }

    /* 9. SO_SNDBUF : Buffer envoi 256 KB */
    int sndbuf = 256 * 1024;
    if (setsockopt(server_fd, SOL_SOCKET, SO_SNDBUF,
                   &sndbuf, sizeof(sndbuf)) < 0) {
        perror("setsockopt SO_SNDBUF");
    }

    /* 10. Bind */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        return -1;
    }

    /* 11. Listen */
    if (listen(server_fd, 128) < 0) {
        perror("listen");
        close(server_fd);
        return -1;
    }

    printf("Serveur optimise en ecoute sur le port %d\n", PORT);
    return server_fd;
}

void handle_client(int client_fd) {
    /* Desactiver Nagle pour latence minimale */
    int nodelay = 1;
    setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));

    /* Definir timeout de reception */
    struct timeval tv = {.tv_sec = 30, .tv_usec = 0};
    setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    /* Communication avec le client (echo) */
    char buffer[1024];
    ssize_t n;
    while ((n = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[n] = '\0';
        printf("Recu : %s", buffer);
        send(client_fd, buffer, (size_t)n, 0);
    }

    if (n == 0) {
        printf("Client deconnecte\n");
    }

    close(client_fd);
}

int main(void) {
    int server_fd = create_optimized_server();
    if (server_fd < 0) {
        exit(EXIT_FAILURE);
    }

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        handle_client(client_fd);
    }

    close(server_fd);
    return 0;
}
