/* ============================================================================
   Section 23.2 : Stringification et concatenation
   Description : Double expansion - piege et solution pour stringify de macros
   Fichier source : 02-stringification-concatenation.md
   ============================================================================ */

#include <stdio.h>

#define VALUE 100

// Piège : stringification directe (ne fait PAS l'expansion)
#define STRINGIFY_DIRECT(x) #x

// Solution : double expansion (force l'expansion avant stringification)
#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

int main(void) {
    // Piège : STRINGIFY_DIRECT(VALUE) → "VALUE" (pas "100")
    printf("Direct : %s\n", STRINGIFY_DIRECT(VALUE));
    // Affiche : Direct : VALUE

    // Solution : STRINGIFY(VALUE) → STRINGIFY_HELPER(100) → "100"
    printf("Double expansion : %s\n", STRINGIFY(VALUE));
    // Affiche : Double expansion : 100

    return 0;
}
