/* ============================================================================
   Section 16.1 : Descripteurs de fichiers
   Description : Ecriture sur stdout et stderr avec write()
   Fichier source : 01-descripteurs-fichiers.md
   ============================================================================ */

#include <unistd.h>
#include <string.h>

int main(void) {
    const char *message = "Bonjour depuis stdout\n";
    const char *erreur = "Ceci est une erreur\n";

    /* Ecrire sur la sortie standard (fd = 1) */
    write(STDOUT_FILENO, message, strlen(message));

    /* Ecrire sur l'erreur standard (fd = 2) */
    write(STDERR_FILENO, erreur, strlen(erreur));

    return 0;
}
