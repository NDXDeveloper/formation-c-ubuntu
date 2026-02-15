/* ============================================================================
   Section 18.12 : Thread-local storage
   Description : Benchmark comparatif global+mutex vs _Thread_local vs pthread_key_t
   Fichier source : 12-thread-local-storage.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define ITERATIONS 10000000

int global_var = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

_Thread_local int tls_var = 0;

pthread_key_t key;

void benchmark_global(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);
        global_var++;
        pthread_mutex_unlock(&mutex);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (double)(end.tv_sec - start.tv_sec) +
                     (double)(end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Global (mutex) : %.3f s\n", elapsed);
}

void benchmark_tls(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < ITERATIONS; i++) {
        tls_var++;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (double)(end.tv_sec - start.tv_sec) +
                     (double)(end.tv_nsec - start.tv_nsec) / 1e9;
    printf("TLS            : %.3f s\n", elapsed);
}

void benchmark_key(void) {
    int *val = malloc(sizeof(int));
    *val = 0;
    pthread_setspecific(key, val);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < ITERATIONS; i++) {
        int *v = pthread_getspecific(key);
        (*v)++;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (double)(end.tv_sec - start.tv_sec) +
                     (double)(end.tv_nsec - start.tv_nsec) / 1e9;
    printf("pthread_key    : %.3f s\n", elapsed);

    free(val);
}

int main(void) {
    pthread_key_create(&key, free);

    printf("Benchmark (%d iterations) :\n", ITERATIONS);
    benchmark_global();
    benchmark_tls();
    benchmark_key();

    pthread_key_delete(key);
    return 0;
}
