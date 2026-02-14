/* ============================================================================
   Section 8.2 : Tableaux multidimensionnels
   Description : Creation et affichage d'une matrice d'identite 5x5
   Fichier source : 02-tableaux-multidimensionnels.md
   ============================================================================ */

#include <stdio.h>

void creer_identite(int mat[][5], int taille) {
    for (int i = 0; i < taille; i++) {
        for (int j = 0; j < taille; j++) {
            mat[i][j] = (i == j) ? 1 : 0;
        }
    }
}

void afficher_matrice(int mat[][5], int taille) {
    for (int i = 0; i < taille; i++) {
        for (int j = 0; j < taille; j++) {
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
}

int main(void) {
    int identite[5][5];

    creer_identite(identite, 5);

    printf("Matrice d'identite 5x5 :\n");
    afficher_matrice(identite, 5);

    return 0;
}
