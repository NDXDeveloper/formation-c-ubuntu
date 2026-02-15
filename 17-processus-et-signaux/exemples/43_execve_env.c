/* ============================================================================
   Section 17.4 : Variables d'environnement
   Description : Executer un programme avec un environnement personnalise (execve)
   Fichier source : 04-variables-environnement.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    /* Creer un nouvel environnement */
    char *newenv[] = {
        "PATH=/usr/bin:/bin",
        "HOME=/tmp",
        "USER=testuser",
        "MA_VARIABLE=ma_valeur",
        NULL  /* Terminateur */
    };

    printf("Execution de /usr/bin/env avec environnement personnalise:\n\n");

    char *argv[] = {"/usr/bin/env", NULL};

    /* Remplacer le processus avec un nouvel environnement */
    execve("/usr/bin/env", argv, newenv);

    /* Si on arrive ici, exec a echoue */
    perror("execve");
    return 1;
}
