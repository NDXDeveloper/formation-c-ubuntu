/* ============================================================================
   Section 22.6 : va_list et macros variadiques
   Description : Macros variadiques de base avec __VA_ARGS__ et ##__VA_ARGS__
   Fichier source : 06-va-list.md
   ============================================================================ */

#include <stdio.h>

// Macro variadique simple
#define AFFICHER(...) printf(__VA_ARGS__)

// Avec ## : fonctionne meme sans arguments variables
#define LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)

// Macro de formatage conditionnel
#define PRINT_IF(condition, fmt, ...) do { \
    if (condition) { \
        printf(fmt, ##__VA_ARGS__); \
    } \
} while(0)

int main(void) {
    // Test AFFICHER
    printf("--- Test AFFICHER ---\n");
    AFFICHER("Bonjour\n");
    AFFICHER("x = %d\n", 42);
    AFFICHER("x = %d, y = %d\n", 10, 20);

    // Test LOG avec ##__VA_ARGS__
    printf("\n--- Test LOG ---\n");
    LOG("Bonjour\n");
    LOG("x = %d\n", 42);

    // Test PRINT_IF
    printf("\n--- Test PRINT_IF ---\n");
    int debug_mode = 1;
    int verbose_mode = 0;

    PRINT_IF(debug_mode, "Mode debug active\n");
    PRINT_IF(verbose_mode, "Mode verbose active\n");
    PRINT_IF(debug_mode, "Valeur : %d\n", 42);

    return 0;
}
