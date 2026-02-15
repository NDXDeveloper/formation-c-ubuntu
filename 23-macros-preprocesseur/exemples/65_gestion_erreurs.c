/* ============================================================================
   Section 23.6 : Macros predefinies utiles
   Description : Macros de gestion d'erreurs avec contexte complet
   Fichier source : 06-macros-predefinies.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Macro pour vérifier les pointeurs
#define CHECK_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            fprintf(stderr, "ERREUR [%s:%d:%s()] - Pointeur NULL\n", \
                    __FILE__, __LINE__, __func__); \
            return -1; \
        } \
    } while(0)

// Macro pour les erreurs fatales
#define FATAL_ERROR(msg) \
    do { \
        fprintf(stderr, "ERREUR FATALE [%s:%d:%s()] - %s\n", \
                __FILE__, __LINE__, __func__, msg); \
        exit(EXIT_FAILURE); \
    } while(0)

// Exemple d'utilisation
int traiter_fichier(const char* nom_fichier) {
    CHECK_NULL(nom_fichier);

    FILE* fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        fprintf(stderr, "ERREUR [%s:%d:%s()] - Impossible d'ouvrir '%s': %s\n",
                __FILE__, __LINE__, __func__, nom_fichier, strerror(errno));
        return -1;
    }

    // Traitement...
    printf("Fichier '%s' ouvert avec succès\n", nom_fichier);

    fclose(fichier);
    return 0;
}

int main(void) {
    printf("Test de gestion d'erreurs\n\n");

    // Test avec un fichier qui n'existe probablement pas
    if (traiter_fichier("fichier_inexistant.txt") != 0) {
        printf("Le traitement a échoué (comportement attendu)\n");
    }

    return 0;
}
