/* ============================================================================
   Section 17.7 : Masquage de signaux
   Description : Visualiser les signaux en attente avec sigpending
   Fichier source : 07-masquage-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void print_pending_signals(void) {
    sigset_t pending;

    if (sigpending(&pending) == -1) {
        perror("sigpending");
        return;
    }

    printf("Signaux en attente: ");

    int found = 0;

    if (sigismember(&pending, SIGINT)) {
        printf("SIGINT ");
        found = 1;
    }
    if (sigismember(&pending, SIGTERM)) {
        printf("SIGTERM ");
        found = 1;
    }
    if (sigismember(&pending, SIGUSR1)) {
        printf("SIGUSR1 ");
        found = 1;
    }
    if (sigismember(&pending, SIGUSR2)) {
        printf("SIGUSR2 ");
        found = 1;
    }

    if (!found) {
        printf("(aucun)");
    }

    printf("\n");
}

int main(void) {
    sigset_t mask;

    printf("PID: %d\n\n", getpid());

    /* Bloquer SIGINT et SIGUSR1 */
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGUSR1);

    printf("Blocage de SIGINT et SIGUSR1\n");
    sigprocmask(SIG_BLOCK, &mask, NULL);

    printf("\nPendant 10 secondes, envoyez des signaux:\n");
    printf("  kill -INT %d\n", getpid());
    printf("  kill -USR1 %d\n", getpid());
    printf("  (ou appuyez sur Ctrl+C)\n\n");

    for (int i = 1; i <= 10; i++) {
        printf("Seconde %d/10 - ", i);
        print_pending_signals();
        sleep(1);
    }

    printf("\nDeblocage des signaux...\n");
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    printf("Signaux delivres!\n");

    sleep(1);

    return 0;
}
