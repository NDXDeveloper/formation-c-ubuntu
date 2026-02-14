/* ============================================================================
   Section 4.5 : Endianness et portabilite
   Description : Detection de l'endianness avec une union
   Fichier source : 05-endianness.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    union {
        int entier;
        unsigned char octets[4];
    } test;

    test.entier = 0x12345678;

    printf("Premier octet en mémoire : 0x%02X\n", test.octets[0]);

    if (test.octets[0] == 0x78) {
        printf("Système Little-Endian\n");
    } else if (test.octets[0] == 0x12) {
        printf("Système Big-Endian\n");
    } else {
        printf("Endianness inconnue\n");
    }

    return 0;
}
