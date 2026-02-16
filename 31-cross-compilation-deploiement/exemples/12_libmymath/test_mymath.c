/* ============================================================================
   Section 31.9 : Distribution de bibliotheques
   Description : Programme de test de la bibliotheque mymath (.so)
   Fichier source : 09-distribution-bibliotheques.md
   ============================================================================ */
#include <stdio.h>
#include "mymath.h"

int main(void) {
    printf("mymath version: %s\n", mymath_version());
    printf("2 + 3 = %d\n", add(2, 3));
    printf("4 * 5 = %d\n", multiply(4, 5));
    printf("2^8 = %d\n", power(2, 8));
    return 0;
}
