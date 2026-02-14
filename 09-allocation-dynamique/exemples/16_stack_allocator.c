/* ============================================================================
   Section 9.7 : Strategies d'allocation personnalisees
   Description : Stack Allocator LIFO (allocation/liberation en ordre inverse)
   Fichier source : 07-strategies-allocation.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct StackHeader {
    size_t size;
} StackHeader;

typedef struct StackAllocator {
    char* memory;
    size_t size;
    size_t offset;
} StackAllocator;

StackAllocator* stack_create(size_t size) {
    StackAllocator* stack = malloc(sizeof(StackAllocator));
    if (stack == NULL) return NULL;

    stack->memory = malloc(size);
    if (stack->memory == NULL) {
        free(stack);
        return NULL;
    }

    stack->size = size;
    stack->offset = 0;

    return stack;
}

void* stack_alloc(StackAllocator* stack, size_t size) {
    if (stack == NULL) return NULL;

    size_t total = sizeof(StackHeader) + size;
    total = (total + 7) & ~7;

    if (stack->offset + total > stack->size) {
        return NULL;
    }

    StackHeader* header = (StackHeader*)(stack->memory + stack->offset);
    header->size = total;

    stack->offset += total;

    return (void*)(header + 1);
}

bool stack_free(StackAllocator* stack, void* ptr) {
    if (stack == NULL || ptr == NULL) return false;

    StackHeader* header = ((StackHeader*)ptr) - 1;

    char* expected_end = (char*)header + header->size;
    char* actual_end = stack->memory + stack->offset;

    if (expected_end != actual_end) {
        fprintf(stderr, "Erreur : violation LIFO (free dans le mauvais ordre)\n");
        return false;
    }

    stack->offset -= header->size;
    return true;
}

void stack_destroy(StackAllocator* stack) {
    if (stack != NULL) {
        free(stack->memory);
        free(stack);
    }
}

int main(void) {
    StackAllocator* stack = stack_create(1024);

    if (stack == NULL) {
        return 1;
    }

    int* a = stack_alloc(stack, sizeof(int));
    char* b = stack_alloc(stack, 100);
    double* c = stack_alloc(stack, sizeof(double));

    if (a && b && c) {
        *a = 42;
        strcpy(b, "Hello");
        *c = 3.14;

        printf("a=%d, b=%s, c=%.2f\n", *a, b, *c);
    }

    stack_free(stack, c);
    stack_free(stack, b);
    stack_free(stack, a);

    stack_destroy(stack);
    return 0;
}
