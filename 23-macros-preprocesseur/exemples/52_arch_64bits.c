/* ============================================================================
   Section 23.4 : Macros cross-platform
   Description : Detection 32 vs 64 bits
   Fichier source : 04-macros-cross-platform.md
   ============================================================================ */

#include <stdio.h>
#include <stdint.h>

// Macro générique pour détecter 64 bits
#if defined(__LP64__) || defined(_WIN64) || \
    (defined(__WORDSIZE) && __WORDSIZE == 64)
    #define ARCH_64BIT
#else
    #define ARCH_32BIT
#endif

int main(void) {
#ifdef ARCH_64BIT
    printf("Architecture 64 bits détectée\n");
    printf("Utilisation de pointeurs 64 bits\n");
#else
    printf("Architecture 32 bits détectée\n");
    printf("Utilisation de pointeurs 32 bits\n");
#endif

    printf("sizeof(void*) = %zu\n", sizeof(void*));
    printf("sizeof(size_t) = %zu\n", sizeof(size_t));

    return 0;
}
