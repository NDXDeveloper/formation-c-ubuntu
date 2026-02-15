/* ============================================================================
   Section 23.2 : Stringification et concatenation
   Description : Concatenation avec nombres pour generer des identificateurs
   Fichier source : 02-stringification-concatenation.md
   ============================================================================ */

#include <stdio.h>

#define MAKE_VERSION(major, minor) v##major##_##minor
#define MAKE_CONSTANT(prefix, num) prefix##num

int main(void) {
    // Génération d'identificateurs
    int MAKE_VERSION(2, 5) = 250;
    int MAKE_CONSTANT(MAX_, 100) = 100;

    // Équivalent à :
    // int v2_5 = 250;
    // int MAX_100 = 100;

    printf("Version : %d\n", v2_5);
    printf("Constante : %d\n", MAX_100);

    return 0;
}
