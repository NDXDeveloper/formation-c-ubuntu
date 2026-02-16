/* ============================================================================
   Section 34.1.3 : Output formate
   Description : Systeme de log avec niveaux et horodatage
   Fichier source : 01.3-output-formate.md
   ============================================================================ */
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define RESET   "\033[0m"

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_SUCCESS,
    LOG_WARNING,
    LOG_ERROR
} LogLevel;

void log_message(LogLevel level, const char *format, ...) {
    // Horodatage
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);

    // Préfixe selon le niveau
    const char *prefix;
    const char *color;

    switch (level) {
        case LOG_DEBUG:
            prefix = "DEBUG";
            color = BLUE;
            break;
        case LOG_INFO:
            prefix = "INFO ";
            color = "";
            break;
        case LOG_SUCCESS:
            prefix = "OK   ";
            color = GREEN;
            break;
        case LOG_WARNING:
            prefix = "WARN ";
            color = YELLOW;
            break;
        case LOG_ERROR:
            prefix = "ERROR";
            color = RED;
            break;
        default:
            prefix = "LOG  ";
            color = "";
    }

    // Afficher le préfixe
    printf("[%s] %s%s%s ", timestamp, color, prefix, RESET);

    // Afficher le message avec arguments variables
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\n");
}

int main(void) {
    log_message(LOG_DEBUG, "Mode debug activé");
    log_message(LOG_INFO, "Démarrage du programme");
    log_message(LOG_SUCCESS, "Connexion établie avec succès");
    log_message(LOG_WARNING, "Fichier de configuration manquant, utilisation des valeurs par défaut");
    log_message(LOG_ERROR, "Impossible d'ouvrir le fichier %s", "data.txt");

    return 0;
}
