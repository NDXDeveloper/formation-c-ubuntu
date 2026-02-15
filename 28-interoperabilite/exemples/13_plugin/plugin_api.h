/* ============================================================================
   Section 28.5 : extern "C" pour C++
   Description : Interface plugin C - API commune pour plugins C et C++
   Fichier source : 05-extern-c.md
   ============================================================================ */

#ifndef PLUGIN_API_H
#define PLUGIN_API_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *name;
    int version;
} PluginInfo;

/* Fonctions que chaque plugin doit exporter */
PluginInfo get_plugin_info(void);
int plugin_init(void);
void plugin_cleanup(void);
int plugin_process(const char *input, char *output, int output_size);

#ifdef __cplusplus
}
#endif

#endif
