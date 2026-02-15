/* ============================================================================
   Section 25.4 : Assertions
   Description : Assertions et tests unitaires complementaires
   Fichier source : 04-assertions.md
   ============================================================================ */

#include <stdio.h>
#include <assert.h>

/* Fonction a tester */
int ajouter(int a, int b) {
    /* Assertion pour detecter les bugs de debordement */
    assert(a >= 0 && b >= 0);  /* Suppose qu'on ne travaille qu'avec des positifs */

    int resultat = a + b;

    /* Postcondition */
    assert(resultat >= a && resultat >= b);

    return resultat;
}

/* Tests unitaires */
void test_ajouter(void) {
    /* Test 1 */
    assert(ajouter(2, 3) == 5);
    printf("Test 1 passe\n");

    /* Test 2 */
    assert(ajouter(0, 0) == 0);
    printf("Test 2 passe\n");

    /* Test 3 */
    assert(ajouter(100, 200) == 300);
    printf("Test 3 passe\n");
}

int main(void) {
    printf("=== Execution des tests ===\n");
    test_ajouter();
    printf("=== Tous les tests sont passes ===\n");

    return 0;
}
