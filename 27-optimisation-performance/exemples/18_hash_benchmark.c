/* ============================================================================
   Section 27.10 : Benchmarking (exemple complet)
   Description : Benchmark de fonctions de hachage djb2 vs FNV-1a sur
                 1 million de chaines - mesure du temps par hash en ns
   Fichier source : 10-benchmarking.md
   ============================================================================ */

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#define DATA_SIZE 1000000
#define RUNS 20

/* Hash function 1 : djb2 */
uint32_t hash_djb2(const char *str) {
    uint32_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + (uint32_t)c;
    }
    return hash;
}

/* Hash function 2 : FNV-1a */
uint32_t hash_fnv1a(const char *str) {
    uint32_t hash = 2166136261u;
    while (*str) {
        hash ^= (uint8_t)(*str++);
        hash *= 16777619u;
    }
    return hash;
}

double benchmark_hash(uint32_t (*hash_func)(const char *),
                      char **data, int count) {
    struct timespec start, end;

    /* Warm-up */
    for (int i = 0; i < count / 10; i++) {
        volatile uint32_t h = hash_func(data[i % count]);
        (void)h;
    }

    /* Mesure */
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < count; i++) {
        volatile uint32_t h = hash_func(data[i]);
        (void)h;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    return (double)(end.tv_sec - start.tv_sec) +
           (double)(end.tv_nsec - start.tv_nsec) / 1e9;
}

int main(void) {
    /* Generer des donnees de test */
    char **data = malloc(DATA_SIZE * sizeof(char *));
    if (!data) {
        fprintf(stderr, "Erreur allocation\n");
        return 1;
    }
    for (int i = 0; i < DATA_SIZE; i++) {
        data[i] = malloc(20);
        if (!data[i]) {
            fprintf(stderr, "Erreur allocation\n");
            return 1;
        }
        snprintf(data[i], 20, "string_%d", i);
    }

    printf("=== Benchmark : Hash Functions ===\n");
    printf("Donnees : %d chaines\n", DATA_SIZE);
    printf("Runs    : %d\n\n", RUNS);

    double times_djb2[RUNS], times_fnv1a[RUNS];

    /* Benchmarks */
    for (int run = 0; run < RUNS; run++) {
        times_djb2[run] = benchmark_hash(hash_djb2, data, DATA_SIZE);
        times_fnv1a[run] = benchmark_hash(hash_fnv1a, data, DATA_SIZE);
    }

    /* Analyse */
    double avg_djb2 = 0.0, avg_fnv1a = 0.0;
    for (int i = 0; i < RUNS; i++) {
        avg_djb2 += times_djb2[i];
        avg_fnv1a += times_fnv1a[i];
    }
    avg_djb2 /= RUNS;
    avg_fnv1a /= RUNS;

    printf("djb2   : %.3f ms (%.2f ns/hash)\n",
           avg_djb2 * 1000.0, avg_djb2 / DATA_SIZE * 1e9);
    printf("FNV-1a : %.3f ms (%.2f ns/hash)\n",
           avg_fnv1a * 1000.0, avg_fnv1a / DATA_SIZE * 1e9);
    printf("\n");

    if (avg_djb2 < avg_fnv1a) {
        printf("djb2 est %.2fx plus rapide\n", avg_fnv1a / avg_djb2);
    } else {
        printf("FNV-1a est %.2fx plus rapide\n", avg_djb2 / avg_fnv1a);
    }

    /* Cleanup */
    for (int i = 0; i < DATA_SIZE; i++) {
        free(data[i]);
    }
    free(data);

    return 0;
}
