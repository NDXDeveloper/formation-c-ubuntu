/* ============================================================================
   Section 17.7 : Masquage de signaux
   Description : Auto-blocage du signal pendant l'execution du handler
   Fichier source : 07-masquage-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigint_handler(int signum) {
    (void)signum;

    printf("\n[Handler] Debut\n");

    /* Le signal SIGINT est automatiquement bloque ici */
    /* Si vous appuyez sur Ctrl+C, il sera mis en attente */

    printf("[Handler] Travail pendant 3 secondes...\n");
    printf("[Handler] Appuyez sur Ctrl+C, il sera mis en attente\n");

    sleep(3);

    printf("[Handler] Fin\n");

    /* A la sortie du handler, SIGINT sera debloque */
    /* et le signal en attente sera delivre */
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;  /* Pas de SA_NODEFER */

    sigaction(SIGINT, &sa, NULL);

    printf("Appuyez sur Ctrl+C\n");

    while (1) {
        sleep(1);
    }

    return 0;
}
