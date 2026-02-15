/* ============================================================================
   Section 16.8 : I/O asynchrone (AIO)
   Description : Lecture de fichier avec io_uring (API moderne Linux)
   Fichier source : 08-io-asynchrone.md
   ============================================================================ */

/* Compiler avec : gcc ... -luring
   Necessite : liburing-dev (sudo apt install liburing-dev) */

#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <liburing.h>

#define QUEUE_DEPTH 1

int main(void) {
    struct io_uring ring;

    /* Creer un fichier de test */
    int fd_w = open("/tmp/uring_test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_w == -1) {
        perror("open write");
        return 1;
    }
    const char *contenu = "Contenu lu avec io_uring!\nDeuxieme ligne.\n";
    write(fd_w, contenu, strlen(contenu));
    close(fd_w);

    /* Initialiser io_uring */
    if (io_uring_queue_init(QUEUE_DEPTH, &ring, 0) < 0) {
        perror("io_uring_queue_init");
        return 1;
    }

    /* Ouvrir un fichier */
    int fd = open("/tmp/uring_test.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        io_uring_queue_exit(&ring);
        return 1;
    }

    /* Preparer le buffer */
    char buffer[1024];
    struct iovec iov = {
        .iov_base = buffer,
        .iov_len = sizeof(buffer) - 1
    };

    /* Obtenir une Submission Queue Entry (SQE) */
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    if (!sqe) {
        fprintf(stderr, "Impossible d'obtenir une SQE\n");
        close(fd);
        io_uring_queue_exit(&ring);
        return 1;
    }

    /* Preparer une operation de lecture */
    io_uring_prep_readv(sqe, fd, &iov, 1, 0);

    /* Attacher des donnees utilisateur (optionnel) */
    io_uring_sqe_set_data(sqe, (void *)"lecture_fichier");

    /* Soumettre la requete */
    if (io_uring_submit(&ring) < 0) {
        perror("io_uring_submit");
        close(fd);
        io_uring_queue_exit(&ring);
        return 1;
    }

    printf("Requete soumise, en attente du resultat...\n");

    /* Attendre la completion */
    struct io_uring_cqe *cqe;
    if (io_uring_wait_cqe(&ring, &cqe) < 0) {
        perror("io_uring_wait_cqe");
        close(fd);
        io_uring_queue_exit(&ring);
        return 1;
    }

    /* Traiter le resultat */
    if (cqe->res < 0) {
        fprintf(stderr, "Erreur lors de la lecture: %s\n",
                strerror(-cqe->res));
    } else {
        buffer[cqe->res] = '\0';
        printf("Lu %d octets:\n%s", cqe->res, buffer);
    }

    /* Recuperer les donnees utilisateur */
    char *user_data = (char *)io_uring_cqe_get_data(cqe);
    printf("Donnees utilisateur: %s\n", user_data);

    /* Marquer la CQE comme vue */
    io_uring_cqe_seen(&ring, cqe);

    /* Nettoyer */
    io_uring_queue_exit(&ring);
    close(fd);

    /* Nettoyage fichier */
    unlink("/tmp/uring_test.txt");

    return 0;
}
