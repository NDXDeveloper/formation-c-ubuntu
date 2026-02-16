/* ============================================================================
   Section 29.6 : Mesure de couverture
   Description : Tests de la calculatrice avec couverture 100%
   Fichier source : 06-mesure-couverture.md
   ============================================================================ */
#include "unity.h"
#include "calculator.h"

void setUp(void) {}
void tearDown(void) {}

void test_add(void) {
    TEST_ASSERT_EQUAL_INT(5, add(2, 3));
}

void test_subtract(void) {
    TEST_ASSERT_EQUAL_INT(2, subtract(5, 3));
}

void test_multiply(void) {
    TEST_ASSERT_EQUAL_INT(6, multiply(2, 3));
}

void test_divide(void) {
    TEST_ASSERT_EQUAL_INT(5, divide_safe(10, 2));
}

void test_divide_by_zero(void) {
    TEST_ASSERT_EQUAL_INT(0, divide_safe(10, 0));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_add);
    RUN_TEST(test_subtract);
    RUN_TEST(test_multiply);
    RUN_TEST(test_divide);
    RUN_TEST(test_divide_by_zero);

    return UNITY_END();
}
