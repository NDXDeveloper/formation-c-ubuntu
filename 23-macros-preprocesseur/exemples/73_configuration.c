/* ============================================================================
   Section 23.7 : X-Macros
   Description : Configuration avec valeurs par defaut via X-Macros
   Fichier source : 07-x-macros.md
   ============================================================================ */

#include <stdio.h>

// Liste des paramètres de configuration
#define CONFIG_LIST \
    X(MAX_CONNECTIONS,  100,   "Nombre maximum de connexions") \
    X(TIMEOUT_MS,       5000,  "Délai d'attente en millisecondes") \
    X(BUFFER_SIZE,      1024,  "Taille du tampon") \
    X(RETRY_COUNT,      3,     "Nombre de tentatives") \
    X(PORT,             8080,  "Port d'écoute")

// Génération de l'enum
#define X(name, value, desc) CONFIG_##name,
typedef enum {
    CONFIG_LIST
    CONFIG_COUNT
} ConfigParam;
#undef X

// Génération du tableau de valeurs par défaut
#define X(name, value, desc) value,
static int config_defaults[] = {
    CONFIG_LIST
};
#undef X

// Génération du tableau de descriptions
#define X(name, value, desc) desc,
static const char* config_descriptions[] = {
    CONFIG_LIST
};
#undef X

// Génération du tableau de noms
#define X(name, value, desc) #name,
static const char* config_names[] = {
    CONFIG_LIST
};
#undef X

// Structure de configuration
typedef struct {
    int values[CONFIG_COUNT];
} Config;

// Initialisation avec valeurs par défaut
void config_init(Config* cfg) {
    for (int i = 0; i < CONFIG_COUNT; i++) {
        cfg->values[i] = config_defaults[i];
    }
}

// Affichage de la configuration
void config_print(const Config* cfg) {
    printf("Configuration:\n");
    printf("==============\n");
    for (int i = 0; i < CONFIG_COUNT; i++) {
        printf("%-20s = %5d  (%s)\n",
               config_names[i],
               cfg->values[i],
               config_descriptions[i]);
    }
}

int main(void) {
    Config cfg;
    config_init(&cfg);
    config_print(&cfg);

    // Modifier une valeur
    printf("\nModification de MAX_CONNECTIONS...\n\n");
    cfg.values[CONFIG_MAX_CONNECTIONS] = 200;
    config_print(&cfg);

    return 0;
}
