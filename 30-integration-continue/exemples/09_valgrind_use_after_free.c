/* ============================================================================
   Section 30.1.6 : Analyse Valgrind automatisee
   Description : Bug intentionnel - utilisation apres liberation (pour Valgrind)
   Fichier source : 01.6-analyse-valgrind.md
   ============================================================================ */
#include <stdlib.h>
#include <stdio.h>

int main(void) {
    int *ptr = malloc(sizeof(int));
    *ptr = 42;
    printf("Value: %d\n", *ptr);

    free(ptr);

    /* Bug : utilisation apres liberation */
    *ptr = 99;
    printf("New value: %d\n", *ptr);

    return 0;
}
