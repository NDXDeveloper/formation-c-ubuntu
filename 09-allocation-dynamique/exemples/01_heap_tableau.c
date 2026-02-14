/* ============================================================================
   Section 9.1 : Stack vs Heap
   Description : Creation d'un tableau sur le Heap qui survit a la fonction
   Fichier source : 01-stack-vs-heap.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

int* creer_tableau(int taille) {
    int* tableau = malloc(taille * sizeof(int));

    if (tableau == NULL) {
        printf("Erreur d'allocation memoire\n");
        return NULL;
    }

    for (int i = 0; i < taille; i++) {
        tableau[i] = i * 10;
    }

    return tableau;
}

int main(void) {
    int taille = 100;

    int* mon_tableau = creer_tableau(taille);

    if (mon_tableau != NULL) {
        printf("Premiere valeur : %d\n", mon_tableau[0]);

        free(mon_tableau);
    }

    return 0;
}
