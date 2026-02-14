/* ============================================================================
   Section 5.2 : Operateurs logiques
   Description : Valeurs booleennes en C (vrai/faux = non-zero/zero)
   Fichier source : 02-operateurs-logiques.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    int valeur1 = 5;
    int valeur2 = 0;
    int valeur3 = -3;

    if (valeur1) {
        printf("valeur1 est considérée comme vraie\n");
    }

    if (valeur2) {
        printf("valeur2 est considérée comme vraie\n");
    }

    if (valeur3) {
        printf("valeur3 est considérée comme vraie\n");
    }

    return 0;
}
