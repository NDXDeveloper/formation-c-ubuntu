/* ============================================================================
   Section 27.9 : Profile-Guided Optimization (PGO)
   Description : Benchmark qsort avec distribution non uniforme (70%% petites
                 valeurs, 30%% grandes) - compiler avec PGO pour optimiser
                 les branches dans la fonction de comparaison
   Fichier source : 09-pgo.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1000000

int compare_int(const void *a, const void *b) {
    return (*(const int *)a - *(const int *)b);
}

int main(void) {
    int *data = malloc(SIZE * sizeof(int));
    if (!data) {
        fprintf(stderr, "Erreur allocation\n");
        return 1;
    }

    /* Generer des donnees selon un pattern reel */
    srand(42);
    for (int i = 0; i < SIZE; i++) {
        /* 70%% des valeurs entre 0-1000 */
        /* 30%% des valeurs entre 1000-100000 */
        if (rand() % 100 < 70) {
            data[i] = rand() % 1000;
        } else {
            data[i] = rand() % 100000;
        }
    }

    clock_t start = clock();
    qsort(data, SIZE, sizeof(int), compare_int);
    clock_t end = clock();

    printf("Temps de tri : %.1f ms\n",
           (double)(end - start) * 1000.0 / CLOCKS_PER_SEC);

    free(data);
    return 0;
}
