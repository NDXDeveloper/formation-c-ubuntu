/* ============================================================================
   Section 17.1 : Creation de processus
   Description : Ordre d'execution non deterministe entre parent et enfant
   Fichier source : 01-creation-processus.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        printf("A\n");
        printf("B\n");
    } else {
        printf("C\n");
        printf("D\n");
    }

    return 0;
}
