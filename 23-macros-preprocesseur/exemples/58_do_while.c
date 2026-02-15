/* ============================================================================
   Section 23.5 : Dangers et pieges des macros
   Description : Idiome do-while(0) pour macros multi-instructions
   Fichier source : 05-dangers-macros.md
   ============================================================================ */

#include <stdio.h>

// Macro correcte avec do-while(0)
#define AFFICHER_DEBUG(msg) \
    do { \
        printf("DEBUG: %s\n", msg); \
        printf("Fichier: %s\n", __FILE__); \
    } while(0)

int main(void) {
    int erreur = 1;

    if (erreur)
        AFFICHER_DEBUG("Une erreur est survenue");
    else
        printf("Tout va bien\n");

    return 0;
}
