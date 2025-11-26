🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 9.7 Stratégies d'allocation personnalisées

## Introduction

Bien que `malloc()` et `free()` soient pratiques et polyvalents, ils ne sont pas toujours optimaux pour tous les cas d'usage. Dans certaines situations, créer votre **propre allocateur mémoire** peut apporter des avantages significatifs en termes de :

- 🚀 **Performance** : Allocations/libérations plus rapides
- 📊 **Prévisibilité** : Comportement déterministe
- 🎯 **Contrôle** : Gestion fine de la mémoire
- 🔍 **Débogage** : Traçage et statistiques intégrées
- 💾 **Optimisation** : Réduction de la fragmentation

> **Note :** Les allocateurs personnalisés sont un sujet avancé, mais comprendre les concepts de base vous aidera à mieux utiliser la mémoire et à lire du code professionnel.

Dans cette section, vous découvrirez :
- Quand et pourquoi créer un allocateur personnalisé
- Les principaux types d'allocateurs
- Implémentations simples et pratiques
- Avantages et limitations de chaque approche
- Cas d'usage concrets

---

## Pourquoi créer un allocateur personnalisé ?

### Limitations de malloc/free

| Problème | Description | Impact |
|----------|-------------|--------|
| **Lenteur** | Recherche dans la free list | ~100 ns par allocation |
| **Fragmentation** | Petits trous inutilisables | Gaspillage mémoire |
| **Overhead** | Métadonnées par bloc | 8-16 octets par bloc |
| **Imprévisibilité** | Temps variable | Problème en temps réel |
| **Contention** | Lock global en multi-thread | Ralentissement parallèle |

### Cas d'usage des allocateurs personnalisés

- ✅ **Jeux vidéo** : Allocations très fréquentes, prévisibilité nécessaire
- ✅ **Systèmes embarqués** : Mémoire limitée, pas de fragmentation
- ✅ **Serveurs haute performance** : Millions d'allocations/seconde
- ✅ **Parsers/Compilateurs** : Vie courte, libération en masse
- ✅ **Applications temps réel** : Latence déterministe requise

### Exemple de gain de performance

```c
// Benchmark : 1 million d'allocations/libérations

// Avec malloc/free standard
Temps : 150 ms
Fragmentation : ~20%

// Avec pool allocator personnalisé
Temps : 15 ms    (10x plus rapide !)
Fragmentation : 0%
```

---

## Types d'allocateurs personnalisés

### Vue d'ensemble

```
Allocateurs personnalisés courants :

┌────────────────────────────────────────────┐
│  1. Pool Allocator (Object Pool)           │
│     → Blocs de taille fixe                 │
│     → Rapide, pas de fragmentation         │
└────────────────────────────────────────────┘

┌────────────────────────────────────────────┐
│  2. Arena Allocator (Bump Allocator)       │
│     → Pointeur qui avance                  │
│     → Très rapide, libération en masse     │
└────────────────────────────────────────────┘

┌────────────────────────────────────────────┐
│  3. Stack Allocator (Linear Allocator)     │
│     → LIFO (dernier alloué = premier free) │
│     → Ultra-rapide, temporaire             │
└────────────────────────────────────────────┘

┌────────────────────────────────────────────┐
│  4. Free List Allocator                    │
│     → Liste chaînée de blocs libres        │
│     → Similaire à malloc mais simplifié    │
└────────────────────────────────────────────┘

┌────────────────────────────────────────────┐
│  5. Buddy Allocator                        │
│     → Découpage en puissances de 2         │
│     → Réduction fragmentation externe      │
└────────────────────────────────────────────┘
```

---

## 1. Pool Allocator (Object Pool)

### Concept

Un **pool allocator** gère un bloc de mémoire divisé en **slots de taille fixe**. Idéal quand vous allouez souvent des objets de même taille.

```
Pool de 10 objets de 32 octets :

┌────┬────┬────┬────┬────┬────┬────┬────┬────┬────┐
│ 0  │ 1  │ 2  │ 3  │ 4  │ 5  │ 6  │ 7  │ 8  │ 9  │
│ L  │ L  │ O  │ L  │ O  │ L  │ L  │ O  │ L  │ L  │
└────┴────┴────┴────┴────┴────┴────┴────┴────┴────┘
  ↑
  Free list : 0 → 1 → 3 → 5 → 6 → 8 → 9 → NULL

L = Libre (dans la free list)
O = Occupé (alloué à l'utilisateur)

pool_alloc() : Prendre le premier de la free list (O(1))
pool_free()  : Ajouter au début de la free list (O(1))
```

### Implémentation simple

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct PoolAllocator {
    void* memory;           // Bloc de mémoire principal
    void* free_list;        // Liste des slots libres
    size_t block_size;      // Taille d'un slot
    size_t num_blocks;      // Nombre total de slots
    size_t used_blocks;     // Nombre de slots utilisés
} PoolAllocator;

// Créer un pool
PoolAllocator* pool_create(size_t block_size, size_t num_blocks) {
    PoolAllocator* pool = malloc(sizeof(PoolAllocator));
    if (pool == NULL) return NULL;

    // Aligner la taille du bloc sur sizeof(void*)
    block_size = (block_size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);

    // Allouer le bloc principal
    size_t total_size = block_size * num_blocks;
    pool->memory = malloc(total_size);

    if (pool->memory == NULL) {
        free(pool);
        return NULL;
    }

    pool->block_size = block_size;
    pool->num_blocks = num_blocks;
    pool->used_blocks = 0;

    // Initialiser la free list : chaque bloc pointe vers le suivant
    pool->free_list = pool->memory;

    char* current = (char*)pool->memory;
    for (size_t i = 0; i < num_blocks - 1; i++) {
        void** slot = (void**)(current + i * block_size);
        *slot = current + (i + 1) * block_size;
    }

    // Dernier bloc pointe vers NULL
    void** last = (void**)(current + (num_blocks - 1) * block_size);
    *last = NULL;

    printf("Pool créé : %zu blocs de %zu octets = %zu octets\n",
           num_blocks, block_size, total_size);

    return pool;
}

// Allouer depuis le pool
void* pool_alloc(PoolAllocator* pool) {
    if (pool == NULL || pool->free_list == NULL) {
        return NULL;  // Pool plein
    }

    // Prendre le premier bloc libre
    void* block = pool->free_list;

    // Mettre à jour la free list
    pool->free_list = *(void**)block;

    pool->used_blocks++;

    return block;
}

// Libérer vers le pool
void pool_free(PoolAllocator* pool, void* ptr) {
    if (pool == NULL || ptr == NULL) return;

    // Vérifier que ptr est dans les limites du pool
    char* start = (char*)pool->memory;
    char* end = start + (pool->block_size * pool->num_blocks);

    if ((char*)ptr < start || (char*)ptr >= end) {
        fprintf(stderr, "Erreur : pointeur hors du pool\n");
        return;
    }

    // Ajouter le bloc en tête de la free list
    *(void**)ptr = pool->free_list;
    pool->free_list = ptr;

    pool->used_blocks--;
}

// Détruire le pool
void pool_destroy(PoolAllocator* pool) {
    if (pool != NULL) {
        free(pool->memory);
        free(pool);
    }
}

// Statistiques
void pool_stats(const PoolAllocator* pool) {
    if (pool == NULL) return;

    printf("=== Pool Stats ===\n");
    printf("Taille bloc    : %zu octets\n", pool->block_size);
    printf("Nombre blocs   : %zu\n", pool->num_blocks);
    printf("Blocs utilisés : %zu (%.1f%%)\n",
           pool->used_blocks,
           100.0 * pool->used_blocks / pool->num_blocks);
    printf("Blocs libres   : %zu\n", pool->num_blocks - pool->used_blocks);
}

// Exemple d'utilisation
int main() {
    // Créer un pool pour des entiers
    PoolAllocator* pool = pool_create(sizeof(int), 10);

    if (pool == NULL) {
        fprintf(stderr, "Erreur de création du pool\n");
        return 1;
    }

    // Allouer plusieurs entiers
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

    // Libérer
    pool_free(pool, b);
    pool_free(pool, a);

    pool_stats(pool);

    // Réallouer (réutilise les blocs libérés)
    int* d = pool_alloc(pool);
    if (d) {
        *d = 40;
        printf("d=%d (bloc réutilisé)\n", *d);
    }

    // Nettoyage
    pool_free(pool, c);
    pool_free(pool, d);
    pool_destroy(pool);

    return 0;
}
```

### Avantages et inconvénients

| Avantage | Inconvénient |
|----------|--------------|
| ✅ Très rapide (O(1)) | ❌ Taille fixe seulement |
| ✅ Pas de fragmentation | ❌ Mémoire pré-allouée (potentiel gaspillage) |
| ✅ Localité cache excellente | ❌ Pool plein = échec |
| ✅ Déterministe | ❌ Un pool par type d'objet |

---

## 2. Arena Allocator (Bump Allocator)

### Concept

Un **arena allocator** maintient un pointeur qui "avance" dans un grand bloc de mémoire. Très rapide mais ne permet pas de libération individuelle.

```
Arena de 1000 octets :

État initial :
┌──────────────────────────────────────┐
│  [      Mémoire libre        ]       │
└──────────────────────────────────────┘
 ↑
 Offset = 0

Après alloc(50) :
┌──────────────────────────────────────┐
│  [Alloué:50B][   Mémoire libre   ]   │
└──────────────────────────────────────┘
                ↑
                Offset = 50

Après alloc(100) :
┌──────────────────────────────────────┐
│  [Alloué:50][Alloué:100][  Libre ]   │
└──────────────────────────────────────┘
                           ↑
                           Offset = 150

Libération : Reset offset = 0 (tout en une fois)
```

### Implémentation

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Arena {
    char* memory;      // Bloc de mémoire
    size_t size;       // Taille totale
    size_t offset;     // Position actuelle
    size_t peak;       // Utilisation maximale atteinte
} Arena;

// Créer une arena
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

    printf("Arena créée : %zu octets\n", size);
    return arena;
}

// Allouer depuis l'arena
void* arena_alloc(Arena* arena, size_t size) {
    if (arena == NULL) return NULL;

    // Alignement sur 8 octets
    size = (size + 7) & ~7;

    // Vérifier si assez d'espace
    if (arena->offset + size > arena->size) {
        fprintf(stderr, "Arena pleine : demandé %zu, reste %zu\n",
                size, arena->size - arena->offset);
        return NULL;
    }

    // Allouer
    void* ptr = arena->memory + arena->offset;
    arena->offset += size;

    // Mettre à jour le peak
    if (arena->offset > arena->peak) {
        arena->peak = arena->offset;
    }

    return ptr;
}

// Reset l'arena (libération en masse)
void arena_reset(Arena* arena) {
    if (arena != NULL) {
        printf("Arena reset : %zu octets libérés\n", arena->offset);
        arena->offset = 0;
    }
}

// Détruire l'arena
void arena_destroy(Arena* arena) {
    if (arena != NULL) {
        free(arena->memory);
        free(arena);
    }
}

// Statistiques
void arena_stats(const Arena* arena) {
    if (arena == NULL) return;

    printf("=== Arena Stats ===\n");
    printf("Taille totale  : %zu octets\n", arena->size);
    printf("Utilisé        : %zu octets (%.1f%%)\n",
           arena->offset,
           100.0 * arena->offset / arena->size);
    printf("Libre          : %zu octets\n", arena->size - arena->offset);
    printf("Peak usage     : %zu octets (%.1f%%)\n",
           arena->peak,
           100.0 * arena->peak / arena->size);
}

// Exemple : Parser de texte
typedef struct Token {
    char* text;
    int line;
    int column;
} Token;

Token* parse_tokens(Arena* arena, const char* source) {
    // Simulation simple : créer quelques tokens
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

int main() {
    // Créer une arena pour le parsing
    Arena* arena = arena_create(1024 * 1024);  // 1 MB

    if (arena == NULL) {
        return 1;
    }

    // Parser plusieurs fichiers
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

        // ✅ Libération en masse après chaque fichier
        arena_reset(arena);
    }

    // Nettoyage final
    arena_destroy(arena);

    return 0;
}
```

### Avantages et inconvénients

| Avantage | Inconvénient |
|----------|--------------|
| ✅ Ultra-rapide (simple addition) | ❌ Pas de libération individuelle |
| ✅ Pas de fragmentation | ❌ Mémoire gaspillée si sous-utilisé |
| ✅ Parfait pour durée de vie courte | ❌ Overflow si trop d'allocations |
| ✅ Excellente localité cache | ❌ Pas adapté aux longues durées |

**Cas d'usage idéaux :**
- Parsers et compilateurs
- Traitement par frame (jeux)
- Requêtes web (durée de vie = une requête)
- Génération de rapports

---

## 3. Stack Allocator

### Concept

Un **stack allocator** fonctionne comme la Stack système : LIFO (Last In, First Out). Le dernier bloc alloué doit être le premier libéré.

```
Stack Allocator :

┌────────────────────────────────────┐
│  [   Espace libre           ]      │
└────────────────────────────────────┘
 ↑
 Top = 0

Alloc A (50B) :
┌────────────────────────────────────┐
│  [A: 50B][     Libre          ]    │
└────────────────────────────────────┘
          ↑
          Top = 50

Alloc B (100B) :
┌────────────────────────────────────┐
│  [A: 50B][B: 100B][   Libre   ]    │
└────────────────────────────────────┘
                   ↑
                   Top = 150

✅ Free B : Top = 50
✅ Free A : Top = 0

❌ Free A avant B : ERREUR (violation LIFO)
```

### Implémentation

```c
#include <stdio.h>
#include <stdlib.h>
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

    // Espace pour le header + alignement
    size_t total = sizeof(StackHeader) + size;
    total = (total + 7) & ~7;

    if (stack->offset + total > stack->size) {
        return NULL;
    }

    // Placer le header
    StackHeader* header = (StackHeader*)(stack->memory + stack->offset);
    header->size = total;

    stack->offset += total;

    // Retourner l'adresse après le header
    return (void*)(header + 1);
}

bool stack_free(StackAllocator* stack, void* ptr) {
    if (stack == NULL || ptr == NULL) return false;

    // Récupérer le header
    StackHeader* header = ((StackHeader*)ptr) - 1;

    // Vérifier que c'est le dernier bloc alloué (LIFO)
    char* expected_end = (char*)header + header->size;
    char* actual_end = stack->memory + stack->offset;

    if (expected_end != actual_end) {
        fprintf(stderr, "Erreur : violation LIFO (free dans le mauvais ordre)\n");
        return false;
    }

    // Libérer
    stack->offset -= header->size;
    return true;
}

void stack_destroy(StackAllocator* stack) {
    if (stack != NULL) {
        free(stack->memory);
        free(stack);
    }
}

// Exemple d'utilisation
int main() {
    StackAllocator* stack = stack_create(1024);

    if (stack == NULL) {
        return 1;
    }

    // Allocations LIFO
    int* a = stack_alloc(stack, sizeof(int));
    char* b = stack_alloc(stack, 100);
    double* c = stack_alloc(stack, sizeof(double));

    if (a && b && c) {
        *a = 42;
        strcpy(b, "Hello");
        *c = 3.14;

        printf("a=%d, b=%s, c=%.2f\n", *a, b, *c);
    }

    // ✅ Libération LIFO (inverse de l'allocation)
    stack_free(stack, c);
    stack_free(stack, b);
    stack_free(stack, a);

    // ❌ Ceci causerait une erreur :
    // stack_free(stack, a);  // a n'est pas le dernier alloué

    stack_destroy(stack);
    return 0;
}
```

### Avantages et inconvénients

| Avantage | Inconvénient |
|----------|--------------|
| ✅ Très rapide | ❌ Ordre LIFO strict |
| ✅ Pas de fragmentation | ❌ Peu flexible |
| ✅ Localité cache | ❌ Difficulté à utiliser correctement |

---

## 4. Free List Allocator

### Concept

Similaire à `malloc()` mais simplifié. Maintient une liste chaînée de blocs libres de tailles variables.

```
Free List :

┌───────────────────────────────────────────┐
│ [Bloc A: 32B][Bloc B: 64B][Bloc C: 128B]  │
└───────────────────────────────────────────┘

Free List : A(32) → C(128) → NULL
            B est occupé

Alloc(48) : Cherche un bloc ≥ 48 dans la liste
            → Trouve C(128), le découpe en 48 + 80
            → Retourne 48, ajoute le reste (80) à la liste

Free List : A(32) → Reste(80) → NULL
```

### Implémentation simplifiée

```c
#include <stdio.h>
#include <stdlib.h>
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

    // Initialiser avec un seul gros bloc libre
    allocator->free_list = (FreeBlock*)allocator->memory;
    allocator->free_list->size = size;
    allocator->free_list->next = NULL;

    return allocator;
}

void* freelist_alloc(FreeListAllocator* allocator, size_t size) {
    if (allocator == NULL || allocator->free_list == NULL) {
        return NULL;
    }

    // Aligner la taille
    size = (size + sizeof(FreeBlock) + 7) & ~7;

    // First-fit : chercher le premier bloc assez grand
    FreeBlock* prev = NULL;
    FreeBlock* current = allocator->free_list;

    while (current != NULL) {
        if (current->size >= size) {
            // Bloc trouvé

            // Si le reste est assez grand, le garder dans la liste
            size_t remainder = current->size - size;

            if (remainder > sizeof(FreeBlock) + 16) {
                // Découper le bloc
                FreeBlock* new_block = (FreeBlock*)((char*)current + size);
                new_block->size = remainder;
                new_block->next = current->next;

                if (prev) {
                    prev->next = new_block;
                } else {
                    allocator->free_list = new_block;
                }
            } else {
                // Prendre le bloc entier
                if (prev) {
                    prev->next = current->next;
                } else {
                    allocator->free_list = current->next;
                }
            }

            // Retourner le pointeur (après le header)
            return (void*)((char*)current + sizeof(FreeBlock));
        }

        prev = current;
        current = current->next;
    }

    return NULL;  // Pas de bloc assez grand
}

void freelist_free(FreeListAllocator* allocator, void* ptr) {
    if (allocator == NULL || ptr == NULL) return;

    // Récupérer le header
    FreeBlock* block = (FreeBlock*)((char*)ptr - sizeof(FreeBlock));

    // Ajouter au début de la free list (simple)
    block->next = allocator->free_list;
    allocator->free_list = block;

    // Note : Une vraie implémentation devrait fusionner les blocs adjacents
}

void freelist_destroy(FreeListAllocator* allocator) {
    if (allocator != NULL) {
        free(allocator->memory);
        free(allocator);
    }
}

int main() {
    FreeListAllocator* allocator = freelist_create(1024);

    if (allocator == NULL) {
        return 1;
    }

    // Allocations de tailles variées
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

    // Libérations
    freelist_free(allocator, b);
    freelist_free(allocator, a);

    // Réallocation (réutilise les blocs libérés)
    int* d = freelist_alloc(allocator, sizeof(int));
    if (d) {
        *d = 100;
        printf("d=%d (bloc réutilisé)\n", *d);
    }

    freelist_free(allocator, d);
    freelist_free(allocator, c);
    freelist_destroy(allocator);

    return 0;
}
```

---

## 5. Buddy Allocator

### Concept

Le **buddy allocator** découpe la mémoire en blocs de puissances de 2. Réduit la fragmentation externe en permettant la fusion de blocs adjacents.

```
Buddy System (1024 octets) :

Niveau 0 :  [          1024          ]

Niveau 1 :  [    512    ][    512    ]

Niveau 2 :  [  256  ][  256  ][  256  ][  256  ]

Alloc(100) : Trouve le plus petit bloc ≥ 100
            → 128 octets (niveau 3)
            → Découpe récursivement

Free : Fusionne avec le "buddy" si les deux sont libres
```

### Implémentation simplifiée (conceptuelle)

```c
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

// Calculer le niveau pour une taille donnée
int get_level(size_t size) {
    if (size <= MIN_BLOCK_SIZE) return 0;
    return (int)ceil(log2(size / MIN_BLOCK_SIZE));
}

// Calculer la taille d'un niveau
size_t get_level_size(int level) {
    return MIN_BLOCK_SIZE << level;  // MIN * 2^level
}

BuddyAllocator* buddy_create(size_t size) {
    BuddyAllocator* buddy = malloc(sizeof(BuddyAllocator));
    if (buddy == NULL) return NULL;

    // Arrondir à la puissance de 2 supérieure
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

    // Initialiser les free lists
    for (int i = 0; i < MAX_LEVELS; i++) {
        buddy->free_lists[i] = NULL;
    }

    // Mettre tout le bloc au niveau max
    int max_level = get_level(power);
    if (max_level < MAX_LEVELS) {
        BuddyBlock* block = (BuddyBlock*)buddy->memory;
        block->is_free = true;
        block->next = NULL;
        buddy->free_lists[max_level] = block;
    }

    printf("Buddy allocator créé : %zu octets (%d niveaux)\n",
           power, max_level);

    return buddy;
}

void* buddy_alloc(BuddyAllocator* buddy, size_t size) {
    if (buddy == NULL) return NULL;

    int level = get_level(size);

    if (level >= MAX_LEVELS) {
        return NULL;
    }

    // Chercher un bloc au bon niveau ou plus grand
    for (int i = level; i < MAX_LEVELS; i++) {
        if (buddy->free_lists[i] != NULL) {
            // Bloc trouvé, le découper si nécessaire
            BuddyBlock* block = buddy->free_lists[i];
            buddy->free_lists[i] = block->next;

            // Découper récursivement jusqu'au niveau voulu
            while (i > level) {
                i--;
                size_t half_size = get_level_size(i);

                // Créer le buddy (moitié du bloc)
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

    return NULL;  // Pas de mémoire disponible
}

void buddy_free(BuddyAllocator* buddy, void* ptr, size_t size) {
    if (buddy == NULL || ptr == NULL) return;

    int level = get_level(size);

    BuddyBlock* block = (BuddyBlock*)ptr;
    block->is_free = true;

    // Ajouter à la free list
    block->next = buddy->free_lists[level];
    buddy->free_lists[level] = block;

    // Note : Une vraie implémentation fusionnerait avec le buddy
}

void buddy_destroy(BuddyAllocator* buddy) {
    if (buddy != NULL) {
        free(buddy->memory);
        free(buddy);
    }
}

int main() {
    BuddyAllocator* buddy = buddy_create(1024);

    if (buddy == NULL) {
        return 1;
    }

    // Allocations
    void* a = buddy_alloc(buddy, 40);   // 64 octets
    void* b = buddy_alloc(buddy, 100);  // 128 octets
    void* c = buddy_alloc(buddy, 200);  // 256 octets

    printf("Allocations effectuées : %p, %p, %p\n", a, b, c);

    // Libérations
    buddy_free(buddy, a, 40);
    buddy_free(buddy, c, 200);
    buddy_free(buddy, b, 100);

    buddy_destroy(buddy);

    return 0;
}
```

---

## Comparaison des allocateurs

### Tableau récapitulatif

| Allocateur | Vitesse | Fragmentation | Libération | Complexité | Cas d'usage |
|------------|---------|---------------|------------|------------|-------------|
| **malloc/free** | Moyenne | Possible | Individuelle | Moyenne | Général |
| **Pool** | Très rapide | Aucune | Individuelle | Faible | Objets fixes |
| **Arena** | Ultra-rapide | Aucune | En masse | Très faible | Durée courte |
| **Stack** | Très rapide | Aucune | LIFO | Faible | Scope imbriqués |
| **Free List** | Rapide | Possible | Individuelle | Moyenne | Tailles variées |
| **Buddy** | Rapide | Réduite | Individuelle + fusion | Élevée | Kernel, systèmes |

### Benchmark comparatif

```
Test : 1 million d'allocations/libérations de 64 octets

malloc/free        : 150 ms  (100% baseline)
Pool Allocator     :  10 ms  (15x plus rapide)
Arena Allocator    :   5 ms  (30x plus rapide)
Stack Allocator    :   8 ms  (19x plus rapide)
Free List          :  40 ms  (4x plus rapide)
Buddy Allocator    :  60 ms  (2.5x plus rapide)
```

---

## Patterns d'utilisation avancés

### Pattern 1 : Allocateur par frame (jeux)

```c
typedef struct GameFrame {
    Arena* temp_arena;    // Reset chaque frame
    Pool* entity_pool;    // Réutilisé entre frames
} GameFrame;

void game_loop() {
    Arena* temp = arena_create(1024 * 1024);  // 1 MB par frame
    Pool* entities = pool_create(sizeof(Entity), 10000);

    while (game_running) {
        // Tout le travail temporaire dans l'arena
        Particle* particles = arena_alloc(temp, 1000 * sizeof(Particle));
        RenderCommand* commands = arena_alloc(temp, 5000 * sizeof(RenderCommand));

        // Entités persistantes dans le pool
        Entity* player = pool_alloc(entities);

        // ... logique du jeu ...

        // ✅ Libération ultra-rapide de tout le temporaire
        arena_reset(temp);
    }

    arena_destroy(temp);
    pool_destroy(entities);
}
```

### Pattern 2 : Allocateur hiérarchique

```c
typedef struct HierarchicalAllocator {
    Arena* persistent;    // Données de session
    Arena* per_request;   // Données par requête
    Pool* small_objects;  // Petits objets fréquents
} HierarchicalAllocator;

void handle_request(HierarchicalAllocator* alloc) {
    // Données persistantes
    User* user = arena_alloc(alloc->persistent, sizeof(User));

    // Données temporaires de la requête
    Buffer* temp = arena_alloc(alloc->per_request, 4096);

    // Petits objets réutilisés
    Token* token = pool_alloc(alloc->small_objects);

    // Traitement...

    // ✅ Libération rapide des données de requête
    arena_reset(alloc->per_request);
    pool_free(alloc->small_objects, token);
}
```

---

## Debugging et instrumentation

### Allocateur avec statistiques

```c
typedef struct InstrumentedPool {
    PoolAllocator* pool;
    size_t total_allocs;
    size_t total_frees;
    size_t peak_usage;
    size_t current_usage;
} InstrumentedPool;

void* instrumented_alloc(InstrumentedPool* ipool) {
    void* ptr = pool_alloc(ipool->pool);

    if (ptr != NULL) {
        ipool->total_allocs++;
        ipool->current_usage++;

        if (ipool->current_usage > ipool->peak_usage) {
            ipool->peak_usage = ipool->current_usage;
        }
    }

    return ptr;
}

void instrumented_free(InstrumentedPool* ipool, void* ptr) {
    pool_free(ipool->pool, ptr);
    ipool->total_frees++;
    ipool->current_usage--;
}

void print_stats(const InstrumentedPool* ipool) {
    printf("=== Allocator Stats ===\n");
    printf("Total allocations : %zu\n", ipool->total_allocs);
    printf("Total libérations : %zu\n", ipool->total_frees);
    printf("Peak usage        : %zu blocs\n", ipool->peak_usage);
    printf("Leaks potentiels  : %zu blocs\n",
           ipool->total_allocs - ipool->total_frees);
}
```

---

## Bonnes pratiques

### ✅ Choisir le bon allocateur

```
Posez-vous ces questions :

1. Taille des objets ?
   → Fixe  : Pool
   → Variée : Free List ou malloc

2. Durée de vie ?
   → Courte uniforme : Arena
   → Longue variée   : malloc ou Pool

3. Pattern de libération ?
   → LIFO     : Stack
   → En masse : Arena
   → Arbitraire : Pool ou malloc

4. Performance critique ?
   → Oui : Pool ou Arena
   → Non : malloc suffit
```

### ✅ Documenter le choix

```c
/**
 * Pool pour les entités du jeu
 *
 * Rationale :
 * - Taille fixe (sizeof(Entity))
 * - Créées/détruites fréquemment (1000x par seconde)
 * - malloc trop lent (50ms → 5ms avec pool)
 * - Pas de fragmentation requise
 */
PoolAllocator* entity_pool = pool_create(sizeof(Entity), 10000);
```

### ✅ Tester et profiler

```bash
# Comparer les performances
time ./programme_malloc
time ./programme_custom_allocator

# Vérifier les fuites
valgrind --leak-check=full ./programme

# Profiler
perf stat -e cache-misses ./programme
```

---

## Limitations et pièges

### ❌ Erreurs courantes

**1. Pool trop petit**
```c
Pool* pool = pool_create(sizeof(Object), 10);

// Si on a besoin de plus de 10 objets → échec !
for (int i = 0; i < 100; i++) {
    Object* obj = pool_alloc(pool);  // NULL après 10
}
```

**2. Oublier de reset l'arena**
```c
Arena* arena = arena_create(1024);

for (int i = 0; i < 1000; i++) {
    char* buf = arena_alloc(arena, 512);
    // ... utilisation ...
    // ❌ Oubli de arena_reset() → Arena pleine !
}
```

**3. Violer le LIFO du stack**
```c
void* a = stack_alloc(stack, 100);
void* b = stack_alloc(stack, 200);

stack_free(stack, a);  // ❌ ERREUR : pas LIFO !
```

---

## Récapitulatif

### Points clés

1. **malloc/free ne sont pas optimaux** pour tous les cas
2. **Pool** : objets de taille fixe, très rapide
3. **Arena** : durée de vie courte, libération en masse
4. **Stack** : ordre LIFO, ultra-rapide
5. **Free List** : tailles variées, comme malloc simplifié
6. **Buddy** : réduction de fragmentation, plus complexe

### Quand utiliser un allocateur personnalisé ?

- ✅ **Performance critique** (millions d'allocs/sec)
- ✅ **Pattern prévisible** (taille fixe, durée courte)
- ✅ **Temps réel** (latence déterministe)
- ✅ **Mémoire contrainte** (embarqué, console)
- ✅ **Profiling montre** que malloc est un goulot

### Quand utiliser malloc/free ?

- ✅ **Prototype rapide**
- ✅ **Tailles et durées imprévisibles**
- ✅ **Pas de contrainte de performance**
- ✅ **Code simple prioritaire**

---

## Pour aller plus loin

### Implémentations réelles

- **jemalloc** : Allocateur multi-thread performant
- **tcmalloc** : Google's Thread-Caching Malloc
- **mimalloc** : Microsoft's allocateur compact
- **dlmalloc** : Doug Lea's malloc (base de la glibc)

### Ressources

- "Game Engine Architecture" (Jason Gregory) - Chapitres sur la mémoire
- "Memory Management" (Linux Kernel Documentation)
- "Malloc Internals" - glibc source code
- Papers : Wilson et al. "Dynamic Storage Allocation: A Survey"

### Sections connexes

- **Section 27.3** : Profiling et optimisation
- **Section 24.2** : Custom allocators avancés
- **Module 6** : Programmation système (gestion mémoire bas niveau)

---


⏭️ [Types définis par l'utilisateur](/10-types-utilisateur/README.md)
