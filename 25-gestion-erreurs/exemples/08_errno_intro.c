/* ============================================================================
   Section 25.2 : La variable errno
   Description : Introduction a errno avec fopen et strerror
   Fichier source : 02-errno.md
   ============================================================================ */

#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(void) {
    FILE *fichier = fopen("fichier_inexistant.txt", "r");

    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier\n");
        printf("Code errno : %d\n", errno);
        printf("Signification : %s\n", strerror(errno));
    } else {
        printf("Fichier ouvert avec succes\n");
        fclose(fichier);
    }

    return 0;
}
