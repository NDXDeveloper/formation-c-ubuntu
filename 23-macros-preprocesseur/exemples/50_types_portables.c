/* ============================================================================
   Section 23.4 : Macros cross-platform
   Description : Types de donnees portables avec stdint.h
   Fichier source : 04-macros-cross-platform.md
   ============================================================================ */

#include <stdio.h>
#include <stdint.h>

// Fonction non portable : int peut être 16, 32 ou 64 bits
void fonction_non_portable(int valeur) {
    (void)valeur;
    printf("Taille de int : %zu octets\n", sizeof(int));
}

// Fonction portable : int32_t est toujours 4 octets
void fonction_portable(int32_t valeur) {
    (void)valeur;
    printf("Taille de int32_t : %zu octets (toujours 4)\n", sizeof(int32_t));
}

int main(void) {
    // Types portables de stdint.h
    int8_t   petit = 127;
    uint8_t  u_petit = 255;
    int16_t  moyen = 32767;
    int32_t  grand = 2147483647;
    int64_t  tres_grand = 9223372036854775807LL;

    (void)petit;
    (void)u_petit;
    (void)moyen;
    (void)grand;
    (void)tres_grand;

    printf("Types à taille fixe :\n");
    printf("int8_t  : %zu octets\n", sizeof(int8_t));
    printf("int16_t : %zu octets\n", sizeof(int16_t));
    printf("int32_t : %zu octets\n", sizeof(int32_t));
    printf("int64_t : %zu octets\n", sizeof(int64_t));

    fonction_non_portable(42);
    fonction_portable(42);

    return 0;
}
