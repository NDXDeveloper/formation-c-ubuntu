/* ============================================================================
   Section 28.5 : extern "C" pour C++
   Description : Programme C appelant du code C++ via extern "C"
   Fichier source : 05-extern-c.md
   ============================================================================ */

#include <stdio.h>
#include "math.h"

int main(void) {
    printf("=== extern \"C\" : C appelle C++ ===\n\n");

    printf("  5 + 3 = %d\n", addition(5, 3));
    printf("  10 - 4 = %d\n", soustraction(10, 4));
    printf("  -3 + 10 = %d\n", addition(-3, 10));
    printf("  100 - 99 = %d\n", soustraction(100, 99));

    printf("\n=== Fin ===\n");
    return 0;
}
