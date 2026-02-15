/* ============================================================================
   Section 27.9 : Profile-Guided Optimization (PGO)
   Description : Branchement previsible (99%% hot / 1%% cold) - compiler avec
                 -fprofile-generate puis -fprofile-use pour optimiser
   Fichier source : 09-pgo.md
   ============================================================================ */

#include <stdio.h>

int main(void) {
    long long sum = 0;

    for (int i = 0; i < 10000000; i++) {
        /* Dans 99%% des cas, i < 9900000 */
        if (i < 9900000) {
            sum += i;   /* Branche "chaude" (hot) */
        } else {
            sum -= i;   /* Branche "froide" (cold) */
        }
    }

    printf("Resultat : %lld\n", sum);
    return 0;
}
