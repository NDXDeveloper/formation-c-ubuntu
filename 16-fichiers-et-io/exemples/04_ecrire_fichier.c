/* ============================================================================
   Section 16.1 : Descripteurs de fichiers
   Description : Ecriture dans un fichier avec comptage d'octets
   Fichier source : 01-descripteurs-fichiers.md
   ============================================================================ */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    int fd;
    const char *texte = "Bonjour Linux!\n";
    ssize_t bytes_written;

    fd = open("/tmp/sortie.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    bytes_written = write(fd, texte, strlen(texte));
    if (bytes_written == -1) {
        perror("write");
        close(fd);
        return 1;
    }

    printf("%zd octets ecrits\n", bytes_written);

    close(fd);

    /* Nettoyage */
    unlink("/tmp/sortie.txt");

    return 0;
}
