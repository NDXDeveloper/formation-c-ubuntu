/* ============================================================================
   Section 03.1 : Core dumps
   Description : Programme avec pile d'appels provoquant un segfault
   Fichier source : 03.1-core-dumps.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

void fonction_niveau_3(void) {
    int *ptr = NULL;
    printf("Je vais crasher...\n");
    *ptr = 42;  /* Segfault ! */
}

void fonction_niveau_2(void) {
    fonction_niveau_3();
}

void fonction_niveau_1(void) {
    fonction_niveau_2();
}

int main(void) {
    printf("Début du programme\n");
    fonction_niveau_1();
    printf("Cette ligne ne sera jamais exécutée\n");
    return 0;
}
