/* ============================================================================
   Section 31.2 : Compiler pour ARM
   Description : Implementation du module utilitaire (detection architecture)
   Fichier source : 02-compiler-arm.md
   ============================================================================ */
#include <stdio.h>
#include "utils.h"

void afficher_architecture(void) {
#ifdef __aarch64__
    printf("Architecture: ARM 64 bits\n");
#elif __arm__
    printf("Architecture: ARM 32 bits\n");
#else
    printf("Architecture: Non-ARM\n");
#endif
}
