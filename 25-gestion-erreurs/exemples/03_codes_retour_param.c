/* ============================================================================
   Section 25.1 : Codes de retour vs exceptions simulees
   Description : Retour de valeur via parametre et code d'erreur
   Fichier source : 01-codes-retour-vs-exceptions.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

int calculer_division(int a, int b, int *resultat) {
    if (b == 0) {
        return -1;  /* Erreur : division par zero */
    }

    *resultat = a / b;
    return 0;  /* Succes */
}

int main(void) {
    int resultat;

    if (calculer_division(10, 0, &resultat) != 0) {
        fprintf(stderr, "Erreur : division par zero\n");
    }

    if (calculer_division(10, 3, &resultat) == 0) {
        printf("10 / 3 = %d\n", resultat);
    }

    return EXIT_SUCCESS;
}
