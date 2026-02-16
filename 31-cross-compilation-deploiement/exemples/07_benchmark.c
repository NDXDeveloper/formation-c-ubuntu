/* ============================================================================
   Section 31.4 : QEMU pour tester les binaires
   Description : Benchmark simple pour mesurer les performances (QEMU vs natif)
   Fichier source : 04-qemu-tests.md
   ============================================================================ */
#include <stdio.h>
#include <time.h>

int main(void) {
    clock_t start = clock();

    long long sum = 0;
    for (long long i = 0; i < 100000000; i++) {
        sum += i;
    }

    clock_t end = clock();
    double duration = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Resultat : %lld\n", sum);
    printf("Temps : %.2f secondes\n", duration);
    return 0;
}
