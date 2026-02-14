/* ============================================================================
   Section 9.7 : Strategies d'allocation personnalisees
   Description : Arena/Bump Allocator (allocation lineaire, liberation en masse)
   Fichier source : 07-strategies-allocation.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Arena {
    char* memory;
    size_t size;
    size_t offset;
    size_t peak;
} Arena;

Arena* arena_create(size_t size) {
    Arena* arena = malloc(sizeof(Arena));
    if (arena == NULL) return NULL;

    arena->memory = malloc(size);
    if (arena->memory == NULL) {
        free(arena);
        return NULL;
    }

    arena->size = size;
    arena->offset = 0;
    arena->peak = 0;

    printf("Arena creee : %zu octets\n", size);
    return arena;
}

void* arena_alloc(Arena* arena, size_t size) {
    if (arena == NULL) return NULL;

    size = (size + 7) & ~7;

    if (arena->offset + size > arena->size) {
        fprintf(stderr, "Arena pleine : demande %zu, reste %zu\n",
                size, arena->size - arena->offset);
        return NULL;
    }

    void* ptr = arena->memory + arena->offset;
    arena->offset += size;

    if (arena->offset > arena->peak) {
        arena->peak = arena->offset;
    }

    return ptr;
}

void arena_reset(Arena* arena) {
    if (arena != NULL) {
        printf("Arena reset : %zu octets liberes\n", arena->offset);
        arena->offset = 0;
    }
}

void arena_destroy(Arena* arena) {
    if (arena != NULL) {
        free(arena->memory);
        free(arena);
    }
}

void arena_stats(const Arena* arena) {
    if (arena == NULL) return;

    printf("=== Arena Stats ===\n");
    printf("Taille totale  : %zu octets\n", arena->size);
    printf("Utilise        : %zu octets (%.1f%%)\n",
           arena->offset,
           100.0 * arena->offset / arena->size);
    printf("Libre          : %zu octets\n", arena->size - arena->offset);
    printf("Peak usage     : %zu octets (%.1f%%)\n",
           arena->peak,
           100.0 * arena->peak / arena->size);
}

typedef struct Token {
    char* text;
    int line;
    int column;
} Token;

Token* parse_tokens(Arena* arena, const char* source) {
    (void)source;
    Token* tokens = arena_alloc(arena, 10 * sizeof(Token));
    if (tokens == NULL) return NULL;

    for (int i = 0; i < 3; i++) {
        tokens[i].text = arena_alloc(arena, 20);
        snprintf(tokens[i].text, 20, "Token%d", i);
        tokens[i].line = i + 1;
        tokens[i].column = 1;
    }

    return tokens;
}

int main(void) {
    Arena* arena = arena_create(1024 * 1024);

    if (arena == NULL) {
        return 1;
    }

    for (int i = 0; i < 3; i++) {
        printf("\n--- Parsing fichier %d ---\n", i + 1);

        Token* tokens = parse_tokens(arena, "dummy source");

        if (tokens) {
            for (int j = 0; j < 3; j++) {
                printf("Token: %s (ligne %d)\n",
                       tokens[j].text, tokens[j].line);
            }
        }

        arena_stats(arena);

        arena_reset(arena);
    }

    arena_destroy(arena);

    return 0;
}
