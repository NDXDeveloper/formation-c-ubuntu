/* ============================================================================
   Section 31.3 : CMake et cross-compilation
   Description : Programme principal du projet CMake cross-compilation
   Fichier source : 03-cmake-cross-compilation.md
   ============================================================================ */
#include <stdio.h>
#include "utils.h"

int main(void) {
    printf("=== Programme cross-compile ===\n");
    afficher_info_systeme();
    return 0;
}
