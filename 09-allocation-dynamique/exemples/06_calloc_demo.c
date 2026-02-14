/* ============================================================================
   Section 9.3 : Les fonctions malloc, calloc, realloc et free
   Description : Allocation avec calloc (initialisation automatique a zero)
   Fichier source : 03-fonctions-allocation.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int taille = 5;

    int* tableau = calloc(taille, sizeof(int));

    if (tableau == NULL) {
        printf("Erreur d'allocation\n");
        return 1;
    }

    for (int i = 0; i < taille; i++) {
        printf("tableau[%d] = %d\n", i, tableau[i]);
    }

    free(tableau);
    return 0;
}
