/* ============================================================================
   Section 04.1 : Memcheck
   Description : Fuite memoire simple detectee par Valgrind
   Fichier source : 04.1-memcheck.md
   ============================================================================ */

#include <stdlib.h>

int main(void) {
    int *tableau = malloc(10 * sizeof(int));

    tableau[0] = 42;

    /* Oups, on oublie de liberer la memoire ! */
    return 0;
}
