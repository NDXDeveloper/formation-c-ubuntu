#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "point.h"  /* Dependance necessaire */

typedef struct {
    Point coin_haut_gauche;
    Point coin_bas_droit;
} Rectangle;

int aire(Rectangle r);
int perimetre(Rectangle r);

#endif /* RECTANGLE_H */
