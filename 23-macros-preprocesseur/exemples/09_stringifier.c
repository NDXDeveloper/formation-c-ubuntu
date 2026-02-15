/* ============================================================================
   Section 23.2 : Stringification et concatenation
   Description : Operateur # - transformation d'arguments en chaines
   Fichier source : 02-stringification-concatenation.md
   ============================================================================ */

#include <stdio.h>

#define STRINGIFIER(x) #x

int main(void) {
    printf("%s\n", STRINGIFIER(Hello));
    // Affiche : Hello

    printf("%s\n", STRINGIFIER(123));
    // Affiche : 123

    printf("%s\n", STRINGIFIER(3 + 5));
    // Affiche : 3 + 5

    return 0;
}
