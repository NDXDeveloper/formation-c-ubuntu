/* ============================================================================
   Section 34.1.1 : Parsing d'arguments
   Description : Parsing avec getopt() - options courtes POSIX
   Fichier source : 01.1-parsing-arguments.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void afficher_aide(const char *nom_prog) {
    printf("Usage: %s [OPTIONS]\n", nom_prog);
    printf("Options:\n");
    printf("  -h          Afficher cette aide\n");
    printf("  -v          Mode verbose\n");
    printf("  -o FILE     Fichier de sortie\n");
    printf("  -n NUMBER   Nombre d'itérations\n");
}

int main(int argc, char *argv[]) {
    int verbose = 0;
    char *output_file = NULL;
    int iterations = 1;
    int opt;

    // Parsing des options
    while ((opt = getopt(argc, argv, "hvo:n:")) != -1) {
        switch (opt) {
            case 'h':
                afficher_aide(argv[0]);
                return 0;

            case 'v':
                verbose = 1;
                break;

            case 'o':
                output_file = optarg;
                break;

            case 'n':
                iterations = atoi(optarg);
                if (iterations <= 0) {
                    fprintf(stderr, "Erreur: -n doit être > 0\n");
                    return 1;
                }
                break;

            case '?':
                // Option inconnue ou argument manquant
                fprintf(stderr, "Essayez '%s -h' pour plus d'informations.\n", argv[0]);
                return 1;

            default:
                afficher_aide(argv[0]);
                return 1;
        }
    }

    // Traitement des arguments non-option (arguments positionnels)
    if (optind < argc) {
        printf("Arguments restants: ");
        for (int i = optind; i < argc; i++) {
            printf("%s ", argv[i]);
        }
        printf("\n");
    }

    // Utilisation des options parsées
    if (verbose) {
        printf("Mode verbose activé\n");
        printf("Fichier de sortie: %s\n", output_file ? output_file : "stdout");
        printf("Itérations: %d\n", iterations);
    }

    return 0;
}
