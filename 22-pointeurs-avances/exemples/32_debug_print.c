/* ============================================================================
   Section 22.6 : va_list et macros variadiques
   Description : Macro de debug conditionnelle avec __FILE__ et __LINE__
   Fichier source : 06-va-list.md
   ============================================================================ */

#include <stdio.h>

// Toujours active pour la demonstration (comme si compile avec -DDEBUG)
#define DEBUG_PRINT(fmt, ...) \
    fprintf(stderr, "[DEBUG] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

int main(void) {
    int x = 42;

    DEBUG_PRINT("Application demarree");
    DEBUG_PRINT("Valeur de x : %d", x);
    DEBUG_PRINT("Calcul : %d + %d = %d", 10, 20, 10 + 20);

    return 0;
}
