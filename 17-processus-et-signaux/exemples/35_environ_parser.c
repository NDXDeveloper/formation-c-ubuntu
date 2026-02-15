/* ============================================================================
   Section 17.4 : Variables d'environnement
   Description : Parser les variables d'environnement (separation nom/valeur)
   Fichier source : 04-variables-environnement.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>

extern char **environ;

void print_variable_details(const char *env_string) {
    /* Copier la chaine pour ne pas modifier l'original */
    char buffer[256];
    strncpy(buffer, env_string, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    /* Trouver le '=' */
    char *equals = strchr(buffer, '=');

    if (equals != NULL) {
        *equals = '\0';  /* Separer nom et valeur */
        char *name = buffer;
        char *value = equals + 1;

        printf("Nom: %-15s Valeur: %s\n", name, value);
    }
}

int main(void) {
    printf("Analyse des variables d'environnement:\n\n");

    for (int i = 0; environ[i] != NULL && i < 10; i++) {
        print_variable_details(environ[i]);
    }

    return 0;
}
