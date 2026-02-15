/* ============================================================================
   Section 22.6 : va_list et macros variadiques
   Description : Systeme de logging par niveaux avec macros variadiques
   Fichier source : 06-va-list.md
   ============================================================================ */

#include <stdio.h>
#include <time.h>

#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 3

#define LOG(level, fmt, ...) do { \
    time_t now = time(NULL); \
    struct tm *t = localtime(&now); \
    const char *level_str[] = {"", "INFO", "WARN", "ERROR"}; \
    printf("[%02d:%02d:%02d] [%5s] " fmt "\n", \
           t->tm_hour, t->tm_min, t->tm_sec, \
           level_str[level], ##__VA_ARGS__); \
} while(0)

#define LOG_INFO(...)  LOG(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_WARN(...)  LOG(LOG_LEVEL_WARN, __VA_ARGS__)
#define LOG_ERROR(...) LOG(LOG_LEVEL_ERROR, __VA_ARGS__)

int main(void) {
    LOG_INFO("Application demarree");
    LOG_WARN("Memoire faible : %d%% utilise", 85);
    LOG_ERROR("Echec de connexion a %s:%d", "192.168.1.1", 8080);

    return 0;
}
