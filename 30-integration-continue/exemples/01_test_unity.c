/* ============================================================================
   Section 30.1.4 : Execution des tests unitaires
   Description : Test de la calculatrice avec Unity (CI automatise)
   Fichier source : 01.4-execution-tests.md
   ============================================================================ */
#include "unity.h"
#include "calculator.h"

void setUp(void) {}
void tearDown(void) {}

void test_add_positive_numbers(void) {
    TEST_ASSERT_EQUAL_INT(5, add(2, 3));
    TEST_ASSERT_EQUAL_INT(100, add(50, 50));
}

void test_add_with_zero(void) {
    TEST_ASSERT_EQUAL_INT(42, add(42, 0));
    TEST_ASSERT_EQUAL_INT(42, add(0, 42));
}

void test_add_negative_numbers(void) {
    TEST_ASSERT_EQUAL_INT(-5, add(-2, -3));
    TEST_ASSERT_EQUAL_INT(0, add(-10, 10));
}

void test_subtract(void) {
    TEST_ASSERT_EQUAL_INT(2, subtract(5, 3));
    TEST_ASSERT_EQUAL_INT(-5, subtract(5, 10));
}

void test_multiply(void) {
    TEST_ASSERT_EQUAL_INT(6, multiply(2, 3));
    TEST_ASSERT_EQUAL_INT(0, multiply(5, 0));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_add_positive_numbers);
    RUN_TEST(test_add_with_zero);
    RUN_TEST(test_add_negative_numbers);
    RUN_TEST(test_subtract);
    RUN_TEST(test_multiply);

    return UNITY_END();
}
