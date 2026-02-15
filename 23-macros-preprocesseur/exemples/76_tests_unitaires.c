/* ============================================================================
   Section 23.7 : X-Macros
   Description : Tests unitaires automatiques avec X-Macros
   Fichier source : 07-x-macros.md
   ============================================================================ */

#include <stdio.h>

// Liste des tests
#define TEST_LIST \
    X(test_addition) \
    X(test_soustraction) \
    X(test_multiplication) \
    X(test_division)

// Génération des prototypes
#define X(name) void name(void);
TEST_LIST
#undef X

// Génération du tableau de tests
#define X(name) {#name, name},
struct TestCase {
    const char* name;
    void (*func)(void);
};

struct TestCase tests[] = {
    TEST_LIST
};
#undef X

// Compteur de tests
static int tests_passed = 0;
static int tests_failed = 0;

// Macro d'assertion simple
#define ASSERT_EQUAL(actual, expected) \
    do { \
        if ((actual) == (expected)) { \
            tests_passed++; \
        } else { \
            printf("  ECHEC: %d != %d\n", (actual), (expected)); \
            tests_failed++; \
        } \
    } while(0)

// Implémentation des tests
void test_addition(void) {
    ASSERT_EQUAL(2 + 2, 4);
    ASSERT_EQUAL(10 + 5, 15);
}

void test_soustraction(void) {
    ASSERT_EQUAL(5 - 3, 2);
    ASSERT_EQUAL(10 - 10, 0);
}

void test_multiplication(void) {
    ASSERT_EQUAL(3 * 4, 12);
    ASSERT_EQUAL(5 * 0, 0);
}

void test_division(void) {
    ASSERT_EQUAL(10 / 2, 5);
    ASSERT_EQUAL(9 / 3, 3);
}

// Exécution de tous les tests
void run_all_tests(void) {
    size_t num_tests = sizeof(tests) / sizeof(tests[0]);

    printf("Execution de %zu tests...\n\n", num_tests);

    for (size_t i = 0; i < num_tests; i++) {
        printf("> %s\n", tests[i].name);
        tests[i].func();
    }

    printf("\n");
    printf("============================\n");
    printf("Resultats:\n");
    printf("  Reussis: %d\n", tests_passed);
    printf("  Echoues: %d\n", tests_failed);
    printf("============================\n");
}

int main(void) {
    run_all_tests();
    return tests_failed > 0 ? 1 : 0;
}
