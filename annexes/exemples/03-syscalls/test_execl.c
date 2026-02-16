// Ligne 513-525 : execl
#include <unistd.h>
#include <stdio.h>

int main() {
    printf("Avant exec\n");

    execl("/bin/ls", "ls", "-l", NULL);

    // Si on arrive ici, c'est une erreur
    perror("execl");
    return 1;
}
