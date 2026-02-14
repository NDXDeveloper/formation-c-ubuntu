/* ============================================================================
   Section 07 : Symbol visibility
   Description : Programme testant la bibliotheque avec visibilite des symboles
   Fichier source : 07-symbol-visibility.md
   ============================================================================ */

#include <stdio.h>
#include "mylib.h"

int main(void) {
    printf("Version de la bibliotheque : %d\n", mylib_get_version());
    mylib_init();
    mylib_process("Hello visibility");

    return 0;
}
