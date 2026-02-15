/* ============================================================================
   Section 22.5 : Fonctions variadiques
   Description : Fonction de logging utilisant vprintf
   Fichier source : 05-fonctions-variadiques.md
   ============================================================================ */

#include <stdio.h>
#include <stdarg.h>

void log_message(const char *niveau, const char *format, ...) {
    printf("[%s] ", niveau);

    va_list args;
    va_start(args, format);

    vprintf(format, args);  // vprintf accepte une va_list !

    va_end(args);
    printf("\n");
}

int main(void) {
    log_message("INFO", "Application demarree");
    log_message("WARNING", "Memoire faible : %d%%", 85);
    log_message("ERROR", "Fichier %s introuvable a la ligne %d", "config.txt", 42);

    return 0;
}
