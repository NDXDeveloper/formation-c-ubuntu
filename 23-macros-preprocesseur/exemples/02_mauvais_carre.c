/* ============================================================================
   Section 23.1 : Macros parametrees
   Description : Bug classique - macro sans parentheses (resultat incorrect)
   Fichier source : 01-macros-parametrees.md
   ============================================================================ */

#include <stdio.h>

// MAUVAIS : Sans parenthèses
#define MAUVAIS_CARRE(x) x * x

int main(void) {
    int resultat = MAUVAIS_CARRE(2 + 3);
    printf("Résultat : %d\n", resultat);
    // On s'attend à (2+3)² = 25
    // Mais on obtient 11 !

    return 0;
}
