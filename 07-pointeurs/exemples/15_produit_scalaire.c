/* ============================================================================
   Section 7.7 : Pointeurs restreints (restrict)
   Description : Produit scalaire avec restrict
   Fichier source : 07-pointeurs-restrict.md
   ============================================================================ */

#include <stdio.h>

double produit_scalaire(const double *restrict a,
                        const double *restrict b,
                        size_t n) {
    double somme = 0.0;
    for (size_t i = 0; i < n; i++) {
        somme += a[i] * b[i];
    }
    return somme;
}

int main(void) {
    double v1[] = {1.0, 2.0, 3.0, 4.0};
    double v2[] = {5.0, 6.0, 7.0, 8.0};

    double resultat = produit_scalaire(v1, v2, 4);
    printf("Produit scalaire : %.1f\n", resultat);

    return 0;
}
