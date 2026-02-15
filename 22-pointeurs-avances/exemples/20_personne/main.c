/* ============================================================================
   Section 22.4 : Pointeurs opaques
   Description : Programme client utilisant le type opaque Personne
   Fichier source : 04-pointeurs-opaques.md
   ============================================================================ */

#include "personne.h"
#include <stdio.h>

int main(void) {
    Personne *p = creer_personne("Dupont", "Jean", 30);
    if (p == NULL) {
        return 1;
    }

    // Acces controle via des fonctions
    definir_salaire(p, 45000.0f);

    printf("Age : %d\n", obtenir_age(p));
    printf("Salaire : %.2f\n", (double)obtenir_salaire(p));

    afficher_personne(p);

    // IMPOSSIBLE : p->age n'est pas accessible !
    // printf("%d\n", p->age);  // Erreur de compilation

    // IMPOSSIBLE : sizeof(*p) ne compile pas
    // int taille = sizeof(*p);  // Erreur : type incomplet

    liberer_personne(p);
    return 0;
}
