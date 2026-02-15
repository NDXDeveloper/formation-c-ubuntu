/* ============================================================================
   Section 17.9 : Named pipes (FIFO)
   Description : Supprimer un FIFO avec unlink
   Fichier source : 09-named-pipes.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

int main(void) {
    const char *fifo_path = "/tmp/mon_fifo_unlink_test";

    /* Creer le FIFO */
    mkfifo(fifo_path, 0666);
    printf("FIFO cree: %s\n", fifo_path);

    /* Supprimer le FIFO */
    if (unlink(fifo_path) == 0) {
        printf("FIFO supprime: %s\n", fifo_path);
    } else {
        perror("unlink");
        return 1;
    }

    return 0;
}
