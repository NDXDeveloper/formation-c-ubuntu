/* ============================================================================
   Section 17.9 : Named pipes (FIFO)
   Description : Ouverture non-bloquante d'un FIFO avec O_NONBLOCK
   Fichier source : 09-named-pipes.md
   ============================================================================ */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

int main(void) {
    const char *fifo_path = "/tmp/fifo_nonblock_test";

    /* Creer le FIFO */
    mkfifo(fifo_path, 0666);

    printf("Tentative d'ouverture non-bloquante...\n");

    int fd = open(fifo_path, O_WRONLY | O_NONBLOCK);

    if (fd == -1) {
        if (errno == ENXIO) {
            printf("Aucun lecteur connecte\n");
        } else {
            perror("open");
        }
    } else {
        printf("Ouvert immediatement (lecteur present)\n");
        write(fd, "Message", 8);
        close(fd);
    }

    /* Nettoyage */
    unlink(fifo_path);

    return 0;
}
