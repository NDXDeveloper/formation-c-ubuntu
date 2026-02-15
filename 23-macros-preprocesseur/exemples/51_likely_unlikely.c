/* ============================================================================
   Section 23.4 : Macros cross-platform
   Description : Extensions GCC/Clang - LIKELY/UNLIKELY pour optimisation
   Fichier source : 04-macros-cross-platform.md
   ============================================================================ */

#include <stdio.h>

// Utiliser les extensions GCC si disponibles
#if defined(__GNUC__) || defined(__clang__)
    #define LIKELY(x)   __builtin_expect(!!(x), 1)
    #define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define LIKELY(x)   (x)
    #define UNLIKELY(x) (x)
#endif

int fonction_avec_optimisation(int* ptr) {
    // Indique que ptr == NULL est très improbable
    if (UNLIKELY(ptr == NULL)) {
        fprintf(stderr, "Erreur : pointeur NULL\n");
        return -1;
    }

    // Cas normal
    return *ptr * 2;
}

int main(void) {
    int valeur = 42;
    int resultat = fonction_avec_optimisation(&valeur);
    printf("Résultat : %d\n", resultat);

    return 0;
}
