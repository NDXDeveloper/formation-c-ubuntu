/* ============================================================================
   Section 25.2 : La variable errno
   Description : errno n'est PAS reinitialise automatiquement apres un succes
   Fichier source : 02-errno.md
   ============================================================================ */

#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(void) {
    /* Premiere operation : echec */
    FILE *f1 = fopen("inexistant.txt", "r");
    if (f1 == NULL) {
        printf("Erreur 1 : %s\n", strerror(errno));  /* errno = ENOENT */
    }

    /* Deuxieme operation : succes */
    FILE *f2 = fopen("/etc/hosts", "r");
    if (f2 != NULL) {
        printf("Fichier ouvert avec succes\n");
        /* errno contient TOUJOURS la valeur ENOENT de l'echec precedent ! */
        printf("errno vaut : %d (%s)\n", errno, strerror(errno));
        fclose(f2);
    }

    return 0;
}
