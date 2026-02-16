/* ============================================================================
   Section 29.4 : Mocking et stubbing
   Description : Service de notification avec injection de dependances (impl)
   Fichier source : 04-mocking-stubbing.md
   ============================================================================ */
#include "notification_service.h"
#include <stdlib.h>
#include <stdio.h>

NotificationService *notification_service_create(NotificationDeps deps) {
    NotificationService *service = malloc(sizeof(NotificationService));
    if (service) {
        service->deps = deps;
    }
    return service;
}

void notification_service_destroy(NotificationService *service) {
    free(service);
}

bool notify_user(NotificationService *service, const char *user_id,
                 const char *message) {
    if (!service || !user_id || !message) return false;

    service->deps.log("Sending notification");

    /* Essayer SMS en premier */
    char phone[20];
    snprintf(phone, sizeof(phone), "+1234567%s", user_id);
    if (service->deps.send_sms(phone, message)) {
        service->deps.log("SMS sent successfully");
        return true;
    }

    /* Fallback sur email */
    char email[100];
    snprintf(email, sizeof(email), "user%s@example.com", user_id);
    if (service->deps.send_email(email, "Notification", message)) {
        service->deps.log("Email sent successfully");
        return true;
    }

    service->deps.log("All notification methods failed");
    return false;
}
