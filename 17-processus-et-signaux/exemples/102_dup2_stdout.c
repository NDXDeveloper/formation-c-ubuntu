/* ============================================================================
   Section 17.8 : Pipes anonymes
   Description : Rediriger stdout vers un pipe avec dup2
   Fichier source : 08-pipes-anonymes.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int pipefd[2];
    pipe(pipefd);

    if (fork() == 0) {
        /* ENFANT : va ecrire dans stdout, mais stdout -> pipe */

        close(pipefd[0]);  /* Ne lit pas */

        /* Rediriger stdout vers le pipe */
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);  /* Fermer l'original (on a la copie) */

        /* Maintenant, printf() ecrit dans le pipe! */
        printf("Ce message va dans le pipe!\n");
        printf("Ligne 2 dans le pipe!\n");

        return 0;
    }
    else {
        /* PARENT : lit depuis le pipe */

        close(pipefd[1]);  /* N'ecrit pas */

        char buffer[200];
        ssize_t n;

        printf("[Parent] Lecture depuis le pipe:\n");

        while ((n = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[n] = '\0';
            printf("%s", buffer);
        }

        close(pipefd[0]);
        wait(NULL);
    }

    return 0;
}
