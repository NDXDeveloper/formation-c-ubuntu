/* ============================================================================
   Section 17.5 : Gestion des signaux
   Description : Ignorer SIGINT (Ctrl+C ne fonctionne plus)
   Fichier source : 05-gestion-signaux.md
   ============================================================================ */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main(void) {
    printf("Programme demarre (PID: %d)\n", getpid());
    printf("Essayez Ctrl+C... ca ne marchera pas!\n\n");

    /* Ignorer SIGINT (Ctrl+C) */
    signal(SIGINT, SIG_IGN);

    /* Boucle finie */
    for (int i = 1; i <= 10; i++) {
        printf("Iteration %d - Ctrl+C ne fonctionne pas\n", i);
        sleep(1);
    }

    printf("\nProgramme termine normalement\n");

    return 0;
}
