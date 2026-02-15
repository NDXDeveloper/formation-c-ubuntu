/* ============================================================================
   Section 17.6 : Signaux courants
   Description : SIGPIPE - ecriture dans un pipe ferme, gestion via errno
   Fichier source : 06-signaux-courants.md
   ============================================================================ */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

int main(void) {
    /* IMPORTANT : Ignorer SIGPIPE dans les applications reseau */
    signal(SIGPIPE, SIG_IGN);

    printf("Serveur reseau (PID: %d)\n", getpid());
    printf("SIGPIPE ignore, gestion via errno\n\n");

    /* Simuler l'ecriture dans un pipe ferme */
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    close(pipefd[0]);  /* Fermer la lecture */

    printf("Tentative d'ecriture dans un pipe ferme...\n");

    if (write(pipefd[1], "test", 4) == -1) {
        if (errno == EPIPE) {
            printf("EPIPE detecte : pipe ferme (mais pas de SIGPIPE!)\n");
            printf("Le programme continue normalement\n");
        }
    }

    close(pipefd[1]);

    printf("\nServeur toujours actif!\n");

    return 0;
}
