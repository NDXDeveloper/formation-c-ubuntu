/* ============================================================================
   Section 17.6 : Signaux courants
   Description : SIGALRM - timeout sur une operation longue
   Fichier source : 06-signaux-courants.md
   ============================================================================ */

#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t timeout_occurred = 0;

void sigalrm_handler(int signum) {
    (void)signum;
    timeout_occurred = 1;
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = sigalrm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);

    printf("Operation avec timeout de 5 secondes\n");
    printf("Simulons une operation longue...\n\n");

    alarm(5);  /* Timeout de 5 secondes */

    int progress = 0;
    while (progress < 100 && !timeout_occurred) {
        printf("\rProgression: %d%%", progress);
        fflush(stdout);

        usleep(100000);  /* 100ms */
        progress += 1;
    }

    alarm(0);  /* Annuler l'alarme si termine a temps */

    printf("\n\n");

    if (timeout_occurred) {
        printf("TIMEOUT! Operation trop longue.\n");
        return 1;
    } else {
        printf("Operation terminee avec succes.\n");
        return 0;
    }
}
