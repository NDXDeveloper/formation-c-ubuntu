/* ============================================================================
   Section 01.1/01.2 : Compilation ar/ranlib et linking statique
   Description : Programme utilisant la bibliotheque statique libmath_utils.a
   Fichier source : 01.1-compilation-ar-ranlib.md
   ============================================================================ */

#include <stdio.h>
#include "math_utils.h"

int main(void) {
    int a = 10, b = 5;

    printf("Addition : %d + %d = %d\n", a, b, addition(a, b));
    printf("Multiplication : %d * %d = %d\n", a, b, multiplication(a, b));

    return 0;
}
