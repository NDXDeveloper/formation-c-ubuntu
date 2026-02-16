/* ============================================================================
   Section 34.1.3 : Output formate
   Description : Barre de progression animee dans le terminal
   Fichier source : 01.3-output-formate.md
   ============================================================================ */
#include <stdio.h>
#include <unistd.h>

void afficher_barre_progression(int pourcentage) {
    int largeur = 50;
    int rempli = (pourcentage * largeur) / 100;

    printf("\r[");
    for (int i = 0; i < largeur; i++) {
        if (i < rempli) {
            printf("█");
        } else {
            printf(" ");
        }
    }
    printf("] %3d%%", pourcentage);
    fflush(stdout);
}

int main(void) {
    printf("Téléchargement en cours...\n");

    for (int i = 0; i <= 100; i += 5) {
        afficher_barre_progression(i);
        usleep(100000);  // 100ms
    }

    printf("\n✓ Téléchargement terminé!\n");
    return 0;
}
