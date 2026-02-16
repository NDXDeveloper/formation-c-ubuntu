// Ligne 293-312 : dup2 redirect
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Rediriger stdout vers le fichier
    dup2(fd, STDOUT_FILENO);
    close(fd);

    printf("Ceci sera écrit dans output.txt\n");

    return 0;
}
