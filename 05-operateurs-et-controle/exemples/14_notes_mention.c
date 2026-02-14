/* ============================================================================
   Section 5.4 : Conditions
   Description : Attribution de mentions avec if / else if / else
   Fichier source : 04-conditions.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    int note = 15;

    if (note >= 16) {
        printf("Mention Très Bien\n");
    } else if (note >= 14) {
        printf("Mention Bien\n");
    } else if (note >= 12) {
        printf("Mention Assez Bien\n");
    } else if (note >= 10) {
        printf("Passable\n");
    } else {
        printf("Insuffisant\n");
    }

    return 0;
}
