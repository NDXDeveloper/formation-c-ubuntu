/* ============================================================================
   Section 35.5 : Performance debugging
   Description : Micro-benchmark avec RDTSC (compteur de cycles CPU x86)
   Fichier source : 05-performance-debugging.md
   ============================================================================ */
#include <x86intrin.h>
#include <stdio.h>
#include <stdint.h>

static inline uint64_t rdtsc(void) {
    return __rdtsc();
}

int main(void) {
    uint64_t start = rdtsc();

    /* Code a mesurer (tres court) */
    volatile int sum = 0;
    for (int i = 0; i < 1000; i++) {
        sum += i;
    }

    uint64_t end = rdtsc();
    printf("Cycles: %lu\n", end - start);
    printf("Sum: %d\n", sum);

    return 0;
}
