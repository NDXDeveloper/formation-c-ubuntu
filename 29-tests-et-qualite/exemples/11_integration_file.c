/* ============================================================================
   Section 29.5 : Tests d'integration
   Description : Test d'integration avec fichier (validation + sauvegarde)
   Fichier source : 05-tests-integration.md
   ============================================================================ */
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* --- Code de production --- */
typedef struct {
    int id;
    char name[50];
} User;

bool validate_user(User *user) {
    if (!user) return false;
    if (strlen(user->name) == 0) return false;
    if (user->id <= 0) return false;
    return true;
}

bool save_user_to_file(User *user, const char *filename) {
    FILE *f = fopen(filename, "a");
    if (!f) return false;

    fprintf(f, "%d,%s\n", user->id, user->name);
    fclose(f);
    return true;
}

bool register_user(User *user, const char *filename) {
    if (!validate_user(user)) {
        return false;
    }
    return save_user_to_file(user, filename);
}

/* --- Tests unitaires --- */
void setUp(void) {}
void tearDown(void) {}

void test_validate_user_with_valid_data(void) {
    User user = {.id = 1, .name = "Alice"};
    TEST_ASSERT_TRUE(validate_user(&user));
}

void test_validate_user_with_empty_name(void) {
    User user = {.id = 1, .name = ""};
    TEST_ASSERT_FALSE(validate_user(&user));
}

void test_validate_user_with_invalid_id(void) {
    User user = {.id = 0, .name = "Alice"};
    TEST_ASSERT_FALSE(validate_user(&user));
}

void test_validate_user_null(void) {
    TEST_ASSERT_FALSE(validate_user(NULL));
}

/* --- Test d'integration --- */
void test_register_user_creates_file_entry(void) {
    User user = {.id = 123, .name = "Bob"};
    const char *test_file = "/tmp/test_users.txt";

    /* Nettoyer avant */
    remove(test_file);

    /* Appeler la fonction qui combine validation + sauvegarde */
    bool result = register_user(&user, test_file);
    TEST_ASSERT_TRUE(result);

    /* Verifier que le fichier a ete cree avec le bon contenu */
    FILE *f = fopen(test_file, "r");
    TEST_ASSERT_NOT_NULL(f);

    char line[100];
    char *got = fgets(line, sizeof(line), f);
    TEST_ASSERT_NOT_NULL(got);
    TEST_ASSERT_EQUAL_STRING("123,Bob\n", line);

    fclose(f);
    remove(test_file);
}

void test_register_user_rejects_invalid(void) {
    User user = {.id = -1, .name = "Alice"};
    const char *test_file = "/tmp/test_users_bad.txt";

    remove(test_file);

    bool result = register_user(&user, test_file);
    TEST_ASSERT_FALSE(result);

    /* Verifier que le fichier n'a PAS ete cree */
    FILE *f = fopen(test_file, "r");
    TEST_ASSERT_NULL(f);
}

void test_register_multiple_users(void) {
    const char *test_file = "/tmp/test_users_multi.txt";
    remove(test_file);

    User u1 = {.id = 1, .name = "Alice"};
    User u2 = {.id = 2, .name = "Bob"};

    TEST_ASSERT_TRUE(register_user(&u1, test_file));
    TEST_ASSERT_TRUE(register_user(&u2, test_file));

    FILE *f = fopen(test_file, "r");
    TEST_ASSERT_NOT_NULL(f);

    char line[100];
    fgets(line, sizeof(line), f);
    TEST_ASSERT_EQUAL_STRING("1,Alice\n", line);
    fgets(line, sizeof(line), f);
    TEST_ASSERT_EQUAL_STRING("2,Bob\n", line);

    fclose(f);
    remove(test_file);
}

int main(void) {
    UNITY_BEGIN();

    /* Tests unitaires */
    RUN_TEST(test_validate_user_with_valid_data);
    RUN_TEST(test_validate_user_with_empty_name);
    RUN_TEST(test_validate_user_with_invalid_id);
    RUN_TEST(test_validate_user_null);

    /* Tests d'integration */
    RUN_TEST(test_register_user_creates_file_entry);
    RUN_TEST(test_register_user_rejects_invalid);
    RUN_TEST(test_register_multiple_users);

    return UNITY_END();
}
