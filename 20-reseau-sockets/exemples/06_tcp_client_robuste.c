/* ============================================================================
   Section 20.5 : Gestion des erreurs reseau
   Description : Client TCP robuste avec send_all, retry et gestion SIGPIPE
   Fichier source : 05-gestion-erreurs-reseau.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024
#define MAX_RETRIES 3
#define RECV_TIMEOUT 10

/* Fonction de log simple */
void log_error(const char *msg) {
    fprintf(stderr, "[ERROR] %s: %s\n", msg, strerror(errno));
}

/* Fonction send avec gestion complete */
ssize_t send_all(int sockfd, const void *buf, size_t len) {
    size_t total = 0;
    const char *ptr = buf;

    while (total < len) {
        ssize_t n = send(sockfd, ptr + total, len - total, 0);

        if (n < 0) {
            if (errno == EINTR) continue;
            if (errno == EPIPE || errno == ECONNRESET) {
                fprintf(stderr, "Connexion fermee lors de l'envoi\n");
            } else {
                log_error("send");
            }
            return -1;
        }

        total += (size_t)n;
    }

    return (ssize_t)total;
}

/* Fonction recv avec timeout et gestion complete */
ssize_t recv_with_error_handling(int sockfd, void *buf, size_t len) {
    ssize_t n;

    do {
        n = recv(sockfd, buf, len, 0);
    } while (n < 0 && errno == EINTR);

    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            fprintf(stderr, "Timeout : pas de reponse du serveur\n");
        } else if (errno == ECONNRESET) {
            fprintf(stderr, "Connexion reinitialisee par le serveur\n");
        } else {
            log_error("recv");
        }
        return -1;
    } else if (n == 0) {
        fprintf(stderr, "Serveur a ferme la connexion\n");
        return 0;
    }

    return n;
}

/* Connexion avec retry */
int connect_to_server(const char *ip, int port) {
    int sockfd;
    struct sockaddr_in server_addr;
    int retry = 0;

    /* Creer le socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        log_error("socket");
        return -1;
    }

    /* Configurer timeout pour recv */
    struct timeval tv = {.tv_sec = RECV_TIMEOUT, .tv_usec = 0};
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        log_error("setsockopt SO_RCVTIMEO");
    }

    /* Preparer adresse serveur */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons((uint16_t)port);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Adresse IP invalide : %s\n", ip);
        close(sockfd);
        return -1;
    }

    /* Tentatives de connexion avec retry */
    while (retry < MAX_RETRIES) {
        printf("Tentative de connexion %d/%d a %s:%d...\n",
               retry + 1, MAX_RETRIES, ip, port);

        if (connect(sockfd, (struct sockaddr*)&server_addr,
                    sizeof(server_addr)) == 0) {
            printf("Connecte avec succes\n");
            return sockfd;
        }

        /* Analyser l'erreur */
        switch (errno) {
            case ECONNREFUSED:
                fprintf(stderr, "Connexion refusee (serveur n'ecoute pas?)\n");
                break;
            case ETIMEDOUT:
                fprintf(stderr, "Timeout (serveur ne repond pas?)\n");
                break;
            case ENETUNREACH:
                fprintf(stderr, "Reseau inaccessible\n");
                break;
            case EHOSTUNREACH:
                fprintf(stderr, "Hote inaccessible\n");
                break;
            default:
                log_error("connect");
                close(sockfd);
                return -1;
        }

        retry++;
        if (retry < MAX_RETRIES) {
            int delay = 2 * retry;
            printf("  Attente de %d secondes avant retry...\n", delay);
            sleep((unsigned int)delay);
        }
    }

    fprintf(stderr, "Impossible de se connecter apres %d tentatives\n", MAX_RETRIES);
    close(sockfd);
    return -1;
}

int main(void) {
    int sockfd;
    char buffer[BUFFER_SIZE];

    /* Ignorer SIGPIPE */
    signal(SIGPIPE, SIG_IGN);

    /* Se connecter au serveur */
    sockfd = connect_to_server(SERVER_IP, SERVER_PORT);
    if (sockfd < 0) {
        exit(EXIT_FAILURE);
    }

    /* Boucle d'interaction */
    while (1) {
        printf("\nMessage a envoyer (ou 'quit') : ");

        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        if (strcmp(buffer, "quit\n") == 0) {
            break;
        }

        /* Envoyer le message */
        if (send_all(sockfd, buffer, strlen(buffer)) < 0) {
            fprintf(stderr, "Erreur lors de l'envoi, fermeture connexion\n");
            break;
        }

        /* Recevoir la reponse */
        ssize_t n = recv_with_error_handling(sockfd, buffer, BUFFER_SIZE - 1);
        if (n < 0) {
            fprintf(stderr, "Erreur lors de la reception, fermeture connexion\n");
            break;
        } else if (n == 0) {
            fprintf(stderr, "Serveur a ferme la connexion\n");
            break;
        }

        buffer[n] = '\0';
        printf("Reponse : %s", buffer);
    }

    /* Nettoyage */
    close(sockfd);
    printf("\nDeconnecte proprement\n");

    return 0;
}
