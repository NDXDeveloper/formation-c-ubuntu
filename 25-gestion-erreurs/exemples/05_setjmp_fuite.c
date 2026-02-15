/* ============================================================================
   Section 25.1 : Codes de retour vs exceptions simulees
   Description : Fuite memoire avec longjmp (bug intentionnel pedagogique)
   Fichier source : 01-codes-retour-vs-exceptions.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

jmp_buf point_retour;

void traitement(const char *buffer) {
    /* Simuler une erreur pendant le traitement */
    if (buffer[0] == '\0') {
        longjmp(point_retour, 1);  /* Saut ! Le buffer n'est JAMAIS libere ! */
    }
}

void fonction_dangereuse(void) {
    char *buffer = malloc(1024);  /* Allocation reussie */

    if (buffer == NULL) {
        longjmp(point_retour, 2);
    }

    buffer[0] = '\0';  /* Donnee qui declenchera l'erreur */
    traitement(buffer);  /* longjmp ici -> buffer fuite ! */

    free(buffer);  /* Cette ligne ne sera jamais executee */
}

int main(void) {
    if (setjmp(point_retour) == 0) {
        fonction_dangereuse();
    } else {
        fprintf(stderr, "Erreur capturee, mais fuite memoire !\n");
    }

    return 0;
}
