/* ============================================================================
   Section 23.7 : X-Macros
   Description : Codes d'erreur avec messages via X-Macros
   Fichier source : 07-x-macros.md
   ============================================================================ */

#include <stdio.h>

// Liste des erreurs avec leurs messages
#define ERROR_LIST \
    X(ERR_SUCCESS,     0,  "Succès") \
    X(ERR_MEMORY,      1,  "Erreur d'allocation mémoire") \
    X(ERR_FILE,        2,  "Erreur de fichier") \
    X(ERR_NETWORK,     3,  "Erreur réseau") \
    X(ERR_PERMISSION,  4,  "Permission refusée") \
    X(ERR_TIMEOUT,     5,  "Délai d'attente dépassé")

// Génération de l'énumération
#define X(name, code, msg) name = code,
typedef enum {
    ERROR_LIST
} ErrorCode;
#undef X

// Génération du tableau de messages
#define X(name, code, msg) [code] = msg,
const char* error_messages[] = {
    ERROR_LIST
};
#undef X

// Fonction pour obtenir le message d'erreur
const char* get_error_message(ErrorCode err) {
    if (err >= 0 && err < (int)(sizeof(error_messages) / sizeof(error_messages[0]))) {
        return error_messages[err];
    }
    return "Erreur inconnue";
}

// Exemple d'utilisation
int main(void) {
    ErrorCode erreurs[] = {
        ERR_SUCCESS,
        ERR_MEMORY,
        ERR_FILE,
        ERR_NETWORK
    };

    printf("Test des codes d'erreur:\n\n");
    for (size_t i = 0; i < sizeof(erreurs) / sizeof(erreurs[0]); i++) {
        printf("Code %d: %s\n", erreurs[i], get_error_message(erreurs[i]));
    }

    return 0;
}
