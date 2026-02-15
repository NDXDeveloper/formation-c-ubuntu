/* ============================================================================
   Section 25.4 : Assertions
   Description : Desactivation des assertions avec NDEBUG
   Fichier source : 04-assertions.md
   ============================================================================ */

#define NDEBUG  /* Desactive les assertions */
#include <assert.h>
#include <stdio.h>

int main(void) {
    int valeur = -10;

    /* Cette assertion est ignoree (code remplace par rien) */
    assert(valeur >= 0);

    printf("valeur = %d\n", valeur);  /* Affiche -10 */
    printf("Les assertions sont desactivees (NDEBUG defini)\n");

    return 0;
}
