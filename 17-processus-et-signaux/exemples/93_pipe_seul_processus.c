/* ============================================================================
   Section 17.8 : Pipes anonymes
   Description : Communication via pipe dans un seul processus
   Fichier source : 08-pipes-anonymes.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    int pipefd[2];
    char buffer[100];
    const char *message = "Hello via pipe!";

    /* Creer le pipe */
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    printf("Envoi du message: \"%s\"\n", message);

    /* Ecrire dans le pipe */
    write(pipefd[1], message, strlen(message) + 1);

    /* Lire depuis le pipe */
    read(pipefd[0], buffer, sizeof(buffer));

    printf("Message recu: \"%s\"\n", buffer);

    /* Fermer les descripteurs */
    close(pipefd[0]);
    close(pipefd[1]);

    return 0;
}
