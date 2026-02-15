/* ============================================================================
   Section 22.6 : va_list et macros variadiques
   Description : Systeme de logging avance combinant macros et fonctions
                 variadiques
   Fichier source : 06-va-list.md
   ============================================================================ */

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

// Niveaux de log
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO  = 1,
    LOG_LEVEL_WARN  = 2,
    LOG_LEVEL_ERROR = 3
} LogLevel;

// Niveau global
static LogLevel g_log_level = LOG_LEVEL_INFO;

// Fonction variadique d'implementation
void log_impl(LogLevel level, const char *file, int line,
              const char *func, const char *fmt, ...) {
    if (level < g_log_level) {
        return;  // Filtrage par niveau
    }

    const char *level_str[] = {"DEBUG", "INFO", "WARN", "ERROR"};

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    printf("[%02d:%02d:%02d] [%5s] %s:%d (%s): ",
           t->tm_hour, t->tm_min, t->tm_sec,
           level_str[level], file, line, func);

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
}

// Macros variadiques pour chaque niveau
#define LOG_DEBUG(fmt, ...) \
    log_impl(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define LOG_INFO(fmt, ...) \
    log_impl(LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define LOG_WARN(fmt, ...) \
    log_impl(LOG_LEVEL_WARN, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define LOG_ERROR(fmt, ...) \
    log_impl(LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

void traiter_fichier(const char *nom) {
    LOG_DEBUG("Debut du traitement de %s", nom);

    if (nom == NULL) {
        LOG_ERROR("Nom de fichier NULL !");
        return;
    }

    LOG_INFO("Traitement de %s en cours", nom);
    LOG_WARN("Fichier volumineux : %s", nom);
}

int main(void) {
    (void)g_log_level;  // Utilise dans log_impl

    LOG_INFO("Application demarree");

    traiter_fichier("document.txt");
    traiter_fichier(NULL);

    LOG_INFO("Application terminee");

    return 0;
}
