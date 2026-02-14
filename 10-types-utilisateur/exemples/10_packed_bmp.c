/* ============================================================================
   Section 10.3 : Packed structures
   Description : Verification des tailles d'en-tetes BMP avec pragma pack
   Fichier source : 03-packed-structures.md
   ============================================================================ */

#include <stdio.h>
#include <stdint.h>

#pragma pack(push, 1)

struct BMPFileHeader {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
};

struct BMPInfoHeader {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
};

#pragma pack(pop)

int main(void) {
    printf("=== VERIFICATION DES TAILLES ===\n\n");

    printf("BMPFileHeader : %zu octets (attendu : 14)\n", sizeof(struct BMPFileHeader));
    printf("BMPInfoHeader : %zu octets (attendu : 16)\n", sizeof(struct BMPInfoHeader));

    if (sizeof(struct BMPFileHeader) == 14 && sizeof(struct BMPInfoHeader) == 16) {
        printf("\nLes structures sont correctement packed !\n");
        printf("Elles correspondent au format BMP standard.\n");
    } else {
        printf("\nProbleme de packing detecte !\n");
    }

    return 0;
}
