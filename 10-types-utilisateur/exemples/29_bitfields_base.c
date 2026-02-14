/* ============================================================================
   Section 10.7 : Structures bit-fields
   Description : Drapeaux simples avec bit-fields (1 bit par flag)
   Fichier source : 07-bit-fields.md
   ============================================================================ */

#include <stdio.h>

struct Drapeaux {
    unsigned int flag1 : 1;
    unsigned int flag2 : 1;
    unsigned int flag3 : 1;
};

int main(void) {
    struct Drapeaux f = {0};

    printf("Taille : %zu octet(s)\n", sizeof(struct Drapeaux));

    f.flag1 = 1;
    f.flag2 = 0;
    f.flag3 = 1;

    printf("flag1 : %u\n", f.flag1);
    printf("flag2 : %u\n", f.flag2);
    printf("flag3 : %u\n", f.flag3);

    return 0;
}
