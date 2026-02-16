// Ligne 466-488 : fork
#include <unistd.h>
#include <stdio.h>

int main() {
    printf("Avant fork\n");

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // Code de l'enfant
        printf("Je suis l'enfant (PID: %d)\n", getpid());
    } else {
        // Code du parent
        printf("Je suis le parent (PID: %d), enfant: %d\n", getpid(), pid);
    }

    printf("Après fork (exécuté par les deux)\n");
    return 0;
}
