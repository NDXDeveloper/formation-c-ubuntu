/* ============================================================================
   Section 20.6 : Options des sockets
   Description : Affichage diagnostic des options d'un socket TCP
   Fichier source : 06-options-sockets.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

void print_socket_options(int sockfd) {
    int value;
    socklen_t len = sizeof(value);

    printf("=== Options du socket %d ===\n", sockfd);

    /* SO_REUSEADDR */
    if (getsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &value, &len) == 0) {
        printf("SO_REUSEADDR: %s\n", value ? "ON" : "OFF");
    }

    /* SO_KEEPALIVE */
    if (getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &value, &len) == 0) {
        printf("SO_KEEPALIVE: %s\n", value ? "ON" : "OFF");
    }

    /* SO_RCVBUF */
    if (getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &value, &len) == 0) {
        printf("SO_RCVBUF: %d octets\n", value);
    }

    /* SO_SNDBUF */
    if (getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &value, &len) == 0) {
        printf("SO_SNDBUF: %d octets\n", value);
    }

    /* TCP_NODELAY */
    if (getsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &value, &len) == 0) {
        printf("TCP_NODELAY: %s\n", value ? "ON" : "OFF");
    }

    /* TCP_KEEPIDLE */
    if (getsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &value, &len) == 0) {
        printf("TCP_KEEPIDLE: %d secondes\n", value);
    }

    /* TCP_KEEPINTVL */
    if (getsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &value, &len) == 0) {
        printf("TCP_KEEPINTVL: %d secondes\n", value);
    }

    /* TCP_KEEPCNT */
    if (getsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &value, &len) == 0) {
        printf("TCP_KEEPCNT: %d\n", value);
    }

    printf("===========================\n");
}

int main(void) {
    /* Creer un socket TCP pour afficher ses options par defaut */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    printf("Options par defaut :\n");
    print_socket_options(sockfd);

    /* Modifier quelques options et reafficher */
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    printf("\nApres modifications :\n");
    print_socket_options(sockfd);

    close(sockfd);
    return 0;
}
