/* ============================================================================
   Section 28.5 : extern "C" pour C++
   Description : Bibliotheque C legacy - fonctions utilisees depuis C++
   Fichier source : 05-extern-c.md
   ============================================================================ */

#include "legacy.h"
#include <stdio.h>

void ancienne_fonction(int x) {
    printf("Fonction C legacy : %d\n", x);
}

int calcul_complexe(int a, int b) {
    return a * a + b * b;
}
