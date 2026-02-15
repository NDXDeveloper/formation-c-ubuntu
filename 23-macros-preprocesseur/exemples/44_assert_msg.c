/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Macro d'assertion personnalisee avec message
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

#define ASSERT_MSG(condition, message) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "Assertion échouée: %s\n", #condition); \
            fprintf(stderr, "Message: %s\n", message); \
            fprintf(stderr, "Fichier: %s, Ligne: %d\n", __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while(0)

int diviser(int a, int b) {
    ASSERT_MSG(b != 0, "Division par zéro");
    return a / b;
}

int main(void) {
    int resultat = diviser(10, 2);
    printf("10 / 2 = %d\n", resultat);

    // Décommentez pour tester l'assertion
    // resultat = diviser(10, 0);

    return 0;
}
