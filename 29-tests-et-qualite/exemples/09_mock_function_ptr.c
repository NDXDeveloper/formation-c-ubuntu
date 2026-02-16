/* ============================================================================
   Section 29.4 : Mocking et stubbing
   Description : Mocking via pointeurs de fonctions (logger stub et mock)
   Fichier source : 04-mocking-stubbing.md
   ============================================================================ */
#include "unity.h"
#include <string.h>

/* --- Logger avec pointeur de fonction --- */
typedef void (*LogFunction)(const char *message);

/* Implémentation production (stub : ne fait rien en test) */
void log_to_file(const char *message) {
    (void)message; /* En production : écriture fichier */
}

/* Pointeur global remplaçable */
LogFunction current_logger = log_to_file;

void log_message(const char *message) {
    if (current_logger) {
        current_logger(message);
    }
}

/* --- Logique métier utilisant le logger --- */
static int order_processed_id = 0;

void process_order(int order_id) {
    log_message("Processing order");
    order_processed_id = order_id;
    log_message("Order completed");
}

/* --- Stub : silencieux --- */
void log_stub(const char *message) {
    (void)message;
}

/* --- Mock : enregistre les appels --- */
static int log_call_count = 0;
static char logged_messages[10][256];

void log_mock(const char *message) {
    if (log_call_count < 10) {
        strncpy(logged_messages[log_call_count], message, 255);
        logged_messages[log_call_count][255] = '\0';
        log_call_count++;
    }
}

/* --- Tests --- */
void setUp(void) {
    log_call_count = 0;
    order_processed_id = 0;
    memset(logged_messages, 0, sizeof(logged_messages));
}

void tearDown(void) {
    current_logger = log_to_file;
}

void test_process_order_with_stub(void) {
    current_logger = log_stub;

    process_order(12345);

    TEST_ASSERT_EQUAL_INT(12345, order_processed_id);
}

void test_process_order_logs_correctly(void) {
    current_logger = log_mock;

    process_order(42);

    TEST_ASSERT_EQUAL_INT(2, log_call_count);
    TEST_ASSERT_EQUAL_STRING("Processing order", logged_messages[0]);
    TEST_ASSERT_EQUAL_STRING("Order completed", logged_messages[1]);
}

void test_log_message_with_null_logger(void) {
    current_logger = NULL;

    process_order(99); /* Ne doit pas crasher */

    TEST_ASSERT_EQUAL_INT(99, order_processed_id);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_process_order_with_stub);
    RUN_TEST(test_process_order_logs_correctly);
    RUN_TEST(test_log_message_with_null_logger);

    return UNITY_END();
}
