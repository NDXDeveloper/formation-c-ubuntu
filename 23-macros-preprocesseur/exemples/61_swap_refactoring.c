/* ============================================================================
   Section 23.5 : Dangers et pieges des macros
   Description : Refactoring macro vers fonction - swap generique
   Fichier source : 05-dangers-macros.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>

// Fonction générique avec void*
void swap_generic(void* a, void* b, size_t taille) {
    unsigned char temp[taille];  // VLA (C99)
    memcpy(temp, a, taille);
    memcpy(a, b, taille);
    memcpy(b, temp, taille);
}

// Macro wrapper type-safe
#define SWAP(a, b) swap_generic(&(a), &(b), sizeof(a))

int main(void) {
    int x = 5, y = 10;
    SWAP(x, y);
    printf("int: x = %d, y = %d\n", x, y);

    double d1 = 3.14, d2 = 2.71;
    SWAP(d1, d2);
    printf("double: d1 = %.2f, d2 = %.2f\n", d1, d2);

    return 0;
}
