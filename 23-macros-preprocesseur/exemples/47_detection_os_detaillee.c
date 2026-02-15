/* ============================================================================
   Section 23.4 : Macros cross-platform
   Description : Detection detaillee du systeme d'exploitation
   Fichier source : 04-macros-cross-platform.md
   ============================================================================ */

#include <stdio.h>

int main(void) {
    printf("Système d'exploitation détecté : ");

#if defined(_WIN32) || defined(_WIN64)
    printf("Windows\n");
#elif defined(__APPLE__) && defined(__MACH__)
    printf("macOS\n");
#elif defined(__linux__)
    printf("Linux\n");
#elif defined(__FreeBSD__)
    printf("FreeBSD\n");
#elif defined(__unix__) || defined(__unix)
    printf("Unix (autre)\n");
#else
    printf("Inconnu\n");
#endif

    return 0;
}
