/* ============================================================================
   Section 23.7 : X-Macros
   Description : Table de dispatch pour commandes avec X-Macros
   Fichier source : 07-x-macros.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>

// Liste des commandes
#define COMMAND_LIST \
    X(CMD_HELP,   help,   "Affiche l'aide") \
    X(CMD_QUIT,   quit,   "Quitte le programme") \
    X(CMD_SAVE,   save,   "Sauvegarde les données") \
    X(CMD_LOAD,   load,   "Charge les données") \
    X(CMD_STATUS, status, "Affiche le statut")

// Génération de l'enum
#define X(id, cmd, desc) id,
typedef enum {
    COMMAND_LIST
    CMD_COUNT,
    CMD_UNKNOWN
} CommandId;
#undef X

// Génération du tableau de commandes
#define X(id, cmd, desc) #cmd,
const char* command_strings[] = {
    COMMAND_LIST
};
#undef X

// Génération du tableau de descriptions
#define X(id, cmd, desc) desc,
const char* command_descriptions[] = {
    COMMAND_LIST
};
#undef X

// Prototypes des fonctions de traitement
void handle_help(void);
void handle_quit(void);
void handle_save(void);
void handle_load(void);
void handle_status(void);

// Génération de la table de dispatch
#define X(id, cmd, desc) handle_##cmd,
typedef void (*CommandHandler)(void);
CommandHandler command_handlers[] = {
    COMMAND_LIST
};
#undef X

// Fonction de parsing de commande
CommandId parse_command(const char* input) {
    for (int i = 0; i < CMD_COUNT; i++) {
        if (strcmp(input, command_strings[i]) == 0) {
            return (CommandId)i;
        }
    }
    return CMD_UNKNOWN;
}

// Exécution d'une commande
void execute_command(const char* input) {
    CommandId cmd = parse_command(input);

    if (cmd != CMD_UNKNOWN) {
        printf("Exécution de '%s': %s\n",
               command_strings[cmd],
               command_descriptions[cmd]);
        command_handlers[cmd]();
    } else {
        printf("Commande inconnue: %s\n", input);
    }
}

// Implémentation des handlers
void handle_help(void) {
    printf("\nCommandes disponibles:\n");
    for (int i = 0; i < CMD_COUNT; i++) {
        printf("  %-10s - %s\n", command_strings[i], command_descriptions[i]);
    }
}

void handle_quit(void)   { printf("  → Fermeture du programme\n"); }
void handle_save(void)   { printf("  → Sauvegarde effectuée\n"); }
void handle_load(void)   { printf("  → Chargement effectué\n"); }
void handle_status(void) { printf("  → Statut: OK\n"); }

int main(void) {
    printf("Système de commandes\n\n");

    const char* test_commands[] = {
        "help",
        "save",
        "status",
        "invalid"
    };

    for (size_t i = 0; i < sizeof(test_commands) / sizeof(test_commands[0]); i++) {
        execute_command(test_commands[i]);
        printf("\n");
    }

    return 0;
}
