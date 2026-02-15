/* ============================================================================
   Section 17.4 : Variables d'environnement
   Description : Configuration d'application via variables d'environnement
   Fichier source : 04-variables-environnement.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    /* Lire le niveau de verbosite depuis une variable */
    char *verbosity = getenv("APP_VERBOSITY");
    int level = 1;  /* Valeur par defaut */

    if (verbosity != NULL) {
        level = atoi(verbosity);
    }

    printf("Niveau de verbosite: %d\n", level);

    if (level >= 2) {
        printf("[DEBUG] Mode verbeux active\n");
    }

    /* Lire un fichier de configuration */
    char *config_file = getenv("APP_CONFIG");

    if (config_file != NULL) {
        printf("Utilisation du fichier de config: %s\n", config_file);
    } else {
        printf("Utilisation du fichier par defaut: /etc/app.conf\n");
    }

    return 0;
}
