/*
 * Section  : 32.5.3 - Modernisation (patterns)
 * Description : Demonstration des patterns de modernisation C99/C11
 * Fichier source : 05.3-modernisation.md
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdalign.h>

/* ============================================ */
/* Pattern 1 : bool standard (C99)              */
/* ============================================ */

static bool is_valid(const char *str) {
    if (str == NULL) return false;
    if (strlen(str) == 0) return false;
    return true;
}

/* ============================================ */
/* Pattern 2 : Types entiers fixes (C99)        */
/* ============================================ */

static void show_fixed_types(void) {
    int32_t counter = 42;
    uint64_t id = 1234567890ULL;

    printf("counter (int32_t) : %d (%zu bytes)\n", counter, sizeof(counter));
    printf("id (uint64_t)     : %lu (%zu bytes)\n",
           (unsigned long)id, sizeof(id));
}

/* ============================================ */
/* Pattern 3 : Initialisation designee (C99)    */
/* ============================================ */

typedef struct {
    int port;
    const char *host;
    int timeout;
} config_t;

/* ============================================ */
/* Pattern 4 : Inline (C99)                     */
/* ============================================ */

static inline int min(int a, int b) {
    return (a < b) ? a : b;
}

static inline int max(int a, int b) {
    return (a > b) ? a : b;
}

/* ============================================ */
/* Pattern 5 : ARRAY_SIZE macro                 */
/* ============================================ */

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* ============================================ */
/* Pattern 6 : Static assertions (C11)          */
/* ============================================ */

_Static_assert(sizeof(int) >= 4, "int doit etre au moins 32 bits");
_Static_assert(sizeof(void*) == 8, "Plateforme 64 bits requise");

/* ============================================ */
/* Pattern 7 : Alignement (C11)                 */
/* ============================================ */

typedef struct {
    char tag;
    alignas(16) int data[4];
} aligned_data_t;

/* ============================================ */
/* Pattern 8 : safe_strcpy                      */
/* ============================================ */

static inline int safe_strcpy(char *dest, size_t dest_size, const char *src) {
    if (dest == NULL || src == NULL || dest_size == 0) {
        return -1;
    }
    size_t src_len = strlen(src);
    if (src_len >= dest_size) {
        return -1;
    }
    memcpy(dest, src, src_len + 1);
    return 0;
}

int main(void) {
    printf("=== Patterns de modernisation C99/C11 ===\n\n");

    /* Pattern 1 : bool */
    printf("--- Pattern 1 : bool ---\n");
    printf("is_valid(\"hello\") = %s\n", is_valid("hello") ? "true" : "false");
    printf("is_valid(\"\")      = %s\n", is_valid("") ? "true" : "false");
    printf("is_valid(NULL)    = %s\n\n", is_valid(NULL) ? "true" : "false");

    /* Pattern 2 : Types fixes */
    printf("--- Pattern 2 : Types entiers fixes ---\n");
    show_fixed_types();
    printf("\n");

    /* Pattern 3 : Initialisation designee */
    printf("--- Pattern 3 : Initialisation designee ---\n");
    config_t cfg = {
        .port = 8080,
        .host = "localhost",
        .timeout = 30
    };
    printf("Config: %s:%d (timeout=%ds)\n\n", cfg.host, cfg.port, cfg.timeout);

    /* Pattern 4 : Inline */
    printf("--- Pattern 4 : Fonctions inline ---\n");
    printf("min(10, 20) = %d\n", min(10, 20));
    printf("max(10, 20) = %d\n\n", max(10, 20));

    /* Pattern 5 : ARRAY_SIZE */
    printf("--- Pattern 5 : ARRAY_SIZE ---\n");
    int values[] = {10, 20, 30, 40, 50};
    printf("Tableau de %zu elements:", ARRAY_SIZE(values));
    for (size_t i = 0; i < ARRAY_SIZE(values); i++) {
        printf(" %d", values[i]);
    }
    printf("\n\n");

    /* Pattern 6 : Static assert (verification a la compilation) */
    printf("--- Pattern 6 : Static assertions ---\n");
    printf("sizeof(int)   = %zu bytes (>= 4 garanti par _Static_assert)\n",
           sizeof(int));
    printf("sizeof(void*) = %zu bytes (== 8 garanti par _Static_assert)\n\n",
           sizeof(void*));

    /* Pattern 7 : Alignement */
    printf("--- Pattern 7 : Alignement ---\n");
    aligned_data_t ad;
    printf("alignof(aligned_data_t.data) = %zu\n", alignof(ad.data));
    printf("sizeof(aligned_data_t)       = %zu\n\n", sizeof(aligned_data_t));

    /* Pattern 8 : safe_strcpy */
    printf("--- Pattern 8 : safe_strcpy ---\n");
    char buf[10];
    if (safe_strcpy(buf, sizeof(buf), "hello") == 0) {
        printf("safe_strcpy(\"hello\")    : OK -> \"%s\"\n", buf);
    }
    if (safe_strcpy(buf, sizeof(buf), "trop_long_pour_buffer") != 0) {
        printf("safe_strcpy(trop long) : Rejete (buffer trop petit)\n");
    }

    return EXIT_SUCCESS;
}
