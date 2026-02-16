/* ============================================================================
   Section 34.1.2 : Interaction avec le systeme
   Description : Execution de programme avec fork() + execvp()
   Fichier source : 01.2-interaction-systeme.md
   ============================================================================ */
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int executer_programme(const char *programme, char *const args[]) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        // Processus enfant: exécuter le programme
        execvp(programme, args);

        // Si on arrive ici, execvp a échoué
        perror("execvp");
        exit(1);
    } else {
        // Processus parent: attendre la fin
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        return -1;
    }
}

int main(void) {
    printf("=== Execution de 'ls -la /tmp' via fork+exec ===\n");
    char *args[] = {"ls", "-la", "/tmp", NULL};
    int code = executer_programme("ls", args);
    printf("\nCode de retour: %d\n", code);
    return 0;
}
