/* ============================================================================
   Section 34.1.2 : Interaction avec le systeme
   Description : Variables d'environnement (getenv, setenv, unsetenv)
   Fichier source : 01.2-interaction-systeme.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // Lire des variables standard
    printf("=== Variables d'environnement ===\n");
    const char *home = getenv("HOME");
    const char *user = getenv("USER");
    const char *path = getenv("PATH");

    printf("HOME: %s\n", home ? home : "(non défini)");
    printf("USER: %s\n", user ? user : "(non défini)");
    printf("PATH: %s\n\n", path ? path : "(non défini)");

    // Modifier des variables
    printf("=== setenv / unsetenv ===\n");
    setenv("MA_VARIABLE", "valeur", 1);
    printf("MA_VARIABLE = %s\n", getenv("MA_VARIABLE"));

    setenv("MA_VARIABLE", "nouvelle_valeur", 1);
    printf("MA_VARIABLE = %s\n", getenv("MA_VARIABLE"));

    unsetenv("MA_VARIABLE");
    const char *val = getenv("MA_VARIABLE");
    printf("MA_VARIABLE = %s\n", val ? val : "(supprimée)");

    return 0;
}
