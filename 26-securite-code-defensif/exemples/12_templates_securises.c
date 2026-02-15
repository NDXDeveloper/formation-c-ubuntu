/* ============================================================================
   Section 26.4 : Format string vulnerabilities
   Description : Systeme de templates securise avec liste blanche
   Fichier source : 04-format-string-vulnerabilities.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* Liste blanche de templates autorises */
typedef struct {
    const char *nom;
    const char *format;
} Template;

static const Template TEMPLATES[] = {
    {"bienvenue",     "Bienvenue %s!\n"},
    {"reminder",      "Rappel pour %s\n"},
    {"confirmation",  "Confirmation pour %s\n"}
};

static const size_t NB_TEMPLATES = sizeof(TEMPLATES) / sizeof(TEMPLATES[0]);

bool generer_email(const char *nom_template, const char *nom) {
    /* Recherche dans la liste blanche */
    for (size_t i = 0; i < NB_TEMPLATES; i++) {
        if (strcmp(nom_template, TEMPLATES[i].nom) == 0) {
            /* Template sur trouve */
            printf(TEMPLATES[i].format, nom);
            return true;
        }
    }

    fprintf(stderr, "Erreur : template '%s' non autorise\n", nom_template);
    return false;
}

/* Validation de format : verifier qu'il n'y a pas de % dans l'entree */
static bool est_format_sur(const char *format) {
    if (strchr(format, '%') != NULL) {
        return false;  /* Rejeter toute chaine contenant '%' */
    }
    return true;
}

int main(void) {
    printf("=== Systeme de templates securise ===\n\n");

    /* Test avec templates valides */
    printf("--- Templates valides ---\n");
    printf("  "); generer_email("bienvenue", "Alice");
    printf("  "); generer_email("reminder", "Bob");
    printf("  "); generer_email("confirmation", "Caroline");

    /* Test avec template invalide */
    printf("\n--- Template invalide ---\n");
    printf("  ");
    if (!generer_email("malicieux", "test")) {
        printf("  Template rejete correctement\n");
    }

    /* Test de validation de format */
    printf("\n--- Validation de format ---\n");
    const char *formats_test[] = {
        "Message normal",
        "Message avec %x",
        "Hello World",
        "Exploit %n"
    };
    int nb = (int)(sizeof(formats_test) / sizeof(formats_test[0]));

    for (int i = 0; i < nb; i++) {
        printf("  \"%s\" -> %s\n", formats_test[i],
               est_format_sur(formats_test[i]) ? "SUR" : "REJETE (contient %%)");
    }

    printf("\n=== Templates securises termine ===\n");
    return 0;
}
