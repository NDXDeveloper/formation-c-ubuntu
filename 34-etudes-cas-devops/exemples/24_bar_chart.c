/* ============================================================================
   Section 34.1.3 : Output formate
   Description : Graphique a barres horizontales en ASCII
   Fichier source : 01.3-output-formate.md
   ============================================================================ */
#include <stdio.h>

void afficher_barre_horizontale(const char *label, int valeur, int max) {
    int largeur_max = 40;
    int largeur = (valeur * largeur_max) / max;

    printf("%-15s │", label);
    for (int i = 0; i < largeur; i++) {
        printf("█");
    }
    printf(" %d\n", valeur);
}

int main(void) {
    printf("Utilisation des ressources\n\n");

    afficher_barre_horizontale("CPU", 65, 100);
    afficher_barre_horizontale("RAM", 82, 100);
    afficher_barre_horizontale("Disque", 45, 100);
    afficher_barre_horizontale("Réseau", 30, 100);

    return 0;
}
