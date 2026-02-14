/* ============================================================================
   Section 5.5 : Boucles
   Description : Calcul de factorielle avec boucle for
   Fichier source : 05-boucles.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    int n = 5;
    unsigned long long factorielle = 1;

    for (int i = 1; i <= n; i++) {
        factorielle *= (unsigned long long)i;
    }

    printf("%d! = %llu\n", n, factorielle);

    return 0;
}
