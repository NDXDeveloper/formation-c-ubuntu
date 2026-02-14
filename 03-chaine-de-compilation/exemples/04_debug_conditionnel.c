/* ============================================================================
   Section 3.3 : Options de GCC
   Description : Compilation conditionnelle avec -DDEBUG
   Fichier source : 03-options-gcc.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    #ifdef DEBUG
    printf("Mode debug activé\n");
    #endif

    printf("Programme principal\n");
    return 0;
}
