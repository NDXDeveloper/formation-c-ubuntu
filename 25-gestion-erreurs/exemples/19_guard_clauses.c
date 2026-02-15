/* ============================================================================
   Section 25.3 : Patterns de gestion d'erreurs
   Description : Guard Clauses (retours anticipes) avec validation et ressources
   Fichier source : 03-patterns-gestion-erreurs.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int sauvegarder_donnees(const char *fichier, const void *data, size_t taille) {
    /* Guard clause 1 : Validation des parametres */
    if (fichier == NULL || data == NULL || taille == 0) {
        fprintf(stderr, "Erreur : parametres invalides\n");
        return -1;
    }

    /* Guard clause 2 : Ouverture du fichier */
    FILE *f = fopen(fichier, "wb");
    if (f == NULL) {
        fprintf(stderr, "Erreur : impossible d'ouvrir %s\n", fichier);
        return -1;
    }

    /* Guard clause 3 : Ecriture */
    if (fwrite(data, 1, taille, f) != taille) {
        fprintf(stderr, "Erreur : ecriture incomplete\n");
        fclose(f);
        return -1;
    }

    /* Succes */
    fclose(f);
    return 0;
}

int main(void) {
    char donnees[] = "Hello, World!";

    /* Test 1 : sauvegarde reussie */
    printf("Test 1 : sauvegarde normale\n");
    if (sauvegarder_donnees("/tmp/output_test.txt", donnees,
                            strlen(donnees)) == 0) {
        printf("Donnees sauvegardees\n\n");
    }

    /* Test 2 : parametres invalides */
    printf("Test 2 : parametres NULL\n");
    if (sauvegarder_donnees(NULL, donnees, strlen(donnees)) != 0) {
        printf("Echec attendu\n\n");
    }

    /* Test 3 : taille zero */
    printf("Test 3 : taille zero\n");
    if (sauvegarder_donnees("/tmp/output_test.txt", donnees, 0) != 0) {
        printf("Echec attendu\n");
    }

    /* Nettoyage */
    remove("/tmp/output_test.txt");

    return EXIT_SUCCESS;
}
