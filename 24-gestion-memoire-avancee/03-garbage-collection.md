🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 24.3 Garbage collection en C

## Introduction

### Qu'est-ce que le Garbage Collection ?

Le **Garbage Collection (GC)**, ou "ramasse-miettes" en français, est un système de gestion automatique de la mémoire qui :

1. **Détecte automatiquement** les objets qui ne sont plus utilisés
2. **Libère automatiquement** la mémoire de ces objets
3. **Élimine** le besoin d'appels manuels à `free()`

**Analogie :** Imaginez votre appartement. Sans garbage collector, vous devez vous rappeler de jeter chaque déchet manuellement. Avec un garbage collector, c'est comme avoir un robot qui détecte automatiquement ce qui n'est plus utile et le jette pour vous.

### Le problème en C

En C standard, vous devez gérer manuellement la mémoire :

```c
// Allocation manuelle
char *buffer = malloc(1024);

// Utilisation
strcpy(buffer, "Hello");

// ⚠️ Vous DEVEZ vous rappeler de libérer
free(buffer);
```

**Si vous oubliez `free()` :**
- 🔴 **Memory leak** (fuite mémoire)
- 🔴 Le programme consomme de plus en plus de RAM
- 🔴 Peut crasher à long terme

**Avec un garbage collector :**
```c
// Allocation (depuis le GC)
char *buffer = gc_malloc(1024);

// Utilisation
strcpy(buffer, "Hello");

// ✅ Pas besoin de free() !
// Le GC libérera automatiquement quand buffer n'est plus utilisé
```

---

## Garbage Collection dans d'autres langages

Pour comprendre le concept, voyons comment d'autres langages gèrent cela :

### Python

```python
# Python
def fonction():
    data = [1, 2, 3, 4, 5]  # Allocation
    # ... utilisation ...
    return
    # data est automatiquement libérée quand plus aucune référence
```

### Java

```java
// Java
void fonction() {
    String message = new String("Hello");  // Allocation
    // ... utilisation ...
}  // message sera garbage collecté automatiquement
```

### JavaScript

```javascript
// JavaScript
function fonction() {
    let obj = { name: "Alice", age: 30 };  // Allocation
    // ... utilisation ...
}  // obj sera garbage collecté
```

### Go

```go
// Go
func fonction() {
    data := make([]int, 100)  // Allocation
    // ... utilisation ...
}  // data sera garbage collecté
```

**Point commun :** Dans tous ces langages, vous n'appelez **jamais** de fonction pour libérer la mémoire. Le GC s'en charge.

---

## Pourquoi le C n'a pas de GC natif ?

### Raisons historiques et techniques

1. **Performance prévisible**
   - Le C est conçu pour les systèmes bas niveau
   - Un GC peut introduire des pauses imprévisibles
   - Critique pour systèmes temps-réel, OS, drivers

2. **Contrôle total**
   - Les développeurs C veulent contrôler exactement quand libérer
   - Pas de "magie" cachée

3. **Footprint minimal**
   - Un GC ajoute du code runtime
   - Augmente la taille des exécutables

4. **Compatibilité avec le matériel**
   - Le C doit pouvoir accéder à des adresses mémoire spécifiques
   - Un GC qui déplace les objets serait problématique

**Mais :** Rien ne nous empêche d'**implémenter** un GC en C pour nos propres besoins !

---

## Stratégies de Garbage Collection

Il existe plusieurs approches pour implémenter un GC. Voyons les principales :

### Vue d'ensemble

| Stratégie | Principe | Complexité | Performance |
|-----------|----------|------------|-------------|
| **Reference Counting** | Compter les références | 🟢 Simple | 🟡 Moyenne |
| **Mark and Sweep** | Marquer puis balayer | 🟡 Moyenne | 🟢 Bonne |
| **Copying GC** | Copier les vivants | 🔴 Complexe | 🟢 Bonne |
| **Generational GC** | Générations d'objets | 🔴 Complexe | 🟢 Très bonne |

Nous allons nous concentrer sur les deux premières, qui sont les plus accessibles.

---

## 1. Reference Counting (Comptage de références)

### Concept

Chaque objet a un **compteur** qui indique combien de pointeurs le référencent. Quand le compteur tombe à zéro, l'objet est immédiatement libéré.

**Analogie :** C'est comme compter combien de personnes regardent un tableau. Quand plus personne ne regarde (compteur = 0), on peut l'enlever.

### Schéma de fonctionnement

```
Objet A créé :
[Objet A | ref_count=1]
   ↑
   ptr1

ptr2 = ptr1 (nouvelle référence) :
[Objet A | ref_count=2]
   ↑     ↑
   ptr1  ptr2

ptr1 = NULL (libération d'une référence) :
[Objet A | ref_count=1]
         ↑
         ptr2

ptr2 = NULL (libération dernière référence) :
[Objet A | ref_count=0] → ♻️ LIBÉRATION AUTOMATIQUE
```

### Implémentation simple

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure de base avec compteur de références
typedef struct {
    int ref_count;      // Compteur de références
    size_t size;        // Taille de l'allocation
    char data[];        // Données flexibles (C99)
} RefCountedObject;

// Allouer un objet avec reference counting
void *rc_alloc(size_t size) {
    RefCountedObject *obj = malloc(sizeof(RefCountedObject) + size);
    if (!obj) {
        return NULL;
    }

    obj->ref_count = 1;  // Initialement 1 référence
    obj->size = size;

    printf("🆕 Objet alloué (%zu bytes), ref_count=1\n", size);

    return obj->data;  // Retourner le pointeur vers les données
}

// Obtenir l'objet depuis un pointeur de données
static RefCountedObject *rc_get_object(void *ptr) {
    if (!ptr) return NULL;

    // Le pointeur pointe vers data, reculer pour obtenir l'objet
    return (RefCountedObject *)((char *)ptr - offsetof(RefCountedObject, data));
}

// Incrémenter le compteur (nouvelle référence)
void *rc_retain(void *ptr) {
    if (!ptr) return NULL;

    RefCountedObject *obj = rc_get_object(ptr);
    obj->ref_count++;

    printf("➕ ref_count++ = %d\n", obj->ref_count);

    return ptr;
}

// Décrémenter le compteur (libérer une référence)
void rc_release(void *ptr) {
    if (!ptr) return;

    RefCountedObject *obj = rc_get_object(ptr);
    obj->ref_count--;

    printf("➖ ref_count-- = %d\n", obj->ref_count);

    // Si plus aucune référence, libérer
    if (obj->ref_count == 0) {
        printf("♻️  Objet libéré (ref_count=0)\n");
        free(obj);
    }
}

// Obtenir le compteur actuel
int rc_get_count(void *ptr) {
    if (!ptr) return 0;
    RefCountedObject *obj = rc_get_object(ptr);
    return obj->ref_count;
}
```

### Exemple d'utilisation

```c
void exemple_reference_counting(void) {
    printf("=== Exemple Reference Counting ===\n\n");

    // Allouer un buffer
    char *buffer = rc_alloc(100);
    if (!buffer) return;

    strcpy(buffer, "Hello, Reference Counting!");
    printf("Buffer: %s\n", buffer);
    printf("Références: %d\n\n", rc_get_count(buffer));

    // Créer une nouvelle référence
    char *buffer2 = rc_retain(buffer);
    printf("buffer2 pointe vers le même objet\n");
    printf("Références: %d\n\n", rc_get_count(buffer));

    // Créer une troisième référence
    char *buffer3 = rc_retain(buffer);
    printf("buffer3 pointe vers le même objet\n");
    printf("Références: %d\n\n", rc_get_count(buffer));

    // Libérer buffer (décrémente)
    printf("Libération de buffer...\n");
    rc_release(buffer);
    printf("Références restantes: %d\n\n", rc_get_count(buffer2));

    // Libérer buffer2 (décrémente)
    printf("Libération de buffer2...\n");
    rc_release(buffer2);
    printf("Références restantes: %d\n\n", rc_get_count(buffer3));

    // Libérer buffer3 (décrémente à 0 → libération automatique)
    printf("Libération de buffer3...\n");
    rc_release(buffer3);

    printf("\n=== Fin de l'exemple ===\n");
}
```

**Sortie :**
```
=== Exemple Reference Counting ===

🆕 Objet alloué (100 bytes), ref_count=1
Buffer: Hello, Reference Counting!
Références: 1

➕ ref_count++ = 2
buffer2 pointe vers le même objet
Références: 2

➕ ref_count++ = 3
buffer3 pointe vers le même objet
Références: 3

Libération de buffer...
➖ ref_count-- = 2
Références restantes: 2

Libération de buffer2...
➖ ref_count-- = 1
Références restantes: 1

Libération de buffer3...
➖ ref_count-- = 0
♻️ Objet libéré (ref_count=0)

=== Fin de l'exemple ===
```

---

### Reference counting avancé : avec destructeurs

Amélioration pour gérer des objets complexes :

```c
// Type de fonction destructeur
typedef void (*DestructorFunc)(void *data);

// Structure améliorée
typedef struct {
    int ref_count;
    size_t size;
    DestructorFunc destructor;  // Fonction appelée avant libération
    char data[];
} RefCountedObjectEx;

// Allouer avec destructeur personnalisé
void *rc_alloc_ex(size_t size, DestructorFunc destructor) {
    RefCountedObjectEx *obj = malloc(sizeof(RefCountedObjectEx) + size);
    if (!obj) return NULL;

    obj->ref_count = 1;
    obj->size = size;
    obj->destructor = destructor;

    return obj->data;
}

// Libérer avec appel du destructeur
void rc_release_ex(void *ptr) {
    if (!ptr) return;

    RefCountedObjectEx *obj = (RefCountedObjectEx *)((char *)ptr -
                              offsetof(RefCountedObjectEx, data));
    obj->ref_count--;

    if (obj->ref_count == 0) {
        // Appeler le destructeur si présent
        if (obj->destructor) {
            obj->destructor(ptr);
        }
        free(obj);
    }
}

// Exemple de destructeur
void file_destructor(void *data) {
    FILE **f = (FILE **)data;
    if (*f) {
        printf("🗑️  Fermeture automatique du fichier\n");
        fclose(*f);
        *f = NULL;
    }
}

// Utilisation
void exemple_avec_destructeur(void) {
    FILE **f_ptr = rc_alloc_ex(sizeof(FILE *), file_destructor);
    *f_ptr = fopen("test.txt", "w");

    if (*f_ptr) {
        fprintf(*f_ptr, "Test\n");
    }

    // Pas besoin de fclose explicite !
    rc_release_ex(f_ptr);
    // Le destructeur fermera automatiquement le fichier
}
```

---

### Avantages du Reference Counting

- ✅ **Libération immédiate** : Dès que ref_count = 0, l'objet est libéré
- ✅ **Prévisible** : Pas de pause GC aléatoire
- ✅ **Simple à implémenter** : Concept facile à comprendre
- ✅ **Déterministe** : On sait exactement quand un objet est libéré

### Inconvénients du Reference Counting

- ❌ **Cycles de références** : Problème majeur (voir ci-dessous)
- ❌ **Overhead** : Chaque opération doit mettre à jour le compteur
- ❌ **Thread-safety** : Nécessite des atomics ou des locks
- ❌ **Overhead mémoire** : Chaque objet a un compteur

---

### Le problème des cycles de références

**Situation problématique :**

```c
typedef struct Node {
    RefCountedObject header;
    struct Node *next;
    int value;
} Node;

// Créer deux nœuds qui se référencent mutuellement
Node *a = rc_alloc(sizeof(Node));  // ref_count = 1
Node *b = rc_alloc(sizeof(Node));  // ref_count = 1

a->next = rc_retain(b);  // b->ref_count = 2
b->next = rc_retain(a);  // a->ref_count = 2

// Libérer nos références
rc_release(a);  // a->ref_count = 1 (toujours référencé par b)
rc_release(b);  // b->ref_count = 1 (toujours référencé par a)

// ⚠️ PROBLÈME : a et b se référencent mutuellement
// Leurs ref_count ne tomberont jamais à 0
// → FUITE MÉMOIRE PERMANENTE
```

**Schéma du cycle :**
```
    ┌─────┐     ┌─────┐
    │  A  │────→│  B  │
    │rc=1 │     │rc=1 │
    └─────┘←────└─────┘
      ↑              ↑
      └──────────────┘
     Cycle de référence !
```

**Solutions possibles :**
1. **Weak references** : Références qui ne comptent pas
2. **Détection de cycles** : Algorithme pour casser les cycles
3. **Éviter les cycles** : Design sans références circulaires

---

## 2. Mark and Sweep (Marquer et Balayer)

### Concept

Le **Mark and Sweep** est une approche différente en deux phases :

1. **Mark (Marquer)** : Parcourir tous les objets accessibles et les marquer
2. **Sweep (Balayer)** : Libérer tous les objets non marqués

**Analogie :** Imaginez un grand ménage de printemps :
1. Vous mettez un sticker vert sur tout ce que vous utilisez encore
2. Vous jetez tout ce qui n'a pas de sticker vert

### Schéma de fonctionnement

```
État initial :
[Obj A] [Obj B] [Obj C] [Obj D] [Obj E]
   ↑       ↑                       ↑
   racine1 racine2                 racine3

Phase 1 - MARK (marquer depuis les racines) :
[Obj A]✓ [Obj B]✓ [Obj C]  [Obj D]  [Obj E]✓
   ↑       ↑                          ↑
   |       |                          |
   └─→ [Obj F]✓                       └─→ [Obj G]✓

Objets marqués : A, B, E, F, G
Objets non marqués : C, D (non accessibles → garbage)

Phase 2 - SWEEP (balayer) :
[Obj A]✓ [Obj B]✓ ♻️C ♻️D [Obj E]✓
                            ↓
                         [Obj G]✓
   ↓
[Obj F]✓

Objets C et D sont libérés
```

### Implémentation simple

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Configuration du GC
#define GC_INITIAL_CAPACITY 256
#define GC_GROWTH_FACTOR 2

// En-tête de chaque objet géré
typedef struct GCObject {
    bool marked;              // Marqué lors du GC
    size_t size;              // Taille de l'objet
    struct GCObject *next;    // Liste chaînée des objets
} GCObject;

// Structure du garbage collector
typedef struct {
    GCObject *first_object;   // Premier objet de la liste
    size_t num_objects;       // Nombre d'objets
    size_t max_objects;       // Seuil pour déclencher le GC
    void **roots;             // Tableau des racines
    size_t num_roots;
    size_t capacity_roots;
} GarbageCollector;

// Créer un garbage collector
GarbageCollector *gc_create(void) {
    GarbageCollector *gc = malloc(sizeof(GarbageCollector));
    if (!gc) return NULL;

    gc->first_object = NULL;
    gc->num_objects = 0;
    gc->max_objects = GC_INITIAL_CAPACITY;
    gc->num_roots = 0;
    gc->capacity_roots = 10;
    gc->roots = malloc(gc->capacity_roots * sizeof(void *));

    printf("🌱 GC créé (seuil: %zu objets)\n", gc->max_objects);

    return gc;
}

// Allouer un objet via le GC
void *gc_alloc(GarbageCollector *gc, size_t size) {
    // Déclencher le GC si nécessaire
    if (gc->num_objects >= gc->max_objects) {
        gc_collect(gc);
    }

    // Allouer l'objet avec son en-tête
    GCObject *obj = malloc(sizeof(GCObject) + size);
    if (!obj) return NULL;

    obj->marked = false;
    obj->size = size;

    // Ajouter à la liste des objets
    obj->next = gc->first_object;
    gc->first_object = obj;
    gc->num_objects++;

    printf("🆕 Objet alloué (%zu bytes) - Total: %zu\n",
           size, gc->num_objects);

    // Retourner le pointeur après l'en-tête
    return (void *)(obj + 1);
}

// Ajouter une racine
void gc_add_root(GarbageCollector *gc, void *ptr) {
    if (gc->num_roots >= gc->capacity_roots) {
        gc->capacity_roots *= 2;
        gc->roots = realloc(gc->roots, gc->capacity_roots * sizeof(void *));
    }

    gc->roots[gc->num_roots++] = ptr;
}

// Retirer une racine
void gc_remove_root(GarbageCollector *gc, void *ptr) {
    for (size_t i = 0; i < gc->num_roots; i++) {
        if (gc->roots[i] == ptr) {
            gc->roots[i] = gc->roots[--gc->num_roots];
            return;
        }
    }
}

// Phase MARK : marquer un objet
static void gc_mark_object(void *ptr) {
    if (!ptr) return;

    GCObject *obj = ((GCObject *)ptr) - 1;

    if (obj->marked) return;  // Déjà marqué

    obj->marked = true;
    printf("  ✓ Objet marqué (%p)\n", ptr);
}

// Phase MARK : marquer depuis les racines
static void gc_mark(GarbageCollector *gc) {
    printf("\n📍 Phase MARK : Marquage des objets accessibles\n");

    for (size_t i = 0; i < gc->num_roots; i++) {
        if (gc->roots[i]) {
            gc_mark_object(gc->roots[i]);
        }
    }
}

// Phase SWEEP : libérer les objets non marqués
static void gc_sweep(GarbageCollector *gc) {
    printf("\n🧹 Phase SWEEP : Libération des objets non marqués\n");

    GCObject **obj_ptr = &gc->first_object;
    size_t collected = 0;

    while (*obj_ptr) {
        GCObject *obj = *obj_ptr;

        if (!obj->marked) {
            // Objet non marqué → à libérer
            *obj_ptr = obj->next;
            printf("  ♻️  Objet collecté (%p, %zu bytes)\n",
                   (void *)(obj + 1), obj->size);
            free(obj);
            gc->num_objects--;
            collected++;
        } else {
            // Objet marqué → réinitialiser le marqueur pour le prochain GC
            obj->marked = false;
            obj_ptr = &obj->next;
        }
    }

    printf("📊 %zu objet(s) collecté(s), %zu restant(s)\n",
           collected, gc->num_objects);

    // Ajuster le seuil
    gc->max_objects = gc->num_objects * GC_GROWTH_FACTOR;
    if (gc->max_objects < GC_INITIAL_CAPACITY) {
        gc->max_objects = GC_INITIAL_CAPACITY;
    }
}

// Lancer une collection
void gc_collect(GarbageCollector *gc) {
    printf("\n🚀 GARBAGE COLLECTION DÉCLENCHÉE\n");
    printf("   Objets avant GC: %zu\n", gc->num_objects);

    gc_mark(gc);
    gc_sweep(gc);

    printf("   Nouveau seuil: %zu objets\n", gc->max_objects);
}

// Détruire le GC (libérer tout)
void gc_destroy(GarbageCollector *gc) {
    printf("\n🗑️  Destruction du GC\n");

    GCObject *obj = gc->first_object;
    while (obj) {
        GCObject *next = obj->next;
        free(obj);
        obj = next;
    }

    free(gc->roots);
    free(gc);
}
```

### Exemple d'utilisation

```c
void exemple_mark_and_sweep(void) {
    printf("=== Exemple Mark and Sweep GC ===\n\n");

    // Créer le GC
    GarbageCollector *gc = gc_create();

    // Allouer des objets
    char *str1 = gc_alloc(gc, 100);
    strcpy(str1, "String 1");
    gc_add_root(gc, str1);  // str1 est une racine

    char *str2 = gc_alloc(gc, 200);
    strcpy(str2, "String 2");
    gc_add_root(gc, str2);  // str2 est une racine

    char *str3 = gc_alloc(gc, 150);
    strcpy(str3, "String 3 - temporaire");
    // str3 n'est PAS une racine

    char *str4 = gc_alloc(gc, 300);
    strcpy(str4, "String 4");
    gc_add_root(gc, str4);  // str4 est une racine

    printf("\n📝 État actuel:\n");
    printf("   str1 (racine): %s\n", str1);
    printf("   str2 (racine): %s\n", str2);
    printf("   str3 (temporaire): %s\n", str3);
    printf("   str4 (racine): %s\n", str4);

    // Déclencher manuellement le GC
    gc_collect(gc);
    // str3 sera collecté car ce n'est pas une racine !

    printf("\n📝 Après GC:\n");
    printf("   str1: %s (toujours valide)\n", str1);
    printf("   str2: %s (toujours valide)\n", str2);
    printf("   str3: [INVALIDE - collecté]\n");
    printf("   str4: %s (toujours valide)\n", str4);

    // Retirer une racine
    gc_remove_root(gc, str2);
    printf("\n🔻 str2 n'est plus une racine\n");

    // Allouer plus d'objets pour déclencher un autre GC
    for (int i = 0; i < 260; i++) {
        gc_alloc(gc, 50);
    }
    // Le GC se déclenche automatiquement
    // str2 sera collecté cette fois

    // Nettoyage
    gc_destroy(gc);
    printf("\n=== Fin de l'exemple ===\n");
}
```

---

### Avantages du Mark and Sweep

- ✅ **Gère les cycles** : Pas de problème de références circulaires
- ✅ **Pas d'overhead constant** : Pas de compteur à chaque opération
- ✅ **Flexible** : Peut gérer des graphes d'objets complexes

### Inconvénients du Mark and Sweep

- ❌ **Pauses GC** : Le programme s'arrête pendant la collection
- ❌ **Complexité** : Plus difficile à implémenter correctement
- ❌ **Nécessite de tracker les racines** : Il faut maintenir une liste des variables vivantes
- ❌ **Fragmentation** : Peut fragmenter la mémoire

---

## Utilisation de Boehm GC

Au lieu d'implémenter votre propre GC, vous pouvez utiliser **Boehm-Demers-Weiser Garbage Collector**, un GC conservateur pour C/C++.

### Installation

```bash
# Ubuntu/Debian
sudo apt-get install libgc-dev

# Fedora/RHEL
sudo dnf install gc-devel

# macOS avec Homebrew
brew install bdw-gc
```

### Exemple d'utilisation

```c
#include <stdio.h>
#include <string.h>
#include <gc.h>  // Boehm GC

void exemple_boehm_gc(void) {
    // Utiliser GC_MALLOC au lieu de malloc
    char *str1 = GC_MALLOC(100);
    strcpy(str1, "Alloué avec Boehm GC");

    char *str2 = GC_MALLOC(200);
    strcpy(str2, "Pas besoin de free()!");

    printf("str1: %s\n", str1);
    printf("str2: %s\n", str2);

    // Pas besoin de free() !
    // Le GC libérera automatiquement quand nécessaire
}

int main(void) {
    // Initialiser le GC (optionnel, fait automatiquement)
    GC_INIT();

    exemple_boehm_gc();

    // Le GC nettoie tout automatiquement
    return 0;
}
```

**Compilation :**
```bash
gcc -o program program.c -lgc
```

### Fonctionnalités de Boehm GC

```c
// Allocation normale (comme malloc)
void *ptr = GC_MALLOC(size);

// Allocation atomique (pas de pointeurs à l'intérieur)
void *data = GC_MALLOC_ATOMIC(size);

// Realloc
ptr = GC_REALLOC(ptr, new_size);

// Forcer une collection
GC_gcollect();

// Obtenir des statistiques
size_t heap_size = GC_get_heap_size();
size_t free_bytes = GC_get_free_bytes();

printf("Heap: %zu bytes, Free: %zu bytes\n", heap_size, free_bytes);
```

---

## Comparaison des approches

### Tableau comparatif

| Critère | Reference Counting | Mark & Sweep | Boehm GC |
|---------|-------------------|--------------|----------|
| **Complexité** | 🟢 Simple | 🟡 Moyenne | 🟢 Simple (bibliothèque) |
| **Cycles** | ❌ Problème | ✅ Gère | ✅ Gère |
| **Pauses** | ✅ Pas de pause | ❌ Pauses | ❌ Pauses |
| **Overhead** | 🟡 À chaque opération | 🟢 Pendant GC uniquement | 🟡 Moyen |
| **Prédictibilité** | ✅ Déterministe | ❌ Moins prévisible | ❌ Moins prévisible |
| **Portabilité** | ✅ Code portable | ✅ Code portable | ⚠️ Dépendance externe |

---

## Cas d'usage pratiques

### Quand utiliser le GC ?

#### ✅ Bons cas d'usage

1. **Applications de haut niveau**
   ```c
   // Application GUI, pas critique pour le temps réel
   char *message = gc_malloc(256);
   sprintf(message, "Hello %s", username);
   show_dialog(message);
   // Pas besoin de libérer
   ```

2. **Prototypage rapide**
   ```c
   // Phase de développement, optimisation plus tard
   void prototype(void) {
       Data *data = gc_malloc(sizeof(Data));
       // Développement rapide sans se soucier du free
   }
   ```

3. **Interpréteurs/Langages de script**
   ```c
   // Implémenter un langage avec GC automatique
   Object *create_object(VM *vm) {
       Object *obj = gc_alloc(vm->gc, sizeof(Object));
       return obj;
   }
   ```

4. **Structures de données complexes**
   ```c
   // Arbres, graphes avec références croisées
   Node *tree = build_complex_tree(gc);
   // Pas besoin de traversée pour libérer
   ```

#### ❌ Mauvais cas d'usage

1. **Systèmes temps-réel**
   ```c
   // ❌ Pauses GC inacceptables
   void control_loop(void) {
       while (1) {
           read_sensors();
           compute_control();  // Doit être déterministe
           write_actuators();
       }
   }
   ```

2. **Drivers/Noyau**
   ```c
   // ❌ Le noyau ne peut pas avoir de GC
   void driver_interrupt_handler(void) {
       // Allocation doit être prévisible et rapide
   }
   ```

3. **Code embarqué contraint**
   ```c
   // ❌ Overhead mémoire et CPU inacceptable
   void embedded_function(void) {
       // 32 KB de RAM totale, pas de place pour GC
   }
   ```

---

## GC Hybride : Le meilleur des deux mondes

Vous pouvez combiner manual memory management et GC :

```c
typedef struct {
    // Zone manuelle (critique pour la performance)
    char fast_buffer[1024];

    // Zone gérée par GC (commodité)
    char *slow_data;  // gc_malloc
} HybridData;

void fonction_hybride(GarbageCollector *gc) {
    HybridData *data = malloc(sizeof(HybridData));  // Manuel

    // Buffer rapide pour données temporaires
    sprintf(data->fast_buffer, "Fast data");

    // Données longue durée via GC
    data->slow_data = gc_alloc(gc, 1024);
    strcpy(data->slow_data, "GC-managed data");

    // Libérer la structure (mais pas slow_data)
    free(data);
    // slow_data sera collecté automatiquement
}
```

---

## Optimisations et techniques avancées

### 1. GC incrémental

Au lieu d'une grosse pause, diviser le travail en petites tranches :

```c
void gc_collect_incremental(GarbageCollector *gc, size_t max_work) {
    // Marquer quelques objets
    for (size_t i = 0; i < max_work && has_more_to_mark(); i++) {
        mark_next_object();
    }

    // Si marquage terminé, faire un peu de sweep
    if (marking_done()) {
        for (size_t i = 0; i < max_work && has_more_to_sweep(); i++) {
            sweep_next_object();
        }
    }
}

// Appeler régulièrement
void game_loop(void) {
    while (running) {
        gc_collect_incremental(&gc, 10);  // 10 unités de travail max
        update_game();
        render_frame();
    }
}
```

### 2. Generational GC

Objets jeunes vs vieux :

```c
typedef enum {
    GENERATION_YOUNG,   // Objets récents
    GENERATION_OLD      // Objets qui ont survécu plusieurs GC
} Generation;

typedef struct {
    Generation generation;
    int gc_age;  // Nombre de GC survécus
    // ...
} GenerationalObject;

void gc_collect_young_generation(GC *gc) {
    // Collecter seulement les jeunes objets (rapide)
    // La plupart des objets meurent jeunes
}

void gc_collect_full(GC *gc) {
    // Collection complète (plus lent, mais rare)
}
```

### 3. Pool allocator + GC

Combiner les deux approches :

```c
typedef struct {
    Pool pool;          // Allocation rapide
    GarbageCollector gc;  // Libération automatique
} PoolGC;

void *pool_gc_alloc(PoolGC *pg) {
    void *ptr = pool_alloc(&pg->pool);
    gc_track(&pg->gc, ptr);  // Tracker pour le GC
    return ptr;
}
```

---

## Debugging et profiling

### Tracker les allocations

```c
typedef struct {
    const char *file;
    int line;
    size_t size;
    void *ptr;
} AllocationInfo;

void *gc_alloc_debug(GC *gc, size_t size, const char *file, int line) {
    void *ptr = gc_alloc(gc, size);

    // Enregistrer l'info
    log_allocation(ptr, size, file, line);

    return ptr;
}

#define GC_ALLOC(gc, size) gc_alloc_debug(gc, size, __FILE__, __LINE__)

// Utilisation
void fonction(void) {
    char *data = GC_ALLOC(gc, 1024);
    // L'allocation est tracée avec fichier et ligne
}
```

### Détection de fuites

```c
void gc_report_leaks(GC *gc) {
    printf("\n=== Rapport de fuites ===\n");

    size_t leaked = 0;
    for (GCObject *obj = gc->first_object; obj; obj = obj->next) {
        printf("⚠️  Fuite: %p (%zu bytes)\n",
               (void *)(obj + 1), obj->size);
        leaked++;
    }

    printf("Total: %zu objet(s) non libéré(s)\n", leaked);
}
```

---

## Bonnes pratiques

### 1. ✅ Documenter l'utilisation du GC

```c
/**
 * parse_json - Parse une chaîne JSON
 *
 * @gc: Garbage collector à utiliser
 * @json: Chaîne JSON à parser
 *
 * Returns: Objet parsé (alloué via GC)
 *
 * Note: L'objet retourné est géré par le GC et sera
 * automatiquement libéré. Ne pas appeler free() dessus.
 */
JsonObject *parse_json(GC *gc, const char *json);
```

### 2. ✅ Séparer le code GC et non-GC

```c
// ✅ BON : Clair et séparé
void *alloc_managed(GC *gc, size_t size) {
    return gc_alloc(gc, size);
}

void *alloc_manual(size_t size) {
    return malloc(size);
}

// ❌ MAUVAIS : Mélange confus
void *alloc_confusing(size_t size) {
    if (use_gc) return gc_alloc(gc, size);
    else return malloc(size);
}
```

### 3. ✅ Tester avec Valgrind même avec GC

```bash
# Vérifier qu'il n'y a pas de fuites dans le GC lui-même
valgrind --leak-check=full ./program
```

### 4. ✅ Profiler l'impact du GC

```c
#include <time.h>

void profile_gc(GC *gc) {
    clock_t start = clock();
    gc_collect(gc);
    clock_t end = clock();

    double ms = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    printf("GC collection: %.2f ms\n", ms);
}
```

---

## Conclusion

Le **Garbage Collection en C** est possible et utile dans certains contextes :

### Points clés à retenir

1. **Reference Counting**
   - ✅ Simple et déterministe
   - ❌ Problème avec les cycles
   - 🎯 Bon pour objets avec ownership clair

2. **Mark and Sweep**
   - ✅ Gère les cycles
   - ❌ Pauses GC
   - 🎯 Bon pour structures complexes

3. **Boehm GC**
   - ✅ Solution clé en main
   - ❌ Dépendance externe
   - 🎯 Bon pour prototypage rapide

### Quand utiliser le GC ?

**✅ Utilisez quand :**
- Applications de haut niveau
- Structures de données complexes
- Prototypage rapide
- Interpréteurs/VMs

**❌ N'utilisez PAS quand :**
- Systèmes temps-réel
- Code noyau/drivers
- Embarqué contraint
- Performance critique absolue

### Alternatives

Avant d'implémenter un GC, considérez :
1. **Custom allocators** (arenas, pools) - Section 24.2
2. **cleanup attribute** - Section 24.1
3. **Ownership discipline** - Conventions claires de qui libère quoi

---

**💡 Conseil final :** Le GC est un outil puissant, mais pas une solution miracle. En C, la gestion manuelle reste souvent le meilleur choix pour la performance et le contrôle. Utilisez le GC quand il simplifie vraiment votre code sans sacrifier vos contraintes de performance.

---

**📚 Ressources complémentaires :**
- [Boehm GC Documentation](https://www.hboehm.info/gc/)
- [Garbage Collection: Algorithms for Automatic Dynamic Memory Management](https://www.amazon.com/Garbage-Collection-Algorithms-Automatic-Management/dp/0471941484) - Livre de référence
- [Memory Management Reference](https://www.memorymanagement.org/) - Ressource complète sur la gestion mémoire

⏭️ [Gestion des erreurs robuste](/25-gestion-erreurs/README.md)
