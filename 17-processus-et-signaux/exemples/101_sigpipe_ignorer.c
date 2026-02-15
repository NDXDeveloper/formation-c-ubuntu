/* ============================================================================
   Section 17.8 : Pipes anonymes
   Description : Ignorer SIGPIPE et detecter EPIPE
   Fichier source : 08-pipes-anonymes.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

int main(void) {
    int pipefd[2];
    pipe(pipefd);

    /* Ignorer SIGPIPE */
    signal(SIGPIPE, SIG_IGN);

    close(pipefd[0]);

    printf("Tentative d'ecriture dans un pipe sans lecteur...\n");

    if (write(pipefd[1], "test", 4) == -1) {
        if (errno == EPIPE) {
            printf("Erreur EPIPE detectee (pas de lecteur)\n");
            printf("Le programme continue normalement\n");
        }
    }

    close(pipefd[1]);

    return 0;
}
