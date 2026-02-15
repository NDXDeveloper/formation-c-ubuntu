/* ============================================================================
   Section 25.2 : La variable errno
   Description : Codes errno avec switch et open sur /etc/shadow
   Fichier source : 02-errno.md
   ============================================================================ */

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    int fd = open("/etc/shadow", O_RDONLY);  /* Fichier protege */

    if (fd == -1) {
        switch (errno) {
            case EACCES:
                fprintf(stderr, "Erreur : Permission refusee\n");
                break;
            case ENOENT:
                fprintf(stderr, "Erreur : Fichier inexistant\n");
                break;
            case EISDIR:
                fprintf(stderr, "Erreur : C'est un repertoire\n");
                break;
            default:
                fprintf(stderr, "Erreur inconnue : %s\n", strerror(errno));
        }
        return 1;
    }

    printf("Fichier ouvert avec succes\n");
    close(fd);
    return 0;
}
