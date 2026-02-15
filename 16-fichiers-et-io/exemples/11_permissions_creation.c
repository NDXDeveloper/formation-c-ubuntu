/* ============================================================================
   Section 16.4 : Permissions et modes
   Description : Creation de fichiers avec differentes permissions
   Fichier source : 04-permissions-modes.md
   ============================================================================ */

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    int fd;

    /* Creer un fichier prive (rw-------, 0600) */
    fd = open("/tmp/secret.txt",
              O_WRONLY | O_CREAT | O_TRUNC,
              S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open secret");
        return 1;
    }
    const char *secret = "Donnees confidentielles\n";
    write(fd, secret, strlen(secret));
    close(fd);

    /* Creer un fichier standard (rw-r--r--, 0644) */
    fd = open("/tmp/data.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open data");
        return 1;
    }
    write(fd, "Hello\n", 6);
    close(fd);

    /* Creer un script executable (rwxr-xr-x, 0755) */
    fd = open("/tmp/script.sh",
              O_WRONLY | O_CREAT | O_TRUNC,
              S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    if (fd == -1) {
        perror("open script");
        return 1;
    }
    const char *script = "#!/bin/bash\necho 'Hello'\n";
    write(fd, script, strlen(script));
    close(fd);

    /* Afficher les permissions */
    printf("Fichiers crees dans /tmp :\n");
    printf("  secret.txt : mode 0600 (rw-------)\n");
    printf("  data.txt   : mode 0644 (rw-r--r--)\n");
    printf("  script.sh  : mode 0755 (rwxr-xr-x)\n");

    /* Nettoyage */
    unlink("/tmp/secret.txt");
    unlink("/tmp/data.txt");
    unlink("/tmp/script.sh");

    return 0;
}
