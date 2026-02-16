/* ============================================================================
   Section 29.7 : Test-Driven Development (TDD)
   Description : Buffer circulaire developpe par TDD (session complete)
   Fichier source : 07-tdd.md
   ============================================================================ */
#include "unity.h"
#include <stdlib.h>
#include <stdbool.h>

/* --- Buffer circulaire --- */
typedef struct {
    int *data;
    size_t capacity;
    size_t size;
    size_t head;
    size_t tail;
} CircularBuffer;

CircularBuffer *cb_create(size_t capacity) {
    CircularBuffer *buf = malloc(sizeof(CircularBuffer));
    if (!buf) return NULL;

    buf->data = malloc(capacity * sizeof(int));
    if (!buf->data) {
        free(buf);
        return NULL;
    }

    buf->capacity = capacity;
    buf->size = 0;
    buf->head = 0;
    buf->tail = 0;
    return buf;
}

size_t cb_size(CircularBuffer *buf) {
    return buf->size;
}

bool cb_is_full(CircularBuffer *buf) {
    return buf->size == buf->capacity;
}

void cb_push(CircularBuffer *buf, int value) {
    buf->data[buf->tail] = value;
    buf->tail = (buf->tail + 1) % buf->capacity;

    if (buf->size < buf->capacity) {
        buf->size++;
    } else {
        /* Buffer plein : avancer head pour ecraser */
        buf->head = (buf->head + 1) % buf->capacity;
    }
}

int cb_pop(CircularBuffer *buf) {
    int value = buf->data[buf->head];
    buf->head = (buf->head + 1) % buf->capacity;
    buf->size--;
    return value;
}

void cb_destroy(CircularBuffer *buf) {
    if (buf) {
        free(buf->data);
        free(buf);
    }
}

/* --- Tests TDD --- */
void setUp(void) {}
void tearDown(void) {}

/* Test 1 : Creer le buffer */
void test_circular_buffer_create(void) {
    CircularBuffer *buf = cb_create(5);
    TEST_ASSERT_NOT_NULL(buf);
    TEST_ASSERT_EQUAL_INT(0, (int)cb_size(buf));
    cb_destroy(buf);
}

/* Test 2 : Ajouter un element */
void test_circular_buffer_push(void) {
    CircularBuffer *buf = cb_create(5);
    cb_push(buf, 42);
    TEST_ASSERT_EQUAL_INT(1, (int)cb_size(buf));
    cb_destroy(buf);
}

/* Test 3 : Lire un element */
void test_circular_buffer_pop(void) {
    CircularBuffer *buf = cb_create(5);
    cb_push(buf, 42);
    int value = cb_pop(buf);
    TEST_ASSERT_EQUAL_INT(42, value);
    TEST_ASSERT_EQUAL_INT(0, (int)cb_size(buf));
    cb_destroy(buf);
}

/* Test 4 : Buffer plein */
void test_circular_buffer_full(void) {
    CircularBuffer *buf = cb_create(3);
    cb_push(buf, 1);
    cb_push(buf, 2);
    cb_push(buf, 3);
    TEST_ASSERT_TRUE(cb_is_full(buf));
    cb_destroy(buf);
}

/* Test 5 : Debordement (overwrite) */
void test_circular_buffer_overwrite(void) {
    CircularBuffer *buf = cb_create(3);
    cb_push(buf, 1);
    cb_push(buf, 2);
    cb_push(buf, 3);
    cb_push(buf, 4); /* Ecrase le 1 */

    TEST_ASSERT_EQUAL_INT(2, cb_pop(buf));
    TEST_ASSERT_EQUAL_INT(3, cb_pop(buf));
    TEST_ASSERT_EQUAL_INT(4, cb_pop(buf));

    cb_destroy(buf);
}

/* Test supplementaire : FIFO */
void test_circular_buffer_fifo_order(void) {
    CircularBuffer *buf = cb_create(5);
    cb_push(buf, 10);
    cb_push(buf, 20);
    cb_push(buf, 30);

    TEST_ASSERT_EQUAL_INT(10, cb_pop(buf));
    TEST_ASSERT_EQUAL_INT(20, cb_pop(buf));
    TEST_ASSERT_EQUAL_INT(30, cb_pop(buf));

    cb_destroy(buf);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_circular_buffer_create);
    RUN_TEST(test_circular_buffer_push);
    RUN_TEST(test_circular_buffer_pop);
    RUN_TEST(test_circular_buffer_full);
    RUN_TEST(test_circular_buffer_overwrite);
    RUN_TEST(test_circular_buffer_fifo_order);

    return UNITY_END();
}
