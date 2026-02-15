/* ============================================================================
   Section 16.6 : dup, dup2 et redirection
   Description : Redirection de la sortie d'un processus enfant (fork+exec)
   Fichier source : 06-dup-redirection.md
   ============================================================================ */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* Processus enfant */

        /* Rediriger stdout vers un fichier */
        int fd = open("/tmp/child_output.txt",
                      O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            perror("open");
            _exit(1);
        }

        dup2(fd, STDOUT_FILENO);
        close(fd);

        /* Executer ls (sa sortie ira dans child_output.txt) */
        execlp("ls", "ls", "-l", "/usr/bin", NULL);

        /* Si on arrive ici, exec a echoue */
        perror("execlp");
        _exit(1);
    } else {
        /* Processus parent */
        wait(NULL);
        printf("Parent : l'enfant a termine\n");
        printf("Resultat dans /tmp/child_output.txt\n");

        /* Afficher les premieres lignes */
        FILE *fp = fopen("/tmp/child_output.txt", "r");
        if (fp) {
            char line[256];
            int count = 0;
            while (fgets(line, sizeof(line), fp) != NULL && count < 5) {
                printf("  %s", line);
                count++;
            }
            if (count == 5) {
                printf("  ...\n");
            }
            fclose(fp);
        }

        /* Nettoyage */
        unlink("/tmp/child_output.txt");
    }

    return 0;
}
