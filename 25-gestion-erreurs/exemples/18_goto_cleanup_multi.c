/* ============================================================================
   Section 25.3 : Patterns de gestion d'erreurs
   Description : Goto cleanup multi-etapes (traitement fichier entree/sortie)
   Fichier source : 03-patterns-gestion-erreurs.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int traiter_donnees(const char *fichier_entree, const char *fichier_sortie) {
    FILE *entree = NULL;
    FILE *sortie = NULL;
    char *buffer = NULL;
    int resultat = -1;  /* Par defaut : echec */

    /* Etape 1 : Ouvrir le fichier d'entree */
    entree = fopen(fichier_entree, "r");
    if (entree == NULL) {
        fprintf(stderr, "Erreur : impossible d'ouvrir %s\n", fichier_entree);
        goto cleanup;
    }

    /* Etape 2 : Ouvrir le fichier de sortie */
    sortie = fopen(fichier_sortie, "w");
    if (sortie == NULL) {
        fprintf(stderr, "Erreur : impossible de creer %s\n", fichier_sortie);
        goto cleanup_entree;
    }

    /* Etape 3 : Allouer le buffer */
    buffer = malloc(4096);
    if (buffer == NULL) {
        fprintf(stderr, "Erreur : allocation memoire echouee\n");
        goto cleanup_sortie;
    }

    /* Traitement des donnees */
    size_t octets_lus;
    while ((octets_lus = fread(buffer, 1, 4096, entree)) > 0) {
        if (fwrite(buffer, 1, octets_lus, sortie) != octets_lus) {
            fprintf(stderr, "Erreur : ecriture echouee\n");
            goto cleanup_buffer;  /* Erreur : on nettoie tout */
        }
    }

    /* Verifier si fread a echoue */
    if (ferror(entree)) {
        fprintf(stderr, "Erreur : lecture echouee\n");
        goto cleanup_buffer;
    }

    printf("Traitement reussi\n");
    resultat = 0;  /* Succes ! */

    /* Nettoyage en cascade (ordre inverse de l'allocation) */
cleanup_buffer:
    free(buffer);
cleanup_sortie:
    fclose(sortie);
cleanup_entree:
    fclose(entree);
cleanup:
    return resultat;
}

int main(void) {
    /* Creer un fichier de test */
    FILE *f = fopen("/tmp/entree_test.txt", "w");
    if (f == NULL) {
        return EXIT_FAILURE;
    }
    fprintf(f, "Donnees de test\nLigne 2\nLigne 3\n");
    fclose(f);

    /* Test : copie reussie */
    printf("Test : traitement de fichier\n");
    int code = traiter_donnees("/tmp/entree_test.txt", "/tmp/sortie_test.txt");
    printf("Code retour : %d\n", code);

    /* Nettoyage */
    remove("/tmp/entree_test.txt");
    remove("/tmp/sortie_test.txt");

    return (code == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
