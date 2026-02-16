// Ligne 396-418 : stat
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>

int main() {
    struct stat st;

    if (stat("file.txt", &st) == -1) {
        perror("stat");
        return 1;
    }

    printf("Taille: %ld octets\n", st.st_size);
    printf("Permissions: %o\n", st.st_mode & 0777);
    printf("Modification: %s", ctime(&st.st_mtime));

    if (S_ISREG(st.st_mode))  printf("Fichier régulier\n");
    if (S_ISDIR(st.st_mode))  printf("Répertoire\n");
    if (S_ISLNK(st.st_mode))  printf("Lien symbolique\n");

    return 0;
}
