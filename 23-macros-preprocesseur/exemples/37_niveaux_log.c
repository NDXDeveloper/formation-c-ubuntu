/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Systeme de logging avec niveaux conditionnels
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#include <stdio.h>

#define LOG_LEVEL 2
// 0 = Aucun, 1 = Erreur, 2 = Warning, 3 = Info, 4 = Debug

#if LOG_LEVEL >= 1
    #define LOG_ERROR(fmt, ...) \
        printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
#else
    #define LOG_ERROR(fmt, ...) do {} while(0)
#endif

#if LOG_LEVEL >= 2
    #define LOG_WARNING(fmt, ...) \
        printf("[WARNING] " fmt "\n", ##__VA_ARGS__)
#else
    #define LOG_WARNING(fmt, ...) do {} while(0)
#endif

#if LOG_LEVEL >= 3
    #define LOG_INFO(fmt, ...) \
        printf("[INFO] " fmt "\n", ##__VA_ARGS__)
#else
    #define LOG_INFO(fmt, ...) do {} while(0)
#endif

#if LOG_LEVEL >= 4
    #define LOG_DEBUG(fmt, ...) \
        printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
    #define LOG_DEBUG(fmt, ...) do {} while(0)
#endif

int main(void) {
    LOG_DEBUG("Démarrage du programme");
    LOG_INFO("Configuration chargée");
    LOG_WARNING("Mémoire limitée détectée");
    LOG_ERROR("Impossible d'ouvrir le fichier");

    return 0;
}
