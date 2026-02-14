#include <stdio.h>
#include <math.h>
#include "point.h"

void afficher_point(Point p) {
    printf("Point(%d, %d)\n", p.x, p.y);
}

double distance_origine(Point p) {
    return sqrt((double)(p.x * p.x + p.y * p.y));
}
