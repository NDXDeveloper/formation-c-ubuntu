/* ============================================================================
   Section 34.1.2 : Interaction avec le systeme
   Description : Copie de fichier avec appels systeme bas niveau (open/read/write)
   Fichier source : 01.2-interaction-systeme.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int copier_fichier(const char *source, const char *destination) {
    int fd_source, fd_dest;
    ssize_t bytes_lus, bytes_ecrits;
    char buffer[4096];

    // Ouvrir le fichier source en lecture
    fd_source = open(source, O_RDONLY);
    if (fd_source == -1) {
        perror("Erreur ouverture source");
        return -1;
    }

    // Créer/ouvrir le fichier destination en écriture
    // 0644 = rw-r--r--
    fd_dest = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dest == -1) {
        perror("Erreur création destination");
        close(fd_source);
        return -1;
    }

    // Copier par blocs
    while ((bytes_lus = read(fd_source, buffer, sizeof(buffer))) > 0) {
        bytes_ecrits = write(fd_dest, buffer, (size_t)bytes_lus);
        if (bytes_ecrits != bytes_lus) {
            perror("Erreur écriture");
            close(fd_source);
            close(fd_dest);
            return -1;
        }
    }

    if (bytes_lus == -1) {
        perror("Erreur lecture");
    }

    close(fd_source);
    close(fd_dest);
    return 0;
}

int main(void) {
    const char *source = "/tmp/test_copy_src.txt";
    const char *dest = "/tmp/test_copy_dst.txt";

    // Creer un fichier source de test
    int fd = open(source, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("creation source");
        return 1;
    }
    const char *contenu = "Contenu du fichier source.\nDeuxieme ligne.\n";
    write(fd, contenu, strlen(contenu));
    close(fd);

    printf("Fichier source cree: %s\n", source);

    // Copier
    if (copier_fichier(source, dest) == 0) {
        printf("Copie reussie: %s -> %s\n", source, dest);
    } else {
        printf("Erreur de copie!\n");
    }

    // Verifier le contenu
    printf("\nContenu de la copie:\n");
    fd = open(dest, O_RDONLY);
    if (fd != -1) {
        char buf[256];
        ssize_t n = read(fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("%s", buf);
        }
        close(fd);
    }

    // Nettoyer
    unlink(source);
    unlink(dest);

    return 0;
}
