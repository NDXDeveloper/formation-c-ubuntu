/* ============================================================================
   Section 23.4 : Macros cross-platform
   Description : Header de detection de plateforme centralise
   Fichier source : 04-macros-cross-platform.md
   ============================================================================ */

#ifndef PLATFORM_H
#define PLATFORM_H

// Détection du système d'exploitation
#if defined(_WIN32) || defined(_WIN64)
    #define OS_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
    #define OS_MACOS
#elif defined(__linux__)
    #define OS_LINUX
#elif defined(__FreeBSD__)
    #define OS_FREEBSD
#elif defined(__unix__) || defined(__unix)
    #define OS_UNIX
#else
    #define OS_UNKNOWN
#endif

// Groupement Unix-like
#if defined(OS_LINUX) || defined(OS_MACOS) || defined(OS_FREEBSD) || defined(OS_UNIX)
    #define OS_UNIX_LIKE
#endif

// Détection du compilateur
#if defined(__clang__)
    #define COMPILER_CLANG
#elif defined(__GNUC__)
    #define COMPILER_GCC
#elif defined(_MSC_VER)
    #define COMPILER_MSVC
#endif

// Séparateur de chemin
#ifdef OS_WINDOWS
    #define PATH_SEP '\\'
    #define PATH_SEP_STR "\\"
#else
    #define PATH_SEP '/'
    #define PATH_SEP_STR "/"
#endif

#endif  // PLATFORM_H
