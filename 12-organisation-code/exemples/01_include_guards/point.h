/* ============================================================================
   Section 12.1 : Fichiers d'en-tete et gardes d'inclusion
   Description : Header du module Point avec gardes d'inclusion
   Fichier source : 01-fichiers-entete.md
   ============================================================================ */

#ifndef POINT_H
#define POINT_H

typedef struct {
    int x;
    int y;
} Point;

void afficher_point(Point p);
double distance_origine(Point p);

#endif /* POINT_H */
