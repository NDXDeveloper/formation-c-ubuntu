/* ============================================================================
   Section 6.3 : Passage par valeur
   Description : Demonstration que la modification d'un parametre n'affecte pas l'original
   Fichier source : 03-passage-par-valeur.md
   ============================================================================ */
#include <stdio.h>

void modifier(int x) {
    printf("  Dans modifier, avant : x = %d\n", x);
    x = 100;
    printf("  Dans modifier, après : x = %d\n", x);
}

int main(void) {
    int nombre = 42;

    printf("Avant l'appel : nombre = %d\n", nombre);
    modifier(nombre);
    printf("Après l'appel : nombre = %d\n", nombre);

    return 0;
}
