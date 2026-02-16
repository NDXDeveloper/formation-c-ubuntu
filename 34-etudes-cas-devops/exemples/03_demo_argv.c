/* ============================================================================
   Section 34.1.1 : Parsing d'arguments
   Description : Demonstration de argc et argv
   Fichier source : 01.1-parsing-arguments.md
   ============================================================================ */
#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("Nombre d'arguments : %d\n", argc);

    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    return 0;
}
