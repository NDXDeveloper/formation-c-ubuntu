/* ============================================================================
   Section 07.1 : gcov
   Description : Mesure de couverture de code avec gcov
   Fichier source : 07.1-gcov.md
   ============================================================================ */

#include <stdio.h>

void print_hello(void) {
    printf("Hello, World!\n");
}

void print_goodbye(void) {
    printf("Goodbye, World!\n");
}

int main(void) {
    print_hello();
    /* print_goodbye() n'est jamais appelee */
    return 0;
}
