/* ============================================================================
   Section 30.1.6 : Analyse Valgrind automatisee
   Description : Bug intentionnel - fuite memoire (pour Valgrind Memcheck)
   Fichier source : 01.6-analyse-valgrind.md
   ============================================================================ */
#include <stdlib.h>
#include <stdio.h>

void allocate_memory(void) {
    int *ptr = malloc(100 * sizeof(int));
    /* Oups ! Pas de free() */
    printf("Allocated 400 bytes\n");
    (void)ptr;
}

int main(void) {
    for (int i = 0; i < 5; i++) {
        allocate_memory();
    }
    printf("Done\n");
    return 0;
}
