/* ============================================================================
   Section 26.3 : Prevention des buffer overflows
   Description : Tests de la bibliotheque safe_string
   Fichier source : 03-prevention-buffer-overflows.md
   ============================================================================ */

#include "safe_string.h"
#include <stdio.h>

int main(void) {
    printf("=== Tests de la bibliotheque safe_string ===\n\n");

    /* Test safe_strcpy */
    printf("--- safe_strcpy ---\n");
    char dest[10];

    if (safe_strcpy(dest, sizeof(dest), "Hello")) {
        printf("  Copie 'Hello' -> OK : \"%s\"\n", dest);
    }

    if (!safe_strcpy(dest, sizeof(dest), "UnTexteBeaucoupTropLong")) {
        printf("  Copie 'UnTexteBeaucoupTropLong' -> REFUSE (trop long)\n");
    }

    if (!safe_strcpy(NULL, sizeof(dest), "Test")) {
        printf("  Copie vers NULL -> REFUSE (pointeur NULL)\n");
    }

    /* Test safe_strcat */
    printf("\n--- safe_strcat ---\n");
    char buf[20] = "Hello";

    if (safe_strcat(buf, sizeof(buf), " World")) {
        printf("  Concat 'Hello' + ' World' -> OK : \"%s\"\n", buf);
    }

    if (!safe_strcat(buf, sizeof(buf), " Ceci est trop long")) {
        printf("  Concat trop longue -> REFUSE (pas assez d'espace)\n");
    }

    /* Test safe_sprintf */
    printf("\n--- safe_sprintf ---\n");
    char buffer[30];

    int result = safe_sprintf(buffer, sizeof(buffer), "Age: %d, Nom: %s", 25, "Alice");
    if (result >= 0) {
        printf("  Formatage -> OK : \"%s\" (%d chars)\n", buffer, result);
    }

    result = safe_sprintf(buffer, 10, "Ce texte est beaucoup trop long pour le buffer de 10 chars");
    if (result < 0) {
        printf("  Formatage trop long -> REFUSE (troncature)\n");
    }

    result = safe_sprintf(NULL, 10, "Test");
    if (result < 0) {
        printf("  Formatage vers NULL -> REFUSE (pointeur NULL)\n");
    }

    printf("\n=== Tous les tests passes ===\n");
    return 0;
}
