/* ============================================================================
   Section 34.1.2 : Interaction avec le systeme
   Description : Informations fichier avec lstat() (permissions, type, dates)
   Fichier source : 01.2-interaction-systeme.md
   ============================================================================ */
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

void afficher_infos_fichier(const char *chemin) {
    struct stat st;

    // lstat() au lieu de stat() pour détecter les liens symboliques
    if (lstat(chemin, &st) == -1) {
        perror("lstat");
        return;
    }

    printf("Fichier: %s\n", chemin);
    printf("Taille: %ld octets\n", st.st_size);
    printf("Blocs: %ld\n", st.st_blocks);
    printf("Inode: %ld\n", st.st_ino);
    printf("Liens: %ld\n", st.st_nlink);

    // Propriétaire
    struct passwd *pw = getpwuid(st.st_uid);
    struct group *gr = getgrgid(st.st_gid);
    printf("Propriétaire: %s (%d)\n", pw ? pw->pw_name : "?", st.st_uid);
    printf("Groupe: %s (%d)\n", gr ? gr->gr_name : "?", st.st_gid);

    // Permissions
    printf("Permissions: ");
    printf((S_ISDIR(st.st_mode)) ? "d" : "-");
    printf((st.st_mode & S_IRUSR) ? "r" : "-");
    printf((st.st_mode & S_IWUSR) ? "w" : "-");
    printf((st.st_mode & S_IXUSR) ? "x" : "-");
    printf((st.st_mode & S_IRGRP) ? "r" : "-");
    printf((st.st_mode & S_IWGRP) ? "w" : "-");
    printf((st.st_mode & S_IXGRP) ? "x" : "-");
    printf((st.st_mode & S_IROTH) ? "r" : "-");
    printf((st.st_mode & S_IWOTH) ? "w" : "-");
    printf((st.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");

    // Type de fichier
    printf("Type: ");
    if (S_ISREG(st.st_mode)) printf("Fichier régulier\n");
    else if (S_ISDIR(st.st_mode)) printf("Répertoire\n");
    else if (S_ISLNK(st.st_mode)) printf("Lien symbolique\n");
    else if (S_ISCHR(st.st_mode)) printf("Périphérique caractère\n");
    else if (S_ISBLK(st.st_mode)) printf("Périphérique bloc\n");
    else if (S_ISFIFO(st.st_mode)) printf("FIFO/pipe\n");
    else if (S_ISSOCK(st.st_mode)) printf("Socket\n");

    // Dates
    printf("Dernier accès: %s", ctime(&st.st_atime));
    printf("Dernière modification: %s", ctime(&st.st_mtime));
    printf("Dernier changement: %s", ctime(&st.st_ctime));
}

int main(void) {
    afficher_infos_fichier("/etc/passwd");
    printf("\n");
    afficher_infos_fichier("/tmp");
    return 0;
}
