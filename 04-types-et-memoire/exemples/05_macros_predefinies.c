/* ============================================================================
   Section 4.3 : Les constantes et les litteraux
   Description : Constantes predefinies du preprocesseur (__FILE__, __LINE__, etc.)
   Fichier source : 03-constantes-litteraux.md
   ============================================================================ */
#include <stdio.h>

void afficher_debug(void) {
    printf("Fichier : %s\n", __FILE__);
    printf("Ligne : %d\n", __LINE__);
    printf("Fonction : %s\n", __func__);
    printf("Compilé le : %s à %s\n", __DATE__, __TIME__);
}

int main(void) {
    afficher_debug();
    return 0;
}
