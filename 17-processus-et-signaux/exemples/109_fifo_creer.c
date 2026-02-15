/* ============================================================================
   Section 17.9 : Named pipes (FIFO)
   Description : Creer un FIFO en C avec mkfifo
   Fichier source : 09-named-pipes.md
   ============================================================================ */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

int main(void) {
    const char *fifo_path = "/tmp/mon_fifo_test";

    /* Creer le FIFO avec permissions rw-rw-rw- */
    if (mkfifo(fifo_path, 0666) == -1) {
        if (errno == EEXIST) {
            printf("Le FIFO existe deja\n");
        } else {
            perror("mkfifo");
            return 1;
        }
    } else {
        printf("FIFO cree avec succes: %s\n", fifo_path);
    }

    /* Nettoyage */
    unlink(fifo_path);

    return 0;
}
