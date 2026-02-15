/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Compatibilite C11+ avec detection des atomics
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>

// Utilisation de fonctionnalités C11+ si disponibles
#if __STDC_VERSION__ >= 201112L
    #include <stdatomic.h>
    #define ATOMIC_AVAILABLE 1
#else
    #define ATOMIC_AVAILABLE 0
#endif

int main(void) {
#if ATOMIC_AVAILABLE
    atomic_int compteur = 0;
    printf("Atomics disponibles (C11+)\n");
#else
    int compteur = 0;
    printf("Atomics non disponibles, utilisation d'int standard\n");
#endif

    compteur++;
    printf("Compteur: %d\n", compteur);

    return 0;
}
