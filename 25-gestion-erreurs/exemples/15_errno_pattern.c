/* ============================================================================
   Section 25.2 : La variable errno
   Description : Pattern recommande pour gestion d'erreurs avec errno
   Fichier source : 02-errno.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int ouvrir_fichier(const char *chemin, FILE **fichier_out) {
    *fichier_out = fopen(chemin, "r");

    if (*fichier_out == NULL) {
        /* Sauvegarde immediate de errno */
        int erreur = errno;

        /* Message d'erreur personnalise selon le code */
        switch (erreur) {
            case ENOENT:
                fprintf(stderr, "Le fichier '%s' n'existe pas\n", chemin);
                break;
            case EACCES:
                fprintf(stderr, "Permission refusee pour '%s'\n", chemin);
                break;
            default:
                fprintf(stderr, "Impossible d'ouvrir '%s': %s\n",
                        chemin, strerror(erreur));
        }

        return -1;  /* Echec */
    }

    return 0;  /* Succes */
}

int main(void) {
    FILE *fichier;

    printf("Test 1 : fichier inexistant\n");
    if (ouvrir_fichier("data.txt", &fichier) != 0) {
        printf("Echec attendu\n\n");
    }

    printf("Test 2 : fichier existant\n");
    if (ouvrir_fichier("/etc/hosts", &fichier) == 0) {
        printf("Fichier ouvert avec succes\n");
        fclose(fichier);
    }

    return EXIT_SUCCESS;
}
