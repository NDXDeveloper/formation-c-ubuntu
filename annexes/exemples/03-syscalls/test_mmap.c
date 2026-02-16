// Ligne 873-895 : mmap anonymous
#include <sys/mman.h>
#include <stdio.h>

int main() {
    size_t size = 4096;
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Utiliser la mémoire
    int *data = (int *)ptr;
    data[0] = 42;
    printf("data[0] = %d\n", data[0]);

    munmap(ptr, size);
    return 0;
}
