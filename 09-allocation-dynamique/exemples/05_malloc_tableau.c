/* ============================================================================
   Section 9.3 : Les fonctions malloc, calloc, realloc et free
   Description : Allocation d'un tableau avec malloc et initialisation manuelle
   Fichier source : 03-fonctions-allocation.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int taille = 10;

    int* tableau = malloc(taille * sizeof(int));

    if (tableau == NULL) {
        printf("Erreur d'allocation\n");
        return 1;
    }

    for (int i = 0; i < taille; i++) {
        tableau[i] = i * 10;
    }

    for (int i = 0; i < taille; i++) {
        printf("tableau[%d] = %d\n", i, tableau[i]);
    }

    free(tableau);

    return 0;
}
