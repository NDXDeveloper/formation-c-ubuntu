/* ============================================================================
   Section 23.1 : Macros parametrees
   Description : Macros MAX et MIN avec plusieurs parametres
   Fichier source : 01-macros-parametrees.md
   ============================================================================ */

#include <stdio.h>

// Macro qui retourne le maximum de deux valeurs
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// Macro qui retourne le minimum de deux valeurs
#define MIN(a, b) ((a) < (b) ? (a) : (b))

int main(void) {
    int x = 10, y = 20;

    printf("Max entre %d et %d : %d\n", x, y, MAX(x, y));
    // Affiche : Max entre 10 et 20 : 20

    printf("Min entre %d et %d : %d\n", x, y, MIN(x, y));
    // Affiche : Min entre 10 et 20 : 10

    return 0;
}
