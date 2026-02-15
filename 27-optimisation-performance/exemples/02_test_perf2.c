/* ============================================================================
   Section 27.3 : Profiling (perf)
   Description : Programme de test pour profiling avec perf - calcul intensif
                 et allocations memoire pour identifier les hotspots
   Fichier source : 03-profiling.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

/* Fonction avec beaucoup de calculs */
double calcul_intensif(int n) {
    double resultat = 0.0;
    for (int i = 0; i < n; i++) {
        resultat += (double)i * (double)i / ((double)i + 1.0);
    }
    return resultat;
}

/* Fonction avec allocations memoire */
void allocation_intensive(int n) {
    for (int i = 0; i < n; i++) {
        int *tableau = malloc(1000 * sizeof(int));
        if (!tableau) break;
        for (int j = 0; j < 1000; j++) {
            tableau[j] = j;
        }
        free(tableau);
    }
}

int main(void) {
    printf("Debut du profiling\n");

    /* ~80%% du temps ici */
    double total = 0.0;
    for (int i = 0; i < 1000; i++) {
        total += calcul_intensif(100000);
    }

    /* ~20%% du temps ici */
    allocation_intensive(10000);

    printf("Total calcul : %g\n", total);
    printf("Fin du profiling\n");
    return 0;
}
