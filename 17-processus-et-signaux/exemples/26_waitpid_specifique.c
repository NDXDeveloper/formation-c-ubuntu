/* ============================================================================
   Section 17.3 : Processus orphelins et zombies
   Description : Attendre un enfant specifique avec waitpid()
   Fichier source : 03-orphelins-zombies.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t child1, child2;

    /* Premier enfant */
    child1 = fork();
    if (child1 == -1) {
        perror("fork");
        return 1;
    }
    if (child1 == 0) {
        printf("[Enfant 1] PID: %d, je dors 3 secondes\n", getpid());
        sleep(3);
        return 1;
    }

    /* Deuxieme enfant */
    child2 = fork();
    if (child2 == -1) {
        perror("fork");
        return 1;
    }
    if (child2 == 0) {
        printf("[Enfant 2] PID: %d, je dors 1 seconde\n", getpid());
        sleep(1);
        return 2;
    }

    /* Parent attend specifiquement child1 */
    printf("[Parent] J'attends specifiquement l'enfant %d...\n", child1);

    int status;
    waitpid(child1, &status, 0);

    printf("[Parent] Enfant %d termine (code: %d)\n",
           child1, WEXITSTATUS(status));

    /* Puis attendre child2 */
    waitpid(child2, &status, 0);

    printf("[Parent] Enfant %d termine (code: %d)\n",
           child2, WEXITSTATUS(status));

    return 0;
}
