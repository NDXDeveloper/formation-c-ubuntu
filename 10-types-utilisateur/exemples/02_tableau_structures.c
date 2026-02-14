/* ============================================================================
   Section 10.1 : struct - Creation d'objets complexes
   Description : Tableau de structures (classe d'etudiants)
   Fichier source : 01-structures.md
   ============================================================================ */

#include <stdio.h>

struct Etudiant {
    char nom[50];
    int age;
    float moyenne;
};

int main(void) {
    struct Etudiant classe[3] = {
        {"Alice", 20, 15.5},
        {"Bob", 22, 14.2},
        {"Charlie", 21, 16.8}
    };

    for (int i = 0; i < 3; i++) {
        printf("Etudiant %d : %s, %d ans, moyenne %.2f\n",
               i + 1, classe[i].nom, classe[i].age, classe[i].moyenne);
    }

    return 0;
}
