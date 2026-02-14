/* ============================================================================
   Section 02.3 : Resolution de symboles au runtime
   Description : Chargeur de plugins dynamiques
   Fichier source : 02.3-resolution-symboles.md
   ============================================================================ */

#include <stdio.h>
#include <dlfcn.h>
#include "plugin_interface.h"

int load_plugin(const char *path) {
    void *handle;
    Plugin *(*get_plugin_func)(void);
    Plugin *plugin;

    /* Charger la bibliotheque */
    handle = dlopen(path, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Erreur: %s\n", dlerror());
        return -1;
    }

    /* Obtenir la fonction get_plugin */
    *(void **)(&get_plugin_func) = dlsym(handle, "get_plugin");
    if (!get_plugin_func) {
        fprintf(stderr, "Symbole get_plugin non trouvé\n");
        dlclose(handle);
        return -1;
    }

    /* Obtenir l'interface du plugin */
    plugin = get_plugin_func();

    printf("Plugin chargé : %s v%s\n", plugin->name, plugin->version);

    /* Utiliser le plugin */
    plugin->init();
    plugin->process("Données de test");
    plugin->cleanup();

    dlclose(handle);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <plugin.so>\n", argv[0]);
        return 1;
    }

    return load_plugin(argv[1]);
}
