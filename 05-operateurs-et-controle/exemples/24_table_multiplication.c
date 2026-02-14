/* ============================================================================
   Section 5.5 : Boucles
   Description : Table de multiplication avec boucles imbriquees
   Fichier source : 05-boucles.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    printf("Table de multiplication de 1 à 5 :\n\n");
    printf("    ");

    for (int j = 1; j <= 5; j++) {
        printf("%4d", j);
    }
    printf("\n");
    printf("   --------------------\n");

    for (int i = 1; i <= 5; i++) {
        printf("%2d |", i);
        for (int j = 1; j <= 5; j++) {
            printf("%4d", i * j);
        }
        printf("\n");
    }

    return 0;
}
