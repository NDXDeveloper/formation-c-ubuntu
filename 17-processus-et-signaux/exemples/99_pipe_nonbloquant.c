/* ============================================================================
   Section 17.8 : Pipes anonymes
   Description : Mode non-bloquant avec fcntl et O_NONBLOCK
   Fichier source : 08-pipes-anonymes.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(void) {
    int pipefd[2];
    pipe(pipefd);

    /* Rendre la lecture non-bloquante */
    int flags = fcntl(pipefd[0], F_GETFL);
    fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);

    char buffer[100];
    ssize_t n = read(pipefd[0], buffer, sizeof(buffer));

    if (n == -1 && errno == EAGAIN) {
        printf("Pipe vide (lecture non-bloquante)\n");
    }

    close(pipefd[0]);
    close(pipefd[1]);

    return 0;
}
