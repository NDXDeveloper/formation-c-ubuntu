// Ligne 1456-1494 : copy_file pattern (ajout main)
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int copy_file(const char *src, const char *dest) {
    int fd_src = open(src, O_RDONLY);
    if (fd_src == -1) {
        perror("open source");
        return -1;
    }

    int fd_dest = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dest == -1) {
        perror("open dest");
        close(fd_src);
        return -1;
    }

    char buffer[4096];
    ssize_t bytes;

    while ((bytes = read(fd_src, buffer, sizeof(buffer))) > 0) {
        if (write(fd_dest, buffer, bytes) != bytes) {
            perror("write");
            close(fd_src);
            close(fd_dest);
            return -1;
        }
    }

    if (bytes == -1) {
        perror("read");
    }

    close(fd_src);
    close(fd_dest);
    return bytes == -1 ? -1 : 0;
}

int main() {
    // Créer un fichier source pour le test
    int fd = open("source.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    write(fd, "test copy\n", 10);
    close(fd);

    int result = copy_file("source.txt", "dest.txt");
    printf("copy_file: %s\n", result == 0 ? "OK" : "ERREUR");
    return 0;
}
