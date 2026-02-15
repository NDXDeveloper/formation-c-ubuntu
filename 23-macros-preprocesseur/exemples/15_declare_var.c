/* ============================================================================
   Section 23.2 : Stringification et concatenation
   Description : Generation de noms de variables avec ##
   Fichier source : 02-stringification-concatenation.md
   ============================================================================ */

#include <stdio.h>

#define DECLARE_VAR(type, prefix, suffix) type prefix##suffix

int main(void) {
    DECLARE_VAR(int, compteur, _principal) = 100;
    DECLARE_VAR(int, compteur, _secondaire) = 50;

    // Équivalent à :
    // int compteur_principal = 100;
    // int compteur_secondaire = 50;

    printf("Principal : %d\n", compteur_principal);
    printf("Secondaire : %d\n", compteur_secondaire);

    return 0;
}
