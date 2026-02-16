// Ligne 1501-1538 : exécuter commande + capturer sortie
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

int main() {
    int pipefd[2];
    char buffer[1024];

    pipe(pipefd);

    pid_t pid = fork();
    if (pid == 0) {
        // Enfant
        close(pipefd[0]);  // Fermer lecture
        dup2(pipefd[1], STDOUT_FILENO);  // Rediriger stdout
        close(pipefd[1]);

        execlp("ls", "ls", "-l", NULL);
        perror("execlp");
        return 1;
    } else {
        // Parent
        close(pipefd[1]);  // Fermer écriture

        ssize_t bytes;
        while ((bytes = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes] = '\0';
            printf("%s", buffer);
        }

        close(pipefd[0]);
        wait(NULL);
    }

    return 0;
}
