#include <stdio.h>
#include "config.h"

void demarrer_serveur(void) {
    printf("Demarrage de %s sur le port %d\n", nom_serveur, port_serveur);
    if (mode_debug) {
        printf("Mode debug active\n");
    }
}
