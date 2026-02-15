/* ============================================================================
   Section 26.8 : Compilation avec hardening flags
   Description : Demonstration de FORTIFY_SOURCE
                 Bug intentionnel : strcpy overflow pour declencher la detection
   Fichier source : 08-hardening-flags.md
   ============================================================================ */

/* ATTENTION : Ce programme contient un bug INTENTIONNEL pour demontrer
   la detection par FORTIFY_SOURCE. Il crashera avec le message :
   "*** buffer overflow detected ***" */

#include <string.h>
#include <stdio.h>

int main(void) {
    /* Desactiver le buffering pour voir les messages avant le crash */
    setbuf(stdout, NULL);

    printf("=== Demonstration FORTIFY_SOURCE ===\n\n");

    char dest[10];
    char source[50] = "Ceci est une chaine beaucoup trop longue";

    printf("Taille dest   : %zu\n", sizeof(dest));
    printf("Taille source : %zu\n", strlen(source));
    printf("\nCopie strcpy(dest, source) -> overflow !\n");

    /* Bug intentionnel : overflow detecte par FORTIFY_SOURCE */
    strcpy(dest, source);

    /* Jamais atteint avec FORTIFY_SOURCE */
    printf("dest = %s\n", dest);

    return 0;
}
