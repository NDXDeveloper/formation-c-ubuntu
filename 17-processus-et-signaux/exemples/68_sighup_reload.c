/* ============================================================================
   Section 17.6 : Signaux courants
   Description : SIGHUP pour rechargement de configuration d'un daemon
   Fichier source : 06-signaux-courants.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    int port;
    char log_level[16];
    int max_connections;
} Config;

Config global_config;
volatile sig_atomic_t reload_requested = 0;

void load_configuration(void) {
    printf("\n[Config] Chargement de la configuration...\n");

    /* Simuler la lecture d'un fichier de config */
    global_config.port = 8080;
    strcpy(global_config.log_level, "INFO");
    global_config.max_connections = 100;

    printf("[Config] Port: %d\n", global_config.port);
    printf("[Config] Log level: %s\n", global_config.log_level);
    printf("[Config] Max connections: %d\n", global_config.max_connections);
    printf("[Config] Configuration chargee\n\n");
}

void sighup_handler(int signum) {
    (void)signum;
    reload_requested = 1;
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = sighup_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGHUP, &sa, NULL);

    printf("Daemon demarre (PID: %d)\n", getpid());

    load_configuration();

    printf("Pour recharger la config: kill -HUP %d\n\n", getpid());

    for (int i = 0; i < 15; i++) {
        if (reload_requested) {
            reload_requested = 0;
            load_configuration();
        }

        printf("Service en cours (port %d)...\n", global_config.port);
        sleep(3);
    }

    return 0;
}
