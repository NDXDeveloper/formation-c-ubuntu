#include <stdio.h>
#include "point.h"
#include "rectangle.h"  /* Inclut indirectement point.h */
#include "math_utils.h"

int main(void) {
    /* Demonstration des include guards :
       point.h est inclus 2 fois (directement + via rectangle.h)
       mais les gardes d'inclusion evitent la redefinition */

    Point p = {3, 4};
    Rectangle rect = {{0, 0}, {10, 5}};

    printf("=== Module Point ===\n");
    afficher_point(p);
    printf("Distance a l'origine : %.2f\n", distance_origine(p));

    printf("\n=== Module Rectangle ===\n");
    printf("Aire du rectangle : %d\n", aire(rect));
    printf("Perimetre du rectangle : %d\n", perimetre(rect));

    printf("\n=== Module Math Utils ===\n");
    printf("Carre de 7 : %d\n", carre(7));
    printf("Racine de 2 : %.6f\n", racine_carree_approx(2.0));
    printf("PI = %.10f\n", PI);

    return 0;
}
