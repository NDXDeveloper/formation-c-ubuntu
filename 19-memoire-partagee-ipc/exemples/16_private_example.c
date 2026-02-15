/* ============================================================================
   Section 19.5 : Memory-mapped Files (mmap)
   Description : MAP_PRIVATE - copy-on-write, fichier non modifie
   Fichier source : 05-mmap.md
   ============================================================================ */
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    int fd = open("private.txt", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    write(fd, "AAAA", 4);

    /* MAP_PRIVATE : modifications privees (copy-on-write) */
    char *data = mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
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

    printf("Apres mapping : %.*s\n", 4, data);

    munmap(data, 4);

    /* Verifier le fichier (reste inchange) */
    fd = open("private.txt", O_RDONLY);
    char buf[4];
    read(fd, buf, 4);
    printf("Fichier : %.*s\n", 4, buf);  /* Affiche : AAAA (inchange) */
    close(fd);

    unlink("private.txt");  /* Nettoyage */
    return 0;
}
