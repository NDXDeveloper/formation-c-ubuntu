// Ligne 131-158 : open/close exemple
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main() {
    // Ouvrir en lecture
    int fd = open("file.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Créer un nouveau fichier
    int fd2 = open("newfile.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd2 == -1) {
        perror("open");
        close(fd);
        return 1;
    }

    close(fd);
    close(fd2);
    return 0;
}
