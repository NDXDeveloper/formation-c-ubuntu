#include "config.h"

void demarrer_serveur(void);

int main(void) {
    mode_debug = 1;  /* Modification de la config */
    demarrer_serveur();
    return 0;
}
