/* ============================================================================
   Section 28.3 : Interfacage avec Rust (FFI)
   Description : Programme C appelant des fonctions Rust via FFI
                 (factorielle, fibonacci, division securisee)
   Fichier source : 03-interfacage-rust.md
   ============================================================================ */

#include <stdio.h>
#include <inttypes.h>
#include "rustlib.h"

int main(void) {
    printf("=== C appelant Rust via FFI ===\n\n");

    /* Factorielle */
    printf("--- Factorielle (Rust) ---\n");
    for (uint32_t i = 0; i <= 12; i++) {
        printf("  %u! = %" PRIu64 "\n", i, factorielle_rust(i));
    }

    /* Fibonacci */
    printf("\n--- Fibonacci (Rust) ---\n");
    for (uint32_t i = 0; i <= 15; i++) {
        printf("  fib(%u) = %" PRIu64 "\n", i, fibonacci_rust(i));
    }

    /* Division securisee */
    printf("\n--- Division securisee (Rust) ---\n");
    int resultat;

    int code = diviser_entiers(10, 3, &resultat);
    if (code == 0) {
        printf("  10 / 3 = %d\n", resultat);
    }

    code = diviser_entiers(100, 7, &resultat);
    if (code == 0) {
        printf("  100 / 7 = %d\n", resultat);
    }

    code = diviser_entiers(10, 0, &resultat);
    if (code != 0) {
        printf("  10 / 0 = ERREUR (code %d)\n", code);
    }

    code = diviser_entiers(10, 2, NULL);
    if (code != 0) {
        printf("  diviser(10, 2, NULL) = ERREUR (code %d)\n", code);
    }

    printf("\n=== Fin ===\n");
    return 0;
}
