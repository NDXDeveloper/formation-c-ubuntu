/* ============================================================================
   Section 6.3 : Passage par valeur
   Description : Echec de l'echange par passage par valeur (copies locales)
   Fichier source : 03-passage-par-valeur.md
   ============================================================================ */
#include <stdio.h>

void echanger_copie(int a, int b) {
    printf("  Avant échange : a=%d, b=%d\n", a, b);
    int temp = a;
    a = b;
    b = temp;
    printf("  Après échange : a=%d, b=%d\n", a, b);
}

int main(void) {
    int x = 10;
    int y = 20;

    printf("Avant l'appel : x=%d, y=%d\n", x, y);
    echanger_copie(x, y);
    printf("Après l'appel : x=%d, y=%d\n", x, y);

    return 0;
}
