/* ============================================================================
   Section 31.4 : QEMU pour tester les binaires
   Description : Programme avec code de retour configurable (test QEMU)
   Fichier source : 04-qemu-tests.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <exit_code>\n", argv[0]);
        return 1;
    }

    int code = atoi(argv[1]);
    printf("Exiting with code: %d\n", code);
    return code;
}
