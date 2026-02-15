/* ============================================================================
   Section 22.2 : Tableaux de pointeurs de fonctions
   Description : Simplification avec typedef pour les tableaux de pointeurs
                 de fonctions
   Fichier source : 02-tableaux-pointeurs-fonctions.md
   ============================================================================ */

#include <stdio.h>

// Definition d'un type pour simplifier
typedef int (*OperationMath)(int, int);

int addition(int a, int b) { return a + b; }
int soustraction(int a, int b) { return a - b; }
int multiplication(int a, int b) { return a * b; }
int division(int a, int b) {
    return (b != 0) ? a / b : 0;
}

int main(void) {
    // Beaucoup plus lisible !
    OperationMath operations[4] = {
        addition,
        soustraction,
        multiplication,
        division
    };

    // Utilisation
    int resultat = operations[2](6, 7);  // Multiplication
    printf("6 * 7 = %d\n", resultat);

    return 0;
}
