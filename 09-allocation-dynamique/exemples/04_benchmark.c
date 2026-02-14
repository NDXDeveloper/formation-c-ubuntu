/* ============================================================================
   Section 9.2 : Duree de vie et performance
   Description : Benchmark comparatif Stack vs Heap (1 million d'allocations)
   Fichier source : 02-duree-vie-performance.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITERATIONS 1000000

void benchmark_stack(void) {
    clock_t start = clock();

    for (int i = 0; i < ITERATIONS; i++) {
        int x = 42;
        (void)x;
    }

    clock_t end = clock();
    double temps = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Stack : %f secondes pour %d allocations\n", temps, ITERATIONS);
}

void benchmark_heap(void) {
    clock_t start = clock();

    for (int i = 0; i < ITERATIONS; i++) {
        int* x = malloc(sizeof(int));
        if (x != NULL) {
            *x = 42;
            free(x);
        }
    }

    clock_t end = clock();
    double temps = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Heap  : %f secondes pour %d allocations\n", temps, ITERATIONS);
}

int main(void) {
    printf("=== Benchmark Stack vs Heap ===\n");
    benchmark_stack();
    benchmark_heap();
    return 0;
}
