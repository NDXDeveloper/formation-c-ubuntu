/* ============================================================================
   Section 7.1 : Concept d'adresse mémoire
   Description : Adresses de plusieurs variables en mémoire
   Fichier source : 01-adresse-memoire.md
   ============================================================================ */

#include <stdio.h>

int main(void) {
    int a = 10;
    int b = 20;
    int c = 30;

    printf("Adresse de a : %p (valeur : %d)\n", (void*)&a, a);
    printf("Adresse de b : %p (valeur : %d)\n", (void*)&b, b);
    printf("Adresse de c : %p (valeur : %d)\n", (void*)&c, c);

    return 0;
}
