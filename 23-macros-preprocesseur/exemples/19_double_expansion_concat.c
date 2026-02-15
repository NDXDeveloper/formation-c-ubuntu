/* ============================================================================
   Section 23.2 : Stringification et concatenation
   Description : Double expansion pour forcer l'expansion avant concatenation
   Fichier source : 02-stringification-concatenation.md
   ============================================================================ */

#include <stdio.h>

#define PREFIX value

// Macro d'indirection pour forcer l'expansion
#define CONCAT_HELPER(a, b) a##b
#define CONCAT(a, b) CONCAT_HELPER(a, b)

int main(void) {
    int value_10 = 100;

    printf("%d\n", CONCAT(PREFIX, _10));
    // PREFIX est d'abord expansé en value
    // Puis value##_10 devient value_10
    // Affiche : 100

    return 0;
}
