/* ============================================================================
   Section 19.5 : Memory-mapped Files (mmap)
   Description : Memoire anonyme MAP_ANONYMOUS (sans fichier)
   Fichier source : 05-mmap.md
   ============================================================================ */
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>

int main(void) {
    size_t size = 4096;  /* 1 page */

    /* Allouer de la memoire (pas de fichier) */
    /* fd = -1, offset = 0 */
    int *data = mmap(NULL, size, PROT_READ | PROT_WRITE,
                     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (data == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    /* Utiliser comme un tableau normal */
    data[0] = 42;
    data[1] = 100;

    printf("data[0] = %d\n", data[0]);
    printf("data[1] = %d\n", data[1]);

    /* Liberer */
    munmap(data, size);

    return 0;
}
