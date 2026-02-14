/* ============================================================================
   Section 5.3 : Operateurs bit-a-bit
   Description : Echange de variables avec XOR (sans variable temporaire)
   Fichier source : 03-operateurs-bitwise.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    int a = 10;
    int b = 20;

    printf("Avant : a=%d, b=%d\n", a, b);

    a = a ^ b;
    b = a ^ b;
    a = a ^ b;

    printf("Après : a=%d, b=%d\n", a, b);

    return 0;
}
