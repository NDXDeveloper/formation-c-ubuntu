/* ============================================================================
   Section 34.1.1 : Parsing d'arguments
   Description : Parsing avec getopt_long() - options courtes et longues
   Fichier source : 01.1-parsing-arguments.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

void afficher_aide(const char *nom_prog) {
    printf("Usage: %s [OPTIONS]\n", nom_prog);
    printf("Options:\n");
    printf("  -h, --help              Afficher cette aide\n");
    printf("  -v, --verbose           Mode verbose\n");
    printf("  -o, --output FILE       Fichier de sortie\n");
    printf("  -n, --iterations NUM    Nombre d'itérations\n");
    printf("      --version           Afficher la version\n");
}

int main(int argc, char *argv[]) {
    int verbose = 0;
    char *output_file = NULL;
    int iterations = 1;

    // Définition des options longues
    static struct option long_options[] = {
        {"help",       no_argument,       0, 'h'},
        {"verbose",    no_argument,       0, 'v'},
        {"output",     required_argument, 0, 'o'},
        {"iterations", required_argument, 0, 'n'},
        {"version",    no_argument,       0, 0},
        {0, 0, 0, 0}  // Terminateur obligatoire
    };

    int opt;
    int option_index = 0;

    // Parsing avec getopt_long
    while ((opt = getopt_long(argc, argv, "hvo:n:",
                              long_options, &option_index)) != -1) {
        switch (opt) {
            case 0:
                // Option longue sans équivalent court
                if (strcmp(long_options[option_index].name, "version") == 0) {
                    printf("Version 1.0.0\n");
                    return 0;
                }
                break;

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
                    fprintf(stderr, "Erreur: --iterations doit être > 0\n");
                    return 1;
                }
                break;

            case '?':
                fprintf(stderr, "Essayez '%s --help' pour plus d'informations.\n",
                        argv[0]);
                return 1;

            default:
                return 1;
        }
    }

    // Vérifications post-parsing
    if (output_file == NULL) {
        fprintf(stderr, "Erreur: l'option -o/--output est obligatoire\n");
        return 1;
    }

    // Utilisation
    if (verbose) {
        printf("Configuration:\n");
        printf("  Fichier de sortie: %s\n", output_file);
        printf("  Itérations: %d\n", iterations);
    }

    return 0;
}
