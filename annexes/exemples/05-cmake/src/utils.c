#include <stdio.h>
#include "mon_projet.h"

void afficher_resultat(int a, int b) {
    printf("%d + %d = %d\n", a, b, ma_lib_add(a, b));
}
