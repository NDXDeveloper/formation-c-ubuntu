/*
 * Section  : 32.5.3 - Modernisation (version moderne)
 * Description : Parser CSV style C11 avec pratiques modernes
 * Fichier source : 05.3-modernisation.md
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#define MAX_TOKENS 100

/* Structure d'erreur moderne */
typedef struct {
    int code;
    const char *message;
} parse_error_t;

/* Types size_t pour les tailles */
typedef struct {
    char **tokens;
    size_t count;
} parse_result_t;

/* Static assertion : verifier constantes */
_Static_assert(MAX_TOKENS > 0, "MAX_TOKENS must be positive");

/* Parse comma-separated values - Version moderne */
static bool parse_csv(const char *input, parse_result_t *result, parse_error_t *error) {
    /* Validation entree */
    if (input == NULL || result == NULL) {
        if (error) {
            error->code = EINVAL;
            error->message = "Invalid argument";
        }
        errno = EINVAL;
        return false;
    }

    /* Initialisation */
    result->tokens = calloc(MAX_TOKENS, sizeof(char*));
    if (result->tokens == NULL) {
        if (error) {
            error->code = ENOMEM;
            error->message = "Memory allocation failed";
        }
        errno = ENOMEM;
        return false;
    }

    result->count = 0;

    /* Copie de travail (ne modifie pas l'entree) */
    char *input_copy = strdup(input);
    if (input_copy == NULL) {
        free(result->tokens);
        if (error) {
            error->code = ENOMEM;
            error->message = "Memory allocation failed";
        }
        errno = ENOMEM;
        return false;
    }

    /* Parsing avec strtok_r (thread-safe) */
    char *saveptr;
    char *token = strtok_r(input_copy, ",", &saveptr);

    while (token != NULL && result->count < MAX_TOKENS) {
        result->tokens[result->count] = strdup(token);
        if (result->tokens[result->count] == NULL) {
            for (size_t i = 0; i < result->count; i++) {
                free(result->tokens[i]);
            }
            free(result->tokens);
            free(input_copy);

            if (error) {
                error->code = ENOMEM;
                error->message = "Memory allocation failed";
            }
            errno = ENOMEM;
            return false;
        }

        result->count++;
        token = strtok_r(NULL, ",", &saveptr);
    }

    free(input_copy);

    if (token != NULL) {
        for (size_t i = 0; i < result->count; i++) {
            free(result->tokens[i]);
        }
        free(result->tokens);

        if (error) {
            error->code = E2BIG;
            error->message = "Too many tokens";
        }
        errno = E2BIG;
        return false;
    }

    return true;
}

/* Liberer les resultats */
static inline void free_parse_result(parse_result_t *result) {
    if (result == NULL) return;

    for (size_t i = 0; i < result->count; i++) {
        free(result->tokens[i]);
    }
    free(result->tokens);
    result->tokens = NULL;
    result->count = 0;
}

int main(void) {
    const char *csv_data = "Alice,20,informatique,Paris";

    printf("=== Parser CSV moderne (C11 style) ===\n");
    printf("Input: %s\n\n", csv_data);

    parse_result_t result = {0};
    parse_error_t error = {0};

    if (parse_csv(csv_data, &result, &error)) {
        for (size_t i = 0; i < result.count; i++) {
            printf("Token %zu: %s\n", i, result.tokens[i]);
        }
        free_parse_result(&result);
        return EXIT_SUCCESS;
    } else {
        fprintf(stderr, "Parse error: %s (code %d)\n",
                error.message, error.code);
        return EXIT_FAILURE;
    }
}
