/* ============================================================================
   Section 24.1.3 : Patterns cleanup
   Description : Bibliotheque complete de patterns cleanup reutilisables
   Fichier source : 01.3-patterns.md
   ============================================================================ */

// cleanup_patterns.h - Bibliotheque complete de patterns cleanup
#ifndef CLEANUP_PATTERNS_H
#define CLEANUP_PATTERNS_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

//==============================================================================
// MEMOIRE
//==============================================================================

static inline void cleanup_free(void *p) {
    void **ptr = (void **)p;
    if (*ptr) {
        free(*ptr);
        *ptr = NULL;
    }
}
#define _cleanup_free_ __attribute__((cleanup(cleanup_free)))

static inline void cleanup_strv_free(char ***strv) {
    if (*strv) {
        for (size_t i = 0; (*strv)[i] != NULL; i++) {
            free((*strv)[i]);
        }
        free(*strv);
        *strv = NULL;
    }
}
#define _cleanup_strv_ __attribute__((cleanup(cleanup_strv_free)))

//==============================================================================
// FICHIERS
//==============================================================================

static inline void cleanup_fclose(FILE **f) {
    if (*f) {
        fclose(*f);
        *f = NULL;
    }
}
#define _cleanup_fclose_ __attribute__((cleanup(cleanup_fclose)))

static inline void cleanup_close(int *fd) {
    if (*fd >= 0) {
        close(*fd);
        *fd = -1;
    }
}
#define _cleanup_close_ __attribute__((cleanup(cleanup_close)))

//==============================================================================
// THREADS ET LOCKS
//==============================================================================

static inline void cleanup_mutex_unlock(pthread_mutex_t **m) {
    if (*m) {
        pthread_mutex_unlock(*m);
    }
}
#define _cleanup_mutex_unlock_ __attribute__((cleanup(cleanup_mutex_unlock)))

static inline void cleanup_rwlock_unlock(pthread_rwlock_t **lock) {
    if (*lock) {
        pthread_rwlock_unlock(*lock);
    }
}
#define _cleanup_rwlock_ __attribute__((cleanup(cleanup_rwlock_unlock)))

//==============================================================================
// UTILITAIRES
//==============================================================================

// Macro generique pour creer vos propres cleanups
#define _cleanup_(fonction) __attribute__((cleanup(fonction)))

// Desactiver temporairement le cleanup
#define STEAL(var) ({ __typeof__(var) _tmp = var; var = NULL; _tmp; })

#endif // CLEANUP_PATTERNS_H
