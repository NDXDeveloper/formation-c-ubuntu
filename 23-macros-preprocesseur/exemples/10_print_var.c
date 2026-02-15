/* ============================================================================
   Section 23.2 : Stringification et concatenation
   Description : Debug printing - affichage de variables avec leur nom
   Fichier source : 02-stringification-concatenation.md
   ============================================================================ */

#include <stdio.h>

#define PRINT_VAR(var) printf(#var " = %d\n", var)

int main(void) {
    int age = 25;
    int score = 1337;

    PRINT_VAR(age);
    // Affiche : age = 25

    PRINT_VAR(score);
    // Affiche : score = 1337

    return 0;
}
