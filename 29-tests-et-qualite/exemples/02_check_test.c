/* ============================================================================
   Section 29.2 : Tests unitaires - Frameworks
   Description : Test avec le framework Check (isolation par fork)
   Fichier source : 02-frameworks-tests-unitaires.md
   ============================================================================ */
#include <check.h>
#include "math_utils.h"

/* Test 1 */
START_TEST(test_add_positive) {
    ck_assert_int_eq(add(2, 3), 5);
}
END_TEST

/* Test 2 */
START_TEST(test_multiply_by_zero) {
    ck_assert_int_eq(multiply(100, 0), 0);
}
END_TEST

/* Suite de tests */
Suite *math_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Math");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_add_positive);
    tcase_add_test(tc_core, test_multiply_by_zero);

    suite_add_tcase(s, tc_core);
    return s;
}

/* Runner */
int main(void) {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = math_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? 0 : 1;
}
