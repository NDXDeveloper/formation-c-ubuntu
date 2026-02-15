/* ============================================================================
   Section 22.6 : va_list et macros variadiques
   Description : Formatage dans une chaine avec vsnprintf
   Fichier source : 06-va-list.md
   ============================================================================ */

#include <stdio.h>
#include <stdarg.h>

char* formater_message(char *buffer, size_t size, const char *format, ...) {
    va_list args;
    va_start(args, format);

    vsnprintf(buffer, size, format, args);

    va_end(args);
    return buffer;
}

int main(void) {
    char msg[100];

    formater_message(msg, sizeof(msg), "Erreur %d : %s", 404, "Not Found");
    printf("%s\n", msg);

    formater_message(msg, sizeof(msg), "Utilisateur: %s, ID: %d", "Alice", 1001);
    printf("%s\n", msg);

    return 0;
}
