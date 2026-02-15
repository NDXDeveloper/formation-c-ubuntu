/* ============================================================================
   Section 23.4 : Macros cross-platform
   Description : Utilisation du header platform.h centralise
   Fichier source : 04-macros-cross-platform.md
   ============================================================================ */

#include <stdio.h>
#include "platform.h"

int main(void) {
#ifdef OS_WINDOWS
    printf("Code spécifique Windows\n");
#endif

#ifdef OS_UNIX_LIKE
    printf("Code pour systèmes Unix-like\n");
#endif

#ifdef COMPILER_GCC
    printf("Compilé avec GCC\n");
#elif defined(COMPILER_CLANG)
    printf("Compilé avec Clang\n");
#endif

    printf("Séparateur de chemin : '%c'\n", PATH_SEP);

    return 0;
}
