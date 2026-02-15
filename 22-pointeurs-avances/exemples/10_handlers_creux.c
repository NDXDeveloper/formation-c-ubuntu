/* ============================================================================
   Section 22.2 : Tableaux de pointeurs de fonctions
   Description : Initialisation partielle et tableaux creux de pointeurs de
                 fonctions
   Fichier source : 02-tableaux-pointeurs-fonctions.md
   ============================================================================ */

#include <stdio.h>

typedef void (*Handler)(int);

void handler_0(int code) { printf("Handler 0 : code %d\n", code); }
void handler_2(int code) { printf("Handler 2 : code %d\n", code); }
void handler_5(int code) { printf("Handler 5 : code %d\n", code); }

int main(void) {
    // Tableau avec initialisation partielle
    Handler handlers[10] = {
        [0] = handler_0,
        [2] = handler_2,
        [5] = handler_5
        // Les autres elements sont automatiquement NULL
    };

    // Execution securisee
    for (int i = 0; i < 10; i++) {
        if (handlers[i] != NULL) {
            printf("Appel du handler %d : ", i);
            handlers[i](i * 100);
        } else {
            printf("Handler %d : non defini\n", i);
        }
    }

    return 0;
}
