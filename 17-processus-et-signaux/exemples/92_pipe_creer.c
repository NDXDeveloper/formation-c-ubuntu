/* ============================================================================
   Section 17.8 : Pipes anonymes
   Description : Creer un pipe et afficher les descripteurs
   Fichier source : 08-pipes-anonymes.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>

int main(void) {
    int pipefd[2];

    /* Creer le pipe */
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    printf("Pipe cree avec succes!\n");
    printf("Descripteur de lecture:  fd[0] = %d\n", pipefd[0]);
    printf("Descripteur d'ecriture: fd[1] = %d\n", pipefd[1]);

    /* Fermer les descripteurs */
    close(pipefd[0]);
    close(pipefd[1]);

    return 0;
}
