/* ============================================================================
   Section 29.2 : Tests unitaires - Frameworks
   Description : Test avec le framework cmocka (orienté mocking)
   Fichier source : 02-frameworks-tests-unitaires.md
   ============================================================================ */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "math_utils.h"

/* Test simple */
static void test_add(void **state) {
    (void) state; /* Unused */
    assert_int_equal(add(2, 3), 5);
}

static void test_multiply(void **state) {
    (void) state;
    assert_int_equal(multiply(4, 5), 20);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_add),
        cmocka_unit_test(test_multiply),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
