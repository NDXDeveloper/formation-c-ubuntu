/* ============================================================================
   Section 16.3 : Difference appels systeme vs bibliotheque
   Description : Benchmark comparant write() vs fprintf() pour 10000 lignes
   Fichier source : 03-systeme-vs-bibliotheque.md
   ============================================================================ */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

/* Test avec appels systeme */
static void test_syscalls(void) {
    int fd = open("/tmp/syscall.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    for (int i = 0; i < 10000; i++) {
        char buf[32];
        int len = snprintf(buf, sizeof(buf), "Ligne %d\n", i);
        write(fd, buf, (size_t)len);  /* 10 000 appels systeme ! */
    }
    close(fd);
}

/* Test avec bibliotheque C */
static void test_stdio(void) {
    FILE *fp = fopen("/tmp/stdio.txt", "w");
    for (int i = 0; i < 10000; i++) {
        fprintf(fp, "Ligne %d\n", i);  /* Bufferise */
    }
    fclose(fp);  /* Un seul (ou quelques) appels systeme */
}

int main(void) {
    clock_t start, end;

    /* Test appels systeme */
    start = clock();
    test_syscalls();
    end = clock();
    printf("Appels systeme : %.3f secondes\n",
           (double)(end - start) / CLOCKS_PER_SEC);

    /* Test bibliotheque C */
    start = clock();
    test_stdio();
    end = clock();
    printf("Bibliotheque C : %.3f secondes\n",
           (double)(end - start) / CLOCKS_PER_SEC);

    /* Nettoyage */
    unlink("/tmp/syscall.txt");
    unlink("/tmp/stdio.txt");

    return 0;
}
