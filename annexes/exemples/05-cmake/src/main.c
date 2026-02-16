#include <stdio.h>
#include "mon_projet.h"

extern void afficher_resultat(int a, int b);

int main(void) {
    printf("Résultat: %d\n", ma_lib_add(3, 4));
    afficher_resultat(10, 20);
    return 0;
}
