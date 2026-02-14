/* ============================================================================
   Section 5.3 : Operateurs bit-a-bit
   Description : Compter les bits a 1 (methode simple et algorithme de Kernighan)
   Fichier source : 03-operateurs-bitwise.md
   ============================================================================ */
#include <stdio.h>

int compter_bits_a_un(unsigned int n) {
    int compteur = 0;
    while (n) {
        compteur += n & 1;
        n >>= 1;
    }
    return compteur;
}

int compter_bits_a_un_optimise(unsigned int n) {
    int compteur = 0;
    while (n) {
        n &= (n - 1);
        compteur++;
    }
    return compteur;
}

int main(void) {
    unsigned int nombre = 13;

    printf("Nombre : %u (binaire : ", nombre);
    for (int i = 7; i >= 0; i--) {
        printf("%d", (nombre >> i) & 1);
    }
    printf(")\n");

    printf("Nombre de bits à 1 : %d\n", compter_bits_a_un(nombre));
    printf("Nombre de bits à 1 (optimisé) : %d\n", compter_bits_a_un_optimise(nombre));

    return 0;
}
