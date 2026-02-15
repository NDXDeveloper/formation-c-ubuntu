/* ============================================================================
   Section 23.6 : Macros predefinies utiles
   Description : __func__ - nom de la fonction courante (C99)
   Fichier source : 06-macros-predefinies.md
   ============================================================================ */

#include <stdio.h>

void fonction_exemple(void) {
    printf("Dans la fonction : %s\n", __func__);
}

int main(void) {
    printf("Dans la fonction : %s\n", __func__);
    fonction_exemple();

    return 0;
}
