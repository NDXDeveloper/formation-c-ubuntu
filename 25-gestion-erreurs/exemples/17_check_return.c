/* ============================================================================
   Section 25.3 : Patterns de gestion d'erreurs
   Description : Pattern Check-and-Return (verification systematique)
   Fichier source : 03-patterns-gestion-erreurs.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Fonction utilitaire pour lire une ligne */
int lire_ligne(FILE *f, char *buffer, size_t taille) {
    if (fgets(buffer, (int)taille, f) == NULL) {
        if (feof(f)) {
            return 1;  /* Fin de fichier */
        }
        return -1;  /* Erreur de lecture */
    }
    return 0;  /* Succes */
}

/* Fonction qui traite un fichier de configuration */
int charger_config(const char *fichier) {
    FILE *f = fopen(fichier, "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur : impossible d'ouvrir %s\n", fichier);
        return -1;
    }

    char ligne[256];

    /* Lecture ligne 1 */
    if (lire_ligne(f, ligne, sizeof(ligne)) != 0) {
        fprintf(stderr, "Erreur : lecture ligne 1\n");
        fclose(f);
        return -1;
    }
    printf("Config 1: %s", ligne);

    /* Lecture ligne 2 */
    if (lire_ligne(f, ligne, sizeof(ligne)) != 0) {
        fprintf(stderr, "Erreur : lecture ligne 2\n");
        fclose(f);
        return -1;
    }
    printf("Config 2: %s", ligne);

    fclose(f);
    return 0;
}

int main(void) {
    /* Creer un fichier de test */
    FILE *f = fopen("/tmp/config_test.txt", "w");
    if (f == NULL) {
        fprintf(stderr, "Erreur : impossible de creer le fichier test\n");
        return EXIT_FAILURE;
    }
    fprintf(f, "serveur=localhost\n");
    fprintf(f, "port=8080\n");
    fclose(f);

    /* Tester avec le fichier cree */
    printf("=== Chargement de la configuration ===\n");
    if (charger_config("/tmp/config_test.txt") != 0) {
        return EXIT_FAILURE;
    }

    printf("Configuration chargee avec succes\n\n");

    /* Tester avec un fichier inexistant */
    printf("=== Test avec fichier inexistant ===\n");
    if (charger_config("inexistant.txt") != 0) {
        printf("Echec attendu\n");
    }

    /* Nettoyage */
    remove("/tmp/config_test.txt");

    return EXIT_SUCCESS;
}
