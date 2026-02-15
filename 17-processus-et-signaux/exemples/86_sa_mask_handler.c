/* ============================================================================
   Section 17.7 : Masquage de signaux
   Description : Bloquer SIGINT pendant le handler SIGUSR1 via sa_mask
   Fichier source : 07-masquage-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigusr1_handler(int signum) {
    (void)signum;
    printf("\n[SIGUSR1 Handler] Debut (3 secondes)\n");
    printf("[SIGUSR1 Handler] SIGINT est bloque pendant ce handler\n");
    sleep(3);
    printf("[SIGUSR1 Handler] Fin\n\n");
}

void sigint_handler(int signum) {
    (void)signum;
    printf("\n[SIGINT Handler] Recu!\n\n");
}

int main(void) {
    struct sigaction sa_usr1, sa_int;

    /* Handler pour SIGINT */
    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_int, NULL);

    /* Handler pour SIGUSR1 */
    sa_usr1.sa_handler = sigusr1_handler;
    sigemptyset(&sa_usr1.sa_mask);

    /* Bloquer SIGINT pendant l'execution du handler SIGUSR1 */
    sigaddset(&sa_usr1.sa_mask, SIGINT);

    sa_usr1.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa_usr1, NULL);

    printf("PID: %d\n\n", getpid());
    printf("Tests:\n");
    printf("1. kill -USR1 %d  (puis appuyez rapidement sur Ctrl+C)\n", getpid());
    printf("2. Ctrl+C sera bloque pendant le handler SIGUSR1\n\n");

    while (1) {
        sleep(1);
    }

    return 0;
}
