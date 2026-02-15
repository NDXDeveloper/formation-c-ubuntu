/* ============================================================================
   Section 25.3 : Patterns de gestion d'erreurs
   Description : Wrapper Functions pour ouverture/fermeture de fichier
   Fichier source : 03-patterns-gestion-erreurs.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

FILE* xfopen(const char *chemin, const char *mode) {
    FILE *f = fopen(chemin, mode);

    if (f == NULL) {
        fprintf(stderr, "Erreur : impossible d'ouvrir '%s' en mode '%s': %s\n",
                chemin, mode, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return f;
}

void xfclose(FILE *f) {
    if (f != NULL && fclose(f) != 0) {
        fprintf(stderr, "Avertissement : fclose a echoue\n");
    }
}

int main(void) {
    FILE *f = xfopen("/tmp/wrapper_test.txt", "w");

    fprintf(f, "Hello, World!\n");

    xfclose(f);

    printf("Fichier ecrit avec succes\n");

    /* Relire pour verifier */
    f = xfopen("/tmp/wrapper_test.txt", "r");
    char buffer[256];
    if (fgets(buffer, (int)sizeof(buffer), f) != NULL) {
        printf("Contenu lu : %s", buffer);
    }
    xfclose(f);

    /* Nettoyage */
    remove("/tmp/wrapper_test.txt");

    return EXIT_SUCCESS;
}
