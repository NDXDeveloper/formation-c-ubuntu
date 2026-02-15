/* ============================================================================
   Section 27.7 : Vectorisation et SIMD (auto-vectorisation)
   Description : Addition de tableaux de floats - comparaison -O2 vs -O3
                 pour demontrer la vectorisation automatique du compilateur
   Fichier source : 07-vectorisation-simd.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 10000000  /* 10 millions */
#define ITERATIONS 10

void add_arrays(float *a, float *b, float *c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

int main(void) {
    float *a = malloc(SIZE * sizeof(float));
    float *b = malloc(SIZE * sizeof(float));
    float *c = malloc(SIZE * sizeof(float));
    if (!a || !b || !c) {
        fprintf(stderr, "Erreur allocation\n");
        return 1;
    }

    /* Initialiser */
    for (int i = 0; i < SIZE; i++) {
        a[i] = (float)i;
        b[i] = (float)i * 2.0f;
    }

    clock_t start = clock();
    for (int k = 0; k < ITERATIONS; k++) {
        add_arrays(a, b, c, SIZE);
    }
    clock_t end = clock();

    printf("Addition de %d floats (%d iterations)\n", SIZE, ITERATIONS);
    printf("Temps moyen : %.1f ms\n",
           (double)(end - start) * 1000.0 / CLOCKS_PER_SEC / ITERATIONS);
    printf("Verification : c[42] = %.1f (attendu: %.1f)\n",
           (double)c[42], 42.0 + 42.0 * 2.0);

    free(a);
    free(b);
    free(c);
    return 0;
}
