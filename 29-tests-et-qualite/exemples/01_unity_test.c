/* ============================================================================
   Section 29.2 : Tests unitaires - Frameworks
   Description : Test avec le framework Unity (ThrowTheSwitch)
   Fichier source : 02-frameworks-tests-unitaires.md
   ============================================================================ */
#include "unity.h"
#include "math_utils.h"

/* Fonction appelée avant chaque test */
void setUp(void) {
    /* Initialisation si nécessaire */
}

/* Fonction appelée après chaque test */
void tearDown(void) {
    /* Nettoyage si nécessaire */
}

/* Test 1 : Addition simple */
void test_add_positive_numbers(void) {
    TEST_ASSERT_EQUAL_INT(5, add(2, 3));
}

/* Test 2 : Addition avec zéro */
void test_add_with_zero(void) {
    TEST_ASSERT_EQUAL_INT(7, add(7, 0));
    TEST_ASSERT_EQUAL_INT(7, add(0, 7));
}

/* Test 3 : Multiplication */
void test_multiply(void) {
    TEST_ASSERT_EQUAL_INT(20, multiply(4, 5));
    TEST_ASSERT_EQUAL_INT(0, multiply(0, 100));
}

/* Point d'entrée du test */
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_add_positive_numbers);
    RUN_TEST(test_add_with_zero);
    RUN_TEST(test_multiply);

    return UNITY_END();
}
