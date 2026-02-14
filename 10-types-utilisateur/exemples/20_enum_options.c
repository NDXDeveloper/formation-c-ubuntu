/* ============================================================================
   Section 10.5 : enum - Code lisible et gestion d'etats
   Description : Options de configuration avec enum et operateurs bit-a-bit
   Fichier source : 05-enumerations.md
   ============================================================================ */

#include <stdio.h>

enum Options {
    OPTION_VERBOSE      = 0x01,
    OPTION_DEBUG        = 0x02,
    OPTION_COULEUR      = 0x04,
    OPTION_COMPRESSION  = 0x08
};

int main(void) {
    unsigned int config = 0;

    config |= OPTION_VERBOSE;
    config |= OPTION_DEBUG;

    if (config & OPTION_VERBOSE) {
        printf("Mode verbose active\n");
    }

    if (config & OPTION_DEBUG) {
        printf("Mode debug active\n");
    }

    if (config & OPTION_COULEUR) {
        printf("Couleurs activees\n");
    } else {
        printf("Couleurs desactivees\n");
    }

    return 0;
}
