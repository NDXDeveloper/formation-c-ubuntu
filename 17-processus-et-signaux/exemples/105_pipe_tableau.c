/* ============================================================================
   Section 17.8 : Pipes anonymes
   Description : Transmettre un tableau de structures via un pipe
   Fichier source : 08-pipes-anonymes.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct {
    int x;
    int y;
} Point;

int main(void) {
    int pipefd[2];
    pipe(pipefd);

    if (fork() == 0) {
        /* ENFANT */
        close(pipefd[1]);

        Point points[5];
        read(pipefd[0], points, sizeof(points));

        printf("[Enfant] Points recus:\n");
        for (int i = 0; i < 5; i++) {
            printf("  Point %d: (%d, %d)\n", i, points[i].x, points[i].y);
        }

        close(pipefd[0]);
        return 0;
    }
    else {
        /* PARENT */
        close(pipefd[0]);

        Point points[5] = {
            {0, 0}, {1, 2}, {3, 4}, {5, 6}, {7, 8}
        };

        printf("[Parent] Envoi de 5 points\n");
        write(pipefd[1], points, sizeof(points));

        close(pipefd[1]);
        wait(NULL);
    }

    return 0;
}
