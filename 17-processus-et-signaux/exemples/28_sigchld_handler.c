/* ============================================================================
   Section 17.3 : Processus orphelins et zombies
   Description : Gestionnaire SIGCHLD pour eviter les zombies de maniere asynchrone
   Fichier source : 03-orphelins-zombies.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

void sigchld_handler(int sig) {
    (void)sig;  /* Parametre non utilise */

    int status;
    pid_t pid;

    /* Attendre tous les enfants termines */
    /* WNOHANG pour ne pas bloquer s'il n'y a plus d'enfants */
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("[Handler] Enfant %d nettoye (code: %d)\n",
               pid, WEXITSTATUS(status));
    }
}

int main(void) {
    /* Installer le gestionnaire de signal */
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("[Parent] Gestionnaire SIGCHLD installe\n");

    /* Creer plusieurs enfants */
    for (int i = 0; i < 5; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            return 1;
        }

        if (pid == 0) {
            printf("[Enfant %d] PID: %d\n", i, getpid());
            sleep((unsigned int)(i + 1));
            return i;
        }

        printf("[Parent] Cree enfant %d avec PID %d\n", i, pid);
    }

    printf("[Parent] Tous les enfants crees, j'attends...\n");

    /* Parent peut faire autre chose */
    sleep(10);

    printf("[Parent] Termine, tous les zombies ont ete nettoyes!\n");

    return 0;
}
