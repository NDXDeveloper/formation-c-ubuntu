/* ============================================================================
   Section 17.4 : Variables d'environnement
   Description : Definir et modifier des variables avec setenv()
   Fichier source : 04-variables-environnement.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    /* Afficher la valeur initiale */
    char *editor = getenv("EDITOR");
    printf("EDITOR initial: %s\n", editor ? editor : "(non defini)");

    /* Definir une nouvelle valeur */
    if (setenv("EDITOR", "vim", 1) == 0) {
        printf("EDITOR modifie avec succes\n");
    } else {
        perror("setenv");
        return 1;
    }

    /* Verifier la modification */
    editor = getenv("EDITOR");
    printf("EDITOR apres modification: %s\n", editor);

    return 0;
}
