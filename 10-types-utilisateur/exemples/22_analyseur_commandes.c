/* ============================================================================
   Section 10.5 : enum - Code lisible et gestion d'etats
   Description : Analyseur de commandes complet avec enum et tableau
   Fichier source : 05-enumerations.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>

typedef enum {
    CMD_AIDE,
    CMD_QUITTER,
    CMD_LISTE,
    CMD_AJOUTER,
    CMD_SUPPRIMER,
    CMD_INCONNUE
} TypeCommande;

typedef struct {
    const char *nom;
    TypeCommande type;
    const char *description;
} Commande;

const Commande commandes[] = {
    {"aide",      CMD_AIDE,      "Affiche l'aide"},
    {"quitter",   CMD_QUITTER,   "Quitte le programme"},
    {"liste",     CMD_LISTE,     "Liste tous les elements"},
    {"ajouter",   CMD_AJOUTER,   "Ajoute un element"},
    {"supprimer", CMD_SUPPRIMER, "Supprime un element"},
    {NULL,        CMD_INCONNUE,  NULL}
};

TypeCommande parser_commande(const char *input) {
    for (int i = 0; commandes[i].nom != NULL; i++) {
        if (strcmp(input, commandes[i].nom) == 0) {
            return commandes[i].type;
        }
    }
    return CMD_INCONNUE;
}

void afficher_aide(void) {
    printf("\n=== COMMANDES DISPONIBLES ===\n\n");
    for (int i = 0; commandes[i].nom != NULL; i++) {
        printf("  %-12s - %s\n", commandes[i].nom, commandes[i].description);
    }
    printf("\n");
}

void executer_commande(TypeCommande cmd) {
    switch (cmd) {
        case CMD_AIDE:
            afficher_aide();
            break;
        case CMD_QUITTER:
            printf("Au revoir !\n");
            break;
        case CMD_LISTE:
            printf("Liste des elements : [element1, element2, element3]\n");
            break;
        case CMD_AJOUTER:
            printf("Ajout d'un nouvel element...\n");
            break;
        case CMD_SUPPRIMER:
            printf("Suppression d'un element...\n");
            break;
        case CMD_INCONNUE:
            printf("Commande inconnue. Tapez 'aide' pour voir les commandes.\n");
            break;
    }
}

int main(void) {
    const char *tests[] = {
        "aide",
        "liste",
        "ajouter",
        "blabla",
        "quitter"
    };

    for (int i = 0; i < 5; i++) {
        printf("> %s\n", tests[i]);
        TypeCommande cmd = parser_commande(tests[i]);
        executer_commande(cmd);
        printf("\n");
    }

    return 0;
}
