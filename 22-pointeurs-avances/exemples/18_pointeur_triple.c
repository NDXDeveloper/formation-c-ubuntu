/* ============================================================================
   Section 22.3 : Pointeurs multi-niveaux
   Description : Pointeur triple et tableau 3D dynamique (cube)
   Fichier source : 03-pointeurs-multi-niveaux.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

// Allocation d'un cube 3D
int*** creer_cube(int profondeur, int lignes, int colonnes) {
    int ***cube = malloc((size_t)profondeur * sizeof(int**));
    if (cube == NULL) return NULL;

    for (int i = 0; i < profondeur; i++) {
        cube[i] = malloc((size_t)lignes * sizeof(int*));
        if (cube[i] == NULL) {
            for (int ii = 0; ii < i; ii++) {
                for (int jj = 0; jj < lignes; jj++) {
                    free(cube[ii][jj]);
                }
                free(cube[ii]);
            }
            free(cube);
            return NULL;
        }

        for (int j = 0; j < lignes; j++) {
            cube[i][j] = malloc((size_t)colonnes * sizeof(int));
            if (cube[i][j] == NULL) {
                for (int jj = 0; jj < j; jj++) {
                    free(cube[i][jj]);
                }
                free(cube[i]);
                for (int ii = 0; ii < i; ii++) {
                    for (int jj = 0; jj < lignes; jj++) {
                        free(cube[ii][jj]);
                    }
                    free(cube[ii]);
                }
                free(cube);
                return NULL;
            }
        }
    }

    return cube;
}

void liberer_cube(int ***cube, int profondeur, int lignes) {
    for (int i = 0; i < profondeur; i++) {
        for (int j = 0; j < lignes; j++) {
            free(cube[i][j]);
        }
        free(cube[i]);
    }
    free(cube);
}

int main(void) {
    // Demonstration pointeur triple simple
    int valeur = 42;
    int *p1 = &valeur;
    int **p2 = &p1;
    int ***p3 = &p2;

    printf("Valeur via p1 : %d\n", *p1);
    printf("Valeur via p2 : %d\n", **p2);
    printf("Valeur via p3 : %d\n", ***p3);

    // Modification via p3
    ***p3 = 100;
    printf("Nouvelle valeur : %d\n", valeur);

    // Cube 3D dynamique
    printf("\n--- Cube 3D dynamique ---\n");
    int ***cube = creer_cube(2, 3, 4);  // 2x3x4

    if (cube != NULL) {
        // Remplissage
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 4; k++) {
                    cube[i][j][k] = i * 12 + j * 4 + k;
                }
            }
        }

        // Affichage
        for (int i = 0; i < 2; i++) {
            printf("Couche %d :\n", i);
            for (int j = 0; j < 3; j++) {
                printf("  ");
                for (int k = 0; k < 4; k++) {
                    printf("%3d ", cube[i][j][k]);
                }
                printf("\n");
            }
        }

        liberer_cube(cube, 2, 3);
    }

    return 0;
}
