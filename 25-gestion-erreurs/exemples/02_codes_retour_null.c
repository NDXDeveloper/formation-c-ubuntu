/* ============================================================================
   Section 25.1 : Codes de retour vs exceptions simulees
   Description : Retour de pointeur NULL pour signaler une erreur
   Fichier source : 01-codes-retour-vs-exceptions.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Convention : NULL = erreur, pointeur valide = succes */
char* allouer_buffer(size_t taille) {
    char *buffer = malloc(taille);

    if (buffer == NULL) {
        return NULL;  /* Erreur d'allocation */
    }

    /* Initialisation du buffer */
    memset(buffer, 0, taille);
    return buffer;  /* Succes */
}

int main(void) {
    char *buffer = allouer_buffer(1024);

    if (buffer == NULL) {
        fprintf(stderr, "Erreur : allocation impossible\n");
        return EXIT_FAILURE;
    }

    printf("Buffer alloue avec succes (%d octets)\n", 1024);

    /* Utilisation du buffer */
    free(buffer);
    return EXIT_SUCCESS;
}
