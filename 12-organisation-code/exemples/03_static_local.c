/* ============================================================================
   Section 12.2 : La portee des variables
   Description : Variables static locales - persistance entre appels de fonction
                 (compteur d'appels et moyenne avec suivi)
   Fichier source : 02-portee-variables.md
   ============================================================================ */

#include <stdio.h>

/* Compteur simple avec static local */
void compter_appels(void) {
    static int compteur = 0;  /* Initialisee UNE SEULE FOIS */
    compteur++;
    printf("Appel numero %d\n", compteur);
}

/* Moyenne avec compteur d'appels */
int calculer_moyenne(int a, int b) {
    static int nb_appels = 0;  /* Persiste entre les appels */
    nb_appels++;

    int moyenne = (a + b) / 2;
    printf("Appel #%d : moyenne de %d et %d = %d\n",
           nb_appels, a, b, moyenne);

    return moyenne;
}

int main(void) {
    printf("=== Compteur d'appels ===\n");
    compter_appels();
    compter_appels();
    compter_appels();

    printf("\n=== Moyenne avec suivi ===\n");
    calculer_moyenne(10, 20);
    calculer_moyenne(5, 15);
    calculer_moyenne(8, 12);

    return 0;
}
