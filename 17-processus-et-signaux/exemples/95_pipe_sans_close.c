/* ============================================================================
   Section 17.8 : Pipes anonymes
   Description : Probleme sans fermeture de fd[1] dans l'enfant (bloque)
   Fichier source : 08-pipes-anonymes.md
   ============================================================================ */

/* ATTENTION : Ce programme bloque volontairement pour illustrer le probleme.
   L'enfant ne ferme pas fd[1], donc read() ne recoit jamais EOF.
   Utilisez Ctrl+C ou kill pour le terminer. */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
    int pipefd[2];
    pipe(pipefd);

    if (fork() == 0) {
        /* ENFANT */
        /* ERREUR: Ne ferme pas fd[1] */
        /* close(pipefd[1]);  <- manquant! */

        char buffer[100];

        printf("[Enfant] Lecture...\n");

        /* Cette lecture ne terminera JAMAIS (pas de EOF) */
        /* car fd[1] est toujours ouvert dans l'enfant */
        while (read(pipefd[0], buffer, sizeof(buffer)) > 0) {
            printf("[Enfant] Donnees recues\n");
        }

        printf("[Enfant] EOF recu\n");  /* N'arrive jamais! */

        return 0;
    }
    else {
        /* PARENT */
        close(pipefd[0]);

        write(pipefd[1], "test", 5);
        close(pipefd[1]);

        wait(NULL);
    }

    return 0;
}
