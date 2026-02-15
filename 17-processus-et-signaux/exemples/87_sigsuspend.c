/* ============================================================================
   Section 17.7 : Masquage de signaux
   Description : Attendre un signal specifique avec sigsuspend
   Fichier source : 07-masquage-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t sigusr1_received = 0;

void sigusr1_handler(int signum) {
    (void)signum;
    sigusr1_received = 1;
    write(STDOUT_FILENO, "[Handler] SIGUSR1 recu\n", 23);
}

int main(void) {
    sigset_t blockmask, waitmask;
    struct sigaction sa;

    /* Installer le handler */
    sa.sa_handler = sigusr1_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa, NULL);

    printf("PID: %d\n", getpid());

    /* Bloquer SIGUSR1 */
    sigemptyset(&blockmask);
    sigaddset(&blockmask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &blockmask, NULL);

    printf("SIGUSR1 bloque. Envoyez-le avec: kill -USR1 %d\n", getpid());

    /* Preparer le masque pour sigsuspend (tous les signaux sauf SIGUSR1) */
    sigfillset(&waitmask);
    sigdelset(&waitmask, SIGUSR1);

    printf("Attente de SIGUSR1...\n");

    /* Attendre SIGUSR1 de maniere atomique */
    sigsuspend(&waitmask);

    printf("Reveil apres SIGUSR1!\n");
    printf("sigusr1_received = %d\n", sigusr1_received);

    return 0;
}
