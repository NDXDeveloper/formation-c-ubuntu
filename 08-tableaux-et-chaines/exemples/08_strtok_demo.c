/* ============================================================================
   Section 8.4 : Manipulation avancee des chaines
   Description : Decoupage de chaine avec strtok
   Fichier source : 04-manipulation-chaines.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>

int main(void) {
    char phrase[] = "Bonjour,le,monde,en,C";
    char *token;

    token = strtok(phrase, ",");

    while (token != NULL) {
        printf("%s\n", token);
        token = strtok(NULL, ",");
    }

    return 0;
}
