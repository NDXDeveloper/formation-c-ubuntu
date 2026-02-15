/* ============================================================================
   Section 19.5 : Memory-mapped Files (mmap)
   Description : MAP_SHARED - modifications persistantes dans le fichier
   Fichier source : 05-mmap.md
   ============================================================================ */
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    int fd = open("shared.txt", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    write(fd, "AAAA", 4);

    /* MAP_SHARED : modifications visibles dans le fichier */
    char *data = mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }
    close(fd);

    printf("Avant : %.*s\n", 4, data);

    /* Modifier */
    data[0] = 'B';
    data[1] = 'B';

    printf("Apres : %.*s\n", 4, data);

    /* Forcer l'ecriture */
    msync(data, 4, MS_SYNC);

    munmap(data, 4);

    /* Verifier que le fichier est modifie */
    fd = open("shared.txt", O_RDONLY);
    char buf[4];
    read(fd, buf, 4);
    printf("Fichier : %.*s\n", 4, buf);  /* Affiche : BBAA */
    close(fd);

    unlink("shared.txt");  /* Nettoyage */
    return 0;
}
