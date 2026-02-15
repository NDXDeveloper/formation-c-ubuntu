/* ============================================================================
   Section 16.3 : Difference appels systeme vs bibliotheque
   Description : Copie de fichier avec les deux approches (syscall et stdio)
   Fichier source : 03-systeme-vs-bibliotheque.md
   ============================================================================ */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

/* Copier avec appels systeme */
static int copy_syscall(const char *src, const char *dst) {
    int fd_in, fd_out;
    char buffer[8192];
    ssize_t n;

    fd_in = open(src, O_RDONLY);
    if (fd_in == -1) {
        perror("open source");
        return -1;
    }

    fd_out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out == -1) {
        perror("open destination");
        close(fd_in);
        return -1;
    }

    while ((n = read(fd_in, buffer, sizeof(buffer))) > 0) {
        if (write(fd_out, buffer, (size_t)n) != n) {
            perror("write");
            close(fd_in);
            close(fd_out);
            return -1;
        }
    }

    if (n == -1) {
        perror("read");
    }

    close(fd_in);
    close(fd_out);
    return 0;
}

/* Copier avec bibliotheque C */
static int copy_stdio(const char *src, const char *dst) {
    FILE *fp_in, *fp_out;
    char buffer[8192];
    size_t n;

    fp_in = fopen(src, "rb");
    if (fp_in == NULL) {
        perror("fopen source");
        return -1;
    }

    fp_out = fopen(dst, "wb");
    if (fp_out == NULL) {
        perror("fopen destination");
        fclose(fp_in);
        return -1;
    }

    while ((n = fread(buffer, 1, sizeof(buffer), fp_in)) > 0) {
        if (fwrite(buffer, 1, n, fp_out) != n) {
            perror("fwrite");
            fclose(fp_in);
            fclose(fp_out);
            return -1;
        }
    }

    if (ferror(fp_in)) {
        perror("fread");
    }

    fclose(fp_in);
    fclose(fp_out);
    return 0;
}

int main(void) {
    /* Creer un fichier source de test */
    FILE *fp = fopen("/tmp/source_test.txt", "w");
    if (fp == NULL) {
        perror("fopen");
        return 1;
    }
    for (int i = 0; i < 1000; i++) {
        fprintf(fp, "Ligne %d: donnees de test pour la copie\n", i);
    }
    fclose(fp);

    /* Copie avec appels systeme */
    printf("Copie avec appels systeme... ");
    if (copy_syscall("/tmp/source_test.txt", "/tmp/copie_syscall.txt") == 0) {
        printf("OK\n");
    }

    /* Copie avec bibliotheque C */
    printf("Copie avec bibliotheque C... ");
    if (copy_stdio("/tmp/source_test.txt", "/tmp/copie_stdio.txt") == 0) {
        printf("OK\n");
    }

    /* Nettoyage */
    unlink("/tmp/source_test.txt");
    unlink("/tmp/copie_syscall.txt");
    unlink("/tmp/copie_stdio.txt");

    return 0;
}
