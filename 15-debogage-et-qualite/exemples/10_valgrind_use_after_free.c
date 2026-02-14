/* ============================================================================
   Section 04.1 : Memcheck
   Description : Use-after-free detecte par Valgrind
   Fichier source : 04.1-memcheck.md
   ============================================================================ */

#include <stdlib.h>
#include <stdio.h>

int main(void) {
    int *data = malloc(sizeof(int));
    *data = 42;

    printf("Avant free: %d\n", *data);

    free(data);

    /* Utilisation apres free */
    printf("Après free: %d\n", *data);

    return 0;
}
