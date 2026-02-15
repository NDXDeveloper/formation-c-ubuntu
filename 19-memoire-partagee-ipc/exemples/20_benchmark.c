/* ============================================================================
   Section 19.5 : Memory-mapped Files (mmap)
   Description : Benchmark comparant read() vs mmap() sur un gros fichier
   Fichier source : 05-mmap.md
   ============================================================================ */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define FILE_SIZE (10 * 1024 * 1024)  /* 10 MB (reduit pour le test) */

double get_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

void benchmark_read(void) {
    int fd = open("testfile.bin", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }
    char buffer[4096];
    long sum = 0;

    double start = get_time();

    ssize_t n;
    while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
        sum += buffer[0];  /* Acces pour forcer la lecture */
    }

    double elapsed = get_time() - start;
    close(fd);

    printf("read() : %.3f secondes (sum=%ld)\n", elapsed, sum);
}

void benchmark_mmap(void) {
    int fd = open("testfile.bin", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }
    struct stat sb;
    fstat(fd, &sb);

    double start = get_time();

    char *data = mmap(NULL, (size_t)sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return;
    }

    long sum = 0;
    for (size_t i = 0; i < (size_t)sb.st_size; i += 4096) {
        sum += data[i];
    }

    munmap(data, (size_t)sb.st_size);
    close(fd);

    double elapsed = get_time() - start;

    printf("mmap() : %.3f secondes (sum=%ld)\n", elapsed, sum);
}

int main(void) {
    /* Creer un fichier de test */
    printf("Creation du fichier de test (%d MB)...\n", FILE_SIZE / (1024 * 1024));
    int fd = open("testfile.bin", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    char buffer[4096] = {0};
    for (int i = 0; i < FILE_SIZE / 4096; i++) {
        write(fd, buffer, sizeof(buffer));
    }
    close(fd);

    /* Benchmarks */
    printf("Benchmarking...\n\n");
    benchmark_read();
    benchmark_mmap();

    /* Nettoyer */
    unlink("testfile.bin");

    return 0;
}
