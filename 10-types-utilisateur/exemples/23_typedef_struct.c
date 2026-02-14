/* ============================================================================
   Section 10.6 : typedef - Simplification de la syntaxe
   Description : typedef avec struct pour simplifier les declarations
   Fichier source : 06-typedef.md
   ============================================================================ */

#include <stdio.h>

typedef struct {
    char nom[50];
    int age;
    float moyenne;
} Etudiant;

void afficher_etudiant(Etudiant e) {
    printf("Nom : %s\n", e.nom);
    printf("Age : %d ans\n", e.age);
    printf("Moyenne : %.2f/20\n", e.moyenne);
}

int main(void) {
    Etudiant alice = {"Alice Dupont", 20, 15.5};
    Etudiant bob = {"Bob Martin", 22, 14.2};

    afficher_etudiant(alice);
    printf("\n");
    afficher_etudiant(bob);

    return 0;
}
