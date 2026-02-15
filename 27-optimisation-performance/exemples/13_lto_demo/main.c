/* ============================================================================
   Section 27.8 : Link-Time Optimization (LTO)
   Description : Programme principal demontrant LTO - compiler avec et sans
                 -flto pour comparer les tailles de binaires et l'inlining
   Fichier source : 08-lto.md
   ============================================================================ */

#include <stdio.h>
#include "math_utils.h"

int main(void) {
    int n = 10;

    int s = square(n);
    int c = cube(n);

    printf("Square(%d) = %d\n", n, s);
    printf("Cube(%d)   = %d\n", n, c);

    /* power_of_two n'est pas appelee : LTO peut l'eliminer */

    return 0;
}
