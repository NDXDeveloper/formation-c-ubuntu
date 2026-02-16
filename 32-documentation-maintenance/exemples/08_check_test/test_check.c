/*
 * Section  : 32.5.2 - Ajout de tests
 * Description : Tests unitaires avec le framework Check (forking)
 * Fichier source : 05.2-ajout-tests.md
 */

#include <check.h>
#include "my_module.h"

START_TEST(test_add) {
    ck_assert_int_eq(5, add(2, 3));
}
END_TEST

START_TEST(test_add_negative) {
    ck_assert_int_eq(-1, add(2, -3));
}
END_TEST

START_TEST(test_add_zero) {
    ck_assert_int_eq(7, add(7, 0));
}
END_TEST

Suite *make_suite(void) {
    Suite *s = suite_create("My Module");
    TCase *tc = tcase_create("Core");

    tcase_add_test(tc, test_add);
    tcase_add_test(tc, test_add_negative);
    tcase_add_test(tc, test_add_zero);
    suite_add_tcase(s, tc);

    return s;
}

int main(void) {
    Suite *s = make_suite();
    SRunner *sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (failed == 0) ? 0 : 1;
}
