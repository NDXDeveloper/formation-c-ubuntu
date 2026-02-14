/* ============================================================================
   Section 9.7 : Strategies d'allocation personnalisees
   Description : Pool Allocator (blocs de taille fixe, O(1) alloc/free)
   Fichier source : 07-strategies-allocation.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

typedef struct PoolAllocator {
    void* memory;
    void* free_list;
    size_t block_size;
    size_t num_blocks;
    size_t used_blocks;
} PoolAllocator;

PoolAllocator* pool_create(size_t block_size, size_t num_blocks) {
    PoolAllocator* pool = malloc(sizeof(PoolAllocator));
    if (pool == NULL) return NULL;

    block_size = (block_size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);

    size_t total_size = block_size * num_blocks;
    pool->memory = malloc(total_size);

    if (pool->memory == NULL) {
        free(pool);
        return NULL;
    }

    pool->block_size = block_size;
    pool->num_blocks = num_blocks;
    pool->used_blocks = 0;

    pool->free_list = pool->memory;

    char* current = (char*)pool->memory;
    for (size_t i = 0; i < num_blocks - 1; i++) {
        void** slot = (void**)(current + i * block_size);
        *slot = current + (i + 1) * block_size;
    }

    void** last = (void**)(current + (num_blocks - 1) * block_size);
    *last = NULL;

    printf("Pool cree : %zu blocs de %zu octets = %zu octets\n",
           num_blocks, block_size, total_size);

    return pool;
}

void* pool_alloc(PoolAllocator* pool) {
    if (pool == NULL || pool->free_list == NULL) {
        return NULL;
    }

    void* block = pool->free_list;
    pool->free_list = *(void**)block;
    pool->used_blocks++;

    return block;
}

void pool_free(PoolAllocator* pool, void* ptr) {
    if (pool == NULL || ptr == NULL) return;

    char* start = (char*)pool->memory;
    char* end = start + (pool->block_size * pool->num_blocks);

    if ((char*)ptr < start || (char*)ptr >= end) {
        fprintf(stderr, "Erreur : pointeur hors du pool\n");
        return;
    }

    *(void**)ptr = pool->free_list;
    pool->free_list = ptr;
    pool->used_blocks--;
}

void pool_destroy(PoolAllocator* pool) {
    if (pool != NULL) {
        free(pool->memory);
        free(pool);
    }
}

void pool_stats(const PoolAllocator* pool) {
    if (pool == NULL) return;

    printf("=== Pool Stats ===\n");
    printf("Taille bloc    : %zu octets\n", pool->block_size);
    printf("Nombre blocs   : %zu\n", pool->num_blocks);
    printf("Blocs utilises : %zu (%.1f%%)\n",
           pool->used_blocks,
           100.0 * pool->used_blocks / pool->num_blocks);
    printf("Blocs libres   : %zu\n", pool->num_blocks - pool->used_blocks);
}

int main(void) {
    PoolAllocator* pool = pool_create(sizeof(int), 10);

    if (pool == NULL) {
        fprintf(stderr, "Erreur de creation du pool\n");
        return 1;
    }

    int* a = pool_alloc(pool);
    int* b = pool_alloc(pool);
    int* c = pool_alloc(pool);

    if (a && b && c) {
        *a = 10;
        *b = 20;
        *c = 30;

        printf("a=%d, b=%d, c=%d\n", *a, *b, *c);

        pool_stats(pool);
    }

    pool_free(pool, b);
    pool_free(pool, a);

    pool_stats(pool);

    int* d = pool_alloc(pool);
    if (d) {
        *d = 40;
        printf("d=%d (bloc reutilise)\n", *d);
    }

    pool_free(pool, c);
    pool_free(pool, d);
    pool_destroy(pool);

    return 0;
}
