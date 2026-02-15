/* ============================================================================
   Section 16.6 : dup, dup2 et redirection
   Description : Redirection de stdout et stderr vers le meme fichier
   Fichier source : 06-dup-redirection.md
   ============================================================================ */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    /* Sauvegarder stdout et stderr */
    int stdout_backup = dup(STDOUT_FILENO);
    int stderr_backup = dup(STDERR_FILENO);

    int fd = open("/tmp/combined.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    /* Rediriger stdout */
    dup2(fd, STDOUT_FILENO);

    /* Rediriger stderr vers stdout (qui pointe vers le fichier) */
    dup2(STDOUT_FILENO, STDERR_FILENO);

    close(fd);

    /* Maintenant stdout et stderr vont dans le meme fichier */
    printf("Message normal\n");
    fprintf(stderr, "Message d'erreur\n");
    printf("Autre message\n");

    fflush(stdout);
    fflush(stderr);

    /* Restaurer stdout et stderr */
    dup2(stdout_backup, STDOUT_FILENO);
    dup2(stderr_backup, STDERR_FILENO);
    close(stdout_backup);
    close(stderr_backup);

    /* Lire et afficher le contenu du fichier */
    fd = open("/tmp/combined.log", O_RDONLY);
    if (fd != -1) {
        char buf[512];
        ssize_t n = read(fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("Contenu de combined.log :\n%s", buf);
        }
        close(fd);
    }

    /* Nettoyage */
    unlink("/tmp/combined.log");

    return 0;
}
