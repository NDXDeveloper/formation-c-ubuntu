/* ============================================================================
   Section 29.3 : Écriture de tests et assertions
   Description : Tests de structures (Employee, liste chaînée, codes d'erreur)
   Fichier source : 03-ecriture-tests.md
   ============================================================================ */
#include "unity.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

void setUp(void) {}
void tearDown(void) {}

/* --- Employee + tests --- */
typedef struct {
    char name[50];
    int age;
    float salary;
} Employee;

Employee create_employee(const char *name, int age, float salary) {
    Employee e;
    strncpy(e.name, name, sizeof(e.name) - 1);
    e.name[sizeof(e.name) - 1] = '\0';
    e.age = age;
    e.salary = salary;
    return e;
}

void test_create_employee(void) {
    const char *expected_name = "Alice";
    int expected_age = 30;
    float expected_salary = 50000.0f;

    Employee emp = create_employee(expected_name, expected_age, expected_salary);

    TEST_ASSERT_EQUAL_STRING(expected_name, emp.name);
    TEST_ASSERT_EQUAL_INT(expected_age, emp.age);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, expected_salary, emp.salary);
}

/* --- Liste chaînée + tests --- */
typedef struct Node {
    int data;
    struct Node *next;
} Node;

Node *create_node(int data) {
    Node *node = malloc(sizeof(Node));
    if (node) {
        node->data = data;
        node->next = NULL;
    }
    return node;
}

void test_create_node(void) {
    Node *node = create_node(42);

    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_EQUAL_INT(42, node->data);
    TEST_ASSERT_NULL(node->next);

    free(node);
}

void test_linked_list_insertion(void) {
    Node *head = create_node(1);
    head->next = create_node(2);
    head->next->next = create_node(3);

    TEST_ASSERT_NOT_NULL(head);
    TEST_ASSERT_EQUAL_INT(1, head->data);
    TEST_ASSERT_NOT_NULL(head->next);
    TEST_ASSERT_EQUAL_INT(2, head->next->data);
    TEST_ASSERT_NOT_NULL(head->next->next);
    TEST_ASSERT_EQUAL_INT(3, head->next->next->data);
    TEST_ASSERT_NULL(head->next->next->next);

    free(head->next->next);
    free(head->next);
    free(head);
}

/* --- Codes d'erreur + tests --- */
typedef enum {
    SUCCESS = 0,
    ERROR_NULL_POINTER = -1,
    ERROR_OUT_OF_RANGE = -2,
    ERROR_ALLOCATION_FAILED = -3
} ErrorCode;

ErrorCode set_array_value(int *arr, size_t len, size_t index, int value) {
    if (arr == NULL) return ERROR_NULL_POINTER;
    if (index >= len) return ERROR_OUT_OF_RANGE;

    arr[index] = value;
    return SUCCESS;
}

void test_set_array_value_success(void) {
    int arr[5] = {0};
    ErrorCode result = set_array_value(arr, 5, 2, 42);

    TEST_ASSERT_EQUAL_INT(SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(42, arr[2]);
}

void test_set_array_value_errors(void) {
    int arr[5] = {0};

    TEST_ASSERT_EQUAL_INT(ERROR_NULL_POINTER, set_array_value(NULL, 5, 0, 42));
    TEST_ASSERT_EQUAL_INT(ERROR_OUT_OF_RANGE, set_array_value(arr, 5, 10, 42));

    for (int i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL_INT(0, arr[i]);
    }
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_create_employee);
    RUN_TEST(test_create_node);
    RUN_TEST(test_linked_list_insertion);
    RUN_TEST(test_set_array_value_success);
    RUN_TEST(test_set_array_value_errors);

    return UNITY_END();
}
