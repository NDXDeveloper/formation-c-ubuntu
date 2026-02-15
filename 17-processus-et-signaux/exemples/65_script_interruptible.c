/* ============================================================================
   Section 17.6 : Signaux courants
   Description : Script interruptible avec barre de progression
   Fichier source : 06-signaux-courants.md
   ============================================================================ */

#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t keep_running = 1;

void sigint_handler(int signum) {
    (void)signum;
    write(STDOUT_FILENO, "\nInterruption demandee...\n", 26);
    keep_running = 0;
}

int main(void) {
    signal(SIGINT, sigint_handler);

    printf("Traitement de 1000 elements\n");
    printf("Ctrl+C pour interrompre proprement\n\n");

    int i;
    for (i = 1; i <= 1000 && keep_running; i++) {
        printf("\rTraitement: %d/1000", i);
        fflush(stdout);
        usleep(10000);  /* 10ms par element */
    }

    printf("\n\nTraitement %s\n",
           keep_running ? "termine" : "interrompu");

    return 0;
}
