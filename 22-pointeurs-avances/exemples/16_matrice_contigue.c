/* ============================================================================
   Section 22.3 : Pointeurs multi-niveaux
   Description : Allocation contigue d'une matrice 2D (cache-friendly)
   Fichier source : 03-pointeurs-multi-niveaux.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

int** creer_matrice_contigue(int lignes, int colonnes) {
    // Allocation du tableau de pointeurs
    int **matrice = malloc((size_t)lignes * sizeof(int*));
    if (matrice == NULL) {
        return NULL;
    }

    // Allocation de toutes les donnees en un seul bloc
    int *data = malloc((size_t)lignes * (size_t)colonnes * sizeof(int));
    if (data == NULL) {
        free(matrice);
        return NULL;
    }

    // Configuration des pointeurs de lignes
    for (int i = 0; i < lignes; i++) {
        matrice[i] = data + i * colonnes;
    }

    return matrice;
}

void liberer_matrice_contigue(int **matrice) {
    if (matrice != NULL) {
        free(matrice[0]);  // Libere toutes les donnees
        free(matrice);     // Libere le tableau de pointeurs
    }
}

int main(void) {
    int lignes = 3, colonnes = 4;
    int **mat = creer_matrice_contigue(lignes, colonnes);

    if (mat == NULL) {
        fprintf(stderr, "Erreur d'allocation\n");
        return 1;
    }

    // Remplissage et affichage
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            mat[i][j] = i * colonnes + j;
        }
    }

    printf("Matrice contigue %dx%d :\n", lignes, colonnes);
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            printf("%3d ", mat[i][j]);
        }
        printf("\n");
    }

    liberer_matrice_contigue(mat);
    return 0;
}
