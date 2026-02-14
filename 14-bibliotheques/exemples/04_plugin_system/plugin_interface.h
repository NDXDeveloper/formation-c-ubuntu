/* ============================================================================
   Section 02.3 : Resolution de symboles au runtime
   Description : Interface de plugin pour systeme de chargement dynamique
   Fichier source : 02.3-resolution-symboles.md
   ============================================================================ */

#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

typedef struct {
    const char *name;
    const char *version;
    int (*init)(void);
    void (*process)(const char *data);
    void (*cleanup)(void);
} Plugin;

/* Fonction que chaque plugin doit exporter */
Plugin *get_plugin(void);

#endif
