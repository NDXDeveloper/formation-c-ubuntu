/* ============================================================================
   Section 30.1.8 : Static analysis
   Description : Code avec problemes intentionnels (pour analyse statique)
   Fichier source : 01.8-static-analysis.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>

int calculate(int x) {
    int result;  /* Variable non initialisee ! */

    if (x == 0) {
        return 0;
    }

    result = 100 / x;
    return result;
}

int main(void) {
    int *ptr = malloc(10 * sizeof(int));
    /* Oups ! Pas de verification de NULL */

    ptr[0] = calculate(5);

    printf("Result: %d\n", ptr[0]);

    /* Oups ! Pas de free() */
    return 0;
}
