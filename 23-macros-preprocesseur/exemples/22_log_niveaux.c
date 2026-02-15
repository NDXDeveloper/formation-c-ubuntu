/* ============================================================================
   Section 23.2 : Stringification et concatenation
   Description : Logging avec niveaux utilisant la stringification
   Fichier source : 02-stringification-concatenation.md
   ============================================================================ */

#include <stdio.h>

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} LogLevel;

#define LOG(level, msg) \
    printf("[%s] %s:%d - %s\n", #level, __FILE__, __LINE__, msg)

int main(void) {
    (void)LOG_DEBUG;
    (void)LOG_INFO;
    (void)LOG_WARNING;
    (void)LOG_ERROR;

    LOG(INFO, "Démarrage du programme");
    LOG(WARNING, "Mémoire faible");
    LOG(ERROR, "Fichier introuvable");

    return 0;
}
