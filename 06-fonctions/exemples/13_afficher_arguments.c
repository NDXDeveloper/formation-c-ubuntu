/* ============================================================================
   Section 6.5 : La fonction main et ses parametres
   Description : Affichage de tous les arguments de la ligne de commande
   Fichier source : 05-fonction-main.md
   ============================================================================ */
#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("=== Liste des arguments ===\n");

    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    return 0;
}
