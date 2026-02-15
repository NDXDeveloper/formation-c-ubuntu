/* ============================================================================
   Section 23.6 : Macros predefinies utiles
   Description : Macro de profilage - mesure du temps d'execution
   Fichier source : 06-macros-predefinies.md
   ============================================================================ */

#include <stdio.h>
#include <time.h>

// Macro pour mesurer le temps d'exécution d'un bloc de code
#define TIME_IT(name, code) \
    do { \
        clock_t start = clock(); \
        code; \
        clock_t end = clock(); \
        double cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC; \
        printf("[PERF] %s pris %.6f secondes (ligne %d)\n", \
               name, cpu_time, __LINE__); \
    } while(0)

// Fonction à profiler
void calcul_intensif(int iterations) {
    volatile long long somme = 0;
    for (int i = 0; i < iterations; i++) {
        somme += i;
    }
}

int main(void) {
    printf("Tests de performance\n\n");

    TIME_IT("Calcul 1M", calcul_intensif(1000000));
    TIME_IT("Calcul 10M", calcul_intensif(10000000));
    TIME_IT("Calcul 100M", calcul_intensif(100000000));

    return 0;
}
