// Test pipe parent-enfant (ligne 330-364)
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main() {
    int pipefd[2];
    char buffer[100];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {  // Enfant
        close(pipefd[1]);  // Fermer écriture

        read(pipefd[0], buffer, sizeof(buffer));
        printf("Enfant a reçu: %s\n", buffer);

        close(pipefd[0]);
    } else {  // Parent
        close(pipefd[0]);  // Fermer lecture

        const char *msg = "Hello from parent";
        write(pipefd[1], msg, strlen(msg) + 1);

        close(pipefd[1]);
        wait(NULL);
    }

    return 0;
}
