/* ============================================================================
   Section 25.5 : Design by contract
   Description : Macros de contrat PRECONDITION/POSTCONDITION avec factorielle
   Fichier source : 05-design-by-contract.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* Macros pour le Design by Contract */
#ifdef NDEBUG
    #define PRECONDITION(condition)  ((void)0)
    #define POSTCONDITION(condition) ((void)0)
    #define INVARIANT(condition)     ((void)0)
#else
    #define PRECONDITION(condition) \
        assert((condition) && "Precondition violee")

    #define POSTCONDITION(condition) \
        assert((condition) && "Postcondition violee")

    #define INVARIANT(condition) \
        assert((condition) && "Invariant viole")
#endif

/* Exemple d'utilisation */
int factorielle(int n) {
    PRECONDITION(n >= 0);
    PRECONDITION(n <= 12);  /* Eviter l'overflow pour int */

    int resultat = 1;
    for (int i = 2; i <= n; i++) {
        resultat *= i;
    }

    POSTCONDITION(resultat > 0);

    return resultat;
}

int main(void) {
    printf("0! = %d\n", factorielle(0));
    printf("1! = %d\n", factorielle(1));
    printf("5! = %d\n", factorielle(5));
    printf("10! = %d\n", factorielle(10));
    printf("12! = %d\n", factorielle(12));

    return 0;
}
