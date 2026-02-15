/* ============================================================================
   Section 23.2 : Stringification et concatenation
   Description : Interface de configuration avec setters generes par macros
   Fichier source : 02-stringification-concatenation.md
   ============================================================================ */

#include <stdio.h>

typedef struct {
    int port;
    char host[100];
    int timeout;
} Config;

// Macro pour générer des setters
#define CONFIG_SETTER(field, type) \
    void config_set_##field(Config* cfg, type value) { \
        cfg->field = value; \
        printf("Configuration mise à jour: " #field " = %d\n", value); \
    }

CONFIG_SETTER(port, int)
CONFIG_SETTER(timeout, int)

int main(void) {
    Config cfg = {0, "", 0};

    config_set_port(&cfg, 8080);
    config_set_timeout(&cfg, 5000);

    printf("\nConfiguration finale:\n");
    printf("Port: %d\n", cfg.port);
    printf("Timeout: %d\n", cfg.timeout);

    return 0;
}
