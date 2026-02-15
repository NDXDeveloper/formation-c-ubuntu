/* ============================================================================
   Section 23.5 : Dangers et pieges des macros
   Description : BUG INTENTIONNEL - problemes de precedence sans parentheses
   Fichier source : 05-dangers-macros.md
   ============================================================================ */

#include <stdio.h>

// Macros dangereuses sans parenthèses
#define MAUVAIS_CARRE(x) x * x
#define MAUVAIS_DOUBLE(x) x + x

// Macros correctes avec parenthèses
#define BON_CARRE(x) ((x) * (x))
#define BON_DOUBLE(x) ((x) + (x))

int main(void) {
    // Test 1 : CARRE sans parenthèses
    int resultat1 = MAUVAIS_CARRE(3 + 2);
    printf("MAUVAIS_CARRE(3 + 2) = %d (attendu 25)\n", resultat1);
    // Devient : 3 + 2 * 3 + 2 = 11

    // Test 2 : DOUBLE sans parenthèses
    int resultat2 = MAUVAIS_DOUBLE(5) * 3;
    printf("MAUVAIS_DOUBLE(5) * 3 = %d (attendu 30)\n", resultat2);
    // Devient : 5 + 5 * 3 = 20

    printf("\nVersions corrigées :\n");

    // Versions correctes
    int resultat3 = BON_CARRE(3 + 2);
    printf("BON_CARRE(3 + 2) = %d\n", resultat3);
    // Devient : ((3 + 2) * (3 + 2)) = 25

    int resultat4 = BON_DOUBLE(5) * 3;
    printf("BON_DOUBLE(5) * 3 = %d\n", resultat4);
    // Devient : ((5) + (5)) * 3 = 30

    return 0;
}
