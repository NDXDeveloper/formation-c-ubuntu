/* ============================================================================
   Section 17.8 : Pipes anonymes
   Description : Communication parent-enfant via pipe
   Fichier source : 08-pipes-anonymes.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
    int pipefd[2];
    pid_t pid;

    /* 1. Creer le pipe AVANT le fork */
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    /* 2. Creer le processus enfant */
    pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* === PROCESSUS ENFANT === */

        /* Fermer l'extremite d'ecriture (on ne l'utilise pas) */
        close(pipefd[1]);

        printf("[Enfant] En attente de donnees...\n");

        /* Lire depuis le pipe */
        char buffer[100];
        ssize_t n = read(pipefd[0], buffer, sizeof(buffer));

        if (n > 0) {
            printf("[Enfant] Message recu: \"%s\"\n", buffer);
        }

        /* Fermer l'extremite de lecture */
        close(pipefd[0]);

        return 0;
    }
    else {
        /* === PROCESSUS PARENT === */

        /* Fermer l'extremite de lecture (on ne l'utilise pas) */
        close(pipefd[0]);

        /* Envoyer un message */
        const char *message = "Bonjour de la part du parent!";

        printf("[Parent] Envoi du message: \"%s\"\n", message);

        write(pipefd[1], message, strlen(message) + 1);

        /* Fermer l'extremite d'ecriture */
        close(pipefd[1]);

        /* Attendre l'enfant */
        wait(NULL);

        printf("[Parent] Communication terminee\n");
    }

    return 0;
}
