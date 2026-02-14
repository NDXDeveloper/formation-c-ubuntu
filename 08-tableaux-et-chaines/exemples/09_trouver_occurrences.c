/* ============================================================================
   Section 8.4 : Manipulation avancee des chaines
   Description : Trouver toutes les occurrences d'un caractere avec strchr
   Fichier source : 04-manipulation-chaines.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>

void trouver_tous(const char *str, char cherche) {
    const char *ptr = str;
    int position = 0;

    while ((ptr = strchr(ptr, cherche)) != NULL) {
        position = ptr - str;
        printf("Trouve '%c' a la position %d\n", cherche, position);
        ptr++;
    }
}

int main(void) {
    char phrase[] = "Bonjour le monde";
    trouver_tous(phrase, 'o');
    return 0;
}
