/* ============================================================================
   Section 5.5 : Boucles
   Description : Puissances de 2 inferieures a 1000 avec while
   Fichier source : 05-boucles.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    int puissance = 1;
    int limite = 1000;

    printf("Puissances de 2 inférieures à %d :\n", limite);

    while (puissance < limite) {
        printf("%d ", puissance);
        puissance *= 2;
    }

    printf("\n");
    return 0;
}
