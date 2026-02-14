/* ============================================================================
   Section 10.4 : union - Optimisation memoire
   Description : Systeme de gestion de variables typees avec tagged union
   Fichier source : 04-unions.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>

enum TypeVariable {
    VAR_INT,
    VAR_DOUBLE,
    VAR_STRING,
    VAR_BOOL
};

struct Variable {
    char nom[32];
    enum TypeVariable type;
    union {
        int i;
        double d;
        char s[128];
        int b;
    } valeur;
};

void initialiser_variable(struct Variable *v, const char *nom, enum TypeVariable type) {
    strncpy(v->nom, nom, 31);
    v->nom[31] = '\0';
    v->type = type;
}

void definir_int(struct Variable *v, int valeur) {
    if (v->type == VAR_INT) {
        v->valeur.i = valeur;
    }
}

void definir_double(struct Variable *v, double valeur) {
    if (v->type == VAR_DOUBLE) {
        v->valeur.d = valeur;
    }
}

void definir_string(struct Variable *v, const char *valeur) {
    if (v->type == VAR_STRING) {
        strncpy(v->valeur.s, valeur, 127);
        v->valeur.s[127] = '\0';
    }
}

void definir_bool(struct Variable *v, int valeur) {
    if (v->type == VAR_BOOL) {
        v->valeur.b = valeur ? 1 : 0;
    }
}

void afficher_variable(struct Variable *v) {
    printf("Variable '%s' = ", v->nom);
    switch (v->type) {
        case VAR_INT:
            printf("%d (int)\n", v->valeur.i);
            break;
        case VAR_DOUBLE:
            printf("%.2f (double)\n", v->valeur.d);
            break;
        case VAR_STRING:
            printf("\"%s\" (string)\n", v->valeur.s);
            break;
        case VAR_BOOL:
            printf("%s (bool)\n", v->valeur.b ? "true" : "false");
            break;
    }
}

int main(void) {
    struct Variable vars[4];

    initialiser_variable(&vars[0], "compteur", VAR_INT);
    definir_int(&vars[0], 42);

    initialiser_variable(&vars[1], "prix", VAR_DOUBLE);
    definir_double(&vars[1], 19.99);

    initialiser_variable(&vars[2], "nom", VAR_STRING);
    definir_string(&vars[2], "Alice");

    initialiser_variable(&vars[3], "actif", VAR_BOOL);
    definir_bool(&vars[3], 1);

    for (int i = 0; i < 4; i++) {
        afficher_variable(&vars[i]);
    }

    return 0;
}
