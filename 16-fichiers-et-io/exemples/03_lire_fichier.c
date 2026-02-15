/* ============================================================================
   Section 16.1 : Descripteurs de fichiers
   Description : Lecture d'un fichier par blocs avec read()
   Fichier source : 01-descripteurs-fichiers.md
   ============================================================================ */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 4096

int main(void) {
    int fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    /* Creer un fichier de test */
    fd = open("/tmp/test_lecture.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open (creation)");
        return 1;
    }
    const char *contenu = "Ligne 1: Bonjour\nLigne 2: Linux\nLigne 3: C\n";
    write(fd, contenu, strlen(contenu));
    close(fd);

    /* Ouvrir en lecture */
    fd = open("/tmp/test_lecture.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("Contenu du fichier :\n");

    /* Lire le fichier par blocs */
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0';  /* Terminer la chaine */
        printf("%s", buffer);
    }

    if (bytes_read == -1) {
        perror("read");
    }

    close(fd);

    /* Nettoyage */
    unlink("/tmp/test_lecture.txt");

    return 0;
}
