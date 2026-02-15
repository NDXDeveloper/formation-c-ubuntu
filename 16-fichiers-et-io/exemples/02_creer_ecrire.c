/* ============================================================================
   Section 16.1 : Descripteurs de fichiers
   Description : Creation et ecriture dans un fichier avec open/write/close
   Fichier source : 01-descripteurs-fichiers.md
   ============================================================================ */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    int fd;
    const char *texte = "Premiere ligne\n";

    /* Creer un fichier avec permissions 0644 (rw-r--r--) */
    fd = open("/tmp/nouveau.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        perror("Erreur creation fichier");
        return 1;
    }

    printf("Fichier ouvert avec succes, descripteur = %d\n", fd);

    /* Ecrire dans le fichier */
    write(fd, texte, strlen(texte));

    close(fd);
    printf("Fichier /tmp/nouveau.txt cree et ecrit.\n");

    /* Nettoyage */
    unlink("/tmp/nouveau.txt");

    return 0;
}
