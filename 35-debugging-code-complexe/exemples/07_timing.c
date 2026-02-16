/* ============================================================================
   Section 35.5 : Performance debugging
   Description : Mesure de temps d'execution avec clock_gettime (CLOCK_MONOTONIC)
   Fichier source : 05-performance-debugging.md
   ============================================================================ */
#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include <stdio.h>

double get_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main(void) {
    double start = get_time();

    /* Code a mesurer */
    volatile long sum = 0;
    for (int i = 0; i < 1000000; i++) {
        sum += i;
    }

    double end = get_time();
    printf("Elapsed: %.6f seconds\n", end - start);
    printf("Sum: %ld\n", sum);

    return 0;
}
