/* ============================================================================
   Section 22.4 : Pointeurs opaques
   Description : Programme client utilisant la pile (stack) opaque
   Fichier source : 04-pointeurs-opaques.md
   ============================================================================ */

#include "stack.h"
#include <stdio.h>

int main(void) {
    Stack *ma_pile = stack_create();
    if (ma_pile == NULL) {
        fprintf(stderr, "Erreur de creation\n");
        return 1;
    }

    // Ajout d'elements
    stack_push(ma_pile, 10);
    stack_push(ma_pile, 20);
    stack_push(ma_pile, 30);

    printf("Taille : %d\n", stack_size(ma_pile));

    // Consultation du sommet
    int valeur;
    if (stack_peek(ma_pile, &valeur)) {
        printf("Sommet : %d\n", valeur);
    }

    // Retrait d'elements
    while (!stack_is_empty(ma_pile)) {
        if (stack_pop(ma_pile, &valeur)) {
            printf("Depile : %d\n", valeur);
        }
    }

    stack_destroy(ma_pile);
    return 0;
}
