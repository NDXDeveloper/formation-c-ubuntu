/* ============================================================================
   Section 4.5 : Endianness et portabilite
   Description : Detection de l'endianness avec un pointeur
   Fichier source : 05-endianness.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    int nombre = 0x12345678;
    unsigned char* ptr = (unsigned char*)&nombre;

    printf("Octets en mémoire : ");
    for (size_t i = 0; i < sizeof(int); i++) {
        printf("0x%02X ", ptr[i]);
    }
    printf("\n");

    if (ptr[0] == 0x78) {
        printf("Little-Endian\n");
    } else {
        printf("Big-Endian\n");
    }

    return 0;
}
