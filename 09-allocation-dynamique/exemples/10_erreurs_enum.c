/* ============================================================================
   Section 9.4 : Gestion des erreurs d'allocation
   Description : Codes d'erreur avec enum et parametre de sortie
   Fichier source : 04-gestion-erreurs.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    SUCCESS = 0,
    ERROR_NULL_PARAM,
    ERROR_ALLOCATION,
    ERROR_INVALID_SIZE
} ErrorCode;

ErrorCode creer_tableau(int** tableau, size_t taille) {
    if (tableau == NULL) {
        return ERROR_NULL_PARAM;
    }

    if (taille == 0) {
        return ERROR_INVALID_SIZE;
    }

    *tableau = malloc(taille * sizeof(int));

    if (*tableau == NULL) {
        return ERROR_ALLOCATION;
    }

    return SUCCESS;
}

const char* error_to_string(ErrorCode err) {
    switch (err) {
        case SUCCESS:            return "Succes";
        case ERROR_NULL_PARAM:   return "Parametre NULL";
        case ERROR_ALLOCATION:   return "Echec d'allocation memoire";
        case ERROR_INVALID_SIZE: return "Taille invalide";
        default:                 return "Erreur inconnue";
    }
}

int main(void) {
    int* mon_tableau = NULL;
    ErrorCode result = creer_tableau(&mon_tableau, 100);

    if (result != SUCCESS) {
        fprintf(stderr, "Erreur : %s\n", error_to_string(result));
        return 1;
    }

    mon_tableau[0] = 42;
    printf("Tableau cree avec succes : %d\n", mon_tableau[0]);

    free(mon_tableau);
    return 0;
}
