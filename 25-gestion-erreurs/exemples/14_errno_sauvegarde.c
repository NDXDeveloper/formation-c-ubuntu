/* ============================================================================
   Section 25.2 : La variable errno
   Description : Sauvegarder errno avant d'appeler d'autres fonctions
   Fichier source : 02-errno.md
   ============================================================================ */

#include <stdio.h>
#include <errno.h>
#include <string.h>

void log_erreur(const char *message) {
    /* Cette fonction pourrait modifier errno */
    fprintf(stderr, "[LOG] %s\n", message);
}

int main(void) {
    FILE *fichier = fopen("inexistant.txt", "r");

    if (fichier == NULL) {
        int erreur_sauvegardee = errno;  /* Sauvegarde */

        log_erreur("Tentative d'ouverture echouee");

        /* Utilisation de la valeur sauvegardee */
        fprintf(stderr, "Code erreur : %s\n", strerror(erreur_sauvegardee));
    }

    return 0;
}
