/* ============================================================================
   Section 23.1 : Macros parametrees
   Description : Genericite des macros - fonctionnent avec differents types
   Fichier source : 01-macros-parametrees.md
   ============================================================================ */

#include <stdio.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int main(void) {
    int i1 = 10, i2 = 20;
    printf("Max int : %d\n", MAX(i1, i2));  // Fonctionne avec int

    double d1 = 3.14, d2 = 2.71;
    printf("Max double : %.2f\n", MAX(d1, d2));  // Fonctionne avec double

    char c1 = 'A', c2 = 'Z';
    printf("Max char : %c\n", MAX(c1, c2));  // Fonctionne avec char

    return 0;
}
