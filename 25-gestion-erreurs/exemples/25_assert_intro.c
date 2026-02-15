/* ============================================================================
   Section 25.4 : Assertions
   Description : Introduction a assert() - crash intentionnel pedagogique
   Fichier source : 04-assertions.md
   ============================================================================ */

#include <stdio.h>
#include <assert.h>

int main(void) {
    int age = 25;

    /* Cette assertion passe (25 >= 0 est vrai) */
    assert(age >= 0);
    printf("Age valide : %d\n", age);

    age = -5;

    /* Cette assertion echoue (-5 >= 0 est faux) */
    assert(age >= 0);  /* Le programme s'arrete ICI */

    printf("Cette ligne ne sera jamais executee\n");

    return 0;
}
