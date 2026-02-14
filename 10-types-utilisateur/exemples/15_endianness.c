/* ============================================================================
   Section 10.4 : union - Optimisation memoire
   Description : Detection de l'endianness via union
   Fichier source : 04-unions.md
   ============================================================================ */

#include <stdio.h>
#include <stdint.h>

union Endian {
    uint32_t value;
    uint8_t bytes[4];
};

void afficher_endianness(void) {
    union Endian test;
    test.value = 0x12345678;

    printf("Valeur : 0x%08X\n", test.value);
    printf("Octets : ");
    for (int i = 0; i < 4; i++) {
        printf("%02X ", test.bytes[i]);
    }
    printf("\n");

    if (test.bytes[0] == 0x78) {
        printf("Architecture : Little Endian\n");
    } else {
        printf("Architecture : Big Endian\n");
    }
}

int main(void) {
    afficher_endianness();
    return 0;
}
