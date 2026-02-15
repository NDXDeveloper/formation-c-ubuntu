/* ============================================================================
   Section 16.4 : Permissions et modes
   Description : Gestionnaire complet affichant permissions, type et acces
   Fichier source : 04-permissions-modes.md
   ============================================================================ */

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static void print_permissions(mode_t mode) {
    /* Type de fichier */
    if (S_ISREG(mode)) printf("-");
    else if (S_ISDIR(mode)) printf("d");
    else if (S_ISLNK(mode)) printf("l");
    else printf("?");

    /* User */
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    if (mode & S_ISUID)
        printf((mode & S_IXUSR) ? "s" : "S");
    else
        printf((mode & S_IXUSR) ? "x" : "-");

    /* Group */
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    if (mode & S_ISGID)
        printf((mode & S_IXGRP) ? "s" : "S");
    else
        printf((mode & S_IXGRP) ? "x" : "-");

    /* Others */
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    if (mode & S_ISVTX)
        printf((mode & S_IXOTH) ? "t" : "T");
    else
        printf((mode & S_IXOTH) ? "x" : "-");

    printf(" (%04o)", mode & 07777);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <fichier>\n", argv[0]);
        return 1;
    }

    struct stat st;
    if (stat(argv[1], &st) == -1) {
        perror("stat");
        return 1;
    }

    printf("Fichier : %s\n", argv[1]);
    printf("Permissions : ");
    print_permissions(st.st_mode);
    printf("\n");

    printf("Proprietaire : UID %d\n", st.st_uid);
    printf("Groupe : GID %d\n", st.st_gid);
    printf("Taille : %ld octets\n", st.st_size);

    /* Tests d'acces */
    printf("\nVos permissions :\n");
    printf("  Lecture :   %s\n", access(argv[1], R_OK) == 0 ? "OUI" : "NON");
    printf("  Ecriture :  %s\n", access(argv[1], W_OK) == 0 ? "OUI" : "NON");
    printf("  Execution : %s\n", access(argv[1], X_OK) == 0 ? "OUI" : "NON");

    return 0;
}
