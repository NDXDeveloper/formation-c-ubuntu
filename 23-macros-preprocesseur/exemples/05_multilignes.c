/* ============================================================================
   Section 23.1 : Macros parametrees
   Description : Macro multi-lignes avec idiome do-while(0)
   Fichier source : 01-macros-parametrees.md
   ============================================================================ */

#include <stdio.h>

#define AFFICHER_INFOS(nom, age) \
    do { \
        printf("Nom : %s\n", nom); \
        printf("Âge : %d\n", age); \
    } while(0)

int main(void) {
    AFFICHER_INFOS("Alice", 30);
    // Affiche :
    // Nom : Alice
    // Âge : 30

    return 0;
}
