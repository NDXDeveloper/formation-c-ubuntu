/* ============================================================================
   Section 4.1 : Types primitifs et leur taille
   Description : Exemples pratiques de declaration avec types classiques et largeur fixe
   Fichier source : 01-types-primitifs.md
   ============================================================================ */
#include <stdio.h>
#include <stdint.h>

int main(void) {
    /* Types classiques */
    char initiale = 'J';
    int age = 30;
    float taille = 1.75f;
    double salaire = 45000.50;

    /* Types a largeur fixe */
    uint32_t timestamp = 1704067200;
    int64_t population_mondiale = 8000000000LL;

    /* Affichage */
    printf("Initiale : %c\n", initiale);
    printf("Age : %d ans\n", age);
    printf("Taille : %.2f m\n", taille);
    printf("Salaire : %.2f €\n", salaire);
    printf("Timestamp : %u\n", (unsigned)timestamp);
    printf("Population : %lld personnes\n", (long long)population_mondiale);

    return 0;
}
