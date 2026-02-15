/* ============================================================================
   Section 26.3 : Prevention des buffer overflows
   Description : Bibliotheque de fonctions de chaines securisees (implementation)
   Fichier source : 03-prevention-buffer-overflows.md
   ============================================================================ */

#include "safe_string.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

bool safe_strcpy(char *dest, size_t dest_size, const char *source) {
    if (dest == NULL || source == NULL || dest_size == 0) {
        return false;
    }

    size_t source_len = strlen(source);
    if (source_len >= dest_size) {
        return false;  /* Source trop longue */
    }

    strcpy(dest, source);
    return true;
}

bool safe_strcat(char *dest, size_t dest_size, const char *source) {
    if (dest == NULL || source == NULL || dest_size == 0) {
        return false;
    }

    size_t dest_len = strlen(dest);
    size_t source_len = strlen(source);

    if (dest_len + source_len >= dest_size) {
        return false;  /* Pas assez d'espace */
    }

    strcat(dest, source);
    return true;
}

int safe_sprintf(char *buffer, size_t buffer_size, const char *format, ...) {
    if (buffer == NULL || format == NULL || buffer_size == 0) {
        return -1;
    }

    va_list args;
    va_start(args, format);

    int result = vsnprintf(buffer, buffer_size, format, args);

    va_end(args);

    if (result < 0 || (size_t)result >= buffer_size) {
        return -1;  /* Erreur ou troncature */
    }

    return result;
}
