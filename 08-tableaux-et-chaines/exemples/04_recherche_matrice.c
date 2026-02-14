/* ============================================================================
   Section 8.2 : Tableaux multidimensionnels
   Description : Recherche d'un element dans une matrice
   Fichier source : 02-tableaux-multidimensionnels.md
   ============================================================================ */

#include <stdio.h>
#include <stdbool.h>

bool rechercher(int mat[][4], int lignes, int colonnes, int valeur,
                int *ligne_trouvee, int *col_trouvee) {
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            if (mat[i][j] == valeur) {
                *ligne_trouvee = i;
                *col_trouvee = j;
                return true;
            }
        }
    }
    return false;
}

int main(void) {
    int matrice[3][4] = {
        {1,  2,  3,  4},
        {5,  6,  7,  8},
        {9, 10, 11, 12}
    };

    int ligne, colonne;
    int cherche = 7;

    if (rechercher(matrice, 3, 4, cherche, &ligne, &colonne)) {
        printf("%d trouve a la position [%d][%d]\n", cherche, ligne, colonne);
    } else {
        printf("%d non trouve\n", cherche);
    }

    return 0;
}
