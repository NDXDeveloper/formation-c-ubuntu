/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Detection de l'architecture processeur
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>

int main(void) {
    printf("Architecture: ");

#if defined(__x86_64__) || defined(_M_X64)
    printf("x86_64 (64 bits)\n");
#elif defined(__i386__) || defined(_M_IX86)
    printf("x86 (32 bits)\n");
#elif defined(__aarch64__) || defined(_M_ARM64)
    printf("ARM64 (64 bits)\n");
#elif defined(__arm__) || defined(_M_ARM)
    printf("ARM (32 bits)\n");
#else
    printf("Inconnue\n");
#endif

    printf("Taille du pointeur: %zu octets\n", sizeof(void*));

    return 0;
}
