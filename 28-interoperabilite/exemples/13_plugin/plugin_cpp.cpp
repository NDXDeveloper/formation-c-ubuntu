/* ============================================================================
   Section 28.5 : extern "C" pour C++
   Description : Plugin ecrit en C++ - compile en .so, charge par dlopen
                 (utilise std::string et std::transform en interne)
   Fichier source : 05-extern-c.md
   ============================================================================ */

#include "plugin_api.h"
#include <string>
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cctype>

extern "C" {

PluginInfo get_plugin_info(void) {
    PluginInfo info;
    info.name = "Plugin C++";
    info.version = 1;
    return info;
}

int plugin_init(void) {
    std::cout << "  Plugin C++ initialise" << std::endl;
    return 0;
}

void plugin_cleanup(void) {
    std::cout << "  Plugin C++ nettoye" << std::endl;
}

int plugin_process(const char *input, char *output, int output_size) {
    std::string str(input);
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    std::snprintf(output, static_cast<size_t>(output_size),
                  "Traite par C++ : %s", str.c_str());
    return 0;
}

}  /* extern "C" */
