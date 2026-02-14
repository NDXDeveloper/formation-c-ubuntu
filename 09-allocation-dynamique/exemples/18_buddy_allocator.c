/* ============================================================================
   Section 9.7 : Strategies d'allocation personnalisees
   Description : Buddy Allocator (puissances de 2, division/fusion de blocs)
   Fichier source : 07-strategies-allocation.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define MIN_BLOCK_SIZE 32
#define MAX_LEVELS 10

typedef struct BuddyBlock {
    bool is_free;
    struct BuddyBlock* next;
} BuddyBlock;

typedef struct BuddyAllocator {
    char* memory;
    size_t size;
    BuddyBlock* free_lists[MAX_LEVELS];
} BuddyAllocator;

int get_level(size_t size) {
    if (size <= MIN_BLOCK_SIZE) return 0;
    return (int)ceil(log2((double)size / MIN_BLOCK_SIZE));
}

size_t get_level_size(int level) {
    return MIN_BLOCK_SIZE << level;
}

BuddyAllocator* buddy_create(size_t size) {
    BuddyAllocator* buddy = malloc(sizeof(BuddyAllocator));
    if (buddy == NULL) return NULL;

    size_t power = 1;
    while (power < size) {
        power <<= 1;
    }

    buddy->memory = malloc(power);
    if (buddy->memory == NULL) {
        free(buddy);
        return NULL;
    }

    buddy->size = power;

    for (int i = 0; i < MAX_LEVELS; i++) {
        buddy->free_lists[i] = NULL;
    }

    int max_level = get_level(power);
    if (max_level < MAX_LEVELS) {
        BuddyBlock* block = (BuddyBlock*)buddy->memory;
        block->is_free = true;
        block->next = NULL;
        buddy->free_lists[max_level] = block;
    }

    printf("Buddy allocator cree : %zu octets (%d niveaux)\n",
           power, max_level);

    return buddy;
}

void* buddy_alloc(BuddyAllocator* buddy, size_t size) {
    if (buddy == NULL) return NULL;

    int level = get_level(size);

    if (level >= MAX_LEVELS) {
        return NULL;
    }

    for (int i = level; i < MAX_LEVELS; i++) {
        if (buddy->free_lists[i] != NULL) {
            BuddyBlock* block = buddy->free_lists[i];
            buddy->free_lists[i] = block->next;

            while (i > level) {
                i--;
                size_t half_size = get_level_size(i);

                BuddyBlock* buddy_block =
                    (BuddyBlock*)((char*)block + half_size);
                buddy_block->is_free = true;
                buddy_block->next = buddy->free_lists[i];
                buddy->free_lists[i] = buddy_block;
            }

            block->is_free = false;
            return (void*)block;
        }
    }

    return NULL;
}

void buddy_free(BuddyAllocator* buddy, void* ptr, size_t size) {
    if (buddy == NULL || ptr == NULL) return;

    int level = get_level(size);

    BuddyBlock* block = (BuddyBlock*)ptr;
    block->is_free = true;

    block->next = buddy->free_lists[level];
    buddy->free_lists[level] = block;
}

void buddy_destroy(BuddyAllocator* buddy) {
    if (buddy != NULL) {
        free(buddy->memory);
        free(buddy);
    }
}

int main(void) {
    BuddyAllocator* buddy = buddy_create(1024);

    if (buddy == NULL) {
        return 1;
    }

    void* a = buddy_alloc(buddy, 40);
    void* b = buddy_alloc(buddy, 100);
    void* c = buddy_alloc(buddy, 200);

    printf("Allocations effectuees : %p, %p, %p\n", a, b, c);

    buddy_free(buddy, a, 40);
    buddy_free(buddy, c, 200);
    buddy_free(buddy, b, 100);

    buddy_destroy(buddy);

    return 0;
}
