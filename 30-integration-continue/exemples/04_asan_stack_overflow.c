/* ============================================================================
   Section 30.1.5 : Verification avec sanitizers
   Description : Bug intentionnel - acces hors limites tableau (pour ASan)
   Fichier source : 01.5-verification-sanitizers.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    int array[5] = {1, 2, 3, 4, 5};

    /* Bug : acces hors limites ! (i <= 5 au lieu de i < 5) */
    for (int i = 0; i <= 5; i++) {
        printf("array[%d] = %d\n", i, array[i]);
    }

    return 0;
}
