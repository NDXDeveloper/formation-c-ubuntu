/* ============================================================================
   Section 23.5 : Dangers et pieges des macros
   Description : BUG INTENTIONNEL - comparaison signed/unsigned avec MAX
   Fichier source : 05-dangers-macros.md
   ============================================================================ */

#include <stdio.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int main(void) {
    int i = -1;
    unsigned int u = 1;

    // Comportement inattendu !
    printf("MAX(-1, 1u) = %u\n", MAX(i, u));
    // -1 converti en unsigned = très grand nombre
    // Donc MAX retourne -1 (interprété comme unsigned)

    return 0;
}
