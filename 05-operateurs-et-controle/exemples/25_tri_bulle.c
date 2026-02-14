/* ============================================================================
   Section 5.5 : Boucles
   Description : Tri a bulle (bubble sort)
   Fichier source : 05-boucles.md
   ============================================================================ */
#include <stdio.h>

void tri_bulle(int *tableau, int taille) {
    for (int i = 0; i < taille - 1; i++) {
        for (int j = 0; j < taille - i - 1; j++) {
            if (tableau[j] > tableau[j + 1]) {
                int temp = tableau[j];
                tableau[j] = tableau[j + 1];
                tableau[j + 1] = temp;
            }
        }
    }
}

void afficher_tableau(int *tableau, int taille) {
    for (int i = 0; i < taille; i++) {
        printf("%d ", tableau[i]);
    }
    printf("\n");
}

int main(void) {
    int nombres[] = {64, 34, 25, 12, 22, 11, 90};
    int taille = (int)(sizeof(nombres) / sizeof(nombres[0]));

    printf("Avant tri : ");
    afficher_tableau(nombres, taille);

    tri_bulle(nombres, taille);

    printf("Après tri : ");
    afficher_tableau(nombres, taille);

    return 0;
}
