/* ============================================================================
   Section 28.5 : extern "C" pour C++
   Description : Application hote C++ chargeant des plugins C et C++ via dlopen
   Fichier source : 05-extern-c.md
   ============================================================================ */

#include <dlfcn.h>
#include <iostream>
#include <cstdlib>
#include "plugin_api.h"

void charger_et_utiliser_plugin(const char *path) {
    void *handle = dlopen(path, RTLD_LAZY);
    if (!handle) {
        std::cerr << "  Erreur : " << dlerror() << std::endl;
        return;
    }

    /* Charger les symboles */
    auto get_info = reinterpret_cast<PluginInfo(*)()>(
        dlsym(handle, "get_plugin_info"));
    auto init = reinterpret_cast<int(*)()>(
        dlsym(handle, "plugin_init"));
    auto process = reinterpret_cast<int(*)(const char *, char *, int)>(
        dlsym(handle, "plugin_process"));
    auto cleanup = reinterpret_cast<void(*)()>(
        dlsym(handle, "plugin_cleanup"));

    if (!get_info || !init || !process || !cleanup) {
        std::cerr << "  Symboles manquants" << std::endl;
        dlclose(handle);
        return;
    }

    /* Utiliser le plugin */
    PluginInfo info = get_info();
    std::cout << "  Plugin : " << info.name
              << " v" << info.version << std::endl;

    init();

    char output[256];
    process("hello world", output, static_cast<int>(sizeof(output)));
    std::cout << "  Sortie : " << output << std::endl;

    cleanup();

    dlclose(handle);
}

int main() {
    std::cout << "=== Systeme de plugins (dlopen) ===" << std::endl;
    std::cout << std::endl;

    std::cout << "--- Plugin C ---" << std::endl;
    charger_et_utiliser_plugin("./plugin_c.so");

    std::cout << std::endl;

    std::cout << "--- Plugin C++ ---" << std::endl;
    charger_et_utiliser_plugin("./plugin_cpp.so");

    std::cout << std::endl << "=== Fin ===" << std::endl;
    return 0;
}
