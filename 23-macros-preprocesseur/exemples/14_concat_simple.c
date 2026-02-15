/* ============================================================================
   Section 23.2 : Stringification et concatenation
   Description : Operateur ## - concatenation de tokens (token pasting)
   Fichier source : 02-stringification-concatenation.md
   ============================================================================ */

#include <stdio.h>

#define CONCAT(a, b) a##b

int main(void) {
    int xy = 42;

    printf("%d\n", CONCAT(x, y));
    // CONCAT(x, y) devient xy
    // Affiche : 42

    return 0;
}
