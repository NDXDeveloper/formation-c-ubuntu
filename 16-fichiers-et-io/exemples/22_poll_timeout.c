/* ============================================================================
   Section 16.7 : I/O multiplexing
   Description : Utilisation de poll() avec timeout sur stdin
   Fichier source : 07-io-multiplexing.md
   ============================================================================ */

/* Tester : echo "hello" | ./22_poll_timeout
   Ou lancer interactivement et taper du texte (timeout 2s) */

#include <stdio.h>
#include <unistd.h>
#include <poll.h>

int main(void) {
    struct pollfd fds[1];
    int iterations = 0;

    /* Surveiller stdin */
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;  /* Interesse par la lecture */

    printf("Surveillance de stdin avec poll() (timeout 2s, 3 tours)\n");

    while (iterations < 3) {
        /* Timeout de 2000 ms = 2 secondes */
        int ret = poll(fds, 1, 2000);

        if (ret == -1) {
            perror("poll");
            return 1;
        } else if (ret == 0) {
            printf("[Tour %d] Timeout ! Rien recu.\n", iterations + 1);
        } else {
            /* Verifier quel evenement s'est produit */
            if (fds[0].revents & POLLIN) {
                char buffer[256];
                ssize_t n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
                if (n <= 0) break;
                buffer[n] = '\0';
                printf("[Tour %d] Vous avez tape : %s",
                       iterations + 1, buffer);
            }
            if (fds[0].revents & POLLHUP) {
                printf("[Tour %d] Fin de l'entree (POLLHUP)\n",
                       iterations + 1);
                break;
            }
        }

        iterations++;
    }

    printf("Fin du programme.\n");

    return 0;
}
