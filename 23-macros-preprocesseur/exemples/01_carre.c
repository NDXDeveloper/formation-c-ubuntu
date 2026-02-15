/* ============================================================================
   Section 23.1 : Macros parametrees
   Description : Macro simple qui calcule le carre d'un nombre
   Fichier source : 01-macros-parametrees.md
   ============================================================================ */

#include <stdio.h>

// Macro qui calcule le carré d'un nombre
#define CARRE(x) ((x) * (x))

int main(void) {
    int a = 5;
    int resultat = CARRE(a);

    printf("Le carré de %d est %d\n", a, resultat);
    // Affiche : Le carré de 5 est 25

    return 0;
}
