/*
 * Section  : 32.5.3 - Modernisation (version legacy)
 * Description : Parser CSV style C89 avec pratiques obsolètes
 * Fichier source : 05.3-modernisation.md
 */

/* parser.c - Version legacy C89 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
typedef int bool;

#define MAX_TOKENS 100

/* Globals (mauvaise pratique mais courant en C89) */
static int error_code = 0;

/* Parse comma-separated values */
int parse_csv(char *input, char **tokens, int *count) {
    int token_idx;
    char *p, *start;

    if (input == NULL || tokens == NULL || count == NULL) {
        error_code = -1;
        return FALSE;
    }

    token_idx = 0;
    start = input;
    p = input;

    while (*p != '\0') {
        if (*p == ',') {
            *p = '\0';
            tokens[token_idx] = (char*)malloc(strlen(start) + 1);
            strcpy(tokens[token_idx], start);
            token_idx++;

            if (token_idx >= MAX_TOKENS) {
                error_code = -2;
                return FALSE;
            }

            start = p + 1;
        }
        p++;
    }

    /* Last token */
    if (*start != '\0') {
        tokens[token_idx] = (char*)malloc(strlen(start) + 1);
        strcpy(tokens[token_idx], start);
        token_idx++;
    }

    *count = token_idx;
    return TRUE;
}

int main(void) {
    char *tokens[MAX_TOKENS];
    int count, i;

    /* Test avec donnees en dur (pas interactif) */
    char input[] = "Alice,20,informatique,Paris";

    printf("=== Parser CSV legacy (C89 style) ===\n");
    printf("Input: %s\n\n", input);

    if (parse_csv(input, tokens, &count) == TRUE) {
        for (i = 0; i < count; i++) {
            printf("Token %d: %s\n", i, tokens[i]);
            free(tokens[i]);
        }
    } else {
        printf("Error: %d\n", error_code);
    }

    return 0;
}
