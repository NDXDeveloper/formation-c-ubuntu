/* ============================================================================
   Section 25.1 : Codes de retour vs exceptions simulees
   Description : Codes d'erreur avec enumerations
   Fichier source : 01-codes-retour-vs-exceptions.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    SUCCES = 0,
    ERREUR_MEMOIRE = 1,
    ERREUR_FICHIER = 2,
    ERREUR_RESEAU = 3,
    ERREUR_PERMISSION = 4
} CodeErreur;

CodeErreur ouvrir_connexion(const char *serveur) {
    if (serveur == NULL) {
        return ERREUR_RESEAU;
    }

    /* Tentative d'allocation */
    void *buffer = malloc(1024);
    if (buffer == NULL) {
        return ERREUR_MEMOIRE;
    }

    /* ... reste du code ... */
    free(buffer);
    return SUCCES;
}

int main(void) {
    CodeErreur resultat = ouvrir_connexion("example.com");

    switch (resultat) {
        case SUCCES:
            printf("Connexion etablie\n");
            break;
        case ERREUR_MEMOIRE:
            fprintf(stderr, "Erreur : memoire insuffisante\n");
            break;
        case ERREUR_RESEAU:
            fprintf(stderr, "Erreur : probleme reseau\n");
            break;
        default:
            fprintf(stderr, "Erreur inconnue\n");
    }

    return (resultat == SUCCES) ? EXIT_SUCCESS : EXIT_FAILURE;
}
