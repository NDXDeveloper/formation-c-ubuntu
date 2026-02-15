/* ============================================================================
   Section 17.1 : Creation de processus
   Description : Fork avec modification de variables - espaces memoire separes
   Fichier source : 01-creation-processus.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    int compteur = 100;
    pid_t pid;

    printf("Avant fork() - compteur = %d\n", compteur);

    pid = fork();

    if (pid == -1) {
        perror("Erreur fork");
        return 1;
    }
    else if (pid == 0) {
        // ENFANT : modifie sa copie
        compteur += 10;
        printf("ENFANT - compteur = %d (adresse: %p)\n",
               compteur, (void*)&compteur);
    }
    else {
        // PARENT : modifie sa copie
        compteur += 50;
        printf("PARENT - compteur = %d (adresse: %p)\n",
               compteur, (void*)&compteur);
    }

    return 0;
}
