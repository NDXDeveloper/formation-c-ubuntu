/* ============================================================================
   Section 10.7 : Structures bit-fields
   Description : Configuration systeme complete avec bit-fields
   Fichier source : 07-bit-fields.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>

typedef struct {
    unsigned int verbose         : 1;
    unsigned int debug           : 1;
    unsigned int couleur         : 1;
    unsigned int niveau_log      : 3;
    unsigned int threads         : 4;
    unsigned int cache_actif     : 1;
    unsigned int compression     : 2;
    unsigned int protocole       : 3;
} Configuration;

void afficher_config(Configuration *cfg) {
    printf("=== CONFIGURATION SYSTEME ===\n");
    printf("Verbose       : %s\n", cfg->verbose ? "Oui" : "Non");
    printf("Debug         : %s\n", cfg->debug ? "Oui" : "Non");
    printf("Couleur       : %s\n", cfg->couleur ? "Oui" : "Non");
    printf("Niveau log    : %u\n", cfg->niveau_log);
    printf("Threads       : %u\n", cfg->threads);
    printf("Cache         : %s\n", cfg->cache_actif ? "Actif" : "Inactif");

    const char *compression[] = {"Aucune", "Rapide", "Maximale"};
    printf("Compression   : %s\n", compression[cfg->compression]);

    printf("Protocole     : v%u\n", cfg->protocole);
    printf("Taille config : %zu octets\n", sizeof(Configuration));
}

void config_par_defaut(Configuration *cfg) {
    memset(cfg, 0, sizeof(Configuration));
    cfg->couleur = 1;
    cfg->niveau_log = 3;
    cfg->threads = 4;
    cfg->cache_actif = 1;
    cfg->compression = 1;
    cfg->protocole = 2;
}

int main(void) {
    Configuration cfg;
    config_par_defaut(&cfg);
    afficher_config(&cfg);

    printf("\n=== Activation mode debug ===\n");
    cfg.debug = 1;
    cfg.verbose = 1;
    cfg.niveau_log = 7;
    afficher_config(&cfg);

    return 0;
}
