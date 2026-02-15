/* ============================================================================
   Section 25.3 : Patterns de gestion d'erreurs
   Description : Wrapper Functions pour allocation memoire securisee
   Fichier source : 03-patterns-gestion-erreurs.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Wrapper qui termine le programme si malloc echoue */
void* xmalloc(size_t taille) {
    void *ptr = malloc(taille);

    if (ptr == NULL) {
        fprintf(stderr, "Erreur fatale : allocation de %zu octets echouee\n",
                taille);
        exit(EXIT_FAILURE);
    }

    return ptr;
}

/* Wrapper qui initialise a zero */
void* xcalloc(size_t nb, size_t taille) {
    void *ptr = calloc(nb, taille);

    if (ptr == NULL) {
        fprintf(stderr, "Erreur fatale : calloc(%zu, %zu) echoue\n",
                nb, taille);
        exit(EXIT_FAILURE);
    }

    return ptr;
}

/* Wrapper pour realloc */
void* xrealloc(void *ptr, size_t nouvelle_taille) {
    void *nouveau = realloc(ptr, nouvelle_taille);

    if (nouveau == NULL && nouvelle_taille > 0) {
        fprintf(stderr, "Erreur fatale : realloc(%zu) echoue\n",
                nouvelle_taille);
        exit(EXIT_FAILURE);
    }

    return nouveau;
}

int main(void) {
    /* Plus besoin de verifier : xmalloc termine le programme en cas d'echec */
    char *buffer = xmalloc(1024);
    strcpy(buffer, "Hello, World!");

    printf("%s\n", buffer);

    /* Redimensionnement sur */
    buffer = xrealloc(buffer, 2048);
    printf("Buffer redimensionne avec succes\n");

    /* Test xcalloc */
    int *tableau = xcalloc(10, sizeof(int));
    printf("Tableau alloue et initialise a zero : %d %d %d\n",
           tableau[0], tableau[1], tableau[2]);

    free(buffer);
    free(tableau);
    return EXIT_SUCCESS;
}
