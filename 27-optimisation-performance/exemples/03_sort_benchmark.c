/* ============================================================================
   Section 27.3 : Profiling (workflow complet)
   Description : Benchmark de tri - bubble sort O(n^2) volontairement lent
                 suivi de qsort O(n log n) pour montrer l'optimisation
                 algorithmique guidee par le profiling
   Fichier source : 03-profiling.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAILLE 50000

void bubble_sort(int *tab, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (tab[j] > tab[j + 1]) {
                int temp = tab[j];
                tab[j] = tab[j + 1];
                tab[j + 1] = temp;
            }
        }
    }
}

int compare_int(const void *a, const void *b) {
    return (*(const int *)a - *(const int *)b);
}

int main(void) {
    int *tableau = malloc(TAILLE * sizeof(int));
    int *copie = malloc(TAILLE * sizeof(int));
    if (!tableau || !copie) {
        fprintf(stderr, "Erreur allocation\n");
        return 1;
    }

    /* Remplir avec valeurs aleatoires */
    srand((unsigned)time(NULL));
    for (int i = 0; i < TAILLE; i++) {
        tableau[i] = rand();
        copie[i] = tableau[i];
    }

    /* Mesurer bubble sort */
    clock_t debut = clock();
    bubble_sort(tableau, TAILLE);
    clock_t fin = clock();
    double temps_bubble = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("Bubble sort (%d elements) : %.3f s\n", TAILLE, temps_bubble);

    /* Mesurer qsort */
    debut = clock();
    qsort(copie, TAILLE, sizeof(int), compare_int);
    fin = clock();
    double temps_qsort = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("qsort       (%d elements) : %.3f s\n", TAILLE, temps_qsort);

    if (temps_qsort > 0.0) {
        printf("Ratio : %.0fx plus rapide avec qsort\n",
               temps_bubble / temps_qsort);
    }

    free(tableau);
    free(copie);
    return 0;
}
