/* ============================================================================
   Section 7.7 : Pointeurs restreints (restrict)
   Description : Multiplication de matrices avec restrict
   Fichier source : 07-pointeurs-restrict.md
   ============================================================================ */

#include <stdio.h>

void multiplier_matrices(double *restrict C,
                         const double *restrict A,
                         const double *restrict B,
                         int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double somme = 0.0;
            for (int k = 0; k < n; k++) {
                somme += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = somme;
        }
    }
}

int main(void) {
    double A[4] = {1, 2, 3, 4};
    double B[4] = {5, 6, 7, 8};
    double C[4] = {0};

    multiplier_matrices(C, A, B, 2);

    printf("Resultat C :\n");
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            printf("%.0f ", C[i * 2 + j]);
        }
        printf("\n");
    }

    return 0;
}
