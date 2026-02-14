/* ============================================================================
   Section 5.5 : Boucles
   Description : Nombres premiers jusqu'a N avec boucles imbriquees
   Fichier source : 05-boucles.md
   ============================================================================ */
#include <stdio.h>
#include <stdbool.h>

bool est_premier(int n) {
    if (n <= 1) {
        return false;
    }

    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            return false;
        }
    }

    return true;
}

int main(void) {
    int limite = 50;

    printf("Nombres premiers jusqu'à %d :\n", limite);

    for (int i = 2; i <= limite; i++) {
        if (est_premier(i)) {
            printf("%d ", i);
        }
    }

    printf("\n");
    return 0;
}
