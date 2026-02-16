// Ligne 899-930 : mmap fichier
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>

int main() {
    int fd = open("file.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct stat st;
    fstat(fd, &st);

    char *mapped = mmap(NULL, st.st_size, PROT_READ,
                        MAP_PRIVATE, fd, 0);
    close(fd);  // Peut fermer après mmap

    if (mapped == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Lire le fichier en mémoire
    printf("%.*s", (int)st.st_size, mapped);

    munmap(mapped, st.st_size);
    return 0;
}
