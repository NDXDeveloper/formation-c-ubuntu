/* ============================================================================
   Section 5.3 : Operateurs bit-a-bit
   Description : Extraction des composantes RGB avec AND et decalage
   Fichier source : 03-operateurs-bitwise.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    unsigned int couleur_rgb = 0xFF3A7C;

    unsigned int rouge = (couleur_rgb >> 16) & 0xFF;
    unsigned int vert = (couleur_rgb >> 8) & 0xFF;
    unsigned int bleu = couleur_rgb & 0xFF;

    printf("Rouge: %u, Vert: %u, Bleu: %u\n", rouge, vert, bleu);

    return 0;
}
