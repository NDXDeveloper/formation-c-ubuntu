/* ============================================================================
   Section 6.1 : Declaration, definition et prototypes
   Description : Programme complet avec prototypes, main et definitions
   Fichier source : 01-declaration-definition.md
   ============================================================================ */
#include <stdio.h>

// ========== PROTOTYPES ==========
int puissance(int base, int exposant);
void afficher_resultat(int base, int exposant, int resultat);
void afficher_titre(void);

// ========== MAIN ==========
int main(void) {
    afficher_titre();

    int base = 2;
    int exposant = 10;
    int resultat = puissance(base, exposant);

    afficher_resultat(base, exposant, resultat);

    return 0;
}

// ========== DEFINITIONS ==========
int puissance(int base, int exposant) {
    int resultat = 1;
    for (int i = 0; i < exposant; i++) {
        resultat *= base;
    }
    return resultat;
}

void afficher_resultat(int base, int exposant, int resultat) {
    printf("%d^%d = %d\n", base, exposant, resultat);
}

void afficher_titre(void) {
    printf("=== Calculateur de puissance ===\n");
}
