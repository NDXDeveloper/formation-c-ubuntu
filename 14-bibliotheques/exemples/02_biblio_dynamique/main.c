/* ============================================================================
   Section 02.1 : Compilation avec -fPIC
   Description : Programme utilisant la bibliotheque dynamique libmath_utils.so
   Fichier source : 02.1-compilation-fpic.md
   ============================================================================ */

#include <stdio.h>
#include "math_utils.h"

int main(void) {
    int a = 10, b = 5;

    printf("Addition : %d + %d = %d\n", a, b, addition(a, b));
    printf("Multiplication : %d * %d = %d\n", a, b, multiplication(a, b));

    return 0;
}
