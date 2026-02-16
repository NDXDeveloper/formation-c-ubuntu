/* ============================================================================
   Section 29.7 : Test-Driven Development (TDD)
   Description : Validation d'email developpee par TDD (Red-Green-Refactor)
   Fichier source : 07-tdd.md
   ============================================================================ */
#include "unity.h"
#include <string.h>
#include <stdbool.h>

/* --- Fonction developpee par TDD --- */
bool is_valid_email(const char *email) {
    if (!email || strlen(email) == 0) {
        return false;
    }

    /* Trouver le @ */
    const char *at_sign = strchr(email, '@');
    if (!at_sign || at_sign == email) {
        return false; /* Pas de @, ou @ au debut */
    }

    /* Verifier la partie apres le @ */
    const char *domain = at_sign + 1;
    if (*domain == '\0') {
        return false; /* Pas de domaine */
    }

    /* Verifier le . dans le domaine */
    const char *dot = strchr(domain, '.');
    if (!dot || dot == domain || *(dot + 1) == '\0') {
        return false; /* Pas de ., ou . au debut/fin */
    }

    return true;
}

/* --- Tests TDD (chaque test correspond a une iteration) --- */
void setUp(void) {}
void tearDown(void) {}

/* Iteration 1 : email valide simple */
void test_valid_email_simple(void) {
    TEST_ASSERT_TRUE(is_valid_email("user@example.com"));
}

/* Iteration 2 : email sans @ */
void test_email_without_at(void) {
    TEST_ASSERT_FALSE(is_valid_email("userexample.com"));
}

/* Iteration 3 : @ au debut */
void test_email_at_at_start(void) {
    TEST_ASSERT_FALSE(is_valid_email("@example.com"));
}

/* Iteration 4 : @ a la fin */
void test_email_at_at_end(void) {
    TEST_ASSERT_FALSE(is_valid_email("user@"));
}

/* Iteration 5 : pas de . dans le domaine */
void test_email_missing_dot_in_domain(void) {
    TEST_ASSERT_FALSE(is_valid_email("user@examplecom"));
}

/* Tests supplementaires */
void test_email_null(void) {
    TEST_ASSERT_FALSE(is_valid_email(NULL));
}

void test_email_empty(void) {
    TEST_ASSERT_FALSE(is_valid_email(""));
}

void test_email_dot_at_domain_start(void) {
    TEST_ASSERT_FALSE(is_valid_email("user@.com"));
}

void test_email_dot_at_domain_end(void) {
    TEST_ASSERT_FALSE(is_valid_email("user@example."));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_valid_email_simple);
    RUN_TEST(test_email_without_at);
    RUN_TEST(test_email_at_at_start);
    RUN_TEST(test_email_at_at_end);
    RUN_TEST(test_email_missing_dot_in_domain);
    RUN_TEST(test_email_null);
    RUN_TEST(test_email_empty);
    RUN_TEST(test_email_dot_at_domain_start);
    RUN_TEST(test_email_dot_at_domain_end);

    return UNITY_END();
}
