/* ============================================================================
   Section 17.8 : Pipes anonymes
   Description : Simuler la commande shell ls -l | wc -l
   Fichier source : 08-pipes-anonymes.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    /* Premier enfant : ls */
    pid_t pid1 = fork();

    if (pid1 == 0) {
        /* ENFANT 1 : ls */

        /* Rediriger stdout vers le pipe */
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        /* Executer ls */
        execlp("ls", "ls", "-l", NULL);

        perror("execlp ls");
        return 1;
    }

    /* Deuxieme enfant : wc */
    pid_t pid2 = fork();

    if (pid2 == 0) {
        /* ENFANT 2 : wc */

        /* Rediriger stdin depuis le pipe */
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        /* Executer wc */
        execlp("wc", "wc", "-l", NULL);

        perror("execlp wc");
        return 1;
    }

    /* PARENT */

    /* Fermer les deux extremites dans le parent */
    close(pipefd[0]);
    close(pipefd[1]);

    /* Attendre les deux enfants */
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
