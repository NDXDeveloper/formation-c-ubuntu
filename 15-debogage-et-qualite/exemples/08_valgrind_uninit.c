/* ============================================================================
   Section 04.1 : Memcheck
   Description : Valeur non initialisee detectee par Valgrind
   Fichier source : 04.1-memcheck.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *values = malloc(5 * sizeof(int));

    /* On initialise seulement les 3 premiers */
    for (int i = 0; i < 3; i++) {
        values[i] = i * 10;
    }

    /* On lit le 4eme (non initialise) */
    if (values[3] > 20) {  /* Valeur non initialisee */
        printf("Grande valeur\n");
    }

    free(values);
    return 0;
}
