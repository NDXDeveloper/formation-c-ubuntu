/* ============================================================================
   Section 23.2 : Stringification et concatenation
   Description : Stringification avec differents types (int, float, string)
   Fichier source : 02-stringification-concatenation.md
   ============================================================================ */

#include <stdio.h>

#define PRINT_INT(var) printf(#var " = %d\n", var)
#define PRINT_FLOAT(var) printf(#var " = %.2f\n", var)
#define PRINT_STRING(var) printf(#var " = %s\n", var)

int main(void) {
    int entier = 42;
    double pi = 3.14159;
    const char* nom = "Alice";

    PRINT_INT(entier);
    // Affiche : entier = 42

    PRINT_FLOAT(pi);
    // Affiche : pi = 3.14

    PRINT_STRING(nom);
    // Affiche : nom = Alice

    return 0;
}
