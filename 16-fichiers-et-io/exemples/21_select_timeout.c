/* ============================================================================
   Section 16.7 : I/O multiplexing
   Description : Utilisation de select() avec timeout sur stdin
   Fichier source : 07-io-multiplexing.md
   ============================================================================ */

/* Tester : echo "hello" | ./21_select_timeout
   Ou lancer interactivement et taper du texte (timeout 2s) */

#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>

int main(void) {
    fd_set readfds;
    struct timeval tv;
    int iterations = 0;

    printf("Surveillance de stdin avec select() (timeout 2s, 3 tours)\n");

    while (iterations < 3) {
        /* 1. Preparer l'ensemble des descripteurs */
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);  /* Surveiller stdin (0) */

        /* 2. Configurer le timeout (2 secondes) */
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        /* 3. Appeler select() */
        int ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);

        if (ret == -1) {
            perror("select");
            return 1;
        } else if (ret == 0) {
            printf("[Tour %d] Timeout ! Rien recu.\n", iterations + 1);
        } else {
            /* 4. Verifier quel descripteur est pret */
            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                char buffer[256];
                ssize_t n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
                if (n <= 0) break;
                buffer[n] = '\0';
                printf("[Tour %d] Vous avez tape : %s", iterations + 1, buffer);
            }
        }

        iterations++;
    }

    printf("Fin du programme.\n");

    return 0;
}
