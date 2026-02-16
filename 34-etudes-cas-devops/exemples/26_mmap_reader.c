/* ============================================================================
   Section 34.2.1 : Lecture efficace de gros fichiers
   Description : Lecture de fichier avec mmap (memory mapping)
   Fichier source : 02.1-lecture-gros-fichiers.md
   ============================================================================ */
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

void lire_mmap(const char *chemin) {
    int fd = open(chemin, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        close(fd);
        return;
    }

    size_t taille = (size_t)sb.st_size;

    char *data = mmap(NULL, taille, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return;
    }

    madvise(data, taille, MADV_SEQUENTIAL);

    clock_t debut = clock();

    // Traiter ligne par ligne
    const char *debut_ligne = data;
    const char *fin_fichier = data + taille;
    long long nb_lignes = 0;

    for (const char *p = data; p < fin_fichier; p++) {
        if (*p == '\n') {
            size_t longueur = (size_t)(p - debut_ligne);

            // Afficher les 5 premieres lignes
            if (nb_lignes < 5) {
                printf("Ligne %lld (%zu octets): %.*s\n",
                       nb_lignes + 1, longueur, (int)longueur, debut_ligne);
            }

            nb_lignes++;
            debut_ligne = p + 1;
        }
    }

    // Derniere ligne si pas de \n final
    if (debut_ligne < fin_fichier) {
        nb_lignes++;
    }

    clock_t fin = clock();
    double temps = (double)(fin - debut) / CLOCKS_PER_SEC;

    printf("\nTotal lignes: %lld\n", nb_lignes);
    printf("Taille: %.2f MB\n", taille / (1024.0 * 1024.0));
    printf("Temps: %.3f secondes\n", temps);
    if (temps > 0) {
        printf("Débit: %.2f MB/s\n", (taille / (1024.0 * 1024.0)) / temps);
    }

    munmap(data, taille);
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fichier>\n", argv[0]);
        return 1;
    }

    lire_mmap(argv[1]);
    return 0;
}
