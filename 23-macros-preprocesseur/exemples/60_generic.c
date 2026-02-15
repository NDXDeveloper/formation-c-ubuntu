/* ============================================================================
   Section 23.5 : Dangers et pieges des macros
   Description : Alternative C11 _Generic pour genericite typee
   Fichier source : 05-dangers-macros.md
   ============================================================================ */

#include <stdio.h>

// Fonctions spécialisées
int max_int(int a, int b) { return (a > b) ? a : b; }
double max_double(double a, double b) { return (a > b) ? a : b; }

// Macro générique avec vérification de type (C11)
#define max(a, b) _Generic((a), \
    int: max_int, \
    double: max_double \
)(a, b)

int main(void) {
    printf("%d\n", max(5, 3));        // Appelle max_int
    printf("%.2f\n", max(5.5, 3.2));  // Appelle max_double

    return 0;
}
