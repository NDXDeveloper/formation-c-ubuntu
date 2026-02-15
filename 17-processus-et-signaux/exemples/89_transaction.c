/* ============================================================================
   Section 17.7 : Masquage de signaux
   Description : Proteger une transaction avec masquage complet
   Fichier source : 07-masquage-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void execute_transaction(void) {
    sigset_t mask, oldmask;

    printf("Debut de la transaction\n");

    /* Bloquer tous les signaux pendant la transaction */
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    /* Transaction atomique */
    printf("  Etape 1: Ouverture\n");
    sleep(1);
    printf("  Etape 2: Modification\n");
    sleep(1);
    printf("  Etape 3: Validation\n");
    sleep(1);

    /* Restaurer les signaux */
    sigprocmask(SIG_SETMASK, &oldmask, NULL);

    printf("Transaction terminee\n");
}

int main(void) {
    printf("PID: %d\n", getpid());
    printf("Les signaux seront bloques pendant les transactions\n\n");

    execute_transaction();

    printf("\nProgramme termine\n");

    return 0;
}
