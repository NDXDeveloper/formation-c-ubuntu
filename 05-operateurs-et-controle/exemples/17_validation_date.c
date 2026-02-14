/* ============================================================================
   Section 5.4 : Conditions
   Description : Validation de date avec annees bissextiles
   Fichier source : 04-conditions.md
   ============================================================================ */
#include <stdio.h>
#include <stdbool.h>

bool est_annee_bissextile(int annee) {
    return (annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0);
}

bool date_valide(int jour, int mois, int annee) {
    if (annee < 1 || mois < 1 || mois > 12 || jour < 1) {
        return false;
    }

    int jours_par_mois[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (est_annee_bissextile(annee)) {
        jours_par_mois[2] = 29;
    }

    if (jour > jours_par_mois[mois]) {
        return false;
    }

    return true;
}

int main(void) {
    printf("29/02/2024 : %s\n", date_valide(29, 2, 2024) ? "Valide" : "Invalide");
    printf("29/02/2023 : %s\n", date_valide(29, 2, 2023) ? "Valide" : "Invalide");
    printf("31/04/2024 : %s\n", date_valide(31, 4, 2024) ? "Valide" : "Invalide");

    return 0;
}
