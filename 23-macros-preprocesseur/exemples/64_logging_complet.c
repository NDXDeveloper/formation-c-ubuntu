/* ============================================================================
   Section 23.6 : Macros predefinies utiles
   Description : Systeme de logging complet avec timestamps et niveaux
   Fichier source : 06-macros-predefinies.md
   ============================================================================ */

#include <stdio.h>
#include <time.h>

// Niveaux de log
typedef enum {
    LOG_LVL_DEBUG,
    LOG_LVL_INFO,
    LOG_LVL_WARNING,
    LOG_LVL_ERROR
} LogLevel;

// Niveau de log global
static LogLevel g_log_level = LOG_LVL_DEBUG;

// Fonction d'aide pour obtenir l'heure actuelle
static void get_timestamp(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

// Macros de logging avec toutes les informations
#define LOG(level, format, ...) \
    do { \
        if (level >= g_log_level) { \
            char timestamp[20]; \
            get_timestamp(timestamp, sizeof(timestamp)); \
            const char* level_str[] = {"DEBUG", "INFO", "WARN", "ERROR"}; \
            fprintf(stderr, "[%s] [%s] %s:%d:%s() - " format "\n", \
                    timestamp, level_str[level], \
                    __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
        } \
    } while(0)

#define LOG_DBG(format, ...)  LOG(LOG_LVL_DEBUG, format, ##__VA_ARGS__)
#define LOG_INF(format, ...)  LOG(LOG_LVL_INFO, format, ##__VA_ARGS__)
#define LOG_WRN(format, ...)  LOG(LOG_LVL_WARNING, format, ##__VA_ARGS__)
#define LOG_ERR(format, ...)  LOG(LOG_LVL_ERROR, format, ##__VA_ARGS__)

// Fonction de démonstration
void fonction_exemple(int valeur) {
    LOG_DBG("Entrée dans la fonction avec valeur=%d", valeur);

    if (valeur < 0) {
        LOG_WRN("Valeur négative détectée : %d", valeur);
    } else if (valeur == 0) {
        LOG_ERR("Valeur nulle non autorisée");
        return;
    } else {
        LOG_INF("Traitement de la valeur %d", valeur);
    }

    LOG_DBG("Sortie de la fonction");
}

int main(void) {
    LOG_INF("Démarrage du programme");

    fonction_exemple(10);
    fonction_exemple(-5);
    fonction_exemple(0);

    LOG_INF("Programme terminé");

    return 0;
}
