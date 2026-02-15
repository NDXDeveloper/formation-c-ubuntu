/* ============================================================================
   Section 17.4 : Variables d'environnement
   Description : Supprimer une variable avec unsetenv()
   Fichier source : 04-variables-environnement.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    /* Creer une variable */
    setenv("MA_VARIABLE", "test", 1);
    printf("Avant unsetenv: %s\n", getenv("MA_VARIABLE"));

    /* La supprimer */
    unsetenv("MA_VARIABLE");

    char *value = getenv("MA_VARIABLE");
    if (value == NULL) {
        printf("Apres unsetenv: variable supprimee\n");
    }

    return 0;
}
