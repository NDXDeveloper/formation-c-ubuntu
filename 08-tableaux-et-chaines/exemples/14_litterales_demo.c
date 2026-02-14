/* ============================================================================
   Section 8.7 : Strings litterales en C
   Description : Utilisation correcte des chaines litterales (const, fonctions, buffer)
   Fichier source : 07-strings-litterales.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>

const char *APP_NAME = "MonApp";
const char *VERSION = "1.0.0";

const char *error_messages[] = {
    "Aucune erreur",
    "Fichier non trouve",
    "Permission refusee",
    "Memoire insuffisante"
};

const char *get_error_message(int code) {
    if (code < 0 || code >= (int)(sizeof(error_messages) / sizeof(error_messages[0]))) {
        return "Erreur inconnue";
    }
    return error_messages[code];
}

void log_message(const char *level, const char *message) {
    printf("[%s] %s\n", level, message);
}

void print_header(void) {
    char header[100];
    snprintf(header, sizeof(header),
             "%s v%s", APP_NAME, VERSION);
    printf("=== %s ===\n", header);
}

int main(void) {
    const char *greeting = "Bonjour";

    print_header();

    log_message("INFO", greeting);
    log_message("ERROR", get_error_message(2));

    char username[50];
    snprintf(username, sizeof(username), "user_%d", 123);
    printf("Utilisateur : %s\n", username);

    const char *welcome =
        "Bienvenue dans l'application.\n"
        "Veuillez vous connecter.";
    printf("%s\n", welcome);

    return 0;
}
