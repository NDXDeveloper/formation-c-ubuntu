/* ============================================================================
   Section 8.3 : Les chaines de caracteres en C
   Description : Manipulation complete (compter_mots, inverser, est_palindrome, trim)
   Fichier source : 03-chaines-en-c.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int compter_mots(const char *str) {
    int compte = 0;
    int dans_mot = 0;

    for (int i = 0; str[i] != '\0'; i++) {
        if (isspace(str[i])) {
            dans_mot = 0;
        } else if (dans_mot == 0) {
            dans_mot = 1;
            compte++;
        }
    }

    return compte;
}

void inverser(char *str) {
    int longueur = strlen(str);

    for (int i = 0; i < longueur / 2; i++) {
        char temp = str[i];
        str[i] = str[longueur - 1 - i];
        str[longueur - 1 - i] = temp;
    }
}

int est_palindrome(const char *str) {
    int gauche = 0;
    int droite = strlen(str) - 1;

    while (gauche < droite) {
        while (gauche < droite && !isalpha(str[gauche])) {
            gauche++;
        }
        while (gauche < droite && !isalpha(str[droite])) {
            droite--;
        }

        if (tolower(str[gauche]) != tolower(str[droite])) {
            return 0;
        }

        gauche++;
        droite--;
    }

    return 1;
}

void trim(char *str) {
    int debut = 0;
    while (isspace(str[debut])) {
        debut++;
    }

    int fin = strlen(str) - 1;
    while (fin >= debut && isspace(str[fin])) {
        fin--;
    }

    int i;
    for (i = 0; debut <= fin; debut++, i++) {
        str[i] = str[debut];
    }
    str[i] = '\0';
}

int main(void) {
    char phrase[] = "Bonjour le monde en C";
    printf("Nombre de mots : %d\n", compter_mots(phrase));

    char mot[] = "Bonjour";
    printf("Avant : %s\n", mot);
    inverser(mot);
    printf("Apres : %s\n", mot);

    char pal1[] = "kayak";
    char pal2[] = "A man a plan a canal Panama";
    printf("\"%s\" palindrome ? %s\n", pal1,
           est_palindrome(pal1) ? "Oui" : "Non");
    printf("\"%s\" palindrome ? %s\n", pal2,
           est_palindrome(pal2) ? "Oui" : "Non");

    char espaces[] = "   texte   ";
    printf("Avant trim : '%s'\n", espaces);
    trim(espaces);
    printf("Apres trim : '%s'\n", espaces);

    return 0;
}
