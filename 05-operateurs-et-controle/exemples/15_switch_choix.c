/* ============================================================================
   Section 5.4 : Conditions
   Description : Instruction switch avec cases et default
   Fichier source : 04-conditions.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    char choix = 'B';

    switch (choix) {
        case 'A':
            printf("Vous avez choisi A\n");
            break;
        case 'B':
            printf("Vous avez choisi B\n");
            break;
        case 'C':
            printf("Vous avez choisi C\n");
            break;
        default:
            printf("Choix non reconnu\n");
            break;
    }

    return 0;
}
