// Ligne 575-597 : waitpid
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        printf("Enfant travaille...\n");
        sleep(2);
        return 42;  // Code de retour
    } else {
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Enfant terminé avec code: %d\n", WEXITSTATUS(status));
        }
    }

    return 0;
}
