/* ============================================================================
   Section 23.6 : Macros predefinies utiles
   Description : Detection des fonctionnalites C11 disponibles
   Fichier source : 06-macros-predefinies.md
   ============================================================================ */

#include <stdio.h>

int main(void) {
    printf("Fonctionnalités disponibles :\n\n");

    // Threads C11
#ifdef __STDC_NO_THREADS__
    printf("  Threads C11 NON disponibles\n");
#else
    printf("  Threads C11 disponibles\n");
#endif

    // Atomics
#ifdef __STDC_NO_ATOMICS__
    printf("  Atomics NON disponibles\n");
#else
    printf("  Atomics disponibles\n");
#endif

    // VLA (Variable Length Arrays)
#ifdef __STDC_NO_VLA__
    printf("  VLA NON disponibles\n");
#else
    printf("  VLA disponibles\n");
#endif

    // Complex numbers
#ifdef __STDC_NO_COMPLEX__
    printf("  Nombres complexes NON disponibles\n");
#else
    printf("  Nombres complexes disponibles\n");
#endif

    return 0;
}
