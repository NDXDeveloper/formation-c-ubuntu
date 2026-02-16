/* ============================================================================
   Section 29.4 : Mocking et stubbing
   Description : Tests du service de notification avec stubs et mocks
   Fichier source : 04-mocking-stubbing.md
   ============================================================================ */
#include "unity.h"
#include "notification_service.h"
#include <string.h>

/* --- Stubs et mocks --- */
static int sms_sent_count = 0;
static int email_sent_count = 0;
static char logged_messages[10][256];
static int log_count = 0;

bool stub_send_sms_success(const char *phone, const char *message) {
    (void)phone; (void)message;
    sms_sent_count++;
    return true;
}

bool stub_send_sms_failure(const char *phone, const char *message) {
    (void)phone; (void)message;
    sms_sent_count++;
    return false;
}

bool stub_send_email_success(const char *email, const char *subject,
                             const char *body) {
    (void)email; (void)subject; (void)body;
    email_sent_count++;
    return true;
}

bool stub_send_email_failure(const char *email, const char *subject,
                             const char *body) {
    (void)email; (void)subject; (void)body;
    email_sent_count++;
    return false;
}

void mock_log(const char *message) {
    if (log_count < 10) {
        strncpy(logged_messages[log_count], message, 255);
        logged_messages[log_count][255] = '\0';
        log_count++;
    }
}

void setUp(void) {
    sms_sent_count = 0;
    email_sent_count = 0;
    log_count = 0;
    memset(logged_messages, 0, sizeof(logged_messages));
}

void tearDown(void) {}

/* --- Tests --- */
void test_notify_user_sends_sms_when_available(void) {
    NotificationDeps deps = {
        .send_sms = stub_send_sms_success,
        .send_email = stub_send_email_success,
        .log = mock_log
    };

    NotificationService *service = notification_service_create(deps);

    bool result = notify_user(service, "123", "Hello");

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(1, sms_sent_count);
    TEST_ASSERT_EQUAL_INT(0, email_sent_count);
    TEST_ASSERT_EQUAL_INT(2, log_count);

    notification_service_destroy(service);
}

void test_notify_user_fallback_to_email(void) {
    NotificationDeps deps = {
        .send_sms = stub_send_sms_failure,
        .send_email = stub_send_email_success,
        .log = mock_log
    };

    NotificationService *service = notification_service_create(deps);

    bool result = notify_user(service, "456", "Hello");

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(1, sms_sent_count);
    TEST_ASSERT_EQUAL_INT(1, email_sent_count);
    TEST_ASSERT_EQUAL_STRING("Email sent successfully", logged_messages[1]);

    notification_service_destroy(service);
}

void test_notify_user_all_methods_fail(void) {
    NotificationDeps deps = {
        .send_sms = stub_send_sms_failure,
        .send_email = stub_send_email_failure,
        .log = mock_log
    };

    NotificationService *service = notification_service_create(deps);

    bool result = notify_user(service, "789", "Hello");

    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL_INT(1, sms_sent_count);
    TEST_ASSERT_EQUAL_INT(1, email_sent_count);
    TEST_ASSERT_EQUAL_STRING("All notification methods failed",
                             logged_messages[1]);

    notification_service_destroy(service);
}

void test_notify_user_null_params(void) {
    TEST_ASSERT_FALSE(notify_user(NULL, "123", "Hello"));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_notify_user_sends_sms_when_available);
    RUN_TEST(test_notify_user_fallback_to_email);
    RUN_TEST(test_notify_user_all_methods_fail);
    RUN_TEST(test_notify_user_null_params);

    return UNITY_END();
}
