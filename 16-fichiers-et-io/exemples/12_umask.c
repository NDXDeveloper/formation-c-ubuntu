/* ============================================================================
   Section 16.4 : Permissions et modes
   Description : Manipulation du umask et son effet sur les permissions
   Fichier source : 04-permissions-modes.md
   ============================================================================ */

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    mode_t old_umask;

    /* Lire le umask actuel (et le changer temporairement) */
    old_umask = umask(0);
    printf("Umask actuel : %04o\n", old_umask);
    umask(old_umask);  /* Restaurer */

    /* Changer le umask pour ce processus */
    umask(0077);  /* Tous les fichiers seront prives */
    printf("Nouveau umask : 0077\n");

    /* Creer un fichier avec ce umask */
    int fd = open("/tmp/prive_umask.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    close(fd);

    /* Verifier les permissions effectives */
    struct stat st;
    if (stat("/tmp/prive_umask.txt", &st) == 0) {
        printf("Permissions demandees : 0666 (rw-rw-rw-)\n");
        printf("Permissions effectives : %04o\n", st.st_mode & 0777);
        printf("Calcul : 0666 & ~0077 = 0600 (rw-------)\n");
    }

    /* Restaurer l'ancien umask */
    umask(old_umask);
    printf("Umask restaure : %04o\n", old_umask);

    /* Nettoyage */
    unlink("/tmp/prive_umask.txt");

    return 0;
}
