/* ============================================================================
   Section 6.3 : Passage par valeur
   Description : Exemple complet combinant passage par valeur et par pointeur
   Fichier source : 03-passage-par-valeur.md
   ============================================================================ */
#include <stdio.h>

// Passage par valeur : lecture seule
int carre(int n) {
    return n * n;
}

// Passage par pointeur : modification
void tripler(int *n) {
    *n = *n * 3;
}

// Passage par valeur de structure (petite)
struct Point {
    int x;
    int y;
};

int distance_manhattan(struct Point p1, struct Point p2) {
    int dx = p1.x - p2.x;
    int dy = p1.y - p2.y;
    return (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
}

// Passage par pointeur de structure (modification)
void deplacer_point(struct Point *p, int dx, int dy) {
    p->x += dx;
    p->y += dy;
}

int main(void) {
    // Test 1 : carre (passage par valeur)
    int nombre = 5;
    int resultat = carre(nombre);
    printf("Le carré de %d est %d\n", nombre, resultat);
    printf("nombre vaut toujours %d\n\n", nombre);

    // Test 2 : tripler (passage par pointeur)
    int valeur = 7;
    printf("Avant tripler : %d\n", valeur);
    tripler(&valeur);
    printf("Après tripler : %d\n\n", valeur);

    // Test 3 : distance (passage par valeur de structures)
    struct Point p1 = {0, 0};
    struct Point p2 = {3, 4};
    int dist = distance_manhattan(p1, p2);
    printf("Distance Manhattan : %d\n\n", dist);

    // Test 4 : deplacer (passage par pointeur de structure)
    printf("Point p1 avant : (%d, %d)\n", p1.x, p1.y);
    deplacer_point(&p1, 10, 20);
    printf("Point p1 après : (%d, %d)\n", p1.x, p1.y);

    return 0;
}
