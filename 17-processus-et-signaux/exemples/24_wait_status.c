/* ============================================================================
   Section 17.3 : Processus orphelins et zombies
   Description : Analyser le code de retour avec les macros WIFEXITED/WEXITSTATUS
   Fichier source : 03-orphelins-zombies.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* Enfant retourne un code */
        printf("[Enfant] Je retourne le code 42\n");
        exit(42);
    }

    /* Parent */
    int status;
    wait(&status);

    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        printf("[Parent] Enfant termine normalement avec le code: %d\n",
               exit_code);
    }
    else if (WIFSIGNALED(status)) {
        int signal_num = WTERMSIG(status);
        printf("[Parent] Enfant termine par le signal: %d\n", signal_num);
    }

    return 0;
}
