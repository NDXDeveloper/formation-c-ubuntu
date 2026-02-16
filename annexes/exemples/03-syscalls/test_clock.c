// Ligne 1004-1024 : clock_gettime benchmark
#include <time.h>
#include <stdio.h>

int main() {
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    // Code à mesurer
    for (int i = 0; i < 1000000; i++);

    clock_gettime(CLOCK_MONOTONIC, &end);

    long seconds = end.tv_sec - start.tv_sec;
    long nanoseconds = end.tv_nsec - start.tv_nsec;
    double elapsed = seconds + nanoseconds * 1e-9;

    printf("Temps: %.6f secondes\n", elapsed);
    return 0;
}
