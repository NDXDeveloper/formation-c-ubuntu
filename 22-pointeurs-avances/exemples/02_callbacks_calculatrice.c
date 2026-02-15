/* ============================================================================
   Section 22.1 : Pointeurs de fonctions (Callbacks)
   Description : Calculatrice generique utilisant des callbacks
   Fichier source : 01-pointeurs-fonctions.md
   ============================================================================ */

#include <stdio.h>

// Definition de plusieurs operations
int addition(int a, int b) {
    return a + b;
}

int multiplication(int a, int b) {
    return a * b;
}

int division(int a, int b) {
    if (b == 0) {
        printf("Erreur : division par zero\n");
        return 0;
    }
    return a / b;
}

// Fonction generique qui prend un callback
int calculer(int x, int y, int (*operation)(int, int)) {
    printf("Calcul de %d et %d...\n", x, y);
    return operation(x, y);
}

int main(void) {
    int a = 10, b = 5;

    // Utilisation avec differentes operations
    printf("Addition : %d\n", calculer(a, b, addition));
    printf("Multiplication : %d\n", calculer(a, b, multiplication));
    printf("Division : %d\n", calculer(a, b, division));

    return 0;
}
