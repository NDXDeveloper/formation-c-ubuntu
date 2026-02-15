/* ============================================================================
   Section 24.1.4 : Limitations et portabilite
   Description : Compilation conditionnelle pour cleanup portable
   Fichier source : 01.4-limitations-portabilite.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

// Detection du support de cleanup
#if defined(__GNUC__) || defined(__clang__)
    #define CLEANUP_SUPPORTED 1
    #define _cleanup_(func) __attribute__((cleanup(func)))
#else
    #define CLEANUP_SUPPORTED 0
    #define _cleanup_(func)
#endif

// Fonctions de cleanup (toujours definies)
static inline void cleanup_free(void *p) {
    void **ptr = (void **)p;
    if (*ptr) {
        free(*ptr);
        *ptr = NULL;
    }
}

static inline void cleanup_fclose(FILE **f) {
    if (*f) {
        fclose(*f);
        *f = NULL;
    }
}

// Macros portables
#if CLEANUP_SUPPORTED
    #define _cleanup_free_ _cleanup_(cleanup_free)
    #define _cleanup_fclose_ _cleanup_(cleanup_fclose)
#else
    #define _cleanup_free_
    #define _cleanup_fclose_
#endif

// Afficher les features disponibles
void print_features(void) {
    printf("=== Features disponibles ===\n");
    #if CLEANUP_SUPPORTED
        printf("  cleanup automatique : Oui (GCC/Clang)\n");
    #else
        printf("  cleanup automatique : Non\n");
    #endif
    printf("\n");
}

// Fonction portable utilisant cleanup si disponible
void fonction_portable(void) {
    _cleanup_free_ char *buffer = malloc(100);
    _cleanup_fclose_ FILE *f = fopen("/dev/null", "r");

    if (!buffer || !f) {
        #if !CLEANUP_SUPPORTED
            free(buffer);
            if (f) fclose(f);
        #endif
        printf("Erreur d'allocation ou d'ouverture\n");
        return;
    }

    sprintf(buffer, "Donnees traitees avec succes");
    printf("Buffer: %s\n", buffer);
    printf("Fichier ouvert: oui\n");

    #if !CLEANUP_SUPPORTED
        free(buffer);
        fclose(f);
    #endif
}

// Alternative portable : goto cleanup pattern
int fonction_traditionnelle(void) {
    char *buffer = NULL;
    FILE *f = NULL;
    int resultat = -1;

    buffer = malloc(1024);
    if (!buffer) {
        goto cleanup;
    }

    f = fopen("/dev/null", "r");
    if (!f) {
        goto cleanup;
    }

    // Traitement
    sprintf(buffer, "Pattern goto cleanup - portable partout");
    printf("Buffer: %s\n", buffer);
    resultat = 0;

cleanup:
    if (f) fclose(f);
    free(buffer);  // free(NULL) est sur
    return resultat;
}

int main(void) {
    print_features();

    printf("--- Avec cleanup (si supporte) ---\n");
    fonction_portable();

    printf("\n--- Pattern goto (toujours portable) ---\n");
    int r = fonction_traditionnelle();
    printf("Resultat: %d\n", r);

    return 0;
}
