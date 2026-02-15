/* ============================================================================
   Section 17.5 : Gestion des signaux
   Description : Bloquer SIGINT temporairement avec sigprocmask()
   Fichier source : 05-gestion-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigint_handler(int signum) {
    (void)signum;
    write(STDOUT_FILENO, "\n[HANDLER] SIGINT recu!\n", 24);
}

int main(void) {
    struct sigaction sa;
    sigset_t mask, oldmask;

    /* Installer le handler */
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    printf("PID: %d\n\n", getpid());

    /* Preparer un masque avec SIGINT */
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);

    printf("Phase 1: SIGINT normal (2 secondes)\n");
    printf("Essayez Ctrl+C maintenant!\n");
    sleep(2);

    /* Bloquer SIGINT */
    printf("\nPhase 2: SIGINT BLOQUE (5 secondes)\n");
    printf("Ctrl+C est maintenant bloque!\n");
    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    sleep(5);

    /* Debloquer SIGINT */
    printf("\nPhase 3: SIGINT debloque\n");
    printf("Les signaux en attente vont etre delivres...\n");
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    sleep(2);
    printf("\nProgramme termine\n");

    return 0;
}
