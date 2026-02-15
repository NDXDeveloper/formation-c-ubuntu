/* ============================================================================
   Section 23.5 : Dangers et pieges des macros
   Description : BUG INTENTIONNEL - MAX appelle la fonction deux fois
   Fichier source : 05-dangers-macros.md
   ============================================================================ */

#include <stdio.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int fonction_avec_effet(void) {
    static int appels = 0;
    printf("Fonction appelée %d fois\n", ++appels);
    return appels;
}

int main(void) {
    printf("Résultat : %d\n", MAX(fonction_avec_effet(), 0));
    /* La fonction est appelée DEUX fois ! */

    return 0;
}
