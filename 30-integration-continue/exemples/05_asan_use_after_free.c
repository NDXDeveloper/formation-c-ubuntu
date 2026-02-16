/* ============================================================================
   Section 30.1.5 : Verification avec sanitizers
   Description : Bug intentionnel - utilisation apres liberation (pour ASan)
   Fichier source : 01.5-verification-sanitizers.md
   ============================================================================ */
#include <stdlib.h>
#include <stdio.h>

int main(void) {
    int *ptr = malloc(sizeof(int));
    *ptr = 42;

    free(ptr);

    /* Bug : utilisation apres liberation ! */
    printf("Value: %d\n", *ptr);

    return 0;
}
