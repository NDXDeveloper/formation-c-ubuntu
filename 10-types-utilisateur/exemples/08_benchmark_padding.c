/* ============================================================================
   Section 10.2 : Alignement memoire et padding
   Description : Impact du padding sur l'utilisation memoire
   Fichier source : 02-alignement-padding.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

#define N 10000000

struct NonOptimise {
    char c1;
    int i1;
    char c2;
    int i2;
};

struct Optimise {
    char c1;
    char c2;
    int i1;
    int i2;
};

int main(void) {
    struct NonOptimise *tab1 = malloc(N * sizeof(struct NonOptimise));
    struct Optimise *tab2 = malloc(N * sizeof(struct Optimise));

    if (tab1 == NULL || tab2 == NULL) {
        free(tab1);
        free(tab2);
        return 1;
    }

    printf("NonOptimise : %zu octets x %d = %.2f Mo\n",
           sizeof(struct NonOptimise), N,
           (sizeof(struct NonOptimise) * N) / (1024.0 * 1024.0));

    printf("Optimise    : %zu octets x %d = %.2f Mo\n",
           sizeof(struct Optimise), N,
           (sizeof(struct Optimise) * N) / (1024.0 * 1024.0));

    free(tab1);
    free(tab2);

    return 0;
}
