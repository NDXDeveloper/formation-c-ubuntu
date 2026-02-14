/* ============================================================================
   Section 04.1 : Memcheck
   Description : Acces hors limites detecte par Valgrind
   Fichier source : 04.1-memcheck.md
   ============================================================================ */

#include <stdlib.h>

int main(void) {
    int *array = malloc(10 * sizeof(int));

    for (int i = 0; i <= 10; i++) {  /* Bug : devrait etre i < 10 */
        array[i] = i * 2;
    }

    free(array);
    return 0;
}
