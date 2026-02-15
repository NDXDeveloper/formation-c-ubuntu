/* ============================================================================
   Section 25.6 : Logging
   Description : Systeme de logging minimaliste avec niveaux et filtrage
   Fichier source : 06-logging.md
   ============================================================================ */

#include <stdio.h>
#include <time.h>
#include <stdarg.h>

/* Niveaux de log */
typedef enum {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
} LogLevel;

/* Niveau minimum (configurable) */
static LogLevel niveau_minimum = LOG_INFO;

/* Noms des niveaux pour l'affichage */
static const char *niveau_noms[] = {
    "TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "FATAL"
};

/* Fonction principale de logging */
void log_message(LogLevel niveau, const char *format, ...) {
    /* Filtrage : ignorer si le niveau est inferieur au minimum */
    if (niveau < niveau_minimum) {
        return;
    }

    /* Horodatage */
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    /* Affichage du timestamp et du niveau */
    printf("[%04d-%02d-%02d %02d:%02d:%02d] [%s] ",
           t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
           t->tm_hour, t->tm_min, t->tm_sec,
           niveau_noms[niveau]);

    /* Affichage du message (avec support des arguments variables) */
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\n");
}

/* Macros pour simplifier l'utilisation */
#define LOG_TRACE(...)   log_message(LOG_TRACE, __VA_ARGS__)
#define LOG_DEBUG(...)   log_message(LOG_DEBUG, __VA_ARGS__)
#define LOG_INFO(...)    log_message(LOG_INFO, __VA_ARGS__)
#define LOG_WARNING(...) log_message(LOG_WARNING, __VA_ARGS__)
#define LOG_ERROR(...)   log_message(LOG_ERROR, __VA_ARGS__)
#define LOG_FATAL(...)   log_message(LOG_FATAL, __VA_ARGS__)

int main(void) {
    /* Configuration du niveau minimum */
    niveau_minimum = LOG_DEBUG;

    LOG_TRACE("Ce message n'apparaitra pas");
    LOG_DEBUG("Demarrage du programme");
    LOG_INFO("Serveur initialise");
    LOG_WARNING("Fichier de config manquant");
    LOG_ERROR("Connexion echouee");
    LOG_FATAL("Erreur critique");

    return 0;
}
