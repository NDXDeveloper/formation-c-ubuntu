/* ============================================================================
   Section 28.5 : extern "C" pour C++
   Description : Plugin ecrit en C - compile en .so, charge par dlopen
   Fichier source : 05-extern-c.md
   ============================================================================ */

#include "plugin_api.h"
#include <string.h>
#include <stdio.h>

PluginInfo get_plugin_info(void) {
    PluginInfo info;
    info.name = "Plugin C";
    info.version = 1;
    return info;
}

int plugin_init(void) {
    printf("  Plugin C initialise\n");
    return 0;
}

void plugin_cleanup(void) {
    printf("  Plugin C nettoye\n");
}

int plugin_process(const char *input, char *output, int output_size) {
    snprintf(output, (size_t)output_size, "Traite par C : %s", input);
    return 0;
}
