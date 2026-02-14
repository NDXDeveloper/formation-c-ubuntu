/* ============================================================================
   Section 5.4 : Conditions
   Description : Calculatrice avec switch et gestion d'erreurs
   Fichier source : 04-conditions.md
   ============================================================================ */
#include <stdio.h>

double calculer(double a, double b, char operateur) {
    if (operateur == '/' && b == 0) {
        printf("Erreur : division par zéro\n");
        return 0;
    }

    switch (operateur) {
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            return a / b;
        default:
            printf("Opérateur non reconnu\n");
            return 0;
    }
}

int main(void) {
    printf("10 + 5 = %.2f\n", calculer(10, 5, '+'));
    printf("10 - 5 = %.2f\n", calculer(10, 5, '-'));
    printf("10 * 5 = %.2f\n", calculer(10, 5, '*'));
    printf("10 / 5 = %.2f\n", calculer(10, 5, '/'));
    printf("10 / 0 = %.2f\n", calculer(10, 0, '/'));

    return 0;
}
