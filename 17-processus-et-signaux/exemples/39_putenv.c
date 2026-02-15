/* ============================================================================
   Section 17.4 : Variables d'environnement
   Description : putenv() - moins recommande que setenv()
   Fichier source : 04-variables-environnement.md
   ============================================================================ */

#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    /* Allocation dynamique ou chaine statique */
    char *env_string = malloc(64);
    if (env_string == NULL) {
        perror("malloc");
        return 1;
    }
    sprintf(env_string, "MA_VAR=test");
    putenv(env_string);

    printf("MA_VAR = %s\n", getenv("MA_VAR"));

    /* Note: ne pas free(env_string) car putenv() l'utilise! */

    return 0;
}
