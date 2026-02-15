/* ============================================================================
   Section 23.2 : Stringification et concatenation
   Description : Macro de debug avancee avec __FILE__ et __LINE__
   Fichier source : 02-stringification-concatenation.md
   ============================================================================ */

#include <stdio.h>

#define DEBUG(var) \
    printf("[DEBUG] %s:%d - " #var " = %d\n", __FILE__, __LINE__, var)

int main(void) {
    int compteur = 42;
    int resultat = 100;

    DEBUG(compteur);
    // Affiche : [DEBUG] 11_debug_avance.c:17 - compteur = 42

    DEBUG(resultat);
    // Affiche : [DEBUG] 11_debug_avance.c:20 - resultat = 100

    return 0;
}
