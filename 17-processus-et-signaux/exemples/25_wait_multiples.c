/* ============================================================================
   Section 17.3 : Processus orphelins et zombies
   Description : Attendre plusieurs enfants avec wait()
   Fichier source : 03-orphelins-zombies.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int num_children = 3;

    /* Creer 3 enfants */
    for (int i = 0; i < num_children; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            return 1;
        }

        if (pid == 0) {
            /* Enfant */
            printf("[Enfant %d] PID: %d, je dors %d secondes\n",
                   i, getpid(), i + 1);
            sleep((unsigned int)(i + 1));
            printf("[Enfant %d] Termine!\n", i);
            return i;
        }
    }

    /* Parent attend TOUS ses enfants */
    printf("[Parent] J'attends mes %d enfants...\n", num_children);

    for (int i = 0; i < num_children; i++) {
        int status;
        pid_t pid = wait(&status);

        if (WIFEXITED(status)) {
            printf("[Parent] Enfant %d termine avec code %d\n",
                   pid, WEXITSTATUS(status));
        }
    }

    printf("[Parent] Tous mes enfants sont termines, pas de zombies!\n");

    return 0;
}
