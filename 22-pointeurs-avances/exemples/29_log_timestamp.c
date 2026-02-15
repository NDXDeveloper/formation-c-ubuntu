/* ============================================================================
   Section 22.6 : va_list et macros variadiques
   Description : Wrapper de printf avec timestamp automatique
   Fichier source : 06-va-list.md
   ============================================================================ */

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

void log_avec_timestamp(const char *format, ...) {
    // Obtenir l'heure actuelle
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    // Afficher le timestamp
    printf("[%02d:%02d:%02d] ", t->tm_hour, t->tm_min, t->tm_sec);

    // Deleguer le formatage a vprintf
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\n");
}

int main(void) {
    log_avec_timestamp("Application demarree");
    log_avec_timestamp("Utilisateur %s connecte", "admin");
    log_avec_timestamp("Traitement de %d fichiers", 42);

    return 0;
}
