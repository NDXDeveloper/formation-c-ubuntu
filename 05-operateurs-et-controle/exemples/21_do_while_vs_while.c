/* ============================================================================
   Section 5.5 : Boucles
   Description : Comparaison do-while vs while (execution au moins une fois)
   Fichier source : 05-boucles.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    int a = 10;

    printf("Boucle while :\n");
    while (a < 5) {
        printf("Cette ligne ne s'affichera pas\n");
        a++;
    }

    printf("Boucle do-while :\n");
    do {
        printf("Cette ligne s'affiche une fois\n");
        a++;
    } while (a < 5);

    return 0;
}
