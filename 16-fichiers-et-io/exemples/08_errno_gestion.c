/* ============================================================================
   Section 16.2 : Appels systeme
   Description : Gestion des erreurs avec errno, perror et strerror
   Fichier source : 02-appels-systeme.md
   ============================================================================ */

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

int main(void) {
    /* Tenter d'ouvrir un fichier inexistant */
    int fd = open("fichier_inexistant_xyz.txt", O_RDONLY);
    if (fd == -1) {
        printf("Erreur numero : %d\n", errno);
        printf("Message : %s\n", strerror(errno));
        perror("open");

        /* Verifier le type d'erreur */
        if (errno == ENOENT) {
            printf("=> Fichier non trouve\n");
        } else if (errno == EACCES) {
            printf("=> Permission refusee\n");
        }
    }

    /* Tenter d'ecrire sur un fd invalide */
    ssize_t n = write(-1, "test", 4);
    if (n == -1) {
        printf("\nEcriture sur fd invalide :\n");
        printf("Erreur numero : %d (%s)\n", errno, strerror(errno));

        if (errno == EBADF) {
            printf("=> Mauvais descripteur de fichier\n");
        }
    }

    return 0;
}
