/* ============================================================================
   Section 26.8 : Compilation avec hardening flags
   Description : Demonstration de la protection stack canary
                 Bug intentionnel : buffer overflow pour declencher la detection
   Fichier source : 08-hardening-flags.md
   ============================================================================ */

/* ATTENTION : Ce programme contient un bug INTENTIONNEL pour demontrer
   la detection par stack canary. Il crashera avec le message :
   "*** stack smashing detected ***" */

#include <string.h>
#include <stdio.h>

void fonction_vulnerable(const char *input) {
    char buffer[10];
    /* Bug intentionnel : strcpy sans verification de longueur */
    strcpy(buffer, input);
    printf("Buffer : %s\n", buffer);
}

int main(void) {
    /* Desactiver le buffering pour voir les messages avant le crash */
    setbuf(stdout, NULL);

    printf("=== Demonstration stack canary ===\n\n");

    /* Entree courte : OK */
    printf("Test 1 (entree courte) :\n");
    fonction_vulnerable("Hello");

    /* Entree trop longue : declenche le canary */
    printf("\nTest 2 (entree trop longue -> stack smashing) :\n");
    fonction_vulnerable("AAAAAAAAAAAAAAAAAAAA");

    /* Jamais atteint si le canary detecte l'overflow */
    printf("\nFin (pas atteint si canary actif)\n");
    return 0;
}
