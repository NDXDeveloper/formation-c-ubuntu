/* ============================================================================
   Section 17.9 : Named pipes (FIFO)
   Description : Verifier l'existence d'un FIFO avant creation
   Fichier source : 09-named-pipes.md
   ============================================================================ */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

int create_fifo_if_needed(const char *path) {
    struct stat st;

    /* Verifier si le fichier existe */
    if (stat(path, &st) == 0) {
        /* Le fichier existe, verifier si c'est un FIFO */
        if (S_ISFIFO(st.st_mode)) {
            printf("FIFO existe deja: %s\n", path);
            return 0;
        } else {
            fprintf(stderr, "Erreur: %s existe mais n'est pas un FIFO\n", path);
            return -1;
        }
    }

    /* Le fichier n'existe pas, creer le FIFO */
    if (mkfifo(path, 0666) == -1) {
        perror("mkfifo");
        return -1;
    }

    printf("FIFO cree: %s\n", path);
    return 0;
}

int main(void) {
    const char *path = "/tmp/my_fifo_test";

    create_fifo_if_needed(path);

    /* Deuxieme appel : le FIFO existe deja */
    create_fifo_if_needed(path);

    /* Nettoyage */
    unlink(path);

    return 0;
}
