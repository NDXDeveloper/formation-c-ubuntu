/* ============================================================================
   Section 31.3 : CMake et cross-compilation
   Description : Implementation info systeme (detection architecture + compilateur)
   Fichier source : 03-cmake-cross-compilation.md
   ============================================================================ */
#include <stdio.h>
#include "utils.h"

void afficher_info_systeme(void) {
    printf("Architecture: ");
#if defined(__aarch64__) || defined(__arm64__)
    printf("ARM 64 bits (aarch64)\n");
#elif defined(__arm__)
    printf("ARM 32 bits\n");
#elif defined(__x86_64__) || defined(__amd64__)
    printf("x86_64\n");
#elif defined(__i386__)
    printf("x86 32 bits\n");
#else
    printf("Inconnue\n");
#endif

    printf("Compile avec: %s\n", __VERSION__);
}
