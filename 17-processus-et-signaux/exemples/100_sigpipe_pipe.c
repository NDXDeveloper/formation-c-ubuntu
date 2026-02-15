/* ============================================================================
   Section 17.8 : Pipes anonymes
   Description : SIGPIPE - ecriture dans un pipe sans lecteur
   Fichier source : 08-pipes-anonymes.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    int pipefd[2];
    pipe(pipefd);

    /* Fermer l'extremite de lecture */
    close(pipefd[0]);

    printf("Tentative d'ecriture dans un pipe sans lecteur...\n");
    fflush(stdout);

    /* Cette ecriture genere SIGPIPE */
    if (write(pipefd[1], "test", 4) == -1) {
        perror("write");
        printf("Cette ligne ne s'affichera probablement pas\n");
    }

    close(pipefd[1]);

    return 0;
}
