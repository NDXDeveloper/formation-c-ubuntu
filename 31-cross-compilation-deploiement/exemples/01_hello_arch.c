/* ============================================================================
   Section 31.2 : Compiler pour ARM
   Description : Hello world avec detection d'architecture via preprocesseur
   Fichier source : 02-compiler-arm.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    printf("Hello from ARM!\n");
    printf("Running on %s architecture\n",
#ifdef __aarch64__
        "ARM 64 bits (aarch64)"
#elif __arm__
        "ARM 32 bits (armv7)"
#else
        "Unknown"
#endif
    );
    return 0;
}
