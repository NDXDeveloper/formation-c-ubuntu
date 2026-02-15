/* ============================================================================
   Section 24.2 : Custom allocators
   Description : Pool allocator - objets de taille fixe avec free individuel
   Fichier source : 02-custom-allocators.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure d'un bloc dans le pool
typedef struct PoolBlock {
    struct PoolBlock *next;  // Pointeur vers le prochain bloc libre
} PoolBlock;

// Structure du pool
typedef struct {
    void *memory;           // Memoire du pool
    PoolBlock *free_list;   // Liste des blocs libres
    size_t block_size;      // Taille de chaque bloc
    size_t num_blocks;      // Nombre total de blocs
    size_t num_allocated;   // Nombre de blocs alloues
} Pool;

// Creer un pool
Pool pool_create(size_t block_size, size_t num_blocks) {
    Pool pool = {0};

    // S'assurer que block_size peut contenir un pointeur
    if (block_size < sizeof(PoolBlock)) {
        block_size = sizeof(PoolBlock);
    }

    pool.block_size = block_size;
    pool.num_blocks = num_blocks;
    pool.num_allocated = 0;

    // Allouer toute la memoire d'un coup
    pool.memory = malloc(block_size * num_blocks);
    if (!pool.memory) {
        return pool;
    }

    // Initialiser la liste libre
    // Chaque bloc pointe vers le suivant
    pool.free_list = (PoolBlock *)pool.memory;
    PoolBlock *current = pool.free_list;

    for (size_t i = 0; i < num_blocks - 1; i++) {
        PoolBlock *next = (PoolBlock *)((char *)current + block_size);
        current->next = next;
        current = next;
    }

    current->next = NULL;  // Dernier bloc

    return pool;
}

// Allouer un objet depuis le pool
void *pool_alloc(Pool *pool) {
    if (pool->free_list == NULL) {
        fprintf(stderr, "Pool plein !\n");
        return NULL;
    }

    // Prendre le premier bloc libre
    PoolBlock *block = pool->free_list;
    pool->free_list = block->next;
    pool->num_allocated++;

    // Effacer la memoire (optionnel mais recommande)
    memset(block, 0, pool->block_size);

    return block;
}

// Liberer un objet vers le pool
void pool_free(Pool *pool, void *ptr) {
    if (ptr == NULL) return;

    // Remettre le bloc en tete de la liste libre
    PoolBlock *block = (PoolBlock *)ptr;
    block->next = pool->free_list;
    pool->free_list = block;
    pool->num_allocated--;
}

// Detruire le pool
void pool_destroy(Pool *pool) {
    free(pool->memory);
    pool->memory = NULL;
    pool->free_list = NULL;
    pool->num_allocated = 0;
}

// Statistiques du pool
void pool_stats(Pool *pool) {
    printf("=== Pool Stats ===\n");
    printf("Block size: %zu bytes\n", pool->block_size);
    printf("Total blocks: %zu\n", pool->num_blocks);
    printf("Allocated: %zu\n", pool->num_allocated);
    printf("Free: %zu\n", pool->num_blocks - pool->num_allocated);
    printf("Memory usage: %zu bytes\n",
           pool->block_size * pool->num_blocks);
}

// Structure d'exemple
typedef struct {
    int id;
    char name[32];
    float value;
} Entity;

void exemple_pool(void) {
    // Creer un pool de 100 entites
    Pool entity_pool = pool_create(sizeof(Entity), 100);

    if (!entity_pool.memory) {
        fprintf(stderr, "Erreur creation pool\n");
        return;
    }

    printf("Pool cree avec succes\n");
    pool_stats(&entity_pool);

    // Allouer des entites
    Entity *e1 = pool_alloc(&entity_pool);
    Entity *e2 = pool_alloc(&entity_pool);
    Entity *e3 = pool_alloc(&entity_pool);

    if (e1 && e2 && e3) {
        e1->id = 1;
        strcpy(e1->name, "Entity 1");
        e1->value = 10.5f;

        e2->id = 2;
        strcpy(e2->name, "Entity 2");
        e2->value = 20.3f;

        e3->id = 3;
        strcpy(e3->name, "Entity 3");
        e3->value = 15.7f;

        printf("\nEntites creees:\n");
        printf("- %s (ID: %d, Value: %.1f)\n", e1->name, e1->id, (double)e1->value);
        printf("- %s (ID: %d, Value: %.1f)\n", e2->name, e2->id, (double)e2->value);
        printf("- %s (ID: %d, Value: %.1f)\n", e3->name, e3->id, (double)e3->value);
    }

    printf("\nApres allocations:\n");
    pool_stats(&entity_pool);

    // Liberer une entite
    pool_free(&entity_pool, e2);
    e2 = NULL;  // Bonne pratique

    printf("\nApres liberation de e2:\n");
    pool_stats(&entity_pool);

    // Reallouer (va reutiliser l'emplacement de e2)
    Entity *e4 = pool_alloc(&entity_pool);
    if (e4) {
        e4->id = 4;
        strcpy(e4->name, "Entity 4");
        e4->value = 99.9f;
        printf("\nNouvelle entite: %s (ID: %d, Value: %.1f)\n",
               e4->name, e4->id, (double)e4->value);
    }

    // Liberer toutes les entites
    pool_free(&entity_pool, e1);
    pool_free(&entity_pool, e3);
    pool_free(&entity_pool, e4);

    printf("\nToutes les entites liberees:\n");
    pool_stats(&entity_pool);

    // Detruire le pool
    pool_destroy(&entity_pool);
}

int main(void) {
    exemple_pool();
    return 0;
}
