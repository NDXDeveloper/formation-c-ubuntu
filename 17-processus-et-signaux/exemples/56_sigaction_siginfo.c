/* ============================================================================
   Section 17.5 : Gestion des signaux
   Description : Handler etendu avec SA_SIGINFO (informations sur l'emetteur)
   Fichier source : 05-gestion-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void sigint_handler_extended(int signum, siginfo_t *info, void *context) {
    (void)context;  /* Non utilise */

    char buffer[256];
    int len;

    len = snprintf(buffer, sizeof(buffer),
                   "\n[HANDLER] Signal %d recu\n"
                   "[HANDLER] Envoye par PID: %d\n"
                   "[HANDLER] UID: %d\n\n",
                   signum, info->si_pid, info->si_uid);

    write(STDOUT_FILENO, buffer, (size_t)len);
}

int main(void) {
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));

    /* Utiliser le handler etendu */
    sa.sa_sigaction = sigint_handler_extended;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO | SA_RESTART;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("PID: %d\n", getpid());
    printf("Envoyez-moi un signal avec: kill -INT %d\n\n", getpid());

    for (int i = 0; i < 10; i++) {
        sleep(1);
    }

    printf("Programme termine\n");

    return 0;
}
