/* ============================================================================
   Section 10.6 : typedef - Simplification de la syntaxe
   Description : Tagged union avec typedef (enum + union + struct)
   Fichier source : 06-typedef.md
   ============================================================================ */

#include <stdio.h>

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING
} TypeDonnee;

typedef union {
    int i;
    float f;
    char *s;
} DonneeUnion;

typedef struct {
    TypeDonnee type;
    DonneeUnion data;
} Donnee;

void afficher_donnee(Donnee d) {
    switch (d.type) {
        case TYPE_INT:
            printf("Entier : %d\n", d.data.i);
            break;
        case TYPE_FLOAT:
            printf("Float : %.2f\n", d.data.f);
            break;
        case TYPE_STRING:
            printf("String : %s\n", d.data.s);
            break;
    }
}

int main(void) {
    Donnee d1 = {TYPE_INT, {.i = 42}};
    Donnee d2 = {TYPE_FLOAT, {.f = 3.14f}};
    Donnee d3 = {TYPE_STRING, {.s = "Bonjour"}};

    afficher_donnee(d1);
    afficher_donnee(d2);
    afficher_donnee(d3);

    return 0;
}
