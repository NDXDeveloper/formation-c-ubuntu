/* ============================================================================
   Section 25.1 : Codes de retour vs exceptions simulees
   Description : Pattern goto pour nettoyage centralise des ressources
   Fichier source : 01-codes-retour-vs-exceptions.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

int traiter_fichier(const char *nom) {
    FILE *fichier = NULL;
    char *buffer1 = NULL;
    char *buffer2 = NULL;
    int resultat = -1;  /* Par defaut : erreur */

    /* Etape 1 : Ouvrir le fichier */
    fichier = fopen(nom, "r");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur : impossible d'ouvrir le fichier\n");
        goto cleanup;  /* Saut vers le nettoyage */
    }

    /* Etape 2 : Allouer le premier buffer */
    buffer1 = malloc(1024);
    if (buffer1 == NULL) {
        fprintf(stderr, "Erreur : allocation buffer1\n");
        goto cleanup_fichier;
    }

    /* Etape 3 : Allouer le deuxieme buffer */
    buffer2 = malloc(2048);
    if (buffer2 == NULL) {
        fprintf(stderr, "Erreur : allocation buffer2\n");
        goto cleanup_buffer1;
    }

    /* Traitement reussi */
    printf("Traitement du fichier reussi\n");
    resultat = 0;  /* Succes */

    /* Nettoyage en cascade (ordre inverse de l'allocation) */
    free(buffer2);
cleanup_buffer1:
    free(buffer1);
cleanup_fichier:
    fclose(fichier);
cleanup:
    return resultat;
}

int main(void) {
    /* Test avec un fichier inexistant */
    printf("Test 1 : fichier inexistant\n");
    int code = traiter_fichier("inexistant.txt");
    printf("Code retour : %d\n\n", code);

    /* Test avec un fichier existant */
    printf("Test 2 : fichier existant\n");
    code = traiter_fichier("/etc/hosts");
    printf("Code retour : %d\n", code);

    return (code == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
