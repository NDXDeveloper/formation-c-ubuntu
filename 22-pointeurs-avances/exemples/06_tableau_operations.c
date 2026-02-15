/* ============================================================================
   Section 22.2 : Tableaux de pointeurs de fonctions
   Description : Tableau de pointeurs de fonctions pour operations
                 arithmetiques
   Fichier source : 02-tableaux-pointeurs-fonctions.md
   ============================================================================ */

#include <stdio.h>

// Definition de plusieurs fonctions avec la meme signature
int addition(int a, int b) {
    return a + b;
}

int soustraction(int a, int b) {
    return a - b;
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

int main(void) {
    // Declaration et initialisation du tableau de pointeurs de fonctions
    int (*operations[4])(int, int) = {
        addition,
        soustraction,
        multiplication,
        division
    };

    int x = 20, y = 5;

    // Appel des fonctions via le tableau
    printf("Addition : %d + %d = %d\n", x, y, operations[0](x, y));
    printf("Soustraction : %d - %d = %d\n", x, y, operations[1](x, y));
    printf("Multiplication : %d * %d = %d\n", x, y, operations[2](x, y));
    printf("Division : %d / %d = %d\n", x, y, operations[3](x, y));

    return 0;
}
