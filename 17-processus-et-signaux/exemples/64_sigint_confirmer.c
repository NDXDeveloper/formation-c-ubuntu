/* ============================================================================
   Section 17.6 : Signaux courants
   Description : Confirmer avant de quitter (double Ctrl+C)
   Fichier source : 06-signaux-courants.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t interrupt_count = 0;

void sigint_handler(int signum) {
    (void)signum;
    interrupt_count++;
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    printf("Programme demarre\n");
    printf("Appuyez 2 fois sur Ctrl+C pour quitter\n\n");

    int work_count = 0;

    while (1) {
        if (interrupt_count == 1) {
            printf("\nCtrl+C detecte. Appuyez encore une fois pour confirmer.\n");
            interrupt_count++;  /* Pour eviter les repetitions du message */
        }
        else if (interrupt_count >= 3) {
            printf("\nConfirmation recue, sortie...\n");
            break;
        }

        printf("Travail en cours... (%d)\n", ++work_count);
        sleep(2);
    }

    printf("Nettoyage et sortie\n");

    return 0;
}
