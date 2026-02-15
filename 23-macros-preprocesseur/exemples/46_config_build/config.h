/* ============================================================================
   Section 23.3 : Compilation conditionnelle
   Description : Header de configuration de build (debug/release/test)
   Fichier source : 03-compilation-conditionnelle.md
   ============================================================================ */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>

// Configuration par défaut si aucun type de build n'est spécifié
#if !defined(BUILD_TYPE_DEBUG) && !defined(BUILD_TYPE_RELEASE) && !defined(BUILD_TYPE_TEST)
    #define BUILD_TYPE_DEBUG
#endif

// Configurations selon le type de build
#ifdef BUILD_TYPE_DEBUG
    #define DEBUG 1
    #define OPTIMISATION 0
    #define LOGGING_LEVEL 4
    #define ASSERTIONS_ENABLED 1
    #define PROFILING_ENABLED 1
#elif defined(BUILD_TYPE_RELEASE)
    #define DEBUG 0
    #define OPTIMISATION 3
    #define LOGGING_LEVEL 1
    #define ASSERTIONS_ENABLED 0
    #define PROFILING_ENABLED 0
#elif defined(BUILD_TYPE_TEST)
    #define DEBUG 1
    #define OPTIMISATION 0
    #define LOGGING_LEVEL 3
    #define ASSERTIONS_ENABLED 1
    #define PROFILING_ENABLED 1
#endif

// Macros de logging conditionnelles
#if LOGGING_LEVEL >= 1
    #define LOG_ERROR(msg) printf("[ERROR] %s\n", msg)
#else
    #define LOG_ERROR(msg) do {} while(0)
#endif

#if LOGGING_LEVEL >= 2
    #define LOG_WARNING(msg) printf("[WARNING] %s\n", msg)
#else
    #define LOG_WARNING(msg) do {} while(0)
#endif

#if LOGGING_LEVEL >= 3
    #define LOG_INFO(msg) printf("[INFO] %s\n", msg)
#else
    #define LOG_INFO(msg) do {} while(0)
#endif

#if LOGGING_LEVEL >= 4
    #define LOG_DEBUG(msg) printf("[DEBUG] %s:%d - %s\n", __FILE__, __LINE__, msg)
#else
    #define LOG_DEBUG(msg) do {} while(0)
#endif

// Assertions conditionnelles
#if ASSERTIONS_ENABLED
    #include <assert.h>
    #define ASSERT(cond) assert(cond)
#else
    #define ASSERT(cond) do {} while(0)
#endif

#endif  // CONFIG_H
