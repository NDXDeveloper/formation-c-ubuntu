/* ============================================================================
   Section 4.4 : Conversion de types
   Description : Exemple complet des conversions (division, promotion, troncature, precision)
   Fichier source : 04-conversion-types.md
   ============================================================================ */
#include <stdio.h>
#include <limits.h>
#include <math.h>

int main(void) {
    printf("=== Conversions de types ===\n\n");

    /* 1. Division entiere vs flottante */
    int a = 7, b = 2;
    printf("1. Division\n");
    printf("   Entière : %d / %d = %d\n", a, b, a / b);
    printf("   Flottante : %d / %d = %.2f\n\n", a, b, (double)a / b);

    /* 2. Promotion automatique */
    char petit = 100;
    int grand = petit;
    printf("2. Promotion char → int\n");
    printf("   char %d → int %d\n\n", petit, grand);

    /* 3. Troncature */
    double pi = 3.14159;
    int pi_tronque = (int)pi;
    int pi_arrondi = (int)round(pi);
    printf("3. Troncature double → int\n");
    printf("   Original : %.5f\n", pi);
    printf("   Tronqué : %d\n", pi_tronque);
    printf("   Arrondi : %d\n\n", pi_arrondi);

    /* 4. Perte de precision */
    int grand_entier = 123456789;
    float flottant = (float)grand_entier;
    printf("4. Perte de précision int → float\n");
    printf("   Original (int) : %d\n", grand_entier);
    printf("   Converti (float) : %.0f\n\n", flottant);

    /* 5. Signed vs Unsigned */
    int negatif = -1;
    unsigned int positif = (unsigned int)negatif;
    printf("5. Conversion signed → unsigned\n");
    printf("   Signé : %d\n", negatif);
    printf("   Non signé : %u\n\n", positif);

    /* 6. Verification des limites */
    double valeur = 3000000000.0;
    printf("6. Vérification des limites\n");
    if (valeur > INT_MAX) {
        printf("   %.0f dépasse INT_MAX (%d)\n", valeur, INT_MAX);
        printf("   Conversion en int non recommandée\n\n");
    }

    /* 7. Calcul de pourcentage */
    int reussis = 42;
    int total = 50;
    double pourcentage = (double)reussis / total * 100.0;
    printf("7. Calcul de pourcentage\n");
    printf("   %d / %d = %.2f%%\n", reussis, total, pourcentage);

    return 0;
}
