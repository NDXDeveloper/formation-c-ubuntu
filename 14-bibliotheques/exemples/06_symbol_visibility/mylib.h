/* ============================================================================
   Section 07 : Symbol visibility
   Description : En-tete public avec symboles marques MYLIB_API
   Fichier source : 07-symbol-visibility.md
   ============================================================================ */

#ifndef MYLIB_H
#define MYLIB_H

#include "mylib_export.h"

/* API publique - tous les symboles marques MYLIB_API */
MYLIB_API void mylib_init(void);
MYLIB_API void mylib_process(const char *data);
MYLIB_API int mylib_get_version(void);

#endif
