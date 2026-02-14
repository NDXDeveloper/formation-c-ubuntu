/* ============================================================================
   Section 7.3 : L'arithmétique des pointeurs
   Description : Equivalence tableau[i] et *(tableau + i)
   Fichier source : 03-arithmetique-pointeurs.md
   ============================================================================ */

#include <stdio.h>

int main(void) {
    int tableau[5] = {10, 20, 30, 40, 50};

    printf("Notation avec crochets [] :\n");
    printf("tableau[0] = %d\n", tableau[0]);
    printf("tableau[1] = %d\n", tableau[1]);
    printf("tableau[2] = %d\n\n", tableau[2]);

    printf("Notation avec arithmetique de pointeurs :\n");
    printf("*(tableau + 0) = %d\n", *(tableau + 0));
    printf("*(tableau + 1) = %d\n", *(tableau + 1));
    printf("*(tableau + 2) = %d\n", *(tableau + 2));

    return 0;
}
