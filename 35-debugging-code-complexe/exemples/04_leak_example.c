/* ============================================================================
   Section 35.2 : Resolution des fuites memoire
   Description : Bug intentionnel - trois types de fuites memoire (pour Valgrind)
   Fichier source : 02-resolution-memory-leaks.md
   ============================================================================ */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char *duplicate_string(const char *str) {
    char *dup = malloc(strlen(str) + 1);
    strcpy(dup, str);
    return dup;  /* Appelant doit free */
}

int main(void) {
    printf("Demonstration de 3 types de fuites memoire :\n");

    /* Fuite 1 : allocation directe non liberee */
    char *buffer = malloc(100);
    buffer[0] = 'A';
    printf("  Fuite 1 : allocation directe (100 octets)\n");
    /* Pas de free(buffer) */

    /* Fuite 2 : valeur de retour non liberee */
    duplicate_string("Hello World");
    printf("  Fuite 2 : retour de fonction ignore (12 octets)\n");

    /* Fuite 3 : perte de pointeur */
    char *data = malloc(50);
    data = NULL;  /* Pointeur ecrase */
    printf("  Fuite 3 : pointeur ecrase (50 octets)\n");

    printf("Termine (162 octets fuites au total)\n");

    (void)buffer;
    (void)data;
    return 0;
}
