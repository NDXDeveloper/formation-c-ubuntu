/* ============================================================================
   Section 26.2 : Validation des entrees
   Description : Validation avec codes de retour enumeres
   Fichier source : 02-validation-entrees.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    VALIDATION_OK = 0,
    VALIDATION_NULL_POINTER,
    VALIDATION_LONGUEUR_INVALIDE,
    VALIDATION_FORMAT_INVALIDE,
    VALIDATION_PLAGE_INVALIDE
} CodeValidation;

CodeValidation valider_nom_utilisateur(const char *nom) {
    if (nom == NULL) {
        return VALIDATION_NULL_POINTER;
    }

    size_t longueur = strlen(nom);
    if (longueur < 3 || longueur > 20) {
        return VALIDATION_LONGUEUR_INVALIDE;
    }

    for (size_t i = 0; i < longueur; i++) {
        if (!isalnum((unsigned char)nom[i]) && nom[i] != '_') {
            return VALIDATION_FORMAT_INVALIDE;
        }
    }

    return VALIDATION_OK;
}

int main(void) {
    printf("=== Validation de noms d'utilisateur ===\n\n");

    const char *messages[] = {
        "Validation reussie",
        "Erreur : pointeur NULL",
        "Erreur : longueur invalide (3-20 caracteres)",
        "Erreur : caracteres invalides (a-z, A-Z, 0-9, _ uniquement)",
        "Erreur : valeur hors limites"
    };

    const char *noms[] = {
        "User_123",
        "ab",
        "NomBeaucoupTropLongPourEtreValide",
        "user@name",
        "alice",
        "Bob_42",
        NULL,
        "hello world",
        "___",
        "a1b"
    };
    int nb = 10;

    for (int i = 0; i < nb; i++) {
        const char *nom = noms[i];
        CodeValidation resultat = valider_nom_utilisateur(nom);

        if (nom != NULL) {
            printf("  \"%s\" -> %s\n", nom, messages[resultat]);
        } else {
            printf("  (NULL) -> %s\n", messages[resultat]);
        }
    }

    return 0;
}
