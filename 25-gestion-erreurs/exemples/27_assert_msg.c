/* ============================================================================
   Section 25.4 : Assertions
   Description : Assertions avec messages personnalises (operateur &&)
                 Crash intentionnel pedagogique
   Fichier source : 04-assertions.md
   ============================================================================ */

#include <assert.h>

int main(void) {
    int valeur = -5;

    /* Message personnalise via l'operateur && */
    assert(valeur >= 0 && "La valeur doit etre positive");

    return 0;
}
