/* ============================================================================
   Section 30.1.4 : Execution des tests unitaires
   Description : Test de la calculatrice avec Criterion (simple + parametrique)
   Fichier source : 01.4-execution-tests.md
   ============================================================================ */
#include <criterion/criterion.h>
#include <criterion/parameterized.h>
#include "calculator.h"

Test(calculator, test_add) {
    cr_assert_eq(add(2, 3), 5);
    cr_assert_eq(add(-1, 1), 0);
}

Test(calculator, test_multiply) {
    cr_assert_eq(multiply(2, 3), 6);
    cr_assert_eq(multiply(0, 100), 0);
}

/* Test parametrique */
struct test_data {
    int a;
    int b;
    int expected;
};

ParameterizedTestParameters(calculator, test_add_params) {
    static struct test_data params[] = {
        {2, 3, 5},
        {-1, 1, 0},
        {0, 0, 0},
        {100, -50, 50},
    };
    size_t nb_params = sizeof(params) / sizeof(struct test_data);
    return cr_make_param_array(struct test_data, params, nb_params);
}

ParameterizedTest(struct test_data *data, calculator, test_add_params) {
    cr_assert_eq(add(data->a, data->b), data->expected);
}
