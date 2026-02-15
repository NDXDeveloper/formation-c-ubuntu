/* ============================================================================
   Section 17.4 : Variables d'environnement
   Description : Acces direct a toutes les variables via le tableau environ
   Fichier source : 04-variables-environnement.md
   ============================================================================ */

#include <stdio.h>

/* Declaration externe du tableau environ */
extern char **environ;

int main(void) {
    printf("Toutes les variables d'environnement:\n");
    printf("=====================================\n\n");

    for (int i = 0; environ[i] != NULL; i++) {
        printf("%s\n", environ[i]);
    }

    return 0;
}
