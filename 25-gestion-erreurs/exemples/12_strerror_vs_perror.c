/* ============================================================================
   Section 25.2 : La variable errno
   Description : Comparaison entre strerror() et perror()
   Fichier source : 02-errno.md
   ============================================================================ */

#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(void) {
    FILE *fichier = fopen("test.txt", "r");

    if (fichier == NULL) {
        /* Methode 1 : strerror (manuel) */
        fprintf(stderr, "Erreur : %s\n", strerror(errno));

        /* Methode 2 : perror (automatique) */
        perror("Erreur");
    }

    return 0;
}
