/* ============================================================================
   Section 3.6 : Symboles et table des symboles
   Description : Programme avec variable globale, fonction et main (3 symboles)
   Fichier source : 06-symboles.md
   ============================================================================ */
#include <stdio.h>

int compteur = 0;

void incrementer(void) {
    compteur++;
}

int main(void) {
    incrementer();
    printf("%d\n", compteur);
    return 0;
}
