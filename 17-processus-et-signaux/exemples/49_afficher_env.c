/* ============================================================================
   Section 17.4 : Variables d'environnement
   Description : Afficher l'environnement complet avec formatage
   Fichier source : 04-variables-environnement.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>

extern char **environ;

void print_environment(void) {
    int count = 0;

    printf("Variables d'environnement:\n");
    printf("==========================\n\n");

    for (int i = 0; environ[i] != NULL; i++) {
        /* Separer nom et valeur pour un affichage formate */
        char *eq = strchr(environ[i], '=');
        if (eq != NULL) {
            int name_len = (int)(eq - environ[i]);
            printf("%-20.*s = %s\n", name_len, environ[i], eq + 1);
            count++;
        }
    }

    printf("\nTotal: %d variables\n", count);
}

int main(void) {
    print_environment();
    return 0;
}
