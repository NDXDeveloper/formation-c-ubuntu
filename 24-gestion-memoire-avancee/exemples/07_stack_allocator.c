/* ============================================================================
   Section 24.2 : Custom allocators
   Description : Stack allocator - allocation LIFO avec marks/restore
   Fichier source : 02-custom-allocators.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

// Marqueur de stack pour sauvegarder/restaurer
typedef struct {
    size_t position;
} StackMark;

// Structure du stack allocator
typedef struct {
    uint8_t *memory;
    size_t capacity;
    size_t top;  // Position actuelle
} StackAllocator;

// Creer un stack allocator
StackAllocator stack_create(size_t capacity) {
    StackAllocator stack = {0};
    stack.memory = malloc(capacity);
    if (stack.memory) {
        stack.capacity = capacity;
        stack.top = 0;
    }
    return stack;
}

// Allouer depuis le stack
void *stack_alloc(StackAllocator *stack, size_t size) {
    // Verifier l'espace disponible
    if (stack->top + size > stack->capacity) {
        fprintf(stderr, "Stack overflow!\n");
        return NULL;
    }

    void *ptr = stack->memory + stack->top;
    stack->top += size;

    return ptr;
}

// Allouer avec alignement
void *stack_alloc_aligned(StackAllocator *stack, size_t size, size_t alignment) {
    uintptr_t current = (uintptr_t)(stack->memory + stack->top);
    uintptr_t aligned = (current + alignment - 1) & ~(alignment - 1);
    size_t padding = (size_t)(aligned - current);

    if (stack->top + padding + size > stack->capacity) {
        return NULL;
    }

    stack->top += padding;
    void *ptr = stack->memory + stack->top;
    stack->top += size;

    return ptr;
}

// Marquer la position actuelle
StackMark stack_mark(StackAllocator *stack) {
    StackMark mark;
    mark.position = stack->top;
    return mark;
}

// Restaurer a une marque
void stack_restore(StackAllocator *stack, StackMark mark) {
    assert(mark.position <= stack->top);
    stack->top = mark.position;
}

// Reinitialiser completement
void stack_reset(StackAllocator *stack) {
    stack->top = 0;
}

// Detruire le stack
void stack_destroy(StackAllocator *stack) {
    free(stack->memory);
    stack->memory = NULL;
    stack->capacity = 0;
    stack->top = 0;
}

// Obtenir l'espace utilise
size_t stack_used(StackAllocator *stack) {
    return stack->top;
}

// Obtenir l'espace libre
size_t stack_free_space(StackAllocator *stack) {
    return stack->capacity - stack->top;
}

void exemple_stack(void) {
    // Creer un stack de 1 MB
    StackAllocator stack = stack_create(1024 * 1024);

    if (!stack.memory) {
        fprintf(stderr, "Erreur creation stack\n");
        return;
    }

    printf("Stack cree: %zu bytes disponibles\n", stack.capacity);

    // Marquer le debut
    StackMark start = stack_mark(&stack);

    // Allouer des donnees temporaires
    char *temp1 = stack_alloc(&stack, 1000);
    if (temp1) {
        strcpy(temp1, "Donnees temporaires 1");
        printf("Alloue temp1: %zu bytes utilises\n", stack_used(&stack));
    }

    // Marquer apres temp1
    StackMark after_temp1 = stack_mark(&stack);

    char *temp2 = stack_alloc(&stack, 2000);
    if (temp2) {
        strcpy(temp2, "Donnees temporaires 2");
        printf("Alloue temp2: %zu bytes utilises\n", stack_used(&stack));
    }

    int *numbers = stack_alloc(&stack, 100 * sizeof(int));
    if (numbers) {
        for (int i = 0; i < 100; i++) {
            numbers[i] = i;
        }
        printf("Alloue numbers: %zu bytes utilises\n", stack_used(&stack));
    }

    // Utiliser les donnees
    printf("\nDonnees:\n");
    if (temp1) printf("temp1: %s\n", temp1);
    if (temp2) printf("temp2: %s\n", temp2);
    if (numbers) printf("numbers[50]: %d\n", numbers[50]);

    // Liberer temp2 et numbers en restaurant after_temp1
    printf("\nRestauration a after_temp1...\n");
    stack_restore(&stack, after_temp1);
    printf("Bytes utilises apres restauration: %zu\n", stack_used(&stack));

    // temp1 est toujours valide, mais temp2 et numbers ne le sont plus !
    if (temp1) printf("temp1 toujours valide: %s\n", temp1);

    // Restaurer au debut
    printf("\nRestauration complete...\n");
    stack_restore(&stack, start);
    printf("Bytes utilises: %zu (retour au debut)\n", stack_used(&stack));

    // Detruire le stack
    stack_destroy(&stack);
    printf("\nStack detruit\n");
}

int main(void) {
    exemple_stack();
    return 0;
}
