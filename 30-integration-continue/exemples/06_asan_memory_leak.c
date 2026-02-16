/* ============================================================================
   Section 30.1.5 : Verification avec sanitizers
   Description : Bug intentionnel - fuite memoire (pour ASan/LSan)
   Fichier source : 01.5-verification-sanitizers.md
   ============================================================================ */
#include <stdlib.h>

void function(void) {
    int *ptr = malloc(100 * sizeof(int));
    /* Oups ! Pas de free() */
    (void)ptr;
}

int main(void) {
    for (int i = 0; i < 10; i++) {
        function();
    }
    return 0;
}
