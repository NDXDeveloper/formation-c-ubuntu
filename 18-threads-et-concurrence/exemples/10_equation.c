/* ============================================================================
   Section 18.3 : Passage de parametres
   Description : Resolution d'equation du second degre dans un thread
   Fichier source : 03-passage-parametres.md
   ============================================================================ */

#include <stdio.h>
#include <pthread.h>
#include <math.h>

typedef struct {
    double a;
    double b;
    double c;
    double delta;
    double x1;
    double x2;
    int solutions;
} EquationData;

void *resoudre_equation(void *arg) {
    EquationData *eq = (EquationData *)arg;

    eq->delta = eq->b * eq->b - 4 * eq->a * eq->c;

    if (eq->delta < 0) {
        eq->solutions = 0;
    } else if (eq->delta == 0) {
        eq->solutions = 1;
        eq->x1 = -eq->b / (2 * eq->a);
    } else {
        eq->solutions = 2;
        eq->x1 = (-eq->b - sqrt(eq->delta)) / (2 * eq->a);
        eq->x2 = (-eq->b + sqrt(eq->delta)) / (2 * eq->a);
    }

    return NULL;
}

int main(void) {
    pthread_t tid;
    EquationData equation = {
        .a = 1,
        .b = -5,
        .c = 6,
        .delta = 0,
        .x1 = 0,
        .x2 = 0,
        .solutions = 0
    };

    printf("Resolution de %.0fx^2 + %.0fx + %.0f = 0\n",
           equation.a, equation.b, equation.c);

    pthread_create(&tid, NULL, resoudre_equation, &equation);
    pthread_join(tid, NULL);

    printf("Delta = %.2f\n", equation.delta);
    if (equation.solutions == 0) {
        printf("Pas de solution reelle\n");
    } else if (equation.solutions == 1) {
        printf("Une solution : x = %.2f\n", equation.x1);
    } else {
        printf("Deux solutions : x1 = %.2f, x2 = %.2f\n",
               equation.x1, equation.x2);
    }

    return 0;
}
