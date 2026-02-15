/* ============================================================================
   Section 16.5 : I/O bufferise vs non bufferise
   Description : Benchmark comparant 4 modes d'ecriture (10000 iterations)
   Fichier source : 05-io-bufferise.md
   ============================================================================ */

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define ITERATIONS 10000

static double get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

static void test_unbuffered(void) {
    int fd = open("/tmp/unbuffered.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    for (int i = 0; i < ITERATIONS; i++) {
        char buf[64];
        int len = snprintf(buf, sizeof(buf), "Ligne %d\n", i);
        write(fd, buf, (size_t)len);
    }
    close(fd);
}

static void test_buffered(void) {
    FILE *fp = fopen("/tmp/buffered.txt", "w");
    for (int i = 0; i < ITERATIONS; i++) {
        fprintf(fp, "Ligne %d\n", i);
    }
    fclose(fp);
}

static void test_buffered_fflush(void) {
    FILE *fp = fopen("/tmp/buffered_flush.txt", "w");
    for (int i = 0; i < ITERATIONS; i++) {
        fprintf(fp, "Ligne %d\n", i);
        fflush(fp);  /* Force l'ecriture a chaque ligne */
    }
    fclose(fp);
}

static void test_no_buffering(void) {
    FILE *fp = fopen("/tmp/no_buffer.txt", "w");
    setbuf(fp, NULL);  /* Desactiver le buffering */
    for (int i = 0; i < ITERATIONS; i++) {
        fprintf(fp, "Ligne %d\n", i);
    }
    fclose(fp);
}

int main(void) {
    double start, end;

    printf("Benchmark avec %d iterations\n\n", ITERATIONS);

    /* Test 1 : Non bufferise (appels systeme) */
    start = get_time_ms();
    test_unbuffered();
    end = get_time_ms();
    printf("1. write() non bufferise : %.2f ms\n", end - start);

    /* Test 2 : Bufferise (bibliotheque C) */
    start = get_time_ms();
    test_buffered();
    end = get_time_ms();
    printf("2. fprintf() bufferise   : %.2f ms\n", end - start);

    /* Test 3 : Bufferise avec fflush a chaque ligne */
    start = get_time_ms();
    test_buffered_fflush();
    end = get_time_ms();
    printf("3. fprintf() + fflush()  : %.2f ms\n", end - start);

    /* Test 4 : Sans buffering via setbuf() */
    start = get_time_ms();
    test_no_buffering();
    end = get_time_ms();
    printf("4. fprintf() sans buffer : %.2f ms\n", end - start);

    /* Nettoyage */
    unlink("/tmp/unbuffered.txt");
    unlink("/tmp/buffered.txt");
    unlink("/tmp/buffered_flush.txt");
    unlink("/tmp/no_buffer.txt");

    return 0;
}
