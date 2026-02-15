/* ============================================================================
   Section 17.1 : Creation de processus
   Description : Processus de travail - parent et enfant avec taches differentes
   Fichier source : 01-creation-processus.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid;

    pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // PROCESSUS ENFANT : Effectue un calcul
        printf("[Enfant %d] Debut du travail...\n", getpid());

        long somme = 0;
        for (long i = 0; i < 1000000000L; i++) {
            somme += i;
        }

        printf("[Enfant %d] Calcul termine: somme = %ld\n",
               getpid(), somme);

        return 0;  // L'enfant se termine
    }
    else {
        // PROCESSUS PARENT : Continue son travail
        printf("[Parent %d] J'ai cree l'enfant %d\n", getpid(), pid);
        printf("[Parent %d] Je continue mon travail pendant que l'enfant calcule\n",
               getpid());

        // Le parent pourrait faire autre chose ici
        sleep(2);  // Simule du travail

        printf("[Parent %d] Mon travail est termine\n", getpid());

        return 0;
    }
}
