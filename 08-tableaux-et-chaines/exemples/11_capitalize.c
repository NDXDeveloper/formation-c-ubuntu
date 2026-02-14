/* ============================================================================
   Section 8.4 : Manipulation avancee des chaines
   Description : Premiere lettre de chaque mot en majuscule (capitalize)
   Fichier source : 04-manipulation-chaines.md
   ============================================================================ */

#include <stdio.h>
#include <ctype.h>

void capitalize(char *str) {
    int nouveau_mot = 1;

    for (int i = 0; str[i]; i++) {
        if (isspace((unsigned char)str[i])) {
            nouveau_mot = 1;
        } else if (nouveau_mot) {
            str[i] = toupper((unsigned char)str[i]);
            nouveau_mot = 0;
        } else {
            str[i] = tolower((unsigned char)str[i]);
        }
    }
}

int main(void) {
    char texte[] = "bonjour le monde en c";
    capitalize(texte);
    printf("%s\n", texte);
    return 0;
}
