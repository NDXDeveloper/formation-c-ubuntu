/* ============================================================================
   Section 34.1 : Création d'un outil CLI système
   Description : Exemple minimal "Hello Tool" - squelette de base CLI
   Fichier source : 01-outil-cli-systeme.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <nom>\n", argv[0]);
        return 1;
    }

    const char *nom = argv[1];

    printf("Bonjour, %s!\n", nom);

    return 0;
}
