/* ============================================================================
   Section 17.4 : Variables d'environnement
   Description : Heritage des variables lors d'un exec()
   Fichier source : 04-variables-environnement.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    setenv("MA_VARIABLE", "test", 1);

    printf("Avant exec, MA_VARIABLE = %s\n", getenv("MA_VARIABLE"));

    /* exec() avec heritage de l'environnement */
    execl("/usr/bin/printenv", "printenv", "MA_VARIABLE", NULL);

    /* Si exec reussit, on n'arrive jamais ici */
    perror("execl");
    return 1;
}
