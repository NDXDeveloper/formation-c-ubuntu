/* ============================================================================
   Section 34.1.2 : Interaction avec le systeme
   Description : Operations fichiers avec la bibliotheque standard + access()
   Fichier source : 01.2-interaction-systeme.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int lire_fichier_texte(const char *chemin) {
    FILE *fp = fopen(chemin, "r");
    if (fp == NULL) {
        perror("Erreur ouverture fichier");
        return -1;
    }

    char ligne[256];
    int numero_ligne = 1;

    // Lire ligne par ligne
    while (fgets(ligne, sizeof(ligne), fp) != NULL) {
        printf("%3d: %s", numero_ligne++, ligne);
    }

    fclose(fp);
    return 0;
}

int ecrire_fichier(const char *chemin, const char *contenu) {
    FILE *fp = fopen(chemin, "w");
    if (fp == NULL) {
        perror("Erreur création fichier");
        return -1;
    }

    fprintf(fp, "%s\n", contenu);
    fclose(fp);
    return 0;
}

int fichier_existe(const char *chemin) {
    return access(chemin, F_OK) == 0;
}

int fichier_lisible(const char *chemin) {
    return access(chemin, R_OK) == 0;
}

int fichier_modifiable(const char *chemin) {
    return access(chemin, W_OK) == 0;
}

int main(void) {
    const char *fichier_test = "/tmp/test_file_ops.txt";

    // Ecrire un fichier
    printf("=== Ecriture du fichier ===\n");
    ecrire_fichier(fichier_test, "Ligne 1: Bonjour\nLigne 2: Monde\nLigne 3: Test");
    printf("Fichier ecrit: %s\n\n", fichier_test);

    // Verifier existence et permissions
    printf("=== Verification access() ===\n");
    printf("%s existe: %s\n", fichier_test,
           fichier_existe(fichier_test) ? "OUI" : "NON");
    printf("  Lisible: %s\n",
           fichier_lisible(fichier_test) ? "OUI" : "NON");
    printf("  Modifiable: %s\n\n",
           fichier_modifiable(fichier_test) ? "OUI" : "NON");

    // Relire le fichier
    printf("=== Lecture du fichier ===\n");
    lire_fichier_texte(fichier_test);

    // Nettoyer
    unlink(fichier_test);

    return 0;
}
