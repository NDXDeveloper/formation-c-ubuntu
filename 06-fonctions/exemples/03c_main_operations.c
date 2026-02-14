/* ============================================================================
   Section 6.1 : Declaration, definition et prototypes
   Description : Compilation separee - programme principal utilisant les operations
   Fichier source : 01-declaration-definition.md
   ============================================================================ */
#include <stdio.h>
#include "03a_operations.h"

int main(void) {
    int x = 10, y = 5;

    printf("%d + %d = %d\n", x, y, additionner(x, y));
    printf("%d - %d = %d\n", x, y, soustraire(x, y));
    printf("%d * %d = %d\n", x, y, multiplier(x, y));

    return 0;
}
