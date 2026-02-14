/* ============================================================================
   Section 10.2 : Alignement memoire et padding
   Description : Impact de l'ordre des membres sur la taille
   Fichier source : 02-alignement-padding.md
   ============================================================================ */

#include <stdio.h>

struct MauvaisOrdre {
    char c1;
    int i;
    char c2;
};

struct BonOrdre {
    char c1;
    char c2;
    int i;
};

int main(void) {
    printf("MauvaisOrdre : %zu octets\n", sizeof(struct MauvaisOrdre));
    printf("BonOrdre     : %zu octets\n", sizeof(struct BonOrdre));
    return 0;
}
