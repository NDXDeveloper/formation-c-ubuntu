/* ============================================================================
   Section 17.7 : Masquage de signaux
   Description : Sauvegarder et restaurer le masque de signaux
   Fichier source : 07-masquage-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void critical_section(void) {
    sigset_t newmask, oldmask;

    /* Preparer un masque bloquant SIGINT */
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);

    /* Bloquer et sauvegarder l'ancien masque */
    printf("Entree en section critique (SIGINT bloque)\n");
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);

    /* Code critique ici */
    printf("Operation sensible en cours...\n");
    sleep(3);

    /* Restaurer l'ancien masque */
    printf("Sortie de section critique (masque restaure)\n");
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
}

int main(void) {
    printf("Programme demarre (PID: %d)\n\n", getpid());

    printf("Avant section critique - Ctrl+C fonctionne\n");
    sleep(2);

    critical_section();

    printf("\nApres section critique - Ctrl+C fonctionne a nouveau\n");
    sleep(2);

    return 0;
}
