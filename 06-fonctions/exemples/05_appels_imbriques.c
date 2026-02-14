/* ============================================================================
   Section 6.2 : La Pile - Stack Frames
   Description : Appels de fonctions imbriques (main -> calculer -> multiplier)
   Fichier source : 02-stack-frames.md
   ============================================================================ */
#include <stdio.h>

int multiplier(int a, int b) {
    int produit = a * b;
    return produit;
}

int calculer(int x, int y) {
    int somme = x + y;
    int triple = multiplier(somme, 3);
    return triple;
}

int main(void) {
    int resultat = calculer(4, 6);
    printf("Résultat = %d\n", resultat);
    return 0;
}
