/* ============================================================================
   Section 6.5 : La fonction main et ses parametres
   Description : Parsing d'options avec getopt (POSIX)
   Fichier source : 05-fonction-main.md
   ============================================================================ */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int verbose = 0;
    char *output_file = NULL;
    int count = 10;
    int opt;

    while ((opt = getopt(argc, argv, "vo:n:h")) != -1) {
        switch (opt) {
            case 'v':
                verbose = 1;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'n':
                count = atoi(optarg);
                break;
            case 'h':
                printf("Usage : %s [-v] [-o fichier] [-n nombre] [-h]\n", argv[0]);
                return 0;
            case '?':
                return 1;
            default:
                return 1;
        }
    }

    printf("Verbose : %s\n", verbose ? "oui" : "non");
    printf("Fichier : %s\n", output_file ? output_file : "(aucun)");
    printf("Count : %d\n", count);

    printf("Arguments restants : ");
    for (int i = optind; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");

    return 0;
}
