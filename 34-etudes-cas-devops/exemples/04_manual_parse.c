/* ============================================================================
   Section 34.1.1 : Parsing d'arguments
   Description : Parsing manuel basique avec strcmp
   Fichier source : 01.1-parsing-arguments.md
   ============================================================================ */
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int verbose = 0;

    // Parcourir les arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [-v|--verbose] [-h|--help]\n", argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Option inconnue : %s\n", argv[i]);
            return 1;
        }
    }

    if (verbose) {
        printf("Mode verbose activé\n");
    }

    return 0;
}
