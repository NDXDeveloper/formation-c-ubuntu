/* ============================================================================
   Section 30.1.4 : Execution des tests unitaires
   Description : Test de la calculatrice avec Check (CI automatise)
   Fichier source : 01.4-execution-tests.md
   ============================================================================ */
#include <check.h>
#include "calculator.h"

START_TEST(test_add) {
    ck_assert_int_eq(add(2, 3), 5);
    ck_assert_int_eq(add(-1, 1), 0);
}
END_TEST

START_TEST(test_multiply) {
    ck_assert_int_eq(multiply(2, 3), 6);
    ck_assert_int_eq(multiply(0, 100), 0);
}
END_TEST

Suite *calculator_suite(void) {
    Suite *s = suite_create("Calculator");
    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_add);
    tcase_add_test(tc_core, test_multiply);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void) {
    Suite *s = calculator_suite();
    SRunner *sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? 0 : 1;
}
