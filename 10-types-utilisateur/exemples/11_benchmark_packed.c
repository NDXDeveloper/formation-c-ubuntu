/* ============================================================================
   Section 10.3 : Packed structures
   Description : Benchmark performance Normal vs Packed
   Fichier source : 03-packed-structures.md
   Compilation : gcc -Wall -Wextra -Werror -std=c17 (sans -pedantic)
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 10000000

struct Normal {
    char c;
    int i;
};

struct __attribute__((packed)) Packed {
    char c;
    int i;
};

int main(void) {
    struct Normal *tab_normal = malloc(N * sizeof(struct Normal));
    struct Packed *tab_packed = malloc(N * sizeof(struct Packed));

    if (tab_normal == NULL || tab_packed == NULL) {
        free(tab_normal);
        free(tab_packed);
        return 1;
    }

    clock_t start, end;
    volatile int sum = 0;

    start = clock();
    for (int i = 0; i < N; i++) {
        sum += tab_normal[i].i;
    }
    end = clock();
    double time_normal = ((double)(end - start)) / CLOCKS_PER_SEC;

    sum = 0;
    start = clock();
    for (int i = 0; i < N; i++) {
        sum += tab_packed[i].i;
    }
    end = clock();
    double time_packed = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("=== BENCHMARK (10 millions d'acces) ===\n\n");
    printf("Normal : %.3f secondes\n", time_normal);
    printf("Packed : %.3f secondes\n", time_packed);
    if (time_normal > 0) {
        printf("Ratio  : %.2fx plus lent\n", time_packed / time_normal);
    }

    printf("\n=== UTILISATION MEMOIRE ===\n\n");
    size_t mem_normal = N * sizeof(struct Normal);
    size_t mem_packed = N * sizeof(struct Packed);

    printf("Normal : %.2f Mo\n", mem_normal / (1024.0 * 1024.0));
    printf("Packed : %.2f Mo\n", mem_packed / (1024.0 * 1024.0));
    printf("Gain   : %.2f Mo (%.1f%%)\n",
           (mem_normal - mem_packed) / (1024.0 * 1024.0),
           100.0 * (mem_normal - mem_packed) / mem_normal);

    free(tab_normal);
    free(tab_packed);

    return 0;
}
