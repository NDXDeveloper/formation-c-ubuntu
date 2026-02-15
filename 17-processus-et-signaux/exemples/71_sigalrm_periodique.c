/* ============================================================================
   Section 17.6 : Signaux courants
   Description : SIGALRM - alarme periodique (reamorcement automatique)
   Fichier source : 06-signaux-courants.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t alarm_count = 0;

void sigalrm_handler(int signum) {
    (void)signum;
    alarm_count++;
    alarm(1);  /* Rearmer pour 1 seconde */
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = sigalrm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &sa, NULL);

    printf("Compteur avec alarme chaque seconde\n");
    printf("Ctrl+C pour arreter\n\n");

    alarm(1);  /* Premiere alarme dans 1 seconde */

    while (alarm_count < 10) {
        printf("Alarmes recues: %d\n", alarm_count);
        sleep(1);
    }

    alarm(0);  /* Desactiver l'alarme */

    printf("\nTermine apres 10 alarmes\n");

    return 0;
}
