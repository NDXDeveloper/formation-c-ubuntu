/* ============================================================================
   Section 23.2 : Stringification et concatenation
   Description : Combinaison de # et ## pour macro de tests unitaires
   Fichier source : 02-stringification-concatenation.md
   ============================================================================ */

#include <stdio.h>

#define TEST(name) \
    void test_##name(void); \
    void test_##name(void)

#define ASSERT_EQUAL(actual, expected) \
    do { \
        if ((actual) != (expected)) { \
            printf("ÉCHEC: " #actual " (%d) != " #expected " (%d)\n", \
                   (actual), (expected)); \
        } else { \
            printf("OK: " #actual " == " #expected "\n"); \
        } \
    } while(0)

// Définition d'un test
TEST(addition) {
    int resultat = 2 + 3;
    ASSERT_EQUAL(resultat, 5);
}

TEST(multiplication) {
    int resultat = 4 * 5;
    ASSERT_EQUAL(resultat, 20);
}

int main(void) {
    test_addition();
    test_multiplication();

    return 0;
}
