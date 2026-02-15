/* ============================================================================
   Section 17.5 : Gestion des signaux
   Description : Gestionnaire personnalise pour SIGINT avec signal()
   Fichier source : 05-gestion-signaux.md
   ============================================================================ */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

/* Fonction appelee lors de la reception du signal */
void signal_handler(int signum) {
    printf("\n[HANDLER] Signal %d recu (SIGINT)!\n", signum);
    printf("[HANDLER] Mais je continue a fonctionner...\n\n");
}

int main(void) {
    printf("Programme demarre (PID: %d)\n", getpid());
    printf("Appuyez sur Ctrl+C pour tester le gestionnaire\n\n");

    /* Installer le gestionnaire pour SIGINT */
    signal(SIGINT, signal_handler);

    for (int i = 1; i <= 10; i++) {
        printf("Iteration %d\n", i);
        sleep(1);
    }

    printf("Programme termine\n");

    return 0;
}
