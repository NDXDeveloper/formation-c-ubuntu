/* ============================================================================
   Section 17.8 : Pipes anonymes
   Description : Calculateur parallele - enfant calcule le carre
   Fichier source : 08-pipes-anonymes.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int pipe_to_child[2], pipe_from_child[2];

    pipe(pipe_to_child);
    pipe(pipe_from_child);

    if (fork() == 0) {
        /* ENFANT : Calcule le carre */
        close(pipe_to_child[1]);
        close(pipe_from_child[0]);

        int number;
        read(pipe_to_child[0], &number, sizeof(int));

        int result = number * number;

        write(pipe_from_child[1], &result, sizeof(int));

        close(pipe_to_child[0]);
        close(pipe_from_child[1]);

        return 0;
    }
    else {
        /* PARENT */
        close(pipe_to_child[0]);
        close(pipe_from_child[1]);

        int number = 7;
        printf("[Parent] Calcul du carre de %d\n", number);

        write(pipe_to_child[1], &number, sizeof(int));

        int result;
        read(pipe_from_child[0], &result, sizeof(int));

        printf("[Parent] Resultat: %d^2 = %d\n", number, result);

        close(pipe_to_child[1]);
        close(pipe_from_child[0]);

        wait(NULL);
    }

    return 0;
}
