/* ============================================================================
   Section 04.1 : Memcheck
   Description : Double free detecte par Valgrind
   Fichier source : 04.1-memcheck.md
   ============================================================================ */

#include <stdlib.h>

int main(void) {
    char *buffer = malloc(100);

    buffer[0] = 'A';

    free(buffer);
    free(buffer);  /* Double free */

    return 0;
}
