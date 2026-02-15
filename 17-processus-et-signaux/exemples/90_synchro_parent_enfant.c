/* ============================================================================
   Section 17.7 : Masquage de signaux
   Description : Synchronisation parent-enfant avec sigsuspend
   Fichier source : 07-masquage-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

volatile sig_atomic_t ready = 0;

void sigusr1_handler(int signum) {
    (void)signum;
    ready = 1;
}

int main(void) {
    sigset_t mask, oldmask;
    struct sigaction sa;

    /* Installer le handler */
    sa.sa_handler = sigusr1_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    /* Bloquer SIGUSR1 */
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    printf("Parent PID: %d\n", getpid());

    pid_t pid = fork();

    if (pid == 0) {
        /* ENFANT */
        printf("[Enfant] Preparation...\n");
        sleep(2);

        printf("[Enfant] Pret! Envoi de SIGUSR1 au parent\n");
        kill(getppid(), SIGUSR1);

        printf("[Enfant] Travail termine\n");
        return 0;
    }
    else {
        /* PARENT */
        printf("[Parent] Attente du signal de l'enfant...\n");

        /* Attendre SIGUSR1 atomiquement */
        while (!ready) {
            sigsuspend(&oldmask);
        }

        printf("[Parent] Signal recu, enfant pret!\n");
        printf("[Parent] Continuation du travail\n");

        wait(NULL);
    }

    return 0;
}
