/* ============================================================================
   Section 26.3 : Prevention des buffer overflows
   Description : Bibliotheque de fonctions de chaines securisees (header)
   Fichier source : 03-prevention-buffer-overflows.md
   ============================================================================ */

#ifndef SAFE_STRING_H
#define SAFE_STRING_H

#include <stddef.h>
#include <stdbool.h>

/* Copie securisee */
bool safe_strcpy(char *dest, size_t dest_size, const char *source);

/* Concatenation securisee */
bool safe_strcat(char *dest, size_t dest_size, const char *source);

/* Formatage securise */
int safe_sprintf(char *buffer, size_t buffer_size, const char *format, ...);

#endif
