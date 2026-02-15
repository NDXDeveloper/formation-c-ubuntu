/* ============================================================================
   Section 17.4 : Variables d'environnement
   Description : Lecture de plusieurs variables d'environnement
   Fichier source : 04-variables-environnement.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    const char *variables[] = {
        "HOME", "USER", "SHELL", "PWD", "LANG", NULL
    };

    printf("Variables d'environnement:\n");
    printf("===========================\n\n");

    for (int i = 0; variables[i] != NULL; i++) {
        char *value = getenv(variables[i]);

        if (value != NULL) {
            printf("%-10s = %s\n", variables[i], value);
        } else {
            printf("%-10s = (non definie)\n", variables[i]);
        }
    }

    return 0;
}
