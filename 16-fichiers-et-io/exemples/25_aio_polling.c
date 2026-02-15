/* ============================================================================
   Section 16.8 : I/O asynchrone (AIO)
   Description : Lecture asynchrone POSIX AIO avec verification par polling
   Fichier source : 08-io-asynchrone.md
   ============================================================================ */

/* Compiler avec : gcc ... -lrt */

#define _XOPEN_SOURCE 600
#include <aio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(void) {
    /* Creer un fichier de test */
    int fd_w = open("/tmp/aio_test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_w == -1) {
        perror("open write");
        return 1;
    }
    const char *contenu = "Contenu lu de maniere asynchrone avec POSIX AIO!\n";
    write(fd_w, contenu, strlen(contenu));
    close(fd_w);

    /* Ouvrir pour lecture */
    int fd = open("/tmp/aio_test.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    /* Preparer la structure aiocb */
    struct aiocb cb;
    memset(&cb, 0, sizeof(cb));

    char buffer[1024];
    cb.aio_fildes = fd;
    cb.aio_buf = buffer;
    cb.aio_nbytes = sizeof(buffer) - 1;
    cb.aio_offset = 0;

    /* Lancer la lecture asynchrone */
    if (aio_read(&cb) == -1) {
        perror("aio_read");
        close(fd);
        return 1;
    }

    printf("Lecture lancee, programme continue...\n");

    /* Faire autre chose pendant que la lecture se fait */
    int calcul = 0;
    for (int i = 0; i < 1000000; i++) {
        calcul += i;
    }
    printf("Calcul effectue pendant l'attente : %d\n", calcul);

    /* Verifier si la lecture est terminee (polling) */
    int status;
    int checks = 0;
    while ((status = aio_error(&cb)) == EINPROGRESS) {
        checks++;
        usleep(1000);  /* Attendre 1ms */
    }

    printf("Verifications polling : %d\n", checks);

    if (status == 0) {
        /* Succes : recuperer le resultat */
        ssize_t bytes = aio_return(&cb);
        printf("Lu %zd octets\n", bytes);
        buffer[bytes] = '\0';
        printf("Contenu: %s", buffer);
    } else {
        fprintf(stderr, "Erreur lors de la lecture: %s\n", strerror(status));
    }

    close(fd);

    /* Nettoyage */
    unlink("/tmp/aio_test.txt");

    return 0;
}
