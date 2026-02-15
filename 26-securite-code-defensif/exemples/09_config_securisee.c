/* ============================================================================
   Section 26.3 : Prevention des buffer overflows
   Description : Chargement securise de fichier de configuration
   Fichier source : 03-prevention-buffer-overflows.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

typedef struct {
    char nom[30];
    char valeur[50];
} ConfigParam;

bool charger_config(const char *fichier, ConfigParam params[],
                    size_t max_params, size_t *nb_params) {
    if (fichier == NULL || params == NULL || nb_params == NULL) {
        return false;
    }

    FILE *f = fopen(fichier, "r");
    if (f == NULL) {
        perror("Erreur d'ouverture du fichier");
        return false;
    }

    char ligne[256];
    *nb_params = 0;
    size_t numero_ligne = 0;

    while (fgets(ligne, sizeof(ligne), f) != NULL && *nb_params < max_params) {
        numero_ligne++;

        /* Ignorer les lignes vides et commentaires */
        if (ligne[0] == '\n' || ligne[0] == '#') {
            continue;
        }

        char nom[30], valeur[50];
        int items = sscanf(ligne, "%29s = %49s", nom, valeur);

        if (items != 2) {
            fprintf(stderr, "Attention : ligne %zu mal formee\n", numero_ligne);
            continue;
        }

        /* Copie securisee */
        strncpy(params[*nb_params].nom, nom, sizeof(params[*nb_params].nom) - 1);
        params[*nb_params].nom[sizeof(params[*nb_params].nom) - 1] = '\0';

        strncpy(params[*nb_params].valeur, valeur, sizeof(params[*nb_params].valeur) - 1);
        params[*nb_params].valeur[sizeof(params[*nb_params].valeur) - 1] = '\0';

        (*nb_params)++;
    }

    fclose(f);
    return true;
}

int main(void) {
    printf("=== Chargement securise de configuration ===\n\n");

    /* Creer un fichier de configuration temporaire */
    const char *fichier_config = "/tmp/test_config_ch26.txt";
    FILE *f = fopen(fichier_config, "w");
    if (f == NULL) {
        perror("Impossible de creer le fichier de test");
        return 1;
    }
    fprintf(f, "# Configuration de test\n");
    fprintf(f, "\n");
    fprintf(f, "serveur = localhost\n");
    fprintf(f, "port = 8080\n");
    fprintf(f, "timeout = 30\n");
    fprintf(f, "mode = production\n");
    fprintf(f, "ligne_invalide\n");
    fclose(f);

    /* Charger la configuration */
    ConfigParam params[100];
    size_t nb_params;

    if (charger_config(fichier_config, params, 100, &nb_params)) {
        printf("Configuration chargee : %zu parametres\n\n", nb_params);

        for (size_t i = 0; i < nb_params; i++) {
            printf("  %s = %s\n", params[i].nom, params[i].valeur);
        }
    } else {
        fprintf(stderr, "Echec du chargement\n");
        return 1;
    }

    /* Test avec fichier inexistant */
    printf("\nTest fichier inexistant :\n");
    if (!charger_config("/tmp/inexistant_ch26.txt", params, 100, &nb_params)) {
        printf("  Echec gere correctement\n");
    }

    /* Nettoyage */
    remove(fichier_config);

    return 0;
}
