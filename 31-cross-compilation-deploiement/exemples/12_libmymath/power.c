/* ============================================================================
   Section 31.9 : Distribution de bibliotheques
   Description : Implementation des fonctions power et version (bibliotheque mymath)
   Fichier source : 09-distribution-bibliotheques.md
   ============================================================================ */
#include "mymath.h"

int power(int base, int exponent) {
    int result = 1;
    for (int i = 0; i < exponent; i++) {
        result *= base;
    }
    return result;
}

const char *mymath_version(void) {
    return "1.0.0";
}
