/* ============================================================================
   Section 23.4 : Macros cross-platform
   Description : Detection et conversion d'endianness (boutisme)
   Fichier source : 04-macros-cross-platform.md
   ============================================================================ */

#include <stdio.h>
#include <stdint.h>

// Détection de l'endianness
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #define MY_IS_BIG_ENDIAN 1
    #define MY_IS_LITTLE_ENDIAN 0
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define MY_IS_BIG_ENDIAN 0
    #define MY_IS_LITTLE_ENDIAN 1
#else
    // Détection à l'exécution
    static inline int detect_little_endian(void) {
        uint32_t test = 0x01020304;
        return ((uint8_t*)&test)[0] == 0x04;
    }
    #define MY_IS_LITTLE_ENDIAN detect_little_endian()
#endif

// Fonctions de conversion d'endianness
#if defined(__GNUC__) || defined(__clang__)
    #define SWAP16(x) __builtin_bswap16(x)
    #define SWAP32(x) __builtin_bswap32(x)
    #define SWAP64(x) __builtin_bswap64(x)
#else
    // Implémentation portable
    static inline uint16_t swap16(uint16_t x) {
        return (uint16_t)((x >> 8) | (x << 8));
    }
    static inline uint32_t swap32(uint32_t x) {
        return ((x >> 24) & 0xff) |
               ((x >> 8) & 0xff00) |
               ((x << 8) & 0xff0000) |
               ((x << 24) & 0xff000000);
    }
    #define SWAP16(x) swap16(x)
    #define SWAP32(x) swap32(x)
#endif

int main(void) {
#if MY_IS_LITTLE_ENDIAN
    printf("Système Little Endian\n");
#elif MY_IS_BIG_ENDIAN
    printf("Système Big Endian\n");
#endif

    // Test de conversion
    uint32_t valeur = 0x12345678;
    printf("Valeur originale : 0x%08X\n", valeur);
    printf("Après swap : 0x%08X\n", SWAP32(valeur));

    return 0;
}
