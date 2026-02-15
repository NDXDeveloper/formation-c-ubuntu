/* ============================================================================
   Section 23.5 : Dangers et pieges des macros
   Description : BUG INTENTIONNEL - evaluation multiple avec effets de bord
   Fichier source : 05-dangers-macros.md
   ============================================================================ */

#include <stdio.h>

#define CARRE(x) ((x) * (x))

int main(void) {
    int compteur = 5;

    printf("Compteur initial : %d\n", compteur);

    int resultat = CARRE(compteur++);
    /* Comportement indéfini ! compteur++ évalué deux fois */

    printf("Résultat : %d\n", resultat);
    printf("Compteur final : %d\n", compteur);

    return 0;
}
