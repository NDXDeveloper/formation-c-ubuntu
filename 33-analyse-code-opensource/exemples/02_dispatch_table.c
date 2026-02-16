/* ============================================================================
   Section 33.2 : Etude de cas Git
   Description : Pattern dispatch table pour router les commandes vers handlers
   Fichier source : 02-etude-cas-git.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================ */
/* Pattern : Table de dispatch (Git)            */
/* ============================================ */

/*
 * Git utilise une table de dispatch pour mapper les commandes
 * (git add, git commit, git log, etc.) vers leurs fonctions.
 * Ce pattern evite de longues chaines if/else ou switch.
 */

/* Type de fonction handler : prend argc/argv, retourne un code */
typedef int (*cmd_handler_t)(int argc, const char **argv);

/* Structure d'une entree dans la table */
typedef struct {
    const char *name;
    cmd_handler_t handler;
    const char *help;
} command_t;

/* ============================================ */
/* Implementation des commandes                 */
/* ============================================ */

static int cmd_help(int argc, const char **argv);

static int cmd_status(int argc, const char **argv)
{
    (void)argc;
    (void)argv;
    printf("  Sur la branche main\n");
    printf("  Rien a valider, repertoire de travail propre\n");
    return 0;
}

static int cmd_add(int argc, const char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "  Erreur: fichier(s) requis\n");
        fprintf(stderr, "  Usage: add <fichier> [fichier...]\n");
        return 1;
    }
    for (int i = 1; i < argc; i++) {
        printf("  Ajout de '%s' a l'index\n", argv[i]);
    }
    return 0;
}

static int cmd_commit(int argc, const char **argv)
{
    const char *message = NULL;

    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "-m") == 0) {
            message = argv[i + 1];
            break;
        }
    }

    if (!message) {
        fprintf(stderr, "  Erreur: message requis (-m \"message\")\n");
        return 1;
    }

    printf("  [main abc1234] %s\n", message);
    printf("  1 fichier modifie, 10 insertions(+)\n");
    return 0;
}

static int cmd_log(int argc, const char **argv)
{
    (void)argc;
    (void)argv;
    printf("  commit abc1234 (HEAD -> main)\n");
    printf("  Author: Dev <dev@example.com>\n");
    printf("  Date:   Lun Fev 15 10:00:00 2026\n");
    printf("\n");
    printf("      Initial commit\n");
    return 0;
}

static int cmd_version(int argc, const char **argv)
{
    (void)argc;
    (void)argv;
    printf("  mini-git version 0.1.0\n");
    return 0;
}

/* ============================================ */
/* Table de dispatch                            */
/* ============================================ */

/*
 * Table triee par nom de commande.
 * Dans Git reel, cette table est dans git.c (struct cmd_struct).
 * L'avantage : ajouter une commande = ajouter 1 ligne.
 */
static const command_t commands[] = {
    { "add",     cmd_add,     "Ajouter des fichiers a l'index" },
    { "commit",  cmd_commit,  "Enregistrer les modifications" },
    { "help",    cmd_help,    "Afficher l'aide" },
    { "log",     cmd_log,     "Afficher l'historique des commits" },
    { "status",  cmd_status,  "Afficher l'etat du repertoire" },
    { "version", cmd_version, "Afficher la version" },
};

#define NUM_COMMANDS (sizeof(commands) / sizeof(commands[0]))

/* ============================================ */
/* Recherche dans la table                      */
/* ============================================ */

static const command_t *find_command(const char *name)
{
    for (size_t i = 0; i < NUM_COMMANDS; i++) {
        if (strcmp(commands[i].name, name) == 0) {
            return &commands[i];
        }
    }
    return NULL;
}

static int cmd_help(int argc, const char **argv)
{
    (void)argc;
    (void)argv;
    printf("  Commandes disponibles:\n");
    for (size_t i = 0; i < NUM_COMMANDS; i++) {
        printf("    %-10s %s\n", commands[i].name, commands[i].help);
    }
    return 0;
}

/* ============================================ */
/* Dispatcher principal                         */
/* ============================================ */

static int dispatch(const char *cmd_name, int argc, const char **argv)
{
    const command_t *cmd = find_command(cmd_name);
    if (!cmd) {
        fprintf(stderr, "  '%s': commande inconnue\n", cmd_name);
        fprintf(stderr, "  Tapez 'help' pour voir les commandes\n");
        return 1;
    }
    return cmd->handler(argc, argv);
}

int main(void)
{
    printf("=== Pattern Dispatch Table (Git) ===\n\n");

    /* Simuler differentes commandes */
    printf("--- mini-git version ---\n");
    const char *v_args[] = { "version" };
    dispatch("version", 1, v_args);

    printf("\n--- mini-git status ---\n");
    const char *s_args[] = { "status" };
    dispatch("status", 1, s_args);

    printf("\n--- mini-git add main.c utils.c ---\n");
    const char *a_args[] = { "add", "main.c", "utils.c" };
    dispatch("add", 3, a_args);

    printf("\n--- mini-git commit -m \"Fix bug\" ---\n");
    const char *c_args[] = { "commit", "-m", "Fix bug" };
    dispatch("commit", 3, c_args);

    printf("\n--- mini-git log ---\n");
    const char *l_args[] = { "log" };
    dispatch("log", 1, l_args);

    printf("\n--- mini-git help ---\n");
    const char *h_args[] = { "help" };
    dispatch("help", 1, h_args);

    printf("\n--- mini-git frobnicate (commande inconnue) ---\n");
    const char *u_args[] = { "frobnicate" };
    dispatch("frobnicate", 1, u_args);

    printf("\n--- Avantages de la dispatch table ---\n");
    printf("1. Ajout d'une commande = 1 ligne dans la table\n");
    printf("2. Separation claire entre dispatch et implementation\n");
    printf("3. Facile a trier, filtrer, lister (help auto)\n");
    printf("4. Pattern utilise dans Git, busybox, systemd...\n");

    return EXIT_SUCCESS;
}
