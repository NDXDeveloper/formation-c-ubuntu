/* ============================================================================
   Section 29.2 : Tests unitaires - Frameworks
   Description : Test avec le framework CUnit (académique)
   Fichier source : 02-frameworks-tests-unitaires.md
   ============================================================================ */
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "math_utils.h"

void test_add(void) {
    CU_ASSERT_EQUAL(add(2, 3), 5);
}

void test_multiply(void) {
    CU_ASSERT_EQUAL(multiply(4, 5), 20);
}

int main(void) {
    CU_pSuite suite = NULL;

    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    suite = CU_add_suite("Math Suite", 0, 0);
    if (NULL == suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_add_test(suite, "test_add", test_add);
    CU_add_test(suite, "test_multiply", test_multiply);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
