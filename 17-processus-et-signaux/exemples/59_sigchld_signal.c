/* ============================================================================
   Section 17.5 : Gestion des signaux
   Description : Gestion de SIGCHLD pour nettoyer les processus enfants
   Fichier source : 05-gestion-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void sigchld_handler(int signum) {
    (void)signum;

    int status;
    pid_t pid;

    /* Attendre tous les enfants termines */
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        char msg[128];
        int len;

        if (WIFEXITED(status)) {
            len = snprintf(msg, sizeof(msg),
                          "[HANDLER] Enfant %d termine (code: %d)\n",
                          pid, WEXITSTATUS(status));
        } else {
            len = snprintf(msg, sizeof(msg),
                          "[HANDLER] Enfant %d termine anormalement\n", pid);
        }

        write(STDOUT_FILENO, msg, (size_t)len);
    }
}

int main(void) {
    struct sigaction sa;

    /* Installer le handler pour SIGCHLD */
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("[Parent] PID: %d\n", getpid());
    printf("[Parent] Creation de 3 enfants...\n\n");

    /* Creer 3 processus enfants */
    for (int i = 1; i <= 3; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            return 1;
        }

        if (pid == 0) {
            /* ENFANT */
            printf("[Enfant %d] PID: %d, je dors %d secondes\n",
                   i, getpid(), i);
            sleep((unsigned int)i);
            printf("[Enfant %d] Termine\n", i);
            exit(i);
        }

        printf("[Parent] Enfant %d cree (PID: %d)\n", i, pid);
    }

    printf("\n[Parent] Tous les enfants crees, j'attends...\n\n");

    /* Parent fait autre chose */
    for (int i = 0; i < 5; i++) {
        printf("[Parent] Travail en cours... (%d/5)\n", i + 1);
        sleep(1);
    }

    printf("\n[Parent] Termine\n");

    return 0;
}
