/* ============================================================================
   Section 16.8 : I/O asynchrone (AIO)
   Description : Serveur TCP echo avec io_uring (haute performance)
   Fichier source : 08-io-asynchrone.md
   ============================================================================ */

/* Compiler avec : gcc ... -luring
   Tester : echo "hello" | nc -q0 localhost 8082 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <liburing.h>

#define QUEUE_DEPTH 256
#define BUF_SIZE 1024
#define PORT 8082

typedef enum {
    OP_ACCEPT,
    OP_READ,
    OP_WRITE
} op_type_t;

typedef struct {
    op_type_t type;
    int fd;
    char buffer[BUF_SIZE];
    size_t len;
} request_t;

static int setup_listening_socket(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) return -1;

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons((uint16_t)port);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(sock);
        return -1;
    }

    if (listen(sock, SOMAXCONN) == -1) {
        close(sock);
        return -1;
    }

    return sock;
}

static void add_accept(struct io_uring *ring, int server_fd) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    if (!sqe) return;

    request_t *req = malloc(sizeof(request_t));
    if (!req) return;
    req->type = OP_ACCEPT;
    req->fd = server_fd;

    io_uring_prep_accept(sqe, server_fd, NULL, NULL, 0);
    io_uring_sqe_set_data(sqe, req);
}

static void add_read(struct io_uring *ring, int client_fd) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    if (!sqe) return;

    request_t *req = malloc(sizeof(request_t));
    if (!req) return;
    req->type = OP_READ;
    req->fd = client_fd;

    io_uring_prep_read(sqe, client_fd, req->buffer, BUF_SIZE, 0);
    io_uring_sqe_set_data(sqe, req);
}

static void add_write(struct io_uring *ring, int client_fd,
                      const char *data, size_t len) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    if (!sqe) return;

    request_t *req = malloc(sizeof(request_t));
    if (!req) return;
    req->type = OP_WRITE;
    req->fd = client_fd;
    req->len = len;
    memcpy(req->buffer, data, len);

    io_uring_prep_write(sqe, client_fd, req->buffer, (unsigned int)len, 0);
    io_uring_sqe_set_data(sqe, req);
}

int main(void) {
    int server_fd = setup_listening_socket(PORT);
    if (server_fd == -1) {
        perror("setup_listening_socket");
        return 1;
    }

    printf("Serveur io_uring demarre sur le port %d\n", PORT);
    printf("Tester avec : echo \"hello\" | nc -q0 localhost %d\n", PORT);

    struct io_uring ring;
    if (io_uring_queue_init(QUEUE_DEPTH, &ring, 0) < 0) {
        perror("io_uring_queue_init");
        close(server_fd);
        return 1;
    }

    /* Soumettre le premier accept */
    add_accept(&ring, server_fd);
    io_uring_submit(&ring);

    int total_messages = 0;

    /* Boucle d'evenements (s'arrete apres 5 messages) */
    while (total_messages < 5) {
        struct io_uring_cqe *cqe;
        int ret = io_uring_wait_cqe_timeout(&ring, &cqe,
            &(struct __kernel_timespec){.tv_sec = 10});

        if (ret == -ETIME) {
            printf("Timeout, arret du serveur.\n");
            break;
        }
        if (ret < 0) {
            fprintf(stderr, "io_uring_wait_cqe: %s\n", strerror(-ret));
            break;
        }

        request_t *req = (request_t *)io_uring_cqe_get_data(cqe);

        if (cqe->res < 0) {
            fprintf(stderr, "Erreur: %s\n", strerror(-cqe->res));
            if (req->type != OP_ACCEPT) close(req->fd);
            free(req);
            io_uring_cqe_seen(&ring, cqe);
            continue;
        }

        switch (req->type) {
            case OP_ACCEPT: {
                int client_fd = cqe->res;
                printf("Nouvelle connexion: fd %d\n", client_fd);

                /* Preparer la lecture pour ce client */
                add_read(&ring, client_fd);

                /* Relancer un accept pour le prochain client */
                add_accept(&ring, server_fd);

                io_uring_submit(&ring);
                break;
            }

            case OP_READ: {
                int bytes_read = cqe->res;

                if (bytes_read == 0) {
                    /* Client deconnecte */
                    printf("Client fd %d deconnecte\n", req->fd);
                    close(req->fd);
                } else {
                    /* Echo : renvoyer les donnees */
                    req->buffer[bytes_read] = '\0';
                    printf("Recu de fd %d: %s", req->fd, req->buffer);

                    add_write(&ring, req->fd,
                              req->buffer, (size_t)bytes_read);
                    io_uring_submit(&ring);
                    total_messages++;
                }
                break;
            }

            case OP_WRITE: {
                /* Ecriture terminee, relancer une lecture */
                add_read(&ring, req->fd);
                io_uring_submit(&ring);
                break;
            }
        }

        free(req);
        io_uring_cqe_seen(&ring, cqe);
    }

    io_uring_queue_exit(&ring);
    close(server_fd);

    printf("Serveur arrete.\n");
    return 0;
}
