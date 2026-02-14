/* ============================================================================
   Section 07 : Symbol visibility
   Description : Macros de visibilite portables (GCC/Windows)
   Fichier source : 07-symbol-visibility.md
   ============================================================================ */

#ifndef MYLIB_EXPORT_H
#define MYLIB_EXPORT_H

#ifdef MYLIB_BUILDING
  /* On compile la bibliotheque */
  #if defined(__GNUC__) && __GNUC__ >= 4
    #define MYLIB_API __attribute__((visibility("default")))
  #elif defined(_WIN32)
    #define MYLIB_API __declspec(dllexport)
  #else
    #define MYLIB_API
  #endif
#else
  /* On utilise la bibliotheque */
  #if defined(_WIN32)
    #define MYLIB_API __declspec(dllimport)
  #else
    #define MYLIB_API
  #endif
#endif

#endif
