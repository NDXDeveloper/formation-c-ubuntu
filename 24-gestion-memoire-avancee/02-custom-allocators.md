🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 24.2 Custom allocators

## Introduction

### Qu'est-ce qu'un allocateur personnalisé ?

Jusqu'à présent, nous avons utilisé les fonctions d'allocation standard de la bibliothèque C :
- `malloc()` pour allouer de la mémoire
- `calloc()` pour allouer et initialiser à zéro
- `realloc()` pour redimensionner une allocation
- `free()` pour libérer la mémoire

Ces fonctions sont **généralistes** : elles fonctionnent pour tous les cas d'usage, mais ne sont **pas optimisées pour des besoins spécifiques**.

Un **allocateur personnalisé (custom allocator)** est une implémentation alternative de la gestion mémoire, conçue pour des besoins particuliers :
- Performance accrue dans certains scénarios
- Utilisation mémoire optimisée
- Gestion simplifiée de la durée de vie des objets
- Débogage et profilage facilités

---

## Pourquoi créer un allocateur personnalisé ?

### Les limitations de malloc/free

`malloc()` et `free()` sont des fonctions remarquables, mais elles ont des compromis :

#### 1. **Overhead de performance**

Chaque appel à `malloc()` ou `free()` a un coût :
```c
// Chaque allocation coûte du temps
for (int i = 0; i < 10000; i++) {
    char *ptr = malloc(64);  // Appel système ou gestion complexe
    // ... utilisation ...
    free(ptr);               // Coût de libération
}
```

**Pourquoi c'est lent ?**
- Recherche d'un bloc libre de la bonne taille
- Gestion des métadonnées (taille du bloc, état, etc.)
- Synchronisation thread-safe (locks internes)
- Fragmentation et compaction

#### 2. **Fragmentation mémoire**

Avec de nombreuses allocations/libérations, la mémoire peut se fragmenter :

```
Mémoire au début :
[════════════════════════════════] 100% libre

Après plusieurs allocations :
[XXXX][libre][XXX][libre][XX][libre][XXXXX]
      ↑           ↑          ↑
      Trous qui ne peuvent pas être utilisés efficacement
```

**Problème :** Même si vous avez assez de mémoire totale, vous ne pouvez peut-être pas allouer un gros bloc contigu.

#### 3. **Overhead mémoire**

`malloc()` ajoute des métadonnées à chaque allocation :

```
Vous demandez : malloc(32)

En réalité :
[Métadonnées: 16-32 bytes][Vos données: 32 bytes][Padding]
                           ↑
                           Ce que vous utilisez
```

**Résultat :** Pour de petites allocations fréquentes, vous pouvez perdre 30-50% de mémoire en overhead !

#### 4. **Gestion complexe de durée de vie**

```c
// Difficile à maintenir
void fonction(void) {
    Objet *obj1 = malloc(sizeof(Objet));
    Objet *obj2 = malloc(sizeof(Objet));
    Objet *obj3 = malloc(sizeof(Objet));

    // ... code complexe ...

    // Vous devez vous souvenir de libérer chaque objet
    free(obj1);
    free(obj2);
    free(obj3);
}
```

---

## Types d'allocateurs personnalisés

Il existe plusieurs stratégies d'allocation, chacune adaptée à des besoins différents :

### Vue d'ensemble

| Type | Principe | Performance | Cas d'usage |
|------|----------|-------------|-------------|
| **Arena** | Tout libérer d'un coup | ⚡⚡⚡ Très rapide | Durée de vie identique |
| **Pool** | Objets de taille fixe | ⚡⚡⚡ Très rapide | Beaucoup d'objets identiques |
| **Stack** | LIFO strict | ⚡⚡⚡ Très rapide | Allocations temporaires |
| **Bump** | Pointeur qui avance | ⚡⚡⚡ Ultra rapide | Pas de free individuel |

Explorons chacun en détail.

---

## 1. Arena Allocator (Memory Arena)

### Concept

Un **arena allocator** alloue de la mémoire depuis un gros bloc pré-alloué, et libère **tout d'un coup** à la fin.

**Analogie :** Imaginez une salle de conférence. Pendant l'événement, vous mettez des chaises partout où vous voulez. À la fin, vous ne rangez pas chaque chaise individuellement - vous videz toute la salle d'un coup.

### Schéma de fonctionnement

```
Début :
[════════════════════════════════] Arena de 10 KB (vide)
 ↑
 used = 0

Allocation 1 (100 bytes) :
[XXXX════════════════════════════]
 ↑
 used = 100

Allocation 2 (200 bytes) :
[XXXX|YYYYYY══════════════════════]
      ↑
      used = 300

Allocation 3 (50 bytes) :
[XXXX|YYYYYY|ZZ════════════════════]
            ↑
            used = 350

Libération : arena_reset()
[════════════════════════════════] Tout remis à 0
 ↑
 used = 0
```

### Implémentation simple

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Structure de l'arena
typedef struct {
    uint8_t *memory;    // Bloc de mémoire
    size_t capacity;    // Taille totale
    size_t used;        // Mémoire utilisée
} Arena;

// Créer une arena
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
    // Vérifier s'il reste assez de place
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
    // Calculer l'adresse alignée
    uintptr_t current = (uintptr_t)(arena->memory + arena->used);
    uintptr_t aligned = (current + alignment - 1) & ~(alignment - 1);
    size_t padding = aligned - current;

    // Vérifier s'il reste assez de place
    if (arena->used + padding + size > arena->capacity) {
        return NULL;
    }

    arena->used += padding;
    void *ptr = arena->memory + arena->used;
    arena->used += size;

    return ptr;
}

// Réinitialiser l'arena (libérer tout)
void arena_reset(Arena *arena) {
    arena->used = 0;
    // La mémoire n'est pas libérée, juste réutilisable
}

// Détruire l'arena
void arena_destroy(Arena *arena) {
    free(arena->memory);
    arena->memory = NULL;
    arena->capacity = 0;
    arena->used = 0;
}
```

### Exemple d'utilisation

```c
void exemple_arena(void) {
    // Créer une arena de 10 KB
    Arena arena = arena_create(10 * 1024);

    if (!arena.memory) {
        fprintf(stderr, "Erreur création arena\n");
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

    // Utiliser la mémoire
    strcpy(str1, "Hello from arena!");
    strcpy(str2, "Arena allocation is fast!");

    for (int i = 0; i < 50; i++) {
        numbers[i] = i * i;
    }

    printf("str1: %s\n", str1);
    printf("str2: %s\n", str2);
    printf("Premiers nombres: %d, %d, %d\n", numbers[0], numbers[1], numbers[2]);

    // Pas besoin de free individuel !
    // Tout sera libéré d'un coup

    // Réinitialiser si besoin (réutiliser l'arena)
    arena_reset(&arena);

    // On peut réutiliser l'arena pour d'autres allocations
    char *new_str = arena_alloc(&arena, 50);
    strcpy(new_str, "Réutilisation de l'arena");
    printf("new_str: %s\n", new_str);

    // Détruire l'arena à la fin
    arena_destroy(&arena);
}
```

### Avantages

- ✅ **Performance exceptionnelle** : Allocation = simple addition
- ✅ **Pas de fragmentation** : Allocation linéaire
- ✅ **Pas d'overhead par allocation** : Pas de métadonnées individuelles
- ✅ **Libération simple** : Un seul appel pour tout libérer
- ✅ **Cache-friendly** : Données contiguës en mémoire

### Inconvénients

- ❌ **Pas de free individuel** : Tout ou rien
- ❌ **Taille fixe** : L'arena peut se remplir
- ❌ **Gaspillage potentiel** : Si l'arena est trop grande

### Cas d'usage idéaux

- 🎯 **Parsing** : Parser un fichier, toutes les données ont la même durée de vie
- 🎯 **Jeux vidéo** : Allocations par frame, libération à la fin de la frame
- 🎯 **Compilateurs** : Parser → AST → Génération de code, puis tout libérer
- 🎯 **Serveurs web** : Allocations par requête, libération après la réponse

---

## 2. Pool Allocator (Object Pool)

### Concept

Un **pool allocator** pré-alloue un grand nombre d'objets **de taille fixe** et les distribue à la demande.

**Analogie :** Une bibliothèque avec des casiers numérotés. Quand vous avez besoin d'un casier, vous en prenez un libre. Quand vous avez fini, vous le remettez dans le pool.

### Schéma de fonctionnement

```
Pool de 8 objets (taille fixe: 64 bytes chacun) :

Début :
[0:libre][1:libre][2:libre][3:libre][4:libre][5:libre][6:libre][7:libre]
 ↑ free_list pointe ici

Alloc obj1 → obtient slot 0 :
[0:USED][1:libre][2:libre][3:libre][4:libre][5:libre][6:libre][7:libre]
         ↑ free_list pointe ici

Alloc obj2 → obtient slot 1 :
[0:USED][1:USED][2:libre][3:libre][4:libre][5:libre][6:libre][7:libre]
                 ↑ free_list

Free obj1 → slot 0 redevient libre :
[0:libre][1:USED][2:libre][3:libre][4:libre][5:libre][6:libre][7:libre]
 ↑ free_list (0 est maintenant en tête de liste libre)
```

### Implémentation

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure d'un bloc dans le pool
typedef struct PoolBlock {
    struct PoolBlock *next;  // Pointeur vers le prochain bloc libre
} PoolBlock;

// Structure du pool
typedef struct {
    void *memory;           // Mémoire du pool
    PoolBlock *free_list;   // Liste des blocs libres
    size_t block_size;      // Taille de chaque bloc
    size_t num_blocks;      // Nombre total de blocs
    size_t num_allocated;   // Nombre de blocs alloués
} Pool;

// Créer un pool
Pool pool_create(size_t block_size, size_t num_blocks) {
    Pool pool = {0};

    // S'assurer que block_size peut contenir un pointeur
    if (block_size < sizeof(PoolBlock)) {
        block_size = sizeof(PoolBlock);
    }

    pool.block_size = block_size;
    pool.num_blocks = num_blocks;
    pool.num_allocated = 0;

    // Allouer toute la mémoire d'un coup
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

    // Effacer la mémoire (optionnel mais recommandé)
    memset(block, 0, pool->block_size);

    return block;
}

// Libérer un objet vers le pool
void pool_free(Pool *pool, void *ptr) {
    if (ptr == NULL) return;

    // Remettre le bloc en tête de la liste libre
    PoolBlock *block = (PoolBlock *)ptr;
    block->next = pool->free_list;
    pool->free_list = block;
    pool->num_allocated--;
}

// Détruire le pool
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
    printf("Memory usage: %zu KB\n",
           (pool->block_size * pool->num_blocks) / 1024);
}
```

### Exemple d'utilisation

```c
// Structure d'exemple
typedef struct {
    int id;
    char name[32];
    float value;
} Entity;

void exemple_pool(void) {
    // Créer un pool de 100 entités
    Pool entity_pool = pool_create(sizeof(Entity), 100);

    if (!entity_pool.memory) {
        fprintf(stderr, "Erreur création pool\n");
        return;
    }

    printf("Pool créé avec succès\n");
    pool_stats(&entity_pool);

    // Allouer des entités
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

        printf("\nEntités créées:\n");
        printf("- %s (ID: %d, Value: %.1f)\n", e1->name, e1->id, e1->value);
        printf("- %s (ID: %d, Value: %.1f)\n", e2->name, e2->id, e2->value);
        printf("- %s (ID: %d, Value: %.1f)\n", e3->name, e3->id, e3->value);
    }

    printf("\nAprès allocations:\n");
    pool_stats(&entity_pool);

    // Libérer une entité
    pool_free(&entity_pool, e2);
    e2 = NULL;  // Bonne pratique

    printf("\nAprès libération de e2:\n");
    pool_stats(&entity_pool);

    // Réallouer (va réutiliser l'emplacement de e2)
    Entity *e4 = pool_alloc(&entity_pool);
    if (e4) {
        e4->id = 4;
        strcpy(e4->name, "Entity 4");
        e4->value = 99.9f;
        printf("\nNouvelle entité: %s\n", e4->name);
    }

    // Libérer toutes les entités
    pool_free(&entity_pool, e1);
    pool_free(&entity_pool, e3);
    pool_free(&entity_pool, e4);

    printf("\nToutes les entités libérées:\n");
    pool_stats(&entity_pool);

    // Détruire le pool
    pool_destroy(&entity_pool);
}
```

### Avantages

- ✅ **Performance constante** : O(1) pour alloc et free
- ✅ **Pas de fragmentation** : Tailles fixes
- ✅ **Cache-friendly** : Objets contigus en mémoire
- ✅ **Free individuel possible** : Contrairement à l'arena
- ✅ **Pas d'overhead par objet** : Utilise le bloc lui-même pour la liste

### Inconvénients

- ❌ **Taille fixe** : Tous les objets doivent avoir la même taille
- ❌ **Capacité limitée** : Nombre maximum d'objets défini à la création
- ❌ **Gaspillage si sous-utilisé** : Mémoire réservée même si non utilisée

### Cas d'usage idéaux

- 🎯 **Jeux vidéo** : Ennemis, projectiles, particules (beaucoup d'objets identiques)
- 🎯 **Réseaux** : Buffers de paquets de taille fixe
- 🎯 **Bases de données** : Pages mémoire de taille fixe
- 🎯 **Serveurs** : Connexions clientes (structure de taille fixe)

---

## 3. Stack Allocator (Bump Allocator)

### Concept

Un **stack allocator** alloue de la mémoire en avançant un pointeur, et libère **dans l'ordre inverse** (LIFO - Last In, First Out).

**Analogie :** Une pile d'assiettes. Vous ne pouvez ajouter et retirer des assiettes que par le haut.

### Schéma de fonctionnement

```
État initial :
[════════════════════════════════]
 ↑ top = 0

Push 100 bytes :
[XXXX════════════════════════════]
     ↑ top = 100

Push 50 bytes :
[XXXX|YY════════════════════════]
       ↑ top = 150

Push 200 bytes :
[XXXX|YY|ZZZZZZ════════════════]
              ↑ top = 350

Pop (200 bytes) :
[XXXX|YY════════════════════════]
       ↑ top = 150

Pop (50 bytes) :
[XXXX════════════════════════════]
     ↑ top = 100
```

### Implémentation

```c
#include <stdio.h>
#include <stdlib.h>
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

// Créer un stack allocator
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
    // Vérifier l'espace disponible
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
    size_t padding = aligned - current;

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

// Restaurer à une marque
void stack_restore(StackAllocator *stack, StackMark mark) {
    assert(mark.position <= stack->top);
    stack->top = mark.position;
}

// Réinitialiser complètement
void stack_reset(StackAllocator *stack) {
    stack->top = 0;
}

// Détruire le stack
void stack_destroy(StackAllocator *stack) {
    free(stack->memory);
    stack->memory = NULL;
    stack->capacity = 0;
    stack->top = 0;
}

// Obtenir l'espace utilisé
size_t stack_used(StackAllocator *stack) {
    return stack->top;
}

// Obtenir l'espace libre
size_t stack_free(StackAllocator *stack) {
    return stack->capacity - stack->top;
}
```

### Exemple d'utilisation

```c
void exemple_stack(void) {
    // Créer un stack de 1 MB
    StackAllocator stack = stack_create(1024 * 1024);

    if (!stack.memory) {
        fprintf(stderr, "Erreur création stack\n");
        return;
    }

    printf("Stack créé: %zu bytes disponibles\n", stack.capacity);

    // Marquer le début
    StackMark start = stack_mark(&stack);

    // Allouer des données temporaires
    char *temp1 = stack_alloc(&stack, 1000);
    strcpy(temp1, "Données temporaires 1");
    printf("Alloué temp1: %zu bytes utilisés\n", stack_used(&stack));

    // Marquer après temp1
    StackMark after_temp1 = stack_mark(&stack);

    char *temp2 = stack_alloc(&stack, 2000);
    strcpy(temp2, "Données temporaires 2");
    printf("Alloué temp2: %zu bytes utilisés\n", stack_used(&stack));

    int *numbers = stack_alloc(&stack, 100 * sizeof(int));
    for (int i = 0; i < 100; i++) {
        numbers[i] = i;
    }
    printf("Alloué numbers: %zu bytes utilisés\n", stack_used(&stack));

    // Utiliser les données
    printf("\nDonnées:\n");
    printf("temp1: %s\n", temp1);
    printf("temp2: %s\n", temp2);
    printf("numbers[50]: %d\n", numbers[50]);

    // Libérer temp2 et numbers en restaurant after_temp1
    printf("\nRestauration à after_temp1...\n");
    stack_restore(&stack, after_temp1);
    printf("Bytes utilisés après restauration: %zu\n", stack_used(&stack));

    // temp1 est toujours valide, mais temp2 et numbers ne le sont plus !
    printf("temp1 toujours valide: %s\n", temp1);

    // Restaurer au début
    printf("\nRestauration complète...\n");
    stack_restore(&stack, start);
    printf("Bytes utilisés: %zu (retour au début)\n", stack_used(&stack));

    // Détruire le stack
    stack_destroy(&stack);
}
```

### Utilisation avec des scopes

```c
// Pattern pratique : utiliser avec des scopes
void fonction_avec_stack(StackAllocator *stack) {
    StackMark mark = stack_mark(stack);

    // Allouer des données temporaires
    char *buffer = stack_alloc(stack, 1024);
    // ... traitement ...

    // À la fin du scope, restaurer automatiquement
    stack_restore(stack, mark);
    // buffer n'est plus valide après cette ligne
}
```

### Avantages

- ✅ **Ultra rapide** : Allocation = simple addition
- ✅ **Pas de fragmentation** : Allocation linéaire
- ✅ **Pas d'overhead** : Pas de métadonnées
- ✅ **Gestion par scope** : Parfait pour allocations temporaires

### Inconvénients

- ❌ **Ordre strict** : Libération uniquement LIFO
- ❌ **Pas de free individuel** : Seulement restauration en bloc
- ❌ **Risque d'erreur** : Facile d'utiliser un pointeur invalide après restauration

### Cas d'usage idéaux

- 🎯 **Parsing récursif** : Chaque niveau de récursion utilise le stack
- 🎯 **Buffers temporaires** : Allocations courte durée dans une fonction
- 🎯 **Jeux vidéo** : Allocations par frame ou par système
- 🎯 **Compilateurs** : Données temporaires pendant l'analyse

---

## Comparaison des allocateurs

### Tableau récapitulatif

| Critère | malloc/free | Arena | Pool | Stack |
|---------|-------------|-------|------|-------|
| **Vitesse alloc** | 🐢 Lente | ⚡⚡⚡ | ⚡⚡⚡ | ⚡⚡⚡ |
| **Vitesse free** | 🐢 Lente | ⚡⚡⚡ (tout) | ⚡⚡⚡ | ⚡⚡⚡ (mark) |
| **Overhead mémoire** | ❌ Élevé | ✅ Minimal | ✅ Nul | ✅ Nul |
| **Fragmentation** | ❌ Oui | ✅ Non | ✅ Non | ✅ Non |
| **Free individuel** | ✅ Oui | ❌ Non | ✅ Oui | ❌ Non |
| **Taille variable** | ✅ Oui | ✅ Oui | ❌ Non | ✅ Oui |
| **Thread-safe** | ✅ Oui | ⚠️ À implémenter | ⚠️ À implémenter | ⚠️ À implémenter |

### Diagramme de décision

```
Quel allocateur choisir ?
│
├─ Tous les objets ont la même durée de vie ?
│  └─ OUI → Arena Allocator
│
├─ Beaucoup d'objets de même taille, créés/détruits ?
│  └─ OUI → Pool Allocator
│
├─ Allocations temporaires avec ordre strict ?
│  └─ OUI → Stack Allocator
│
└─ Besoin de free individuel avec tailles variables ?
   └─ OUI → malloc/free standard (ou allocateur hybride)
```

---

## Exemple complet : Utilisation dans un parseur

Voici un exemple réaliste d'utilisation d'un arena allocator dans un parseur JSON simple :

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

// Créer un parseur
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
    // Sauter les espaces et séparateurs JSON (: et ,)
    while (isspace(parser->input[parser->pos]) ||
           parser->input[parser->pos] == ':' ||
           parser->input[parser->pos] == ',') {
        parser->pos++;
    }

    char c = parser->input[parser->pos];

    // Fin de l'input
    if (c == '\0') {
        Token *token = parser_alloc_token(parser);
        token->type = TOKEN_EOF;
        return token;
    }

    // Caractères simples
    Token *token = parser_alloc_token(parser);

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
        memcpy(token->value, parser->input + start, length);
        token->value[length] = '\0';
        token->length = length;

        parser->pos++;  // Sauter "
        return token;
    }

    // Nombre
    if (isdigit(c) || c == '-') {
        size_t start = parser->pos;

        if (c == '-') parser->pos++;
        while (isdigit(parser->input[parser->pos])) {
            parser->pos++;
        }

        size_t length = parser->pos - start;
        token->type = TOKEN_NUMBER;
        token->value = parser_alloc_string(parser, length);
        memcpy(token->value, parser->input + start, length);
        token->value[length] = '\0';
        token->length = length;

        return token;
    }

    return NULL;
}

// Détruire le parseur (libère tout l'arena d'un coup)
void parser_destroy(Parser *parser) {
    arena_destroy(&parser->arena);
}

// Exemple d'utilisation
void exemple_parser(void) {
    const char *json = "{\"name\": \"Alice\", \"age\": 30, \"city\": \"Paris\"}";

    printf("Parsing JSON: %s\n\n", json);

    // Créer le parseur avec une arena de 4 KB
    Parser parser = parser_create(json, 4096);

    if (!parser.arena.memory) {
        fprintf(stderr, "Erreur création parseur\n");
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

    printf("\n%d tokens parsés\n", count);
    printf("Mémoire utilisée: %zu bytes\n", parser.arena.used);

    // Libérer tout d'un coup (tous les tokens, toutes les strings)
    parser_destroy(&parser);
    printf("Parseur détruit (tout libéré d'un coup)\n");
}
```

**Avantages de cette approche :**
- ✅ Pas de free individuel pour chaque token/string
- ✅ Très rapide (allocations linéaires)
- ✅ Pas de fuites possibles
- ✅ Code simple et lisible

---

## Allocateurs hybrides

Dans la vraie vie, vous pouvez combiner plusieurs stratégies :

### Exemple : Arena + Pool

```c
typedef struct {
    Arena arena;           // Pour allocations à durée de vie longue
    Pool temp_pool;        // Pour petits objets temporaires
} HybridAllocator;

HybridAllocator hybrid_create(size_t arena_size,
                              size_t pool_block_size,
                              size_t pool_count) {
    HybridAllocator hybrid = {0};
    hybrid.arena = arena_create(arena_size);
    hybrid.temp_pool = pool_create(pool_block_size, pool_count);
    return hybrid;
}

// Utilisation
void fonction(HybridAllocator *alloc) {
    // Grande structure → arena
    BigStruct *big = arena_alloc(&alloc->arena, sizeof(BigStruct));

    // Petits objets temporaires → pool
    SmallObj *small1 = pool_alloc(&alloc->temp_pool);
    SmallObj *small2 = pool_alloc(&alloc->temp_pool);

    // Traitement...

    // Libérer les petits objets
    pool_free(&alloc->temp_pool, small1);
    pool_free(&alloc->temp_pool, small2);

    // big sera libéré avec tout l'arena à la fin
}
```

---

## Bonnes pratiques

### 1. Choisir le bon allocateur

```c
// ✅ BON : Arena pour parsing
void parser_function(void) {
    Arena arena = arena_create(10 * 1024);
    // Parser, créer AST
    // Tout libérer d'un coup
    arena_destroy(&arena);
}

// ❌ MAUVAIS : malloc/free pour chaque token
void parser_function_bad(void) {
    for (int i = 0; i < 1000; i++) {
        Token *t = malloc(sizeof(Token));  // Lent !
        // ...
        free(t);  // Beaucoup d'overhead
    }
}
```

### 2. Dimensionner correctement

```c
// ✅ BON : Taille appropriée
Arena arena = arena_create(1024 * 1024);  // 1 MB pour gros document

// ❌ MAUVAIS : Trop petit
Arena arena = arena_create(100);  // Va vite déborder

// ❌ MAUVAIS : Trop grand
Arena arena = arena_create(1024 * 1024 * 1024);  // 1 GB gaspillés
```

### 3. Documenter la durée de vie

```c
// ✅ BON : Clair et documenté
/**
 * parse_config - Parse un fichier de configuration
 *
 * Note: Toutes les données retournées sont allouées dans l'arena
 * et seront invalidées quand l'arena sera détruite.
 */
Config *parse_config(Arena *arena, const char *filename);
```

### 4. Attention aux pointeurs pendants

```c
// ⚠️ DANGER
char *get_temp_string(StackAllocator *stack) {
    StackMark mark = stack_mark(stack);
    char *str = stack_alloc(stack, 100);
    strcpy(str, "temporary");
    stack_restore(stack, mark);  // str n'est plus valide !
    return str;  // ❌ ERREUR : pointeur pendant
}

// ✅ BON
void use_temp_string(StackAllocator *stack) {
    StackMark mark = stack_mark(stack);
    char *str = stack_alloc(stack, 100);
    strcpy(str, "temporary");
    // Utiliser str ici
    printf("%s\n", str);
    stack_restore(stack, mark);  // OK, on n'utilise plus str après
}
```

---

## Performance : Benchmarks

Voici des résultats typiques de benchmarks (ordre de grandeur) :

### Allocations de 64 bytes, 1 million de fois

| Allocateur | Temps | Vitesse relative |
|------------|-------|------------------|
| malloc/free | 150 ms | 1x (baseline) |
| Arena | 5 ms | **30x plus rapide** |
| Pool | 3 ms | **50x plus rapide** |
| Stack | 2 ms | **75x plus rapide** |

### Utilisation mémoire (overhead)

| Allocateur | Overhead par allocation |
|------------|------------------------|
| malloc/free | 16-32 bytes |
| Arena | 0 bytes |
| Pool | 0 bytes (réutilise l'espace) |
| Stack | 0 bytes |

---

## Quand utiliser des custom allocators ?

### ✅ Utilisez quand :

1. **Performance critique**
   - Jeux vidéo, systèmes temps réel
   - Serveurs haute performance

2. **Patterns d'allocation prévisibles**
   - Durée de vie identique (arena)
   - Taille fixe (pool)
   - Ordre LIFO (stack)

3. **Contrôle total nécessaire**
   - Débogage approfondi
   - Profilage mémoire
   - Optimisations spécifiques

### ❌ N'utilisez PAS quand :

1. **Patterns d'allocation imprévisibles**
   - malloc/free est optimisé pour ça

2. **Code générique**
   - Bibliothèques publiques doivent être flexibles

3. **Prototypage rapide**
   - malloc/free est plus simple au début

---

## Ressources et références

### Projets open-source utilisant des custom allocators

- **Redis** : Utilise jemalloc (allocateur optimisé)
- **Nginx** : Pool allocator pour requêtes HTTP
- **SQLite** : Allocateur personnalisé pour pages mémoire
- **Unreal Engine** : Multiples allocateurs (arena, pool, etc.)

### Lectures recommandées

- [Memory Allocation Strategies](https://www.gingerbill.org/series/memory-allocation-strategies/) - Série d'articles excellente
- [Allocators in Rust](https://doc.rust-lang.org/std/alloc/index.html) - Concepts similaires
- [jemalloc](http://jemalloc.net/) - Allocateur général très performant

---

## Conclusion

Les **custom allocators** sont un outil puissant pour optimiser la gestion mémoire en C :

✅ **Performance** : 10x à 100x plus rapides que malloc/free dans certains cas  
✅ **Contrôle** : Vous décidez comment gérer la mémoire  
✅ **Simplicité** : Code plus simple sans free() partout  
✅ **Debugging** : Plus facile à tracer et profiler

**Points clés à retenir :**

1. **Arena** : Quand tout a la même durée de vie
2. **Pool** : Quand beaucoup d'objets de même taille
3. **Stack** : Quand ordre LIFO strict
4. **Hybride** : Combiner selon les besoins

**Conseil final :** Commencez par profiler votre code avec malloc/free standard. Si la gestion mémoire est un bottleneck et que vous avez des patterns prévisibles, alors envisagez un custom allocator.

---

**🎯 Dans la section suivante (24.3 Garbage collection en C), nous verrons comment implémenter des systèmes de garbage collection automatique en C pour gérer la mémoire sans appels explicites à free().**

⏭️ [Garbage collection en C](/24-gestion-memoire-avancee/03-garbage-collection.md)
