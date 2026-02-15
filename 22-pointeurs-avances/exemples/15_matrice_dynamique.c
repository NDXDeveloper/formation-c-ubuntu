/* ============================================================================
   Section 22.3 : Pointeurs multi-niveaux
   Description : Allocation et manipulation d'une matrice 2D dynamique
   Fichier source : 03-pointeurs-multi-niveaux.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

// Allocation d'une matrice 2D dynamique
int** creer_matrice(int lignes, int colonnes) {
    // Allocation du tableau de pointeurs (les lignes)
    int **matrice = malloc((size_t)lignes * sizeof(int*));
    if (matrice == NULL) {
        return NULL;
    }

    // Allocation de chaque ligne
    for (int i = 0; i < lignes; i++) {
        matrice[i] = malloc((size_t)colonnes * sizeof(int));
        if (matrice[i] == NULL) {
            // En cas d'erreur, liberer ce qui a ete alloue
            for (int j = 0; j < i; j++) {
                free(matrice[j]);
            }
            free(matrice);
            return NULL;
        }
    }

    return matrice;
}

// Liberation de la matrice
void liberer_matrice(int **matrice, int lignes) {
    for (int i = 0; i < lignes; i++) {
        free(matrice[i]);  // Liberer chaque ligne
    }
    free(matrice);  // Liberer le tableau de pointeurs
}

// Initialisation de la matrice
void initialiser_matrice(int **matrice, int lignes, int colonnes) {
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            matrice[i][j] = i * colonnes + j;
        }
    }
}

// Affichage de la matrice
void afficher_matrice(int **matrice, int lignes, int colonnes) {
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            printf("%3d ", matrice[i][j]);
        }
        printf("\n");
    }
}

int main(void) {
    int lignes = 3, colonnes = 4;

    // Creation de la matrice
    int **matrice = creer_matrice(lignes, colonnes);
    if (matrice == NULL) {
        fprintf(stderr, "Erreur d'allocation\n");
        return 1;
    }

    // Utilisation
    initialiser_matrice(matrice, lignes, colonnes);
    printf("Matrice %dx%d :\n", lignes, colonnes);
    afficher_matrice(matrice, lignes, colonnes);

    // Liberation
    liberer_matrice(matrice, lignes);

    return 0;
}
