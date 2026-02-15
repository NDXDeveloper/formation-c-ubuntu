/* ============================================================================
   Section 17.8 : Pipes anonymes
   Description : Communication bidirectionnelle avec deux pipes
   Fichier source : 08-pipes-anonymes.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
    int pipe_parent_to_child[2];  /* Parent -> Enfant */
    int pipe_child_to_parent[2];  /* Enfant -> Parent */

    /* Creer les deux pipes */
    if (pipe(pipe_parent_to_child) == -1 || pipe(pipe_child_to_parent) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {
        /* === ENFANT === */

        /* Fermer les extremites non utilisees */
        close(pipe_parent_to_child[1]);  /* On n'ecrit pas dans pipe1 */
        close(pipe_child_to_parent[0]);  /* On ne lit pas depuis pipe2 */

        /* Recevoir du parent */
        char buffer[100];
        read(pipe_parent_to_child[0], buffer, sizeof(buffer));
        printf("[Enfant] Recu du parent: \"%s\"\n", buffer);

        /* Envoyer au parent */
        const char *response = "Message de l'enfant";
        write(pipe_child_to_parent[1], response, strlen(response) + 1);
        printf("[Enfant] Envoye au parent: \"%s\"\n", response);

        /* Fermer les descripteurs */
        close(pipe_parent_to_child[0]);
        close(pipe_child_to_parent[1]);

        return 0;
    }
    else {
        /* === PARENT === */

        /* Fermer les extremites non utilisees */
        close(pipe_parent_to_child[0]);  /* On ne lit pas depuis pipe1 */
        close(pipe_child_to_parent[1]);  /* On n'ecrit pas dans pipe2 */

        /* Envoyer a l'enfant */
        const char *message = "Message du parent";
        write(pipe_parent_to_child[1], message, strlen(message) + 1);
        printf("[Parent] Envoye a l'enfant: \"%s\"\n", message);

        /* Recevoir de l'enfant */
        char buffer[100];
        read(pipe_child_to_parent[0], buffer, sizeof(buffer));
        printf("[Parent] Recu de l'enfant: \"%s\"\n", buffer);

        /* Fermer les descripteurs */
        close(pipe_parent_to_child[1]);
        close(pipe_child_to_parent[0]);

        wait(NULL);
    }

    return 0;
}
