/* ============================================================================
   Section 4.2 : Signed vs Unsigned
   Description : Exemple complet et comparatif signed/unsigned avec underflow
   Fichier source : 02-signed-vs-unsigned.md
   ============================================================================ */
#include <stdio.h>
#include <stdint.h>
#include <limits.h>

int main(void) {
    /* Types classiques */
    int negatif = -100;
    unsigned int positif = 100;

    /* Underflow demonstration */
    unsigned int x = 5;
    unsigned int y = 10;
    unsigned int resultat = x - y;

    /* Types a largeur fixe */
    int32_t signe32 = -2000000000;
    uint32_t non_signe32 = 4000000000U;

    printf("=== Types classiques ===\n");
    printf("Négatif (int) : %d\n", negatif);
    printf("Positif (unsigned int) : %u\n", positif);

    printf("\n=== Underflow ===\n");
    printf("5 - 10 en unsigned : %u\n", resultat);
    printf("(devrait être -5, mais unsigned boucle)\n");

    printf("\n=== Types à largeur fixe ===\n");
    printf("int32_t : %d\n", signe32);
    printf("uint32_t : %u\n", non_signe32);

    printf("\n=== Limites ===\n");
    printf("INT_MAX : %d\n", INT_MAX);
    printf("UINT_MAX : %u\n", UINT_MAX);

    return 0;
}
