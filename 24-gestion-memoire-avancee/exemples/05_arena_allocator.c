/* ============================================================================
   Section 24.2 : Custom allocators
   Description : Arena allocator - allocation lineaire avec liberation groupee
   Fichier source : 02-custom-allocators.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Structure de l'arena
typedef struct {
    uint8_t *memory;    // Bloc de memoire
    size_t capacity;    // Taille totale
    size_t used;        // Memoire utilisee
} Arena;

// Creer une arena
Arena arena_create(size_t capacity) {
    Arena arena = {0};
    arena.memory = malloc(capacity);
    if (arena.memory) {
        arena.capacity = capacity;
        arena.used = 0;
    }
    return arena;
}

// Allouer depuis l'arena
void *arena_alloc(Arena *arena, size_t size) {
    // Verifier s'il reste assez de place
    if (arena->used + size > arena->capacity) {
        fprintf(stderr, "Arena pleine !\n");
        return NULL;
    }

    // Calculer le pointeur de retour
    void *ptr = arena->memory + arena->used;

    // Avancer le curseur
    arena->used += size;

    return ptr;
}

// Allouer avec alignement (important !)
void *arena_alloc_aligned(Arena *arena, size_t size, size_t alignment) {
    // Calculer l'adresse alignee
    uintptr_t current = (uintptr_t)(arena->memory + arena->used);
    uintptr_t aligned = (current + alignment - 1) & ~(alignment - 1);
    size_t padding = (size_t)(aligned - current);

    // Verifier s'il reste assez de place
    if (arena->used + padding + size > arena->capacity) {
        return NULL;
    }

    arena->used += padding;
    void *ptr = arena->memory + arena->used;
    arena->used += size;

    return ptr;
}

// Reinitialiser l'arena (liberer tout)
void arena_reset(Arena *arena) {
    arena->used = 0;
    // La memoire n'est pas liberee, juste reutilisable
}

// Detruire l'arena
void arena_destroy(Arena *arena) {
    free(arena->memory);
    arena->memory = NULL;
    arena->capacity = 0;
    arena->used = 0;
}

void exemple_arena(void) {
    // Creer une arena de 10 KB
    Arena arena = arena_create(10 * 1024);

    if (!arena.memory) {
        fprintf(stderr, "Erreur creation arena\n");
        return;
    }

    // Allouer plusieurs objets
    char *str1 = arena_alloc(&arena, 100);
    char *str2 = arena_alloc(&arena, 200);
    int *numbers = arena_alloc(&arena, 50 * sizeof(int));

    if (!str1 || !str2 || !numbers) {
        fprintf(stderr, "Erreur allocation\n");
        arena_destroy(&arena);
        return;
    }

    // Utiliser la memoire
    strcpy(str1, "Hello from arena!");
    strcpy(str2, "Arena allocation is fast!");

    for (int i = 0; i < 50; i++) {
        numbers[i] = i * i;
    }

    printf("str1: %s\n", str1);
    printf("str2: %s\n", str2);
    printf("Premiers nombres: %d, %d, %d\n", numbers[0], numbers[1], numbers[2]);
    printf("Memoire utilisee: %zu / %zu bytes\n", arena.used, arena.capacity);

    // Pas besoin de free individuel !
    // Reinitialiser si besoin (reutiliser l'arena)
    arena_reset(&arena);
    printf("\nApres reset: %zu bytes utilises\n", arena.used);

    // On peut reutiliser l'arena pour d'autres allocations
    char *new_str = arena_alloc(&arena, 50);
    if (new_str) {
        strcpy(new_str, "Reutilisation de l'arena");
        printf("new_str: %s\n", new_str);
    }

    // Test allocation alignee
    printf("\n--- Test alignement ---\n");
    arena_reset(&arena);
    arena_alloc(&arena, 3);  // 3 bytes, desaligne
    double *aligned_double = arena_alloc_aligned(&arena, sizeof(double), sizeof(double));
    if (aligned_double) {
        *aligned_double = 3.14;
        printf("Double aligne a %p: %.2f\n", (void *)aligned_double, *aligned_double);
        printf("Aligne sur %zu bytes: %s\n", sizeof(double),
               ((uintptr_t)aligned_double % sizeof(double)) == 0 ? "oui" : "non");
    }

    // Detruire l'arena a la fin
    arena_destroy(&arena);
    printf("\nArena detruite\n");
}

int main(void) {
    exemple_arena();
    return 0;
}
