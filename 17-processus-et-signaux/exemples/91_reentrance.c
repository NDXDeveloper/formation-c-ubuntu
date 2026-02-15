/* ============================================================================
   Section 17.7 : Masquage de signaux
   Description : Reentrance securisee avec masquage pendant modification
   Fichier source : 07-masquage-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t counter = 0;

void increment_counter(void) {
    sigset_t mask, oldmask;

    /* Bloquer les signaux pendant la modification */
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    /* Operation critique */
    counter++;
    printf("Counter = %d\n", counter);

    /* Restaurer */
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
}

void signal_handler(int signum) {
    (void)signum;
    /* Peut appeler increment_counter en toute securite */
    increment_counter();
}

int main(void) {
    signal(SIGUSR1, signal_handler);

    printf("PID: %d\n", getpid());

    for (int i = 0; i < 5; i++) {
        increment_counter();
        sleep(1);
    }

    return 0;
}
