/* ============================================================================
   Section 17.4 : Variables d'environnement
   Description : Heritage des variables d'environnement lors d'un fork()
   Fichier source : 04-variables-environnement.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    /* Parent definit une variable */
    setenv("MA_VARIABLE", "valeur_parent", 1);

    printf("[Parent] MA_VARIABLE = %s\n", getenv("MA_VARIABLE"));

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* ENFANT */
        printf("[Enfant] MA_VARIABLE heritee = %s\n",
               getenv("MA_VARIABLE"));

        /* Enfant modifie la variable */
        setenv("MA_VARIABLE", "valeur_enfant", 1);
        printf("[Enfant] MA_VARIABLE modifiee = %s\n",
               getenv("MA_VARIABLE"));

        return 0;
    }
    else {
        /* PARENT */
        wait(NULL);

        /* La modification de l'enfant n'affecte PAS le parent */
        printf("[Parent] MA_VARIABLE apres fork = %s\n",
               getenv("MA_VARIABLE"));
    }

    return 0;
}
