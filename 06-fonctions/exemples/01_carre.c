/* ============================================================================
   Section 6.1 : Declaration, definition et prototypes
   Description : Prototypes et fonctions simples (carre et afficher_carre)
   Fichier source : 01-declaration-definition.md
   ============================================================================ */
#include <stdio.h>

// Prototypes
int carre(int n);
void afficher_carre(int n);

int main(void) {
    int nombre = 7;
    afficher_carre(nombre);
    return 0;
}

// Definition de carre
int carre(int n) {
    return n * n;
}

// Definition de afficher_carre
void afficher_carre(int n) {
    printf("Le carré de %d est %d\n", n, carre(n));
}
