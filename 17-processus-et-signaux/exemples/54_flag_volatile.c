/* ============================================================================
   Section 17.5 : Gestion des signaux
   Description : Pattern variable flag avec volatile sig_atomic_t
   Fichier source : 05-gestion-signaux.md
   ============================================================================ */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

/* Variable globale volatile */
volatile sig_atomic_t interrupted = 0;

void sigint_handler(int signum) {
    (void)signum;  /* Parametre non utilise */
    interrupted = 1;
}

int main(void) {
    printf("Programme demarre (PID: %d)\n", getpid());
    printf("Appuyez sur Ctrl+C pour arreter proprement\n\n");

    signal(SIGINT, sigint_handler);

    int count = 0;
    while (!interrupted) {
        printf("Iteration %d\n", ++count);
        sleep(1);
    }

    printf("\n[MAIN] Interruption detectee\n");
    printf("[MAIN] Nettoyage en cours...\n");

    /* Faire le nettoyage necessaire */
    sleep(1);

    printf("[MAIN] Terminaison propre\n");

    return 0;
}
