/* ============================================================================
   Section 17.8 : Pipes anonymes
   Description : Lecture bloquante - read attend les donnees
   Fichier source : 08-pipes-anonymes.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int pipefd[2];
    pipe(pipefd);

    if (fork() == 0) {
        /* ENFANT */
        close(pipefd[1]);

        printf("[Enfant] Lecture (va bloquer 3 secondes)...\n");

        char buffer[100];
        read(pipefd[0], buffer, sizeof(buffer));  /* BLOQUE */

        printf("[Enfant] Donnees recues: \"%s\"\n", buffer);
        close(pipefd[0]);

        return 0;
    }
    else {
        /* PARENT */
        close(pipefd[0]);

        printf("[Parent] Attente de 3 secondes avant d'ecrire...\n");
        sleep(3);

        printf("[Parent] Ecriture dans le pipe\n");
        write(pipefd[1], "Donnees!", 9);

        close(pipefd[1]);
        wait(NULL);
    }

    return 0;
}
