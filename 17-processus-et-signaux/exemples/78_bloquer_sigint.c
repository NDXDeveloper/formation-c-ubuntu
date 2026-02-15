/* ============================================================================
   Section 17.7 : Masquage de signaux
   Description : Bloquer SIGINT temporairement avec sigprocmask
   Fichier source : 07-masquage-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main(void) {
    sigset_t newmask, oldmask;

    printf("PID: %d\n", getpid());

    /* Preparer le masque avec SIGINT */
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);

    printf("Phase 1: SIGINT normal (3 secondes)\n");
    printf("Essayez Ctrl+C maintenant!\n");
    sleep(3);

    /* Bloquer SIGINT */
    printf("\nPhase 2: SIGINT BLOQUE (5 secondes)\n");
    printf("Ctrl+C ne fonctionnera pas temporairement!\n");
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);
    sleep(5);

    /* Debloquer SIGINT */
    printf("\nPhase 3: SIGINT debloque\n");
    printf("Les signaux en attente seront delivres maintenant...\n");
    sigprocmask(SIG_UNBLOCK, &newmask, NULL);

    sleep(2);
    printf("\nProgramme termine normalement\n");

    return 0;
}
