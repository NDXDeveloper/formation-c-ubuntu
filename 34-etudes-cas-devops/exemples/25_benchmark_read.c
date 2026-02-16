/* ============================================================================
   Section 34.2.1 : Lecture efficace de gros fichiers
   Description : Benchmark comparatif fread/fgets/mmap
   Fichier source : 02.1-lecture-gros-fichiers.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE (64 * 1024)

typedef struct {
    const char *nom;
    double temps;
    double debit;  // MB/s
} ResultatBenchmark;

// Benchmark fgets()
double bench_fgets(const char *chemin, size_t *taille_fichier) {
    FILE *fp = fopen(chemin, "r");
    if (!fp) return -1;

    char ligne[4096];
    clock_t debut = clock();

    while (fgets(ligne, sizeof(ligne), fp) != NULL) {
        /* traitement */
    }

    clock_t fin = clock();

    fseek(fp, 0, SEEK_END);
    *taille_fichier = (size_t)ftell(fp);

    fclose(fp);
    return (double)(fin - debut) / CLOCKS_PER_SEC;
}

// Benchmark fread()
double bench_fread(const char *chemin, size_t *taille_fichier) {
    FILE *fp = fopen(chemin, "rb");
    if (!fp) return -1;

    char *buffer = malloc(BUFFER_SIZE);
    if (!buffer) { fclose(fp); return -1; }
    clock_t debut = clock();

    while (fread(buffer, 1, BUFFER_SIZE, fp) > 0) {
        /* traitement */
    }

    clock_t fin = clock();

    fseek(fp, 0, SEEK_END);
    *taille_fichier = (size_t)ftell(fp);

    free(buffer);
    fclose(fp);
    return (double)(fin - debut) / CLOCKS_PER_SEC;
}

// Benchmark mmap()
double bench_mmap(const char *chemin, size_t *taille_fichier) {
    int fd = open(chemin, O_RDONLY);
    if (fd == -1) return -1;

    struct stat sb;
    fstat(fd, &sb);
    *taille_fichier = (size_t)sb.st_size;

    char *data = mmap(NULL, *taille_fichier, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        close(fd);
        return -1;
    }

    clock_t debut = clock();

    long long compteur = 0;
    for (size_t i = 0; i < *taille_fichier; i++) {
        if (data[i] == '\n') compteur++;
    }
    (void)compteur;

    clock_t fin = clock();

    munmap(data, *taille_fichier);
    close(fd);
    return (double)(fin - debut) / CLOCKS_PER_SEC;
}

void afficher_resultats(ResultatBenchmark *resultats, int nb_resultats) {
    printf("+------------------------+---------------+---------------+\n");
    printf("| %-22s | %-13s | %-13s |\n", "Methode", "Temps (s)", "Debit (MB/s)");
    printf("+------------------------+---------------+---------------+\n");

    for (int i = 0; i < nb_resultats; i++) {
        printf("| %-22s | %13.3f | %13.2f |\n",
               resultats[i].nom,
               resultats[i].temps,
               resultats[i].debit);
    }

    printf("+------------------------+---------------+---------------+\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fichier>\n", argv[0]);
        return 1;
    }

    const char *fichier = argv[1];
    size_t taille;

    ResultatBenchmark resultats[4];
    int idx = 0;

    printf("Test fread()...\n");
    double temps = bench_fread(fichier, &taille);
    if (temps > 0) {
        resultats[idx].nom = "fread() (64KB buffer)";
        resultats[idx].temps = temps;
        resultats[idx].debit = (taille / (1024.0 * 1024.0)) / temps;
        idx++;
    }

    printf("Test fgets()...\n");
    temps = bench_fgets(fichier, &taille);
    if (temps > 0) {
        resultats[idx].nom = "fgets() (ligne/ligne)";
        resultats[idx].temps = temps;
        resultats[idx].debit = (taille / (1024.0 * 1024.0)) / temps;
        idx++;
    }

    printf("Test mmap()...\n");
    temps = bench_mmap(fichier, &taille);
    if (temps > 0) {
        resultats[idx].nom = "mmap() + parcours";
        resultats[idx].temps = temps;
        resultats[idx].debit = (taille / (1024.0 * 1024.0)) / temps;
        idx++;
    }

    printf("\nResultats pour %s (%.2f MB):\n\n",
           fichier, taille / (1024.0 * 1024.0));
    afficher_resultats(resultats, idx);

    return 0;
}
