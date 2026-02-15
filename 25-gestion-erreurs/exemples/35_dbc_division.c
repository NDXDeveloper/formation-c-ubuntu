/* ============================================================================
   Section 25.5 : Design by contract
   Description : Division entiere avec contrat (pre/postconditions)
   Fichier source : 05-design-by-contract.md
   ============================================================================ */

#include <stdio.h>
#include <assert.h>

/**
 * Division entiere.
 *
 * CONTRAT :
 * Preconditions :
 *   - diviseur != 0
 *   - reste != NULL
 *
 * Postconditions :
 *   - resultat * diviseur + reste == dividende
 */
int diviser(int dividende, int diviseur, int *reste) {
    /* Preconditions */
    assert(diviseur != 0 && "Le diviseur ne peut pas etre zero");
    assert(reste != NULL && "Le pointeur reste doit etre valide");

    /* Calcul */
    int quotient = dividende / diviseur;
    *reste = dividende % diviseur;

    /* Postconditions */
    assert(quotient * diviseur + *reste == dividende &&
           "Le theoreme de la division doit etre respecte");

    return quotient;
}

int main(void) {
    int reste;

    /* Utilisation correcte */
    int q = diviser(17, 5, &reste);
    printf("17 / 5 = %d reste %d\n", q, reste);

    q = diviser(100, 7, &reste);
    printf("100 / 7 = %d reste %d\n", q, reste);

    q = diviser(-15, 4, &reste);
    printf("-15 / 4 = %d reste %d\n", q, reste);

    return 0;
}
