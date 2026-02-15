/* ============================================================================
   Section 17.1 : Creation de processus
   Description : Fonctions utiles liees aux processus (getpid, getppid, etc.)
   Fichier source : 01-creation-processus.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>

int main(void) {
    printf("Mon PID: %d\n", getpid());
    printf("PID de mon parent: %d\n", getppid());
    printf("Mon groupe de processus: %d\n", getpgrp());
    printf("Mon UID: %d\n", getuid());

    return 0;
}
