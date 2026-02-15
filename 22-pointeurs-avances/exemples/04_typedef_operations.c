/* ============================================================================
   Section 22.1 : Pointeurs de fonctions (Callbacks)
   Description : Utilisation de typedef pour simplifier les pointeurs de
                 fonctions
   Fichier source : 01-pointeurs-fonctions.md
   ============================================================================ */

#include <stdio.h>

// Definition d'un type pour les pointeurs de fonctions
typedef int (*OperationBinaire)(int, int);

int addition(int a, int b) {
    return a + b;
}

int soustraction(int a, int b) {
    return a - b;
}

// Fonction qui prend un callback (syntaxe plus lisible)
int appliquer_operation(int x, int y, OperationBinaire op) {
    return op(x, y);
}

int main(void) {
    OperationBinaire mon_operation;

    mon_operation = addition;
    printf("10 + 5 = %d\n", appliquer_operation(10, 5, mon_operation));

    mon_operation = soustraction;
    printf("10 - 5 = %d\n", appliquer_operation(10, 5, mon_operation));

    return 0;
}
