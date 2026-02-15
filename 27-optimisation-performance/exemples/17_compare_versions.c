/* ============================================================================
   Section 27.10 : Benchmarking (comparaison de versions)
   Description : Comparaison boucle O(n) vs formule O(1) pour la somme de 0..n
                 avec mediane sur 20 runs et calcul du speedup
   Fichier source : 10-benchmarking.md
   ============================================================================ */

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITERATIONS 100000
#define RUNS 10

int compare_double(const void *a, const void *b) {
    double diff = *(const double *)a - *(const double *)b;
    return (diff > 0) - (diff < 0);
}

/* Version 1 : Boucle O(n) */
int version_baseline(int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += i;
    }
    return sum;
}

/* Version 2 : Formule O(1) */
int version_optimisee(int n) {
    return n * (n - 1) / 2;
}

double benchmark_function(int (*func)(int), int n) {
    struct timespec start, end;
    double times[RUNS];

    /* Warm-up */
    for (int i = 0; i < 3; i++) {
        volatile int r = func(n);
        (void)r;
    }

    /* Mesures */
    for (int run = 0; run < RUNS; run++) {
        clock_gettime(CLOCK_MONOTONIC, &start);

        for (int i = 0; i < ITERATIONS; i++) {
            volatile int result = func(n);
            (void)result;
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        times[run] = (double)(end.tv_sec - start.tv_sec) +
                     (double)(end.tv_nsec - start.tv_nsec) / 1e9;
    }

    /* Mediane (plus robuste que la moyenne) */
    qsort(times, RUNS, sizeof(double), compare_double);
    return times[RUNS / 2];
}

int main(void) {
    int n = 10000;

    printf("=== Comparaison de performances ===\n");
    printf("Taille : %d, Iterations : %d, Runs : %d\n\n", n, ITERATIONS, RUNS);

    double time_baseline = benchmark_function(version_baseline, n);
    double time_optimisee = benchmark_function(version_optimisee, n);

    printf("Baseline  (boucle)  : %.3f ms\n", time_baseline * 1000.0);
    printf("Optimisee (formule) : %.3f ms\n", time_optimisee * 1000.0);
    printf("\n");

    double speedup = time_baseline / time_optimisee;
    double improvement = ((time_baseline - time_optimisee) / time_baseline)
                         * 100.0;

    printf("Speedup      : %.2fx\n", speedup);
    printf("Amelioration : %.1f%%\n", improvement);

    return 0;
}
