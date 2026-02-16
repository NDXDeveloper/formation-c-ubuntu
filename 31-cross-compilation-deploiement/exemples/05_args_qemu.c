/* ============================================================================
   Section 31.4 : QEMU pour tester les binaires
   Description : Programme avec arguments en ligne de commande (test QEMU)
   Fichier source : 04-qemu-tests.md
   ============================================================================ */
#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("Nombre d'arguments : %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("  argv[%d] = %s\n", i, argv[i]);
    }
    return 0;
}
