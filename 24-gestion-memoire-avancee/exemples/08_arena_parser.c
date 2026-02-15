/* ============================================================================
   Section 24.2 : Custom allocators
   Description : Parseur JSON simple utilisant un arena allocator
   Fichier source : 02-custom-allocators.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

// ============================================================================
// Arena allocator (inline pour ce fichier)
// ============================================================================

typedef struct {
    uint8_t *memory;
    size_t capacity;
    size_t used;
} Arena;

Arena arena_create(size_t capacity) {
    Arena arena = {0};
    arena.memory = malloc(capacity);
    if (arena.memory) {
        arena.capacity = capacity;
        arena.used = 0;
    }
    return arena;
}

void *arena_alloc(Arena *arena, size_t size) {
    if (arena->used + size > arena->capacity) {
        fprintf(stderr, "Arena pleine !\n");
        return NULL;
    }
    void *ptr = arena->memory + arena->used;
    arena->used += size;
    return ptr;
}

void arena_destroy(Arena *arena) {
    free(arena->memory);
    arena->memory = NULL;
    arena->capacity = 0;
    arena->used = 0;
}

// ============================================================================
// Parseur JSON simple
// ============================================================================

// Types de tokens JSON
typedef enum {
    TOKEN_LBRACE,    // {
    TOKEN_RBRACE,    // }
    TOKEN_LBRACKET,  // [
    TOKEN_RBRACKET,  // ]
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_EOF
} TokenType;

// Structure d'un token
typedef struct {
    TokenType type;
    char *value;
    size_t length;
} Token;

// Contexte du parseur avec arena
typedef struct {
    const char *input;
    size_t pos;
    Arena arena;  // Toutes les allocations vont ici
} Parser;

// Creer un parseur
Parser parser_create(const char *input, size_t arena_size) {
    Parser parser = {0};
    parser.input = input;
    parser.pos = 0;
    parser.arena = arena_create(arena_size);
    return parser;
}

// Allouer un token (depuis l'arena)
Token *parser_alloc_token(Parser *parser) {
    return arena_alloc(&parser->arena, sizeof(Token));
}

// Allouer une string (depuis l'arena)
char *parser_alloc_string(Parser *parser, size_t length) {
    return arena_alloc(&parser->arena, length + 1);
}

// Parser un token
Token *parser_next_token(Parser *parser) {
    // Sauter les espaces et separateurs JSON (: et ,)
    while (isspace((unsigned char)parser->input[parser->pos]) ||
           parser->input[parser->pos] == ':' ||
           parser->input[parser->pos] == ',') {
        parser->pos++;
    }

    char c = parser->input[parser->pos];

    // Fin de l'input
    if (c == '\0') {
        Token *token = parser_alloc_token(parser);
        if (!token) return NULL;
        token->type = TOKEN_EOF;
        token->value = NULL;
        token->length = 0;
        return token;
    }

    // Caracteres simples
    Token *token = parser_alloc_token(parser);
    if (!token) return NULL;

    token->value = NULL;
    token->length = 0;

    switch (c) {
        case '{':
            token->type = TOKEN_LBRACE;
            parser->pos++;
            return token;
        case '}':
            token->type = TOKEN_RBRACE;
            parser->pos++;
            return token;
        case '[':
            token->type = TOKEN_LBRACKET;
            parser->pos++;
            return token;
        case ']':
            token->type = TOKEN_RBRACKET;
            parser->pos++;
            return token;
        default:
            break;
    }

    // String
    if (c == '"') {
        parser->pos++;  // Sauter "
        size_t start = parser->pos;

        while (parser->input[parser->pos] != '"' &&
               parser->input[parser->pos] != '\0') {
            parser->pos++;
        }

        size_t length = parser->pos - start;
        token->type = TOKEN_STRING;
        token->value = parser_alloc_string(parser, length);
        if (!token->value) return NULL;
        memcpy(token->value, parser->input + start, length);
        token->value[length] = '\0';
        token->length = length;

        parser->pos++;  // Sauter "
        return token;
    }

    // Nombre
    if (isdigit((unsigned char)c) || c == '-') {
        size_t start = parser->pos;

        if (c == '-') parser->pos++;
        while (isdigit((unsigned char)parser->input[parser->pos])) {
            parser->pos++;
        }

        size_t length = parser->pos - start;
        token->type = TOKEN_NUMBER;
        token->value = parser_alloc_string(parser, length);
        if (!token->value) return NULL;
        memcpy(token->value, parser->input + start, length);
        token->value[length] = '\0';
        token->length = length;

        return token;
    }

    return NULL;
}

// Detruire le parseur (libere tout l'arena d'un coup)
void parser_destroy(Parser *parser) {
    arena_destroy(&parser->arena);
}

// Exemple d'utilisation
void exemple_parser(void) {
    const char *json = "{\"name\": \"Alice\", \"age\": 30, \"city\": \"Paris\"}";

    printf("Parsing JSON: %s\n\n", json);

    // Creer le parseur avec une arena de 4 KB
    Parser parser = parser_create(json, 4096);

    if (!parser.arena.memory) {
        fprintf(stderr, "Erreur creation parseur\n");
        return;
    }

    // Parser tous les tokens
    Token *token;
    int count = 0;

    while ((token = parser_next_token(&parser)) && token->type != TOKEN_EOF) {
        printf("Token %d: ", ++count);

        switch (token->type) {
            case TOKEN_LBRACE:
                printf("{\n");
                break;
            case TOKEN_RBRACE:
                printf("}\n");
                break;
            case TOKEN_LBRACKET:
                printf("[\n");
                break;
            case TOKEN_RBRACKET:
                printf("]\n");
                break;
            case TOKEN_STRING:
                printf("STRING: \"%s\"\n", token->value);
                break;
            case TOKEN_NUMBER:
                printf("NUMBER: %s\n", token->value);
                break;
            default:
                break;
        }
    }

    printf("\n%d tokens parses\n", count);
    printf("Memoire utilisee: %zu bytes\n", parser.arena.used);

    // Liberer tout d'un coup (tous les tokens, toutes les strings)
    parser_destroy(&parser);
    printf("Parseur detruit (tout libere d'un coup)\n");
}

int main(void) {
    exemple_parser();
    return 0;
}
