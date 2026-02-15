/* ============================================================================
   Section 23.7 : X-Macros
   Description : Mini-framework de menu avec X-Macros
   Fichier source : 07-x-macros.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>

// Liste des options du menu
#define MENU_OPTIONS \
    X(1, "Nouveau fichier",    "Creer un nouveau document") \
    X(2, "Ouvrir",             "Ouvrir un document existant") \
    X(3, "Enregistrer",        "Sauvegarder le document") \
    X(4, "Enregistrer sous",   "Sauvegarder avec un nouveau nom") \
    X(5, "Imprimer",           "Imprimer le document") \
    X(0, "Quitter",            "Fermer l'application")

// Génération des handlers (prototypes)
#define X(id, label, desc) void menu_handler_##id(void);
MENU_OPTIONS
#undef X

// Structure pour les options
typedef struct {
    int id;
    const char* label;
    const char* description;
    void (*handler)(void);
} MenuOption;

// Génération du tableau d'options
#define X(id, label, desc) {id, label, desc, menu_handler_##id},
MenuOption menu_options[] = {
    MENU_OPTIONS
};
#undef X

const int menu_count = (int)(sizeof(menu_options) / sizeof(menu_options[0]));

// Affichage du menu
void display_menu(void) {
    printf("\n=== MENU PRINCIPAL ===\n");

    for (int i = 0; i < menu_count; i++) {
        MenuOption* opt = &menu_options[i];
        printf(" %d. %s\n", opt->id, opt->label);
    }

    printf("======================\n");
    printf("\nChoisissez une option: ");
}

// Exécution d'une option
int execute_menu_option(int choice) {
    for (int i = 0; i < menu_count; i++) {
        if (menu_options[i].id == choice) {
            printf("\n-> %s\n", menu_options[i].description);
            menu_options[i].handler();
            return choice != 0;  // Continue si pas "Quitter"
        }
    }

    printf("\nOption invalide!\n");
    return 1;
}

// Implémentation des handlers
#define X(id, label, desc) \
    void menu_handler_##id(void) { \
        printf("  Execution de: %s\n", label); \
        if (id == 0) printf("  Au revoir!\n"); \
    }
MENU_OPTIONS
#undef X

int main(void) {
    printf("Application de demonstration - X-Macros\n");

    int running = 1;
    while (running) {
        display_menu();

        int choice;
        if (scanf("%d", &choice) == 1) {
            running = execute_menu_option(choice);
        } else {
            // Nettoyer le buffer d'entrée
            while (getchar() != '\n');
            printf("\nEntree invalide!\n");
        }
    }

    return 0;
}
