/* ============================================================================
   Section 23.4 : Macros cross-platform
   Description : Couleurs ANSI dans le terminal (portable)
   Fichier source : 04-macros-cross-platform.md
   ============================================================================ */

#include <stdio.h>

// Sur Windows 10+, les codes ANSI sont supportes aussi
#ifdef _WIN32
    #define INIT_COLORS() ((void)0)
#else
    #define INIT_COLORS() ((void)0)
#endif

// Codes couleurs ANSI (fonctionnent sur tous les systèmes modernes)
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"

int main(void) {
    INIT_COLORS();

    printf(COLOR_RED "Texte rouge\n" COLOR_RESET);
    printf(COLOR_GREEN "Texte vert\n" COLOR_RESET);
    printf(COLOR_YELLOW "Texte jaune\n" COLOR_RESET);
    printf(COLOR_BLUE "Texte bleu\n" COLOR_RESET);

    return 0;
}
