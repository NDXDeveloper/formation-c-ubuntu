/* ============================================================================
   Section 10.4 : union - Optimisation memoire
   Description : Tagged union (union avec champ type pour securite)
   Fichier source : 04-unions.md
   ============================================================================ */

#include <stdio.h>

enum TypeDonnee {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING
};

struct Valeur {
    enum TypeDonnee type;
    union {
        int i;
        float f;
        char *s;
    } data;
};

void afficher_valeur(struct Valeur v) {
    switch (v.type) {
        case TYPE_INT:
            printf("Entier : %d\n", v.data.i);
            break;
        case TYPE_FLOAT:
            printf("Float  : %.2f\n", v.data.f);
            break;
        case TYPE_STRING:
            printf("String : %s\n", v.data.s);
            break;
    }
}

int main(void) {
    struct Valeur v1 = {TYPE_INT, {.i = 42}};
    struct Valeur v2 = {TYPE_FLOAT, {.f = 3.14f}};
    struct Valeur v3 = {TYPE_STRING, {.s = "Bonjour"}};

    afficher_valeur(v1);
    afficher_valeur(v2);
    afficher_valeur(v3);

    return 0;
}
