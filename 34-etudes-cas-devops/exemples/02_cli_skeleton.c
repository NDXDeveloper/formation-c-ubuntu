/* ============================================================================
   Section 34.1 : Création d'un outil CLI système
   Description : Squelette complet d'outil CLI avec getopt_long
   Fichier source : 01-outil-cli-systeme.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#define VERSION "1.0.0"

static int verbose = 0;
static char *output_file = NULL;

void afficher_version(const char *nom_prog) {
    printf("%s version %s\n", nom_prog, VERSION);
}

void afficher_aide(const char *nom_prog) {
    printf("Usage: %s [OPTIONS] FILE...\n\n", nom_prog);
    printf("Description:\n");
    printf("  Votre description ici.\n\n");
    printf("Options:\n");
    printf("  -h, --help              Afficher cette aide\n");
    printf("  -V, --version           Afficher la version\n");
    printf("  -v, --verbose           Mode verbose\n");
    printf("  -o, --output FILE       Fichier de sortie (defaut: stdout)\n");
    printf("\nExemples:\n");
    printf("  %s fichier.txt\n", nom_prog);
    printf("  %s -v -o resultat.txt input.dat\n", nom_prog);
}

int traiter_fichier(const char *chemin) {
    if (verbose) {
        fprintf(stderr, "Traitement de %s...\n", chemin);
    }

    FILE *fp = fopen(chemin, "r");
    if (fp == NULL) {
        fprintf(stderr, "Erreur: impossible d'ouvrir '%s'\n", chemin);
        return -1;
    }

    fclose(fp);
    return 0;
}

int main(int argc, char *argv[]) {
    static struct option long_options[] = {
        {"help",    no_argument,       0, 'h'},
        {"version", no_argument,       0, 'V'},
        {"verbose", no_argument,       0, 'v'},
        {"output",  required_argument, 0, 'o'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "hVvo:",
                              long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                afficher_aide(argv[0]);
                return 0;
            case 'V':
                afficher_version(argv[0]);
                return 0;
            case 'v':
                verbose = 1;
                break;
            case 'o':
                output_file = optarg;
                break;
            case '?':
                fprintf(stderr, "Essayez '%s --help' pour plus d'informations.\n",
                        argv[0]);
                return 1;
            default:
                return 1;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Erreur: aucun fichier specifie\n");
        fprintf(stderr, "Essayez '%s --help' pour plus d'informations.\n", argv[0]);
        return 1;
    }

    (void)output_file;

    int erreurs = 0;
    for (int i = optind; i < argc; i++) {
        if (traiter_fichier(argv[i]) != 0) {
            erreurs++;
        }
    }

    return erreurs > 0 ? 1 : 0;
}
