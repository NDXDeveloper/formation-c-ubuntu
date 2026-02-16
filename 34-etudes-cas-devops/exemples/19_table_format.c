/* ============================================================================
   Section 34.1.3 : Output formate
   Description : Tableau formate avec alignement et bordures ASCII
   Fichier source : 01.3-output-formate.md
   ============================================================================ */
#include <stdio.h>

typedef struct {
    char nom[32];
    int age;
    float salaire;
} Personne;

void afficher_tableau(Personne *personnes, int count) {
    // En-tête
    printf("+----------------------+-----+------------+\n");
    printf("| %-20s | %-3s | %-10s |\n", "Nom", "Age", "Salaire");
    printf("+----------------------+-----+------------+\n");

    // Lignes de données
    for (int i = 0; i < count; i++) {
        printf("| %-20s | %3d | %10.2f |\n",
               personnes[i].nom,
               personnes[i].age,
               (double)personnes[i].salaire);
    }

    printf("+----------------------+-----+------------+\n");
}

int main(void) {
    Personne personnes[] = {
        {"Alice Dupont", 28, 45000.50f},
        {"Bob Martin", 35, 52000.00f},
        {"Charlie Durant", 42, 61500.75f}
    };

    afficher_tableau(personnes, 3);
    return 0;
}
