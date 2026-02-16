/* ============================================================================
   Section 33.1 : Pattern Goto Cleanup
   Description : Pattern goto cleanup (Git / Linux Kernel) pour liberation propre
   Fichier source : README.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================ */
/* Pattern : goto cleanup (Linux Kernel / Git)  */
/* ============================================ */

/*
 * Ce pattern evite la duplication de code de nettoyage.
 * Au lieu de liberer les ressources a chaque point d'echec,
 * on saute vers un label unique de nettoyage.
 */

/* Simulation d'une operation multi-ressources */
static int process_data(const char *filename, const char *output)
{
    FILE *in = NULL;
    FILE *out = NULL;
    char *buffer = NULL;
    int result = -1;

    /* Allocation du buffer */
    buffer = malloc(256);
    if (!buffer) {
        fprintf(stderr, "Erreur: allocation buffer echouee\n");
        goto cleanup;
    }

    /* Ouverture du fichier d'entree */
    in = fopen(filename, "r");
    if (!in) {
        fprintf(stderr, "Erreur: impossible d'ouvrir '%s'\n", filename);
        goto cleanup;
    }

    /* Ouverture du fichier de sortie */
    out = fopen(output, "w");
    if (!out) {
        fprintf(stderr, "Erreur: impossible de creer '%s'\n", output);
        goto cleanup;
    }

    /* Traitement : copier avec prefixe de numero de ligne */
    int line_num = 1;
    while (fgets(buffer, 256, in) != NULL) {
        fprintf(out, "%4d | %s", line_num++, buffer);
    }

    printf("Traitement reussi: %d lignes copiees\n", line_num - 1);
    result = 0;  /* Succes */

cleanup:
    /* Nettoyage unique : chaque ressource est liberee si elle a ete allouee */
    if (out)
        fclose(out);
    if (in)
        fclose(in);
    free(buffer);  /* free(NULL) est safe */

    return result;
}

/* ============================================ */
/* Comparaison : sans goto (code duplique)      */
/* ============================================ */

/*
 * ANTI-PATTERN : Sans goto, le code de nettoyage est duplique.
 * Plus le nombre de ressources augmente, plus c'est penible.
 */
static int process_data_without_goto(const char *filename, const char *output)
{
    char *buffer = malloc(256);
    if (!buffer) {
        fprintf(stderr, "Erreur: allocation buffer echouee\n");
        return -1;  /* Rien a liberer */
    }

    FILE *in = fopen(filename, "r");
    if (!in) {
        fprintf(stderr, "Erreur: impossible d'ouvrir '%s'\n", filename);
        free(buffer);  /* Duplication #1 */
        return -1;
    }

    FILE *out = fopen(output, "w");
    if (!out) {
        fprintf(stderr, "Erreur: impossible de creer '%s'\n", output);
        fclose(in);    /* Duplication #2 */
        free(buffer);  /* Duplication #3 */
        return -1;
    }

    int line_num = 1;
    while (fgets(buffer, 256, in) != NULL) {
        fprintf(out, "%4d | %s", line_num++, buffer);
    }

    fclose(out);
    fclose(in);
    free(buffer);

    return 0;
}

int main(void)
{
    printf("=== Pattern Goto Cleanup (Git / Linux Kernel) ===\n\n");

    /* Creer un fichier de test */
    const char *test_file = "/tmp/goto_test.txt";
    const char *out_file = "/tmp/goto_output.txt";

    FILE *f = fopen(test_file, "w");
    if (!f) {
        fprintf(stderr, "Erreur creation fichier test\n");
        return EXIT_FAILURE;
    }
    fprintf(f, "Premiere ligne\n");
    fprintf(f, "Deuxieme ligne\n");
    fprintf(f, "Troisieme ligne\n");
    fclose(f);

    /* Test 1 : Cas nominal (succes) */
    printf("--- Test 1 : Cas nominal ---\n");
    int ret = process_data(test_file, out_file);
    printf("Retour: %d (%s)\n\n", ret, ret == 0 ? "succes" : "echec");

    /* Test 2 : Fichier inexistant (goto cleanup apres echec) */
    printf("--- Test 2 : Fichier inexistant ---\n");
    ret = process_data("fichier_inexistant.txt", out_file);
    printf("Retour: %d (%s)\n\n", ret, ret == 0 ? "succes" : "echec");

    /* Test 3 : Meme chose sans goto */
    printf("--- Test 3 : Sans goto (meme resultat, code duplique) ---\n");
    ret = process_data_without_goto(test_file, out_file);
    printf("Retour: %d (%s)\n\n", ret, ret == 0 ? "succes" : "echec");

    /* Afficher le resultat */
    printf("--- Contenu du fichier de sortie ---\n");
    f = fopen(out_file, "r");
    if (f) {
        char buf[512];
        while (fgets(buf, (int)sizeof(buf), f) != NULL) {
            printf("%s", buf);
        }
        fclose(f);
    }

    /* Nettoyage */
    remove(test_file);
    remove(out_file);

    printf("\n--- Avantages du goto cleanup ---\n");
    printf("1. Un seul point de sortie pour le nettoyage\n");
    printf("2. Pas de duplication de code de liberation\n");
    printf("3. Facile a maintenir quand on ajoute des ressources\n");
    printf("4. Pattern standard dans le noyau Linux et Git\n");

    return EXIT_SUCCESS;
}
