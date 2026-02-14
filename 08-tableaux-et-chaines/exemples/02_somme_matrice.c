/* ============================================================================
   Section 8.2 : Tableaux multidimensionnels
   Description : Somme des elements d'une matrice
   Fichier source : 02-tableaux-multidimensionnels.md
   ============================================================================ */

#include <stdio.h>

int somme_matrice(int mat[][4], int lignes) {
    int somme = 0;
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < 4; j++) {
            somme += mat[i][j];
        }
    }
    return somme;
}

int main(void) {
    int matrice[3][4] = {
        {1,  2,  3,  4},
        {5,  6,  7,  8},
        {9, 10, 11, 12}
    };

    int total = somme_matrice(matrice, 3);
    printf("Somme totale : %d\n", total);

    return 0;
}
