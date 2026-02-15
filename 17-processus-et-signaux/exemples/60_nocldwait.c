/* ============================================================================
   Section 17.5 : Gestion des signaux
   Description : SA_NOCLDWAIT pour eviter les zombies sans handler
   Fichier source : 05-gestion-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int main(void) {
    struct sigaction sa;

    /* Configuration pour ne pas creer de zombies */
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDWAIT;  /* Pas de zombies! */

    sigaction(SIGCHLD, &sa, NULL);

    printf("[Parent] PID: %d\n", getpid());

    for (int i = 0; i < 5; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            printf("[Enfant %d] PID: %d\n", i, getpid());
            sleep(1);
            exit(0);
        }
    }

    printf("[Parent] Tous les enfants crees\n");
    printf("[Parent] Pas de zombies grace a SA_NOCLDWAIT!\n");

    sleep(3);

    return 0;
}
