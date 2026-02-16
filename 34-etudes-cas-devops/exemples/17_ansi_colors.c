/* ============================================================================
   Section 34.1.3 : Output formate
   Description : Couleurs et styles ANSI dans le terminal
   Fichier source : 01.3-output-formate.md
   ============================================================================ */
#include <stdio.h>

// Codes de couleur ANSI
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

// Styles
#define BOLD      "\033[1m"
#define DIM       "\033[2m"
#define ITALIC    "\033[3m"
#define UNDERLINE "\033[4m"

// Fonds
#define BG_RED     "\033[41m"
#define BG_GREEN   "\033[42m"
#define BG_YELLOW  "\033[43m"
#define BG_BLUE    "\033[44m"

int main(void) {
    printf("=== Couleurs de texte ===\n");
    printf(RED "Texte rouge\n" RESET);
    printf(GREEN "Texte vert\n" RESET);
    printf(YELLOW "Texte jaune\n" RESET);
    printf(BLUE "Texte bleu\n" RESET);
    printf(MAGENTA "Texte magenta\n" RESET);
    printf(CYAN "Texte cyan\n" RESET);

    printf("\n=== Styles ===\n");
    printf(BOLD "Texte gras\n" RESET);
    printf(DIM "Texte atténué\n" RESET);
    printf(ITALIC "Texte italique\n" RESET);
    printf(UNDERLINE "Texte souligné\n" RESET);
    printf(BOLD RED "Texte gras et rouge\n" RESET);

    printf("\n=== Couleurs de fond ===\n");
    printf(BG_RED "Fond rouge" RESET "\n");
    printf(BG_GREEN "Fond vert" RESET "\n");
    printf(BG_BLUE WHITE "Fond bleu texte blanc" RESET "\n");

    return 0;
}
