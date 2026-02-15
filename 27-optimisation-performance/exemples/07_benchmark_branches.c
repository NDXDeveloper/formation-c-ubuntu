/* ============================================================================
   Section 27.5 : Branch prediction
   Description : Benchmark branches previsibles (pattern regulier i%%2) vs
                 imprevisibles (pattern pseudo-aleatoire) - mesure du cout
                 des mauvaises predictions
   Fichier source : 05-branch-prediction.md
   ============================================================================ */

#include <stdio.h>
#include <time.h>

#define ITERATIONS 100000000

void test_predictable(void) {
    volatile int somme = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        if (i % 2 == 0) {  /* Pattern regulier */
            somme += i;
        }
    }
    printf("  Somme (previsible)   : %d\n", somme);
}

void test_unpredictable(void) {
    volatile int somme = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        if (((unsigned)i * 1103515245u + 12345u) & 1u) {  /* Pseudo-aleatoire */
            somme += i;
        }
    }
    printf("  Somme (imprevisible) : %d\n", somme);
}

int main(void) {
    clock_t debut, fin;

    printf("=== Branches previsibles ===\n");
    debut = clock();
    test_predictable();
    fin = clock();
    double temps_pred = (double)(fin - debut) * 1000.0 / CLOCKS_PER_SEC;
    printf("  Temps : %.1f ms\n\n", temps_pred);

    printf("=== Branches imprevisibles ===\n");
    debut = clock();
    test_unpredictable();
    fin = clock();
    double temps_unpred = (double)(fin - debut) * 1000.0 / CLOCKS_PER_SEC;
    printf("  Temps : %.1f ms\n\n", temps_unpred);

    if (temps_pred > 0.0) {
        printf("Ratio : imprevisible %.1fx plus lent\n",
               temps_unpred / temps_pred);
    }

    return 0;
}
