/* ============================================================================
   Section 29.4 : Mocking et stubbing
   Description : Service de notification avec injection de dependances (header)
   Fichier source : 04-mocking-stubbing.md
   ============================================================================ */
#ifndef NOTIFICATION_SERVICE_H
#define NOTIFICATION_SERVICE_H

#include <stdbool.h>

/* Dependances abstraites */
typedef struct {
    bool (*send_sms)(const char *phone, const char *message);
    bool (*send_email)(const char *email, const char *subject, const char *body);
    void (*log)(const char *message);
} NotificationDeps;

/* Service */
typedef struct {
    NotificationDeps deps;
} NotificationService;

NotificationService *notification_service_create(NotificationDeps deps);
void notification_service_destroy(NotificationService *service);
bool notify_user(NotificationService *service, const char *user_id,
                 const char *message);

#endif
