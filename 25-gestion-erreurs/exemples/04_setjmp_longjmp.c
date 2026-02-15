/* ============================================================================
   Section 25.1 : Codes de retour vs exceptions simulees
   Description : Exemple complet de setjmp/longjmp (exceptions simulees)
   Fichier source : 01-codes-retour-vs-exceptions.md
   ============================================================================ */

#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>

jmp_buf point_capture;

typedef enum {
    AUCUNE_ERREUR = 0,
    ERREUR_MEMOIRE = 1,
    ERREUR_FICHIER = 2
} TypeErreur;

void allouer_donnees(void) {
    void *ptr = malloc(1000000000000UL);  /* Allocation volontairement enorme */

    if (ptr == NULL) {
        printf("Erreur detectee, declenchement de l'exception...\n");
        longjmp(point_capture, ERREUR_MEMOIRE);  /* "Lance" l'exception */
    }

    free(ptr);
}

void fonction_intermediaire(void) {
    printf("Entree dans fonction_intermediaire\n");
    allouer_donnees();
    printf("Cette ligne ne sera jamais executee\n");
}

int main(void) {
    printf("Debut du programme\n");

    /* Equivalent du "try" : on enregistre le point de retour */
    int code_erreur = setjmp(point_capture);

    if (code_erreur == AUCUNE_ERREUR) {
        /* Bloc "try" : code normal */
        printf("Execution du code protege\n");
        fonction_intermediaire();
        printf("Fin normale du programme\n");
    } else {
        /* Bloc "catch" : gestion de l'erreur */
        printf("Exception capturee !\n");

        switch (code_erreur) {
            case ERREUR_MEMOIRE:
                fprintf(stderr, "Erreur : allocation memoire echouee\n");
                break;
            case ERREUR_FICHIER:
                fprintf(stderr, "Erreur : probleme de fichier\n");
                break;
            default:
                fprintf(stderr, "Erreur inconnue\n");
        }

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
