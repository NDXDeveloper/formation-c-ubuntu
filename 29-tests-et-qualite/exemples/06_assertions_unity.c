/* ============================================================================
   Section 29.3 : Écriture de tests et assertions
   Description : Démonstration des assertions Unity (entiers, flottants,
                 chaînes, tableaux, mémoire, pointeurs, booléens)
   Fichier source : 03-ecriture-tests.md
   ============================================================================ */
#include "unity.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* --- Assertions d'entiers --- */
void test_integer_assertions(void) {
    int age = 25;
    TEST_ASSERT_EQUAL_INT(25, age);
    TEST_ASSERT_EQUAL_INT_MESSAGE(25, age, "L'age devrait etre 25");

    unsigned int count = 10;
    TEST_ASSERT_EQUAL_UINT(10, count);

    int8_t small = 5;
    TEST_ASSERT_EQUAL_INT8(5, small);

    int64_t large = 1000000000LL;
    TEST_ASSERT_EQUAL_INT64(1000000000LL, large);
}

/* --- Assertions de flottants --- */
void test_float_assertions(void) {
    float pi = 3.14159f;
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 3.14159f, pi);

    double e = 2.71828;
    TEST_ASSERT_DOUBLE_WITHIN(0.00001, 2.71828, e);

    float result = 0.1f + 0.2f;
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.3f, result);
}

/* --- Assertions de chaînes --- */
void test_string_assertions(void) {
    const char *name = "Alice";
    TEST_ASSERT_EQUAL_STRING("Alice", name);

    const char *empty = "";
    TEST_ASSERT_EQUAL_STRING("", empty);
}

void test_null_strings(void) {
    const char *str = NULL;
    TEST_ASSERT_NULL(str);
}

/* --- Assertions de tableaux --- */
void test_array_assertions(void) {
    int expected[] = {1, 2, 3, 4, 5};
    int actual[] = {1, 2, 3, 4, 5};
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 5);

    float expected_f[] = {1.0f, 2.0f, 3.0f};
    float actual_f[] = {1.0f, 2.0f, 3.0f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_f, actual_f, 3);

    const char *expected_str[] = {"foo", "bar", "baz"};
    const char *actual_str[] = {"foo", "bar", "baz"};
    TEST_ASSERT_EQUAL_STRING_ARRAY(expected_str, actual_str, 3);
}

/* --- Assertions de mémoire --- */
void test_memory_assertions(void) {
    typedef struct {
        int x;
        int y;
    } Point;

    Point p1 = {10, 20};
    Point p2 = {10, 20};
    TEST_ASSERT_EQUAL_MEMORY(&p1, &p2, sizeof(Point));

    uint8_t buffer1[] = {0x01, 0x02, 0x03, 0xFF};
    uint8_t buffer2[] = {0x01, 0x02, 0x03, 0xFF};
    TEST_ASSERT_EQUAL_MEMORY(buffer1, buffer2, 4);
}

/* --- Assertions de pointeurs --- */
void test_pointer_assertions(void) {
    int value = 42;
    int *ptr = &value;
    int *null_ptr = NULL;

    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT_NULL(null_ptr);

    int *ptr2 = &value;
    TEST_ASSERT_EQUAL_PTR(ptr, ptr2);
    TEST_ASSERT_EQUAL_INT(42, *ptr);
}

void test_malloc_success(void) {
    int *array = malloc(10 * sizeof(int));
    TEST_ASSERT_NOT_NULL(array);
    array[0] = 42;
    TEST_ASSERT_EQUAL_INT(42, array[0]);
    free(array);
}

/* --- Assertions booléennes --- */
void test_boolean_assertions(void) {
    int age = 18;
    TEST_ASSERT_TRUE(age >= 18);
    TEST_ASSERT_FALSE(age < 0);
}

/* --- Assertions de comparaison --- */
void test_comparison_assertions(void) {
    int temperature = 25;
    TEST_ASSERT_GREATER_THAN(20, temperature);
    TEST_ASSERT_LESS_THAN(30, temperature);
    TEST_ASSERT_GREATER_OR_EQUAL(25, temperature);
    TEST_ASSERT_LESS_OR_EQUAL(25, temperature);
    TEST_ASSERT_INT_WITHIN(5, 25, temperature);
}

/* --- Limites de types --- */
void test_type_limits(void) {
    int8_t max8 = INT8_MAX;
    int8_t min8 = INT8_MIN;
    TEST_ASSERT_EQUAL_INT8(127, max8);
    TEST_ASSERT_EQUAL_INT8(-128, min8);

    uint32_t max32 = UINT32_MAX;
    TEST_ASSERT_EQUAL_UINT32(4294967295U, max32);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_integer_assertions);
    RUN_TEST(test_float_assertions);
    RUN_TEST(test_string_assertions);
    RUN_TEST(test_null_strings);
    RUN_TEST(test_array_assertions);
    RUN_TEST(test_memory_assertions);
    RUN_TEST(test_pointer_assertions);
    RUN_TEST(test_malloc_success);
    RUN_TEST(test_boolean_assertions);
    RUN_TEST(test_comparison_assertions);
    RUN_TEST(test_type_limits);

    return UNITY_END();
}
