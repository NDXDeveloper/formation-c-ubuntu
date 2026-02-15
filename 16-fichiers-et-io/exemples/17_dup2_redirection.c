/* ============================================================================
   Section 16.6 : dup, dup2 et redirection
   Description : Sauvegarde et restauration de stdout avec dup/dup2
   Fichier source : 06-dup-redirection.md
   ============================================================================ */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    /* 1. Sauvegarder stdout */
    int stdout_backup = dup(STDOUT_FILENO);
    if (stdout_backup == -1) {
        perror("dup");
        return 1;
    }

    printf("Message sur le terminal\n");
    fflush(stdout);  /* Important : vider le buffer avant redirection */

    /* 2. Rediriger stdout vers un fichier */
    int fd = open("/tmp/log_redir.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);

    printf("Message dans log.txt\n");
    printf("Autre message dans log.txt\n");
    fflush(stdout);

    /* 3. Restaurer stdout original */
    dup2(stdout_backup, STDOUT_FILENO);
    close(stdout_backup);

    printf("De retour sur le terminal\n");

    /* Afficher le contenu du fichier */
    fd = open("/tmp/log_redir.txt", O_RDONLY);
    if (fd != -1) {
        char buf[256];
        ssize_t n = read(fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("Contenu du fichier redirige :\n%s", buf);
        }
        close(fd);
    }

    /* Nettoyage */
    unlink("/tmp/log_redir.txt");

    return 0;
}
