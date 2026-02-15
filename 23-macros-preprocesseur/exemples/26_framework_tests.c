/* ============================================================================
   Section 23.2 : Stringification et concatenation
   Description : Mini-framework de tests combinant # et ## (exemple complet)
   Fichier source : 02-stringification-concatenation.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>

// Compteurs de tests
static int tests_total = 0;
static int tests_reussis = 0;

// Macro de test qui génère automatiquement le nom de fonction
#define TEST(name) \
    void test_##name(void); \
    void run_test_##name(void) { \
        printf("\n> Test: " #name "\n"); \
        tests_total++; \
        test_##name(); \
    } \
    void test_##name(void)

// Macros d'assertion
#define ASSERT_EQUAL(actual, expected) \
    do { \
        if ((actual) == (expected)) { \
            printf("  OK " #actual " == " #expected "\n"); \
            tests_reussis++; \
        } else { \
            printf("  ECHEC " #actual " (%d) != " #expected " (%d)\n", \
                   (actual), (expected)); \
        } \
    } while(0)

#define ASSERT_STRING_EQUAL(actual, expected) \
    do { \
        if (strcmp((actual), (expected)) == 0) { \
            printf("  OK " #actual " == " #expected "\n"); \
            tests_reussis++; \
        } else { \
            printf("  ECHEC " #actual " (\"%s\") != " #expected " (\"%s\")\n", \
                   (actual), (expected)); \
        } \
    } while(0)

// Définition des tests
TEST(addition) {
    int resultat = 2 + 3;
    ASSERT_EQUAL(resultat, 5);
}

TEST(multiplication) {
    int resultat = 4 * 5;
    ASSERT_EQUAL(resultat, 20);
}

TEST(concatenation) {
    char buffer[100];
    strcpy(buffer, "Hello");
    strcat(buffer, " World");
    ASSERT_STRING_EQUAL(buffer, "Hello World");
}

TEST(division) {
    int resultat = 20 / 4;
    ASSERT_EQUAL(resultat, 5);
}

// Fonction principale
int main(void) {
    printf("=== Début des tests ===\n");

    run_test_addition();
    run_test_multiplication();
    run_test_concatenation();
    run_test_division();

    printf("\n=== Résultats ===\n");
    printf("Tests réussis: %d/%d\n", tests_reussis, tests_total);

    return (tests_reussis == tests_total) ? 0 : 1;
}
