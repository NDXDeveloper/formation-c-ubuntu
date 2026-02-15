/* ============================================================================
   Section 27.10 : Benchmarking
   Description : Template de benchmark robuste avec warm-up, statistiques
                 (moyenne, ecart-type, min, max) et barriere anti-optimisation
                 (__asm__ volatile)
   Fichier source : 10-benchmarking.md
   ============================================================================ */

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#define ITERATIONS 1000000
#define WARMUP_RUNS 3
#define BENCHMARK_RUNS 10

/* Fonction a benchmarker */
int fonction_a_tester(int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += i;
    }
    return sum;
}

/* Empecher l'optimisation */
void escape(volatile void *p) {
    __asm__ __volatile__("" : : "g"(p) : "memory");
}

void clobber(void) {
    __asm__ __volatile__("" : : : "memory");
}

int main(void) {
    struct timespec start, end;
    double times[BENCHMARK_RUNS];

    /* Warm-up */
    for (int i = 0; i < WARMUP_RUNS; i++) {
        volatile int result = fonction_a_tester(1000);
        escape(&result);
    }

    /* Benchmarks */
    for (int run = 0; run < BENCHMARK_RUNS; run++) {
        clock_gettime(CLOCK_MONOTONIC, &start);

        for (int i = 0; i < ITERATIONS; i++) {
            volatile int result = fonction_a_tester(1000);
            escape(&result);
            clobber();
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        double elapsed = (double)(end.tv_sec - start.tv_sec) +
                         (double)(end.tv_nsec - start.tv_nsec) / 1e9;
        times[run] = elapsed / ITERATIONS;
    }

    /* Analyse statistique */
    double sum = 0.0, min = times[0], max = times[0];
    for (int i = 0; i < BENCHMARK_RUNS; i++) {
        sum += times[i];
        if (times[i] < min) min = times[i];
        if (times[i] > max) max = times[i];
    }
    double mean = sum / BENCHMARK_RUNS;

    /* Ecart-type */
    double variance = 0.0;
    for (int i = 0; i < BENCHMARK_RUNS; i++) {
        variance += (times[i] - mean) * (times[i] - mean);
    }
    double stddev = sqrt(variance / BENCHMARK_RUNS);

    printf("=== Resultats du benchmark ===\n");
    printf("Iterations : %d\n", ITERATIONS);
    printf("Runs       : %d\n", BENCHMARK_RUNS);
    printf("\n");
    printf("Temps moyen : %.3f ns\n", mean * 1e9);
    printf("Ecart-type  : %.3f ns (%.1f%%)\n",
           stddev * 1e9, (stddev / mean) * 100.0);
    printf("Min         : %.3f ns\n", min * 1e9);
    printf("Max         : %.3f ns\n", max * 1e9);

    return 0;
}
