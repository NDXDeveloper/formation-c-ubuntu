/* ============================================================================
   Section 3.5 : Fichiers intermediaires
   Description : Programme elabore avec fonction, macro et variable globale
   Fichier source : 05-fichiers-intermediaires.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>

#define TAILLE_MAX 100
#define CARRE(x) ((x) * (x))

/* Variable globale */
int compteur = 0;

/* Fonction auxiliaire */
int calculer_somme(int n) {
    int somme = 0;
    for (int i = 1; i <= n; i++) {
        somme += i;
        compteur++;
    }
    return somme;
}

int main(void) {
    int valeur = 5;
    int resultat = calculer_somme(valeur);
    int carre = CARRE(valeur);

    printf("Somme de 1 à %d : %d\n", valeur, resultat);
    printf("Carré de %d : %d\n", valeur, carre);
    printf("Nombre d'itérations : %d\n", compteur);

    return EXIT_SUCCESS;
}
