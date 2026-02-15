/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Detection du systeme d'exploitation via macros predefinies
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>

int main(void) {
#ifdef _WIN32
    printf("Système: Windows (32 ou 64 bits)\n");
#elif defined(__linux__)
    printf("Système: Linux\n");
#elif defined(__APPLE__) && defined(__MACH__)
    printf("Système: macOS\n");
#elif defined(__unix__)
    printf("Système: Unix\n");
#else
    printf("Système: Inconnu\n");
#endif

    return 0;
}
