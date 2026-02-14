/* ============================================================================
   Section 5.3 : Operateurs bit-a-bit
   Description : Verifier si un nombre est une puissance de 2 avec n & (n-1)
   Fichier source : 03-operateurs-bitwise.md
   ============================================================================ */
#include <stdio.h>

int est_puissance_de_deux(unsigned int n) {
    return (n != 0) && ((n & (n - 1)) == 0);
}

int main(void) {
    unsigned int nombres[] = {1, 2, 3, 4, 5, 8, 15, 16, 32, 100};
    int taille = (int)(sizeof(nombres) / sizeof(nombres[0]));

    for (int i = 0; i < taille; i++) {
        if (est_puissance_de_deux(nombres[i])) {
            printf("%u est une puissance de 2\n", nombres[i]);
        } else {
            printf("%u n'est pas une puissance de 2\n", nombres[i]);
        }
    }

    return 0;
}
