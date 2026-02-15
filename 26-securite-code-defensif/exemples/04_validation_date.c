/* ============================================================================
   Section 26.2 : Validation des entrees
   Description : Validation semantique (structure Date avec annees bissextiles)
   Fichier source : 02-validation-entrees.md
   ============================================================================ */

#include <stdio.h>
#include <stdbool.h>

typedef struct {
    int jour;
    int mois;
    int annee;
} Date;

bool valider_date(const Date *date) {
    if (date == NULL) {
        return false;
    }

    /* Validation de l'annee */
    if (date->annee < 1900 || date->annee > 2100) {
        fprintf(stderr, "  Erreur : annee invalide (%d)\n", date->annee);
        return false;
    }

    /* Validation du mois */
    if (date->mois < 1 || date->mois > 12) {
        fprintf(stderr, "  Erreur : mois invalide (%d)\n", date->mois);
        return false;
    }

    /* Jours par mois */
    int jours_par_mois[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    /* Annee bissextile */
    bool bissextile = (date->annee % 4 == 0 && date->annee % 100 != 0) ||
                      (date->annee % 400 == 0);

    if (bissextile && date->mois == 2) {
        jours_par_mois[1] = 29;
    }

    /* Validation du jour */
    if (date->jour < 1 || date->jour > jours_par_mois[date->mois - 1]) {
        fprintf(stderr, "  Erreur : jour invalide pour ce mois (%d/%d/%d)\n",
                date->jour, date->mois, date->annee);
        return false;
    }

    return true;
}

int main(void) {
    printf("=== Validation semantique de dates ===\n\n");

    Date dates[] = {
        {29, 2, 2024},   /* Bissextile : valide */
        {29, 2, 2023},   /* Non bissextile : invalide */
        {31, 12, 2025},  /* Dernier jour decembre : valide */
        {31, 4, 2025},   /* Avril a 30 jours : invalide */
        {15, 6, 2000},   /* Date normale : valide */
        {0, 1, 2025},    /* Jour 0 : invalide */
        {1, 13, 2025},   /* Mois 13 : invalide */
        {29, 2, 2000},   /* 2000 bissextile (divisible par 400) : valide */
        {29, 2, 1900},   /* 1900 non bissextile (divisible par 100) : invalide */
    };
    int nb = (int)(sizeof(dates) / sizeof(dates[0]));

    for (int i = 0; i < nb; i++) {
        printf("Date %02d/%02d/%04d : ", dates[i].jour, dates[i].mois, dates[i].annee);
        if (valider_date(&dates[i])) {
            printf("VALIDE\n");
        } else {
            printf("INVALIDE\n");
        }
    }

    return 0;
}
