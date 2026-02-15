/* ============================================================================
   Section 17.5 : Gestion des signaux
   Description : Timeout avec alarm() et SIGALRM + longjmp
   Fichier source : 05-gestion-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>

static jmp_buf jump_buffer;
static volatile sig_atomic_t timeout_occurred = 0;

void alarm_handler(int signum) {
    (void)signum;
    timeout_occurred = 1;
    longjmp(jump_buffer, 1);
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = alarm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);

    printf("Vous avez 5 secondes pour appuyer sur Entree!\n");

    /* Definir un timeout de 5 secondes */
    alarm(5);

    if (setjmp(jump_buffer) == 0) {
        /* Premiere execution */
        char buffer[100];
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            /* Annuler l'alarme si l'utilisateur a repondu a temps */
            alarm(0);
            printf("Bravo! Vous avez ete assez rapide!\n");
        }
    } else {
        /* Retour depuis longjmp (timeout) */
        printf("\n\nTemps ecoule! Trop lent!\n");
    }

    (void)timeout_occurred;

    return 0;
}
