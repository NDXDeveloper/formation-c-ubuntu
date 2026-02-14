#include <stdlib.h>
#include "rectangle.h"

int aire(Rectangle r) {
    int largeur = abs(r.coin_bas_droit.x - r.coin_haut_gauche.x);
    int hauteur = abs(r.coin_bas_droit.y - r.coin_haut_gauche.y);
    return largeur * hauteur;
}

int perimetre(Rectangle r) {
    int largeur = abs(r.coin_bas_droit.x - r.coin_haut_gauche.x);
    int hauteur = abs(r.coin_bas_droit.y - r.coin_haut_gauche.y);
    return 2 * (largeur + hauteur);
}
