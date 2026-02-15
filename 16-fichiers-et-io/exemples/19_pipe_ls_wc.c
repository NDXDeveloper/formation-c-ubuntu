/* ============================================================================
   Section 16.6 : dup, dup2 et redirection
   Description : Implementation de "ls | wc -l" avec pipe et dup2
   Fichier source : 06-dup-redirection.md
   ============================================================================ */

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void) {
    int pipefd[2];

    /* Creer un pipe */
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    printf("Equivalent de : ls /tmp | wc -l\n");
    fflush(stdout);

    /* Premier fork : ls */
    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        return 1;
    }

    if (pid1 == 0) {
        /* Enfant 1 : ls */
        close(pipefd[0]);  /* Fermer lecture (pas besoin) */

        /* Rediriger stdout vers l'ecriture du pipe */
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        /* Executer ls */
        execlp("ls", "ls", "/tmp", NULL);
        perror("execlp ls");
        _exit(1);
    }

    /* Deuxieme fork : wc -l */
    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        return 1;
    }

    if (pid2 == 0) {
        /* Enfant 2 : wc -l */
        close(pipefd[1]);  /* Fermer ecriture (pas besoin) */

        /* Rediriger stdin vers la lecture du pipe */
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        /* Executer wc -l */
        execlp("wc", "wc", "-l", NULL);
        perror("execlp wc");
        _exit(1);
    }

    /* Parent : fermer les deux extremites du pipe */
    close(pipefd[0]);
    close(pipefd[1]);

    /* Attendre les deux enfants */
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
