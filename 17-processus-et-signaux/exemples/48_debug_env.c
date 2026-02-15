/* ============================================================================
   Section 17.4 : Variables d'environnement
   Description : Mode debug active par variable d'environnement
   Fichier source : 04-variables-environnement.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_debug_enabled(void) {
    char *debug = getenv("DEBUG");
    return (debug != NULL &&
            (strcmp(debug, "1") == 0 ||
             strcmp(debug, "true") == 0 ||
             strcmp(debug, "yes") == 0));
}

void debug_log(const char *message) {
    if (is_debug_enabled()) {
        fprintf(stderr, "[DEBUG] %s\n", message);
    }
}

int main(void) {
    debug_log("Demarrage de l'application");

    /* Code principal */
    printf("Application en cours d'execution...\n");

    debug_log("Traitement termine");

    return 0;
}
