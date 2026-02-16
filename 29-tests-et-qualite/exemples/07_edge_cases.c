/* ============================================================================
   Section 29.3 : Écriture de tests et assertions
   Description : Tests de cas limites (NULL, zéro, débordements, tableaux vides)
   Fichier source : 03-ecriture-tests.md
   ============================================================================ */
#include "unity.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

void setUp(void) {}
void tearDown(void) {}

/* --- Fonction temperature + tests --- */
typedef enum {
    TEMP_OK,
    TEMP_TOO_LOW,
    TEMP_TOO_HIGH
} TempStatus;

TempStatus check_temperature(int temp) {
    if (temp < 18) return TEMP_TOO_LOW;
    if (temp > 25) return TEMP_TOO_HIGH;
    return TEMP_OK;
}

void test_temperature_ranges(void) {
    TEST_ASSERT_EQUAL_INT(TEMP_TOO_LOW, check_temperature(17));
    TEST_ASSERT_EQUAL_INT(TEMP_OK, check_temperature(18));
    TEST_ASSERT_EQUAL_INT(TEMP_OK, check_temperature(25));
    TEST_ASSERT_EQUAL_INT(TEMP_TOO_HIGH, check_temperature(26));
    TEST_ASSERT_EQUAL_INT(TEMP_OK, check_temperature(22));
}

/* --- safe_string_copy + tests --- */
char *safe_string_copy(const char *src) {
    if (src == NULL) return NULL;

    char *dest = malloc(strlen(src) + 1);
    if (dest == NULL) return NULL;

    strcpy(dest, src);
    return dest;
}

void test_string_copy_edge_cases(void) {
    char *result = safe_string_copy("Hello");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("Hello", result);
    free(result);

    result = safe_string_copy(NULL);
    TEST_ASSERT_NULL(result);

    result = safe_string_copy("");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("", result);
    free(result);
}

/* --- divide + tests --- */
int divide(int a, int b) {
    if (b == 0) return 0;
    return a / b;
}

void test_divide_edge_cases(void) {
    TEST_ASSERT_EQUAL_INT(5, divide(10, 2));
    TEST_ASSERT_EQUAL_INT(0, divide(10, 0));
    TEST_ASSERT_EQUAL_INT(0, divide(0, 5));
}

/* --- sum_array + tests --- */
int sum_array(const int *arr, size_t len) {
    if (arr == NULL || len == 0) return 0;

    int sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += arr[i];
    }
    return sum;
}

void test_sum_array_edge_cases(void) {
    int arr[] = {1, 2, 3};
    TEST_ASSERT_EQUAL_INT(6, sum_array(arr, 3));
    TEST_ASSERT_EQUAL_INT(0, sum_array(arr, 0));
    TEST_ASSERT_EQUAL_INT(0, sum_array(NULL, 5));

    int single[] = {42};
    TEST_ASSERT_EQUAL_INT(42, sum_array(single, 1));
}

/* --- safe_add (débordement) + tests --- */
int safe_add(int a, int b) {
    if (a > 0 && b > INT_MAX - a) return INT_MAX;
    if (a < 0 && b < INT_MIN - a) return INT_MIN;
    return a + b;
}

void test_integer_overflow(void) {
    TEST_ASSERT_EQUAL_INT(5, safe_add(2, 3));
    TEST_ASSERT_EQUAL_INT(INT_MAX, safe_add(INT_MAX, 1));
    TEST_ASSERT_EQUAL_INT(INT_MIN, safe_add(INT_MIN, -1));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_temperature_ranges);
    RUN_TEST(test_string_copy_edge_cases);
    RUN_TEST(test_divide_edge_cases);
    RUN_TEST(test_sum_array_edge_cases);
    RUN_TEST(test_integer_overflow);

    return UNITY_END();
}
