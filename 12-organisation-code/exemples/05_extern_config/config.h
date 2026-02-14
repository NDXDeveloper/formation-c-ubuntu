/* ============================================================================
   Section 12.2 : La portee des variables
   Description : Configuration globale partagee via extern - declarations
   Fichier source : 02-portee-variables.md
   ============================================================================ */

#ifndef CONFIG_H
#define CONFIG_H

/* Declarations (extern) */
extern int port_serveur;
extern const char* nom_serveur;
extern int mode_debug;

#endif /* CONFIG_H */
