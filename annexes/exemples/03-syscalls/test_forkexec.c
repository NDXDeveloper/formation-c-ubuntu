// Ligne 529-549 : fork + exec
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // Enfant exécute ls
        execl("/bin/ls", "ls", "-l", NULL);
        perror("execl");
        return 1;
    } else {
        // Parent attend
        wait(NULL);
        printf("Enfant terminé\n");
    }

    return 0;
}
