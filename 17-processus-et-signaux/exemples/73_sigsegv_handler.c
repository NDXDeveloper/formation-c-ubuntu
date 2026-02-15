/* ============================================================================
   Section 17.6 : Signaux courants
   Description : SIGSEGV - capturer une violation de segmentation
   Fichier source : 06-signaux-courants.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigsegv_handler(int signum) {
    (void)signum;
    write(STDERR_FILENO, "\nSIGSEGV capture!\n", 18);
    write(STDERR_FILENO, "Erreur memoire detectee\n", 24);
    _exit(1);  /* Terminer immediatement */
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = sigsegv_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGSEGV, &sa, NULL);

    printf("Programme avec gestion SIGSEGV\n");
    printf("Declenchement volontaire d'un segfault...\n\n");

    /* Provoquer un SIGSEGV */
    int *ptr = NULL;
    *ptr = 42;  /* Dereferencement de NULL */

    printf("Cette ligne ne sera jamais affichee\n");

    return 0;
}
