/* ============================================================================
   Section 8.2 : Tableaux multidimensionnels
   Description : Transposition d'une matrice (3x4 vers 4x3)
   Fichier source : 02-tableaux-multidimensionnels.md
   ============================================================================ */

#include <stdio.h>

void transposer(int mat[][4], int resultat[][3], int lignes, int colonnes) {
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            resultat[j][i] = mat[i][j];
        }
    }
}

void afficher_matrice_3x4(int mat[][4], int lignes) {
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%3d ", mat[i][j]);
        }
        printf("\n");
    }
}

void afficher_matrice_4x3(int mat[][3], int lignes) {
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%3d ", mat[i][j]);
        }
        printf("\n");
    }
}

int main(void) {
    int matrice[3][4] = {
        {1,  2,  3,  4},
        {5,  6,  7,  8},
        {9, 10, 11, 12}
    };

    int transposee[4][3];

    printf("Matrice originale (3x4) :\n");
    afficher_matrice_3x4(matrice, 3);

    transposer(matrice, transposee, 3, 4);

    printf("\nMatrice transposee (4x3) :\n");
    afficher_matrice_4x3(transposee, 4);

    return 0;
}
