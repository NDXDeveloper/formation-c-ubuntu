/* ============================================================================
   Section 6.4 : Valeurs de retour et codes d'erreur
   Description : Division securisee avec parametres de sortie (output parameters)
   Fichier source : 04-valeurs-retour.md
   ============================================================================ */
#include <stdio.h>

int diviser(int a, int b, int *quotient, int *reste) {
    if (b == 0) {
        return -1;  // Erreur : division par zero
    }

    *quotient = a / b;
    *reste = a % b;

    return 0;  // Succes
}

int main(void) {
    int q, r;
    int status = diviser(17, 5, &q, &r);

    if (status != 0) {
        printf("Erreur : division par zéro\n");
        return 1;
    }

    printf("17 / 5 = %d reste %d\n", q, r);
    return 0;
}
