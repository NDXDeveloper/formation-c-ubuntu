/*
 * Section  : 32.5.2 - Ajout de tests
 * Description : Tests unitaires avec le framework Unity
 * Fichier source : 05.2-ajout-tests.md
 */

#include "unity.h"
#include "my_module.h"

/* Setup : appele avant chaque test */
void setUp(void) {
    /* Initialisation */
}

/* Teardown : appele apres chaque test */
void tearDown(void) {
    /* Nettoyage */
}

/* Test add avec nombres positifs */
void test_add_function_positive_numbers(void) {
    TEST_ASSERT_EQUAL_INT(5, add(2, 3));
}

/* Test add avec zero */
void test_add_function_zero(void) {
    TEST_ASSERT_EQUAL_INT(7, add(7, 0));
}

/* Test add avec nombres negatifs */
void test_add_function_negative(void) {
    TEST_ASSERT_EQUAL_INT(-1, add(2, -3));
}

/* Test multiply */
void test_multiply_basic(void) {
    TEST_ASSERT_EQUAL_INT(6, multiply(2, 3));
}

/* Test multiply par zero */
void test_multiply_by_zero(void) {
    TEST_ASSERT_EQUAL_INT(0, multiply(42, 0));
}

/* Main */
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_add_function_positive_numbers);
    RUN_TEST(test_add_function_zero);
    RUN_TEST(test_add_function_negative);
    RUN_TEST(test_multiply_basic);
    RUN_TEST(test_multiply_by_zero);
    return UNITY_END();
}
