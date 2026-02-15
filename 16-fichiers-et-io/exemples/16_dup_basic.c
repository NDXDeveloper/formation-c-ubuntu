/* ============================================================================
   Section 16.6 : dup, dup2 et redirection
   Description : Duplication de descripteurs avec dup() et partage d'offset
   Fichier source : 06-dup-redirection.md
   ============================================================================ */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    int fd = open("/tmp/dup_test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("fd original : %d\n", fd);

    /* Dupliquer le descripteur */
    int fd_copy = dup(fd);
    if (fd_copy == -1) {
        perror("dup");
        close(fd);
        return 1;
    }

    printf("fd duplique : %d\n", fd_copy);

    /* Les deux pointent vers le meme fichier et partagent l'offset */
    write(fd, "ABC", 3);       /* offset -> 3 */
    write(fd_copy, "DEF", 3);  /* offset -> 6 (continue apres ABC) */
    write(fd, "GHI\n", 4);    /* offset -> 10 */

    close(fd);
    close(fd_copy);

    /* Lire et afficher le contenu */
    fd = open("/tmp/dup_test.txt", O_RDONLY);
    if (fd != -1) {
        char buf[64];
        ssize_t n = read(fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("Contenu du fichier : %s", buf);
        }
        close(fd);
    }

    /* Nettoyage */
    unlink("/tmp/dup_test.txt");

    return 0;
}
