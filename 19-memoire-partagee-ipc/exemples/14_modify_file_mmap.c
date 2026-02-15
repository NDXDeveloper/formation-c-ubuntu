/* ============================================================================
   Section 19.5 : Memory-mapped Files (mmap)
   Description : Modifier un fichier avec mmap (mise en majuscules)
   Fichier source : 05-mmap.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fichier>\n", argv[0]);
        return 1;
    }

    /* 1. Ouvrir en lecture/ecriture */
    int fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    /* 2. Obtenir la taille */
    struct stat sb;
    fstat(fd, &sb);
    size_t file_size = (size_t)sb.st_size;

    /* 3. Mapper avec PROT_WRITE et MAP_SHARED */
    /*    MAP_SHARED = les modifications seront ecrites dans le fichier */
    char *data = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    close(fd);

    printf("Contenu original : %.*s\n", (int)file_size, data);

    /* 4. Modifier le contenu (mettre en majuscules) */
    for (size_t i = 0; i < file_size; i++) {
        data[i] = (char)toupper((unsigned char)data[i]);
    }

    printf("Contenu modifie : %.*s\n", (int)file_size, data);

    /* 5. Forcer la synchronisation sur disque (optionnel) */
    if (msync(data, file_size, MS_SYNC) == -1) {
        perror("msync");
    }

    /* 6. Liberer */
    munmap(data, file_size);

    printf("Fichier modifie avec succes\n");

    return 0;
}
