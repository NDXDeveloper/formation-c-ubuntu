/* ============================================================================
   Section 6.4 : Valeurs de retour et codes d'erreur
   Description : Conversion securisee avec strtol et gestion d'erreur
   Fichier source : 04-valeurs-retour.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(void) {
    const char *str = "12345abc";
    char *endptr;

    errno = 0;
    long valeur = strtol(str, &endptr, 10);

    if (errno != 0) {
        perror("strtol");
        return 1;
    }

    printf("Valeur convertie : %ld\n", valeur);
    printf("Caractères non convertis : %s\n", endptr);

    return 0;
}
