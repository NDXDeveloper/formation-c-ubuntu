/* ============================================================================
   Section 5.4 : Conditions
   Description : Operateur ternaire (condition ? vrai : faux)
   Fichier source : 04-conditions.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    int age = 20;

    const char *statut = (age >= 18) ? "majeur" : "mineur";
    printf("Vous êtes %s\n", statut);

    int a = 10, b = 20;
    int max = (a > b) ? a : b;
    printf("Le maximum est : %d\n", max);

    return 0;
}
