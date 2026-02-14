/* ============================================================================
   Section 10.1 : struct - Creation d'objets complexes
   Description : Gestion d'un etudiant avec structure
   Fichier source : 01-structures.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>

struct Etudiant {
    char nom[50];
    int age;
    float moyenne;
};

int main(void) {
    struct Etudiant etudiant1;

    strcpy(etudiant1.nom, "Alice Dupont");
    etudiant1.age = 20;
    etudiant1.moyenne = 15.5;

    printf("Informations de l'etudiant :\n");
    printf("Nom : %s\n", etudiant1.nom);
    printf("Age : %d ans\n", etudiant1.age);
    printf("Moyenne : %.2f/20\n", etudiant1.moyenne);

    return 0;
}
