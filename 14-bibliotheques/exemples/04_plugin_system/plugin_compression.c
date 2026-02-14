/* ============================================================================
   Section 02.3 : Resolution de symboles au runtime
   Description : Plugin de compression (exemple de plugin dynamique)
   Fichier source : 02.3-resolution-symboles.md
   ============================================================================ */

#include <stdio.h>
#include "plugin_interface.h"

static int compression_init(void) {
    printf("Plugin Compression initialisé\n");
    return 0;
}

static void compression_process(const char *data) {
    printf("Compression de : %s\n", data);
}

static void compression_cleanup(void) {
    printf("Plugin Compression nettoyé\n");
}

static Plugin compression_plugin = {
    .name = "Compression",
    .version = "1.0",
    .init = compression_init,
    .process = compression_process,
    .cleanup = compression_cleanup
};

Plugin *get_plugin(void) {
    return &compression_plugin;
}
