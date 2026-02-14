/* ============================================================================
   Section 9.7 : Strategies d'allocation personnalisees
   Description : Free List Allocator (first-fit, tailles variables)
   Fichier source : 07-strategies-allocation.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct FreeBlock {
    size_t size;
    struct FreeBlock* next;
} FreeBlock;

typedef struct FreeListAllocator {
    char* memory;
    size_t size;
    FreeBlock* free_list;
} FreeListAllocator;

FreeListAllocator* freelist_create(size_t size) {
    FreeListAllocator* allocator = malloc(sizeof(FreeListAllocator));
    if (allocator == NULL) return NULL;

    allocator->memory = malloc(size);
    if (allocator->memory == NULL) {
        free(allocator);
        return NULL;
    }

    allocator->size = size;

    allocator->free_list = (FreeBlock*)allocator->memory;
    allocator->free_list->size = size;
    allocator->free_list->next = NULL;

    return allocator;
}

void* freelist_alloc(FreeListAllocator* allocator, size_t size) {
    if (allocator == NULL || allocator->free_list == NULL) {
        return NULL;
    }

    size = (size + sizeof(FreeBlock) + 7) & ~7;

    FreeBlock* prev = NULL;
    FreeBlock* current = allocator->free_list;

    while (current != NULL) {
        if (current->size >= size) {
            size_t remainder = current->size - size;

            if (remainder > sizeof(FreeBlock) + 16) {
                FreeBlock* new_block = (FreeBlock*)((char*)current + size);
                new_block->size = remainder;
                new_block->next = current->next;

                if (prev) {
                    prev->next = new_block;
                } else {
                    allocator->free_list = new_block;
                }
            } else {
                if (prev) {
                    prev->next = current->next;
                } else {
                    allocator->free_list = current->next;
                }
            }

            return (void*)((char*)current + sizeof(FreeBlock));
        }

        prev = current;
        current = current->next;
    }

    return NULL;
}

void freelist_free(FreeListAllocator* allocator, void* ptr) {
    if (allocator == NULL || ptr == NULL) return;

    FreeBlock* block = (FreeBlock*)((char*)ptr - sizeof(FreeBlock));

    block->next = allocator->free_list;
    allocator->free_list = block;
}

void freelist_destroy(FreeListAllocator* allocator) {
    if (allocator != NULL) {
        free(allocator->memory);
        free(allocator);
    }
}

int main(void) {
    FreeListAllocator* allocator = freelist_create(1024);

    if (allocator == NULL) {
        return 1;
    }

    int* a = freelist_alloc(allocator, sizeof(int));
    char* b = freelist_alloc(allocator, 100);
    double* c = freelist_alloc(allocator, sizeof(double));

    if (a && b && c) {
        *a = 42;
        strcpy(b, "Hello from free list");
        *c = 3.14159;

        printf("a=%d\n", *a);
        printf("b=%s\n", b);
        printf("c=%.5f\n", *c);
    }

    freelist_free(allocator, b);
    freelist_free(allocator, a);

    int* d = freelist_alloc(allocator, sizeof(int));
    if (d) {
        *d = 100;
        printf("d=%d (bloc reutilise)\n", *d);
    }

    freelist_free(allocator, d);
    freelist_free(allocator, c);
    freelist_destroy(allocator);

    return 0;
}
