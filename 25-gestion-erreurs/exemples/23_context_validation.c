/* ============================================================================
   Section 25.3 : Patterns de gestion d'erreurs
   Description : Context Object - validation de formulaire avec accumulation
   Fichier source : 03-patterns-gestion-erreurs.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    int nb_erreurs;
    int nb_avertissements;
    char messages[10][256];  /* Historique des erreurs */
    int nb_messages;
} ContexteValidation;

void init_contexte(ContexteValidation *ctx) {
    memset(ctx, 0, sizeof(*ctx));
}

void ajouter_erreur(ContexteValidation *ctx, const char *message) {
    ctx->nb_erreurs++;

    if (ctx->nb_messages < 10) {
        snprintf(ctx->messages[ctx->nb_messages], 256,
                 "[ERREUR] %s", message);
        ctx->nb_messages++;
    }
}

void ajouter_avertissement(ContexteValidation *ctx, const char *message) {
    ctx->nb_avertissements++;

    if (ctx->nb_messages < 10) {
        snprintf(ctx->messages[ctx->nb_messages], 256,
                 "[WARN] %s", message);
        ctx->nb_messages++;
    }
}

bool valider_nom(const char *nom, ContexteValidation *ctx) {
    if (nom == NULL || nom[0] == '\0') {
        ajouter_erreur(ctx, "Le nom est vide");
        return false;
    }

    if (strlen(nom) < 2) {
        ajouter_avertissement(ctx, "Le nom est tres court");
    }

    return true;
}

bool valider_age(int age, ContexteValidation *ctx) {
    if (age < 0) {
        ajouter_erreur(ctx, "L'age est negatif");
        return false;
    }

    if (age > 150) {
        ajouter_erreur(ctx, "L'age est irrealiste");
        return false;
    }

    if (age < 18) {
        ajouter_avertissement(ctx, "L'utilisateur est mineur");
    }

    return true;
}

bool valider_email(const char *email, ContexteValidation *ctx) {
    if (email == NULL || email[0] == '\0') {
        ajouter_erreur(ctx, "L'email est vide");
        return false;
    }

    if (strchr(email, '@') == NULL) {
        ajouter_erreur(ctx, "L'email ne contient pas de @");
        return false;
    }

    return true;
}

bool valider_utilisateur(const char *nom, int age, const char *email,
                         ContexteValidation *ctx) {
    bool valide = true;

    valide &= valider_nom(nom, ctx);
    valide &= valider_age(age, ctx);
    valide &= valider_email(email, ctx);

    return valide;
}

void afficher_rapport(const ContexteValidation *ctx) {
    printf("\n=== Rapport de validation ===\n");
    printf("Erreurs : %d\n", ctx->nb_erreurs);
    printf("Avertissements : %d\n", ctx->nb_avertissements);

    printf("\nDetails :\n");
    for (int i = 0; i < ctx->nb_messages; i++) {
        printf("  %s\n", ctx->messages[i]);
    }
}

int main(void) {
    ContexteValidation ctx;
    init_contexte(&ctx);

    /* Test 1 : Utilisateur invalide */
    printf("Test 1 : Utilisateur invalide\n");
    bool ok = valider_utilisateur("J", 200, "invalide", &ctx);
    afficher_rapport(&ctx);
    printf("Resultat : %s\n", ok ? "VALIDE" : "INVALIDE");

    /* Test 2 : Utilisateur valide avec avertissement */
    printf("\n\nTest 2 : Utilisateur mineur\n");
    init_contexte(&ctx);  /* Reinitialisation */
    ok = valider_utilisateur("Alice", 16, "alice@example.com", &ctx);
    afficher_rapport(&ctx);
    printf("Resultat : %s\n", ok ? "VALIDE" : "INVALIDE");

    return 0;
}
