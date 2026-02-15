/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Operateur defined() - test d'existence de macros
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>

#define FEATURE_A
// #define FEATURE_B  ← Non défini

int main(void) {
#if defined(FEATURE_A)
    printf("Feature A activée\n");
#endif

#if defined(FEATURE_B)
    printf("Feature B activée\n");
#else
    printf("Feature B désactivée\n");
#endif

    // Combinaisons logiques
#if defined(FEATURE_A) && !defined(FEATURE_B)
    printf("Seulement Feature A est active\n");
#endif

    return 0;
}
