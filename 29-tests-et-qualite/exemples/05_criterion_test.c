/* ============================================================================
   Section 29.2 : Tests unitaires - Frameworks
   Description : Test avec le framework Criterion (moderne, paramétrique)
   Fichier source : 02-frameworks-tests-unitaires.md
   ============================================================================ */
#include <criterion/criterion.h>
#include <criterion/parameterized.h>
#include "math_utils.h"

Test(math, add) {
    cr_assert_eq(add(2, 3), 5);
}

Test(math, multiply) {
    cr_assert_eq(multiply(4, 5), 20);
}

/* Struct nommée pour le test paramétrique */
struct add_params {
    int a;
    int b;
    int expected;
};

/* Test paramétrique */
ParameterizedTestParameters(math, parametric_add) {
    static struct add_params params[] = {
        {1, 2, 3},
        {5, 5, 10},
        {-1, 1, 0},
    };

    return cr_make_param_array(struct add_params, params,
                               sizeof(params) / sizeof(params[0]));
}

ParameterizedTest(struct add_params *param, math, parametric_add) {
    cr_assert_eq(add(param->a, param->b), param->expected);
}
