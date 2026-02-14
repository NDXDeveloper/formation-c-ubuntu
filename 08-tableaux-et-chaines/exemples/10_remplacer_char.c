/* ============================================================================
   Section 8.4 : Manipulation avancee des chaines
   Description : Remplacer un caractere dans une chaine
   Fichier source : 04-manipulation-chaines.md
   ============================================================================ */

#include <stdio.h>

void remplacer_char(char *str, char ancien, char nouveau) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ancien) {
            str[i] = nouveau;
        }
    }
}

int main(void) {
    char phrase[] = "Bonjour le monde";
    printf("Avant : %s\n", phrase);

    remplacer_char(phrase, 'o', '0');
    printf("Apres : %s\n", phrase);

    return 0;
}
