/* ============================================================================
   Section 8.3 : Les chaines de caracteres en C
   Description : Fonction qui compte les voyelles d'une chaine
   Fichier source : 03-chaines-en-c.md
   ============================================================================ */

#include <stdio.h>
#include <ctype.h>

int compter_voyelles(const char *str) {
    int compte = 0;

    for (int i = 0; str[i] != '\0'; i++) {
        char c = tolower(str[i]);
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'y') {
            compte++;
        }
    }

    return compte;
}

int main(void) {
    char phrase[] = "Bonjour le monde";
    printf("Voyelles : %d\n", compter_voyelles(phrase));

    return 0;
}
