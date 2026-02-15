/* ============================================================================
   Section 19.5 : Memory-mapped Files (mmap)
   Description : Lire un fichier avec mmap
   Fichier source : 05-mmap.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fichier>\n", argv[0]);
        return 1;
    }

    /* 1. Ouvrir le fichier */
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    /* 2. Obtenir la taille du fichier */
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        close(fd);
        return 1;
    }

    size_t file_size = (size_t)sb.st_size;
    printf("Taille du fichier : %zu octets\n", file_size);

    /* 3. Mapper le fichier en memoire */
    char *file_content = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_content == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    /* 4. On peut fermer le descripteur maintenant (le mapping persiste) */
    close(fd);

    /* 5. Utiliser le contenu comme un simple pointeur */
    printf("\nContenu du fichier :\n");
    printf("---\n");
    fflush(stdout);

    /* Ecrire sur stdout */
    write(STDOUT_FILENO, file_content, file_size);

    printf("---\n");

    /* Ou utiliser comme un tableau */
    printf("Premier caractere : '%c'\n", file_content[0]);
    printf("Dernier caractere : 0x%02x\n", (unsigned char)file_content[file_size - 1]);

    /* 6. Liberer le mapping */
    if (munmap(file_content, file_size) == -1) {
        perror("munmap");
        return 1;
    }

    printf("Mapping libere avec succes\n");

    return 0;
}
