/* ============================================================================
   Section 10.2 : Alignement memoire et padding
   Description : Verifier l'alignement des types avec alignof (C11)
   Fichier source : 02-alignement-padding.md
   ============================================================================ */

#include <stdio.h>
#include <stdalign.h>

int main(void) {
    printf("Alignement de char   : %zu\n", alignof(char));
    printf("Alignement de short  : %zu\n", alignof(short));
    printf("Alignement de int    : %zu\n", alignof(int));
    printf("Alignement de long   : %zu\n", alignof(long));
    printf("Alignement de double : %zu\n", alignof(double));
    printf("Alignement de void*  : %zu\n", alignof(void*));

    return 0;
}
