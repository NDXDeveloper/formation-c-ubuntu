/* ============================================================================
   Section 17.3 : Processus orphelins et zombies
   Description : Creation d'un processus zombie (pedagogique)
   Fichier source : 03-orphelins-zombies.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* ENFANT : se termine rapidement */
        printf("[Enfant] PID: %d, je me termine!\n", getpid());
        return 0;  /* L'enfant se termine */
    }
    else {
        /* PARENT : ne fait PAS de wait() */
        printf("[Parent] Mon enfant %d va devenir zombie...\n", pid);

        sleep(2);  /* Laisse le temps a l'enfant de se terminer */

        printf("[Parent] Verifiez avec 'ps aux | grep Z' dans un autre terminal!\n");
        printf("[Parent] Appuyez sur Entree pour continuer...\n");
        getchar();

        /* Toujours pas de wait(), l'enfant reste zombie */
        printf("[Parent] Je me termine sans avoir attendu mon enfant.\n");

        return 0;
    }
}
