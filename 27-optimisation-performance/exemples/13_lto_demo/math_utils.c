/* ============================================================================
   Section 27.8 : Link-Time Optimization (LTO)
   Description : Implementation des fonctions mathematiques - power_of_two
                 n'est jamais appelee (eliminee par LTO)
   Fichier source : 08-lto.md
   ============================================================================ */

#include "math_utils.h"

int square(int x) {
    return x * x;
}

int cube(int x) {
    return x * x * x;
}

/* Cette fonction n'est jamais utilisee - LTO peut l'eliminer */
int power_of_two(int x) {
    int result = 1;
    for (int i = 0; i < x; i++) {
        result *= 2;
    }
    return result;
}
