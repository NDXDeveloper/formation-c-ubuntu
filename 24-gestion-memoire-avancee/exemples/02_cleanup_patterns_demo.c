/* ============================================================================
   Section 24.1.3 : Patterns cleanup
   Description : Traitement de fichier utilisateurs avec cleanup_patterns.h
   Fichier source : 01.3-patterns.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include "cleanup_patterns.h"

int traiter_fichier_utilisateurs(const char *chemin) {
    _cleanup_fclose_ FILE *f = NULL;
    _cleanup_strv_ char **lignes = NULL;
    _cleanup_free_ char *ligne_courante = NULL;
    int nb_utilisateurs = 0;

    f = fopen(chemin, "r");
    if (!f) {
        perror("fopen");
        return -1;
    }

    // Allouer tableau de lignes (max 100)
    lignes = calloc(101, sizeof(char *));
    if (!lignes) {
        return -1;  // f ferme automatiquement
    }

    ligne_courante = malloc(256);
    if (!ligne_courante) {
        return -1;  // f ET lignes nettoyes
    }

    // Lire les lignes
    while (fgets(ligne_courante, 256, f) && nb_utilisateurs < 100) {
        // Retirer le \n
        ligne_courante[strcspn(ligne_courante, "\n")] = '\0';

        // Sauvegarder la ligne
        lignes[nb_utilisateurs] = strdup(ligne_courante);
        if (!lignes[nb_utilisateurs]) {
            return -1;  // Tout nettoye
        }

        nb_utilisateurs++;
    }

    lignes[nb_utilisateurs] = NULL;  // Marqueur de fin

    // Afficher les utilisateurs
    printf("=== %d utilisateurs trouves ===\n", nb_utilisateurs);
    for (int i = 0; lignes[i] != NULL; i++) {
        printf("%d. %s\n", i + 1, lignes[i]);
    }

    return nb_utilisateurs;
    // Tout est automatiquement nettoye : f, lignes, ligne_courante
}

int main(void) {
    // Creer un fichier de test
    FILE *f = fopen("utilisateurs.txt", "w");
    if (!f) {
        perror("fopen");
        return 1;
    }
    fprintf(f, "Alice\nBob\nCharlie\nDiane\nEve\n");
    fclose(f);

    int resultat = traiter_fichier_utilisateurs("utilisateurs.txt");

    if (resultat >= 0) {
        printf("\nTraitement reussi !\n");
    } else {
        fprintf(stderr, "\nErreur lors du traitement\n");
    }

    // Nettoyer le fichier de test
    remove("utilisateurs.txt");

    return resultat >= 0 ? 0 : 1;
}
