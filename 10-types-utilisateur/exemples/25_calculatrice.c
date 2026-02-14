/* ============================================================================
   Section 10.6 : typedef - Simplification de la syntaxe
   Description : Calculatrice avec typedef de pointeurs de fonctions
   Fichier source : 06-typedef.md
   ============================================================================ */

#include <stdio.h>

typedef int (*Operation)(int, int);

int additionner(int a, int b) {
    return a + b;
}

int soustraire(int a, int b) {
    return a - b;
}

int multiplier(int a, int b) {
    return a * b;
}

int diviser(int a, int b) {
    if (b == 0) {
        printf("Erreur : division par zero\n");
        return 0;
    }
    return a / b;
}

int calculer(int a, int b, Operation op) {
    return op(a, b);
}

int main(void) {
    int x = 10, y = 5;

    printf("%d + %d = %d\n", x, y, calculer(x, y, additionner));
    printf("%d - %d = %d\n", x, y, calculer(x, y, soustraire));
    printf("%d * %d = %d\n", x, y, calculer(x, y, multiplier));
    printf("%d / %d = %d\n", x, y, calculer(x, y, diviser));

    return 0;
}
