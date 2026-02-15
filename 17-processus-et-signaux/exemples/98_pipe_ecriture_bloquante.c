/* ============================================================================
   Section 17.8 : Pipes anonymes
   Description : Ecriture bloquante - buffer du pipe limite
   Fichier source : 08-pipes-anonymes.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>

int main(void) {
    int pipefd[2];
    pipe(pipefd);

    printf("Remplissage du pipe jusqu'a ce qu'il soit plein...\n");

    char data[1024];
    size_t total = 0;

    /* Ecrire jusqu'au blocage (ou erreur) */
    while (1) {
        ssize_t n = write(pipefd[1], data, sizeof(data));

        if (n == -1) {
            perror("write");
            break;
        }

        total += (size_t)n;
        printf("Ecrit: %zu Ko\n", total / 1024);

        /* Arreter apres ~100 Ko pour l'exemple */
        if (total > 100 * 1024) {
            break;
        }
    }

    printf("Total ecrit: %zu Ko\n", total / 1024);

    close(pipefd[0]);
    close(pipefd[1]);

    return 0;
}
