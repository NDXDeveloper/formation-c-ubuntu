/* ============================================================================
   Section 22.2 : Tableaux de pointeurs de fonctions
   Description : Tableaux multidimensionnels de pointeurs de fonctions
   Fichier source : 02-tableaux-pointeurs-fonctions.md
   ============================================================================ */

#include <stdio.h>

typedef int (*Operation)(int, int);

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }
int div_safe(int a, int b) { return (b != 0) ? a / b : 0; }
int mod(int a, int b) { return (b != 0) ? a % b : 0; }
int max(int a, int b) { return (a > b) ? a : b; }

int main(void) {
    // Tableau 2D : categories d'operations
    Operation operations[2][3] = {
        // Categorie 0 : Operations arithmetiques de base
        {add, sub, mul},
        // Categorie 1 : Operations avancees
        {div_safe, mod, max}
    };

    printf("Categorie 0, Operation 0 (addition) : 10 + 5 = %d\n",
           operations[0][0](10, 5));
    printf("Categorie 0, Operation 2 (multiplication) : 10 * 5 = %d\n",
           operations[0][2](10, 5));
    printf("Categorie 1, Operation 1 (modulo) : 10 %% 3 = %d\n",
           operations[1][1](10, 3));
    printf("Categorie 1, Operation 2 (maximum) : max(10, 5) = %d\n",
           operations[1][2](10, 5));

    return 0;
}
