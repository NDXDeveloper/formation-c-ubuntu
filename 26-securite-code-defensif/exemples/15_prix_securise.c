/* ============================================================================
   Section 26.5 : Integer overflow
   Description : Calcul de prix securise avec verification d'overflow
   Fichier source : 05-integer-overflow.md
   ============================================================================ */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int64_t prix_unitaire;  /* En centimes, type plus grand */
    int64_t quantite;
} Article;

bool calculer_total_securise(Article articles[], int nb_articles, int64_t *total) {
    if (articles == NULL || total == NULL || nb_articles <= 0) {
        return false;
    }

    *total = 0;

    for (int i = 0; i < nb_articles; i++) {
        /* Validation des valeurs */
        if (articles[i].prix_unitaire < 0 || articles[i].quantite < 0) {
            fprintf(stderr, "Erreur : valeurs negatives\n");
            return false;
        }

        /* Verification overflow pour la multiplication */
        if (articles[i].quantite > 0 &&
            articles[i].prix_unitaire > INT64_MAX / articles[i].quantite) {
            fprintf(stderr, "Erreur : overflow dans le calcul du sous-total\n");
            return false;
        }

        int64_t sous_total = articles[i].prix_unitaire * articles[i].quantite;

        /* Verification overflow pour l'addition */
        if (sous_total > INT64_MAX - *total) {
            fprintf(stderr, "Erreur : overflow dans le total\n");
            return false;
        }

        *total += sous_total;
    }

    return true;
}

int main(void) {
    printf("=== Calcul de prix securise ===\n\n");

    /* Test normal */
    printf("--- Panier normal ---\n");
    Article panier1[] = {
        {100000, 3000},   /* 1000.00 EUR x 3000 */
        {50000, 1000},    /*  500.00 EUR x 1000 */
        {2500, 5}         /*   25.00 EUR x 5    */
    };

    int64_t total;
    if (calculer_total_securise(panier1, 3, &total)) {
        printf("  Article 1 : %.2f EUR x 3000\n", 100000 / 100.0);
        printf("  Article 2 : %.2f EUR x 1000\n", 50000 / 100.0);
        printf("  Article 3 : %.2f EUR x 5\n", 2500 / 100.0);
        printf("  Total : %lld centimes (%.2f EUR)\n",
               (long long)total, total / 100.0);
    }

    /* Test avec valeurs negatives */
    printf("\n--- Panier avec prix negatif ---\n");
    Article panier2[] = {
        {-100, 5}
    };
    if (!calculer_total_securise(panier2, 1, &total)) {
        printf("  Rejete : prix negatif detecte\n");
    }

    /* Test avec tres grandes valeurs */
    printf("\n--- Panier avec tres grandes valeurs ---\n");
    Article panier3[] = {
        {INT64_MAX / 2, 3}  /* Va provoquer un overflow */
    };
    if (!calculer_total_securise(panier3, 1, &total)) {
        printf("  Rejete : overflow detecte\n");
    }

    /* Test avec pointeur NULL */
    printf("\n--- Panier NULL ---\n");
    if (!calculer_total_securise(NULL, 1, &total)) {
        printf("  Rejete : pointeur NULL detecte\n");
    }

    printf("\n=== Fin du calcul securise ===\n");
    return 0;
}
