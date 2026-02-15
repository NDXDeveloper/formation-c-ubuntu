/* ============================================================================
   Section 17.6 : Signaux courants
   Description : SIGCHLD - gestion automatique des processus enfants
   Fichier source : 06-signaux-courants.md
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

    /* Recuperer tous les enfants termines */
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        char msg[128];
        int len;

        if (WIFEXITED(status)) {
            len = snprintf(msg, sizeof(msg),
                          "[SIGCHLD] Enfant %d termine (code: %d)\n",
                          pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            len = snprintf(msg, sizeof(msg),
                          "[SIGCHLD] Enfant %d tue par signal %d\n",
                          pid, WTERMSIG(status));
        } else {
            len = snprintf(msg, sizeof(msg),
                          "[SIGCHLD] Enfant %d changement d'etat\n", pid);
        }

        write(STDOUT_FILENO, msg, (size_t)len);
    }
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    printf("Serveur avec gestion automatique des enfants\n");
    printf("PID: %d\n\n", getpid());

    /* Creer des processus enfants */
    for (int i = 1; i <= 5; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            return 1;
        }

        if (pid == 0) {
            printf("[Enfant %d] Demarre (PID: %d)\n", i, getpid());
            sleep((unsigned int)(i * 2));
            printf("[Enfant %d] Termine\n", i);
            exit(i);
        }

        printf("[Parent] Enfant %d cree (PID: %d)\n", i, pid);
    }

    printf("\n[Parent] Tous les enfants crees, en attente...\n\n");

    sleep(15);

    printf("[Parent] Termine\n");

    return 0;
}
