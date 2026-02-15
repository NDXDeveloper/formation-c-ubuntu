/* ============================================================================
   Section 25.2 : La variable errno
   Description : Demonstration de perror() pour afficher l'erreur sur stderr
   Fichier source : 02-errno.md
   ============================================================================ */

#include <stdio.h>
#include <errno.h>

int main(void) {
    FILE *fichier = fopen("inexistant.txt", "r");

    if (fichier == NULL) {
        perror("Erreur fopen");
        return 1;
    }

    fclose(fichier);
    return 0;
}
