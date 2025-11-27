🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 18.12 Thread-local storage

## Table des matières
- [Introduction](#introduction)
- [Le problème des variables globales](#le-probl%C3%A8me-des-variables-globales)
- [Qu'est-ce que le Thread-Local Storage ?](#quest-ce-que-le-thread-local-storage-)
- [Méthode 1 : _Thread_local (C11)](#m%C3%A9thode-1--_thread_local-c11)
- [Méthode 2 : pthread_key_t (POSIX)](#m%C3%A9thode-2--pthread_key_t-posix)
- [Comparaison des méthodes](#comparaison-des-m%C3%A9thodes)
- [Cas d'usage classiques](#cas-dusage-classiques)
- [TLS et bibliothèques](#tls-et-biblioth%C3%A8ques)
- [Patterns avancés](#patterns-avanc%C3%A9s)
- [Performance et limitations](#performance-et-limitations)
- [Erreurs courantes](#erreurs-courantes)
- [Quand utiliser TLS ?](#quand-utiliser-tls-)

---

## Introduction

Dans les programmes multi-threadés, les variables **globales** sont partagées entre tous les threads. Parfois, on a besoin que chaque thread ait sa **propre copie** d'une variable, sans partage.

**Exemple problématique** :
```c
// Variable globale partagée
int compteur = 0;

void *thread_func(void *arg) {
    for (int i = 0; i < 1000; i++) {
        compteur++;  // ❌ Race condition
    }
    return NULL;
}
```

**Solutions connues** :
1. ✅ Protéger avec un mutex (mais lent)
2. ✅ Passer en paramètre (mais compliqué)
3. ✅ **Thread-Local Storage** : Chaque thread a sa propre copie

**Thread-Local Storage (TLS)** permet de déclarer des variables qui sont **uniques par thread**, comme des variables locales mais avec portée globale.

---

## Le problème des variables globales

### Scénario : Buffer de formatage

Imaginez une fonction qui formate des chaînes dans un buffer global :

```c
// Buffer global partagé
char buffer[256];

char *format_message(int id, const char *msg) {
    snprintf(buffer, sizeof(buffer), "[%d] %s", id, msg);
    return buffer;  // Retourne le buffer global
}

void *thread_func(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 10; i++) {
        char *msg = format_message(id, "Hello");
        printf("%s\n", msg);  // ❌ Peut afficher n'importe quoi !
        usleep(100);
    }

    return NULL;
}
```

**Problème** :
```
Thread 1 : format_message(1, "Hello") → buffer = "[1] Hello"
Thread 2 : format_message(2, "World") → buffer = "[2] World"  ← Écrase T1
Thread 1 : printf("%s") → Affiche "[2] World" au lieu de "[1] Hello" !
```

### Solutions possibles

**Solution 1 : Mutex (lent)**
```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
char buffer[256];

char *format_message(int id, const char *msg) {
    pthread_mutex_lock(&mutex);  // Sérialise tout
    snprintf(buffer, sizeof(buffer), "[%d] %s", id, msg);
    char *result = strdup(buffer);  // Copie nécessaire
    pthread_mutex_unlock(&mutex);
    return result;  // Doit être libéré par l'appelant
}
```

**Problèmes** :
- Sérialisation (pas de concurrence)
- Allocation mémoire supplémentaire
- Gestion de la mémoire compliquée

**Solution 2 : Paramètre (verbeux)**
```c
char *format_message(char *buffer, size_t size, int id, const char *msg) {
    snprintf(buffer, size, "[%d] %s", id, msg);
    return buffer;
}

void *thread_func(void *arg) {
    char buffer[256];  // Buffer local
    // ...
    format_message(buffer, sizeof(buffer), id, "Hello");
}
```

**Problèmes** :
- API verbeuse
- Buffer doit être passé partout

**Solution 3 : Thread-Local Storage (élégant)**
```c
_Thread_local char buffer[256];  // Une copie par thread

char *format_message(int id, const char *msg) {
    snprintf(buffer, sizeof(buffer), "[%d] %s", id, msg);
    return buffer;  // Chaque thread a son propre buffer
}
```

**Avantages** :
- ✅ Pas de mutex (rapide)
- ✅ Pas d'allocation dynamique
- ✅ API simple
- ✅ Chaque thread a sa copie isolée

---

## Qu'est-ce que le Thread-Local Storage ?

### Définition

Le **Thread-Local Storage (TLS)** est un mécanisme qui permet de créer des variables dont chaque thread possède une **copie indépendante**.

**Analogie** : Imaginez un casier personnel à l'école.
- **Variable globale** : Un seul tableau partagé par tous les élèves
- **Variable TLS** : Chaque élève a son propre casier (même nom, contenu différent)

### Visualisation

```
Variable globale :
==================
┌─────────────────────────────────┐
│    int compteur = 0;            │
│                                 │
│    [Mémoire partagée]           │
│                                 │
│    Thread 1 ──┐                 │
│               ├─→ compteur      │
│    Thread 2 ──┤                 │
│               └─→ (même case)   │
│    Thread 3 ──┘                 │
└─────────────────────────────────┘


Variable TLS :
==============
┌─────────────────────────────────┐
│ _Thread_local int compteur = 0; │
│                                 │
│ Thread 1 → compteur (copie 1)   │
│ Thread 2 → compteur (copie 2)   │
│ Thread 3 → compteur (copie 3)   │
│                                 │
│ Chacun a sa propre copie        │
└─────────────────────────────────┘
```

### Caractéristiques

**Variables TLS** :
- ✅ Portée globale (accessible partout dans le thread)
- ✅ Durée de vie liée au thread
- ✅ Initialisation à chaque création de thread
- ✅ Pas de synchronisation nécessaire
- ❌ Ne sont **pas** partagées entre threads

---

## Méthode 1 : _Thread_local (C11)

### Syntaxe

```c
_Thread_local type nom_variable = valeur_initiale;
```

**Note** : Le standard C11 définit aussi `thread_local` via `<threads.h>`, mais il est moins supporté.

### Exemple basique

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Variable TLS : Chaque thread a sa copie
_Thread_local int thread_id = 0;
_Thread_local int compteur = 0;

void *thread_func(void *arg) {
    thread_id = *(int *)arg;  // Initialiser l'ID du thread

    for (int i = 0; i < 5; i++) {
        compteur++;
        printf("Thread %d : compteur = %d\n", thread_id, compteur);
        sleep(1);
    }

    return NULL;
}

int main(void) {
    pthread_t t1, t2, t3;
    int ids[] = {1, 2, 3};

    pthread_create(&t1, NULL, thread_func, &ids[0]);
    pthread_create(&t2, NULL, thread_func, &ids[1]);
    pthread_create(&t3, NULL, thread_func, &ids[2]);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    return 0;
}
```

**Sortie** :
```
Thread 1 : compteur = 1
Thread 2 : compteur = 1  ← Chaque thread compte indépendamment
Thread 3 : compteur = 1
Thread 1 : compteur = 2
Thread 2 : compteur = 2
Thread 3 : compteur = 2
...
```

### Portée et liaison

Les variables TLS peuvent avoir différentes portées :

```c
// TLS globale (visible dans tout le fichier)
_Thread_local int global_tls = 0;

// TLS statique (visible seulement dans ce fichier)
static _Thread_local int static_tls = 0;

void fonction(void) {
    // TLS locale à la fonction (static obligatoire)
    static _Thread_local int local_tls = 0;

    local_tls++;
    printf("local_tls = %d\n", local_tls);
}
```

### Types supportés

```c
// Types simples
_Thread_local int entier = 0;
_Thread_local double reel = 0.0;
_Thread_local char caractere = 'A';

// Pointeurs
_Thread_local int *pointeur = NULL;

// Tableaux
_Thread_local char buffer[256];

// Structures
typedef struct {
    int x;
    int y;
} Point;
_Thread_local Point position = {0, 0};

// ❌ ERREUR : Pas d'initialisation dynamique
// _Thread_local int *ptr = malloc(100);  // Pas autorisé !
```

### Initialisation

```c
// Valeur constante : OK
_Thread_local int x = 42;

// Constante calculée au compile-time : OK
_Thread_local int y = 2 + 3;

// ❌ Expression non-constante : ERREUR
// _Thread_local int z = fonction();
// _Thread_local int *p = malloc(100);
```

**Pour initialisation dynamique**, utilisez une fonction d'initialisation :

```c
_Thread_local char *buffer = NULL;

void init_thread_local(void) {
    if (buffer == NULL) {
        buffer = malloc(1024);
    }
}

void *thread_func(void *arg) {
    init_thread_local();  // Initialiser au démarrage du thread

    // Utiliser buffer...

    free(buffer);  // Nettoyer avant de terminer
    return NULL;
}
```

---

## Méthode 2 : pthread_key_t (POSIX)

### Concept

L'API POSIX fournit un système de **clés** pour stocker des données spécifiques à chaque thread.

**Analogie** : Un casier avec clé
- **Clé** : `pthread_key_t` (partagée entre tous)
- **Casier** : Chaque thread a son propre casier associé à cette clé
- **Contenu** : `void *` (pointeur vers n'importe quoi)

### Étapes d'utilisation

1. **Créer une clé** (une fois, partagée)
2. **Associer une valeur** (dans chaque thread)
3. **Récupérer la valeur** (dans chaque thread)
4. **Détruire la clé** (à la fin)

### API complète

#### pthread_key_create() : Créer une clé

```c
int pthread_key_create(pthread_key_t *key, void (*destructor)(void *));
```

**Paramètres** :
- `key` : Pointeur vers la clé à créer
- `destructor` : Fonction appelée automatiquement quand le thread se termine (peut être NULL)

**Retour** : 0 en cas de succès

#### pthread_setspecific() : Associer une valeur

```c
int pthread_setspecific(pthread_key_t key, const void *value);
```

**Paramètres** :
- `key` : La clé
- `value` : Pointeur vers la valeur (propre au thread)

#### pthread_getspecific() : Récupérer la valeur

```c
void *pthread_getspecific(pthread_key_t key);
```

**Retour** : Le pointeur associé à cette clé pour ce thread (NULL si aucun)

#### pthread_key_delete() : Détruire la clé

```c
int pthread_key_delete(pthread_key_t key);
```

**Note** : Ne libère **pas** les valeurs associées, seulement la clé elle-même.

### Exemple complet

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_key_t thread_data_key;

// Structure de données thread-local
typedef struct {
    int id;
    int compteur;
} ThreadData;

// Destructeur : Libère automatiquement la mémoire
void cleanup_thread_data(void *data) {
    printf("Nettoyage des données du thread\n");
    free(data);
}

void *thread_func(void *arg) {
    int id = *(int *)arg;

    // Allouer et initialiser les données TLS
    ThreadData *data = malloc(sizeof(ThreadData));
    data->id = id;
    data->compteur = 0;

    // Associer à la clé
    pthread_setspecific(thread_data_key, data);

    // Utiliser les données
    for (int i = 0; i < 5; i++) {
        // Récupérer les données du thread
        ThreadData *my_data = pthread_getspecific(thread_data_key);

        my_data->compteur++;
        printf("Thread %d : compteur = %d\n",
               my_data->id, my_data->compteur);

        sleep(1);
    }

    // Le destructeur sera appelé automatiquement
    return NULL;
}

int main(void) {
    pthread_t threads[3];
    int ids[] = {1, 2, 3};

    // Créer la clé (avec destructeur)
    pthread_key_create(&thread_data_key, cleanup_thread_data);

    // Créer les threads
    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }

    // Attendre
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    // Détruire la clé
    pthread_key_delete(&thread_data_key);

    return 0;
}
```

### Destructeur automatique

Le **destructeur** est très utile pour éviter les fuites mémoire :

```c
void cleanup(void *ptr) {
    printf("Libération automatique de %p\n", ptr);
    free(ptr);
}

pthread_key_create(&key, cleanup);  // Avec destructeur

// Dans le thread
char *buffer = malloc(1024);
pthread_setspecific(key, buffer);

// À la fin du thread : cleanup(buffer) appelé automatiquement
```

**Ordre d'appel** : Les destructeurs sont appelés dans un ordre **non défini** à la fin du thread.

### pthread_once() : Initialisation unique

Pour créer la clé **une seule fois** de manière thread-safe :

```c
pthread_key_t key;
pthread_once_t once_control = PTHREAD_ONCE_INIT;

void create_key(void) {
    pthread_key_create(&key, cleanup);
    printf("Clé créée une seule fois\n");
}

void *thread_func(void *arg) {
    // S'assurer que la clé est créée (thread-safe)
    pthread_once(&once_control, create_key);

    // Utiliser la clé...
    char *data = malloc(100);
    pthread_setspecific(key, data);

    return NULL;
}
```

---

## Comparaison des méthodes

### Tableau comparatif

| Critère | _Thread_local (C11) | pthread_key_t (POSIX) |
|---------|---------------------|------------------------|
| **Standard** | C11 | POSIX |
| **Syntaxe** | Simple | Complexe (API) |
| **Performance** | Très rapide (direct) | Légèrement plus lent |
| **Types** | Tous types | Pointeurs seulement |
| **Initialisation** | Compile-time | Runtime |
| **Destructeur** | Non | Oui (automatique) |
| **Portabilité** | C11+ | POSIX (large) |
| **Use case** | Variables simples | Structures allouées |

### Exemple côte à côte

**Avec _Thread_local** :
```c
_Thread_local int compteur = 0;

void *thread_func(void *arg) {
    compteur++;
    printf("%d\n", compteur);
    return NULL;
}
```

**Avec pthread_key_t** :
```c
pthread_key_t compteur_key;

void init_key(void) {
    pthread_key_create(&compteur_key, free);
}

void *thread_func(void *arg) {
    int *compteur = malloc(sizeof(int));
    *compteur = 0;
    pthread_setspecific(compteur_key, compteur);

    (*compteur)++;
    printf("%d\n", *compteur);

    return NULL;
}
```

### Recommandations

**Utilisez _Thread_local si** :
- Vous avez C11 ou plus récent
- Variables simples (int, char[], struct)
- Performance critique
- Pas besoin de destructeur automatique

**Utilisez pthread_key_t si** :
- Besoin de destructeur automatique
- Allocation dynamique complexe
- Compatibilité avec C99
- Bibliothèque partagée avec plusieurs threads

---

## Cas d'usage classiques

### 1. errno (thread-safe)

La variable `errno` est TLS pour être thread-safe :

```c
// Dans la glibc : errno est TLS
#define errno (*__errno_location())

// Implémentation simplifiée
_Thread_local int thread_errno = 0;

int *__errno_location(void) {
    return &thread_errno;
}

// Chaque thread a son propre errno
void *thread_func(void *arg) {
    int fd = open("fichier.txt", O_RDONLY);
    if (fd == -1) {
        printf("Erreur : %d\n", errno);  // errno du thread
    }
    return NULL;
}
```

### 2. Buffer de formatage

```c
_Thread_local char format_buffer[1024];

char *thread_safe_format(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(format_buffer, sizeof(format_buffer), fmt, args);
    va_end(args);
    return format_buffer;  // Safe : buffer unique par thread
}

void *thread_func(void *arg) {
    char *msg1 = thread_safe_format("Message %d", 1);
    printf("%s\n", msg1);  // OK

    char *msg2 = thread_safe_format("Message %d", 2);
    printf("%s\n", msg2);  // OK (écrase msg1 dans CE thread)

    return NULL;
}
```

### 3. Contexte de thread

```c
typedef struct {
    int thread_id;
    char name[64];
    void *user_data;
    int error_count;
} ThreadContext;

_Thread_local ThreadContext *context = NULL;

void init_thread_context(int id, const char *name) {
    context = malloc(sizeof(ThreadContext));
    context->thread_id = id;
    strncpy(context->name, name, sizeof(context->name) - 1);
    context->user_data = NULL;
    context->error_count = 0;
}

void cleanup_thread_context(void) {
    free(context);
    context = NULL;
}

void log_error(const char *msg) {
    if (context) {
        context->error_count++;
        printf("[Thread %d:%s] Erreur #%d: %s\n",
               context->thread_id, context->name,
               context->error_count, msg);
    }
}

void *worker(void *arg) {
    init_thread_context(*(int *)arg, "Worker");

    // Simuler des erreurs
    log_error("Connexion échouée");
    log_error("Timeout");

    cleanup_thread_context();
    return NULL;
}
```

### 4. Générateur de nombres aléatoires

```c
#include <stdlib.h>
#include <time.h>

_Thread_local unsigned int random_seed = 0;

void init_random(void) {
    // Seed unique par thread
    random_seed = time(NULL) ^ (unsigned int)pthread_self();
}

int thread_safe_random(void) {
    if (random_seed == 0) {
        init_random();
    }
    return rand_r(&random_seed);
}

void *thread_func(void *arg) {
    // Chaque thread a sa propre séquence aléatoire
    for (int i = 0; i < 5; i++) {
        printf("Random : %d\n", thread_safe_random());
    }
    return NULL;
}
```

### 5. Pool de buffers

```c
#define NUM_BUFFERS 10

typedef struct {
    char buffers[NUM_BUFFERS][256];
    int current;
} BufferPool;

_Thread_local BufferPool pool = {.current = 0};

char *get_buffer(void) {
    char *buf = pool.buffers[pool.current];
    pool.current = (pool.current + 1) % NUM_BUFFERS;
    return buf;
}

// Utilisation
void *thread_func(void *arg) {
    char *b1 = get_buffer();
    strcpy(b1, "Premier");

    char *b2 = get_buffer();
    strcpy(b2, "Deuxième");

    printf("%s, %s\n", b1, b2);  // OK : buffers distincts

    return NULL;
}
```

---

## TLS et bibliothèques

### Bibliothèque thread-safe avec TLS

```c
// library.h
void lib_init(void);
void lib_cleanup(void);
int lib_operation(void);

// library.c
#include <pthread.h>
#include <stdlib.h>

typedef struct {
    int state;
    char *buffer;
    int initialized;
} LibContext;

pthread_key_t lib_context_key;
pthread_once_t key_once = PTHREAD_ONCE_INIT;

void destroy_context(void *ctx) {
    LibContext *context = ctx;
    if (context) {
        free(context->buffer);
        free(context);
    }
}

void create_key(void) {
    pthread_key_create(&lib_context_key, destroy_context);
}

LibContext *get_context(void) {
    pthread_once(&key_once, create_key);

    LibContext *ctx = pthread_getspecific(lib_context_key);
    if (!ctx) {
        // Première utilisation dans ce thread
        ctx = malloc(sizeof(LibContext));
        ctx->state = 0;
        ctx->buffer = malloc(1024);
        ctx->initialized = 1;
        pthread_setspecific(lib_context_key, ctx);
    }

    return ctx;
}

void lib_init(void) {
    LibContext *ctx = get_context();
    ctx->state = 1;
}

int lib_operation(void) {
    LibContext *ctx = get_context();

    if (!ctx->initialized) {
        return -1;  // Pas initialisé
    }

    // Utiliser ctx->buffer, ctx->state, etc.
    sprintf(ctx->buffer, "État : %d", ctx->state);

    return 0;
}
```

### Pattern singleton par thread

```c
typedef struct {
    int data;
    // ...
} Resource;

_Thread_local Resource *singleton = NULL;

Resource *get_resource(void) {
    if (singleton == NULL) {
        singleton = malloc(sizeof(Resource));
        singleton->data = 0;
    }
    return singleton;
}

void cleanup_resource(void) {
    if (singleton) {
        free(singleton);
        singleton = NULL;
    }
}

// Utilisation
void *thread_func(void *arg) {
    Resource *r = get_resource();
    r->data = 42;

    // Travail...

    cleanup_resource();
    return NULL;
}
```

---

## Patterns avancés

### 1. Cache thread-local

```c
#define CACHE_SIZE 100

typedef struct {
    char key[50];
    char value[200];
    int valid;
} CacheEntry;

_Thread_local struct {
    CacheEntry entries[CACHE_SIZE];
    int hits;
    int misses;
} thread_cache = {.hits = 0, .misses = 0};

int cache_lookup(const char *key, char *value) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (thread_cache.entries[i].valid &&
            strcmp(thread_cache.entries[i].key, key) == 0) {
            strcpy(value, thread_cache.entries[i].value);
            thread_cache.hits++;
            return 1;
        }
    }

    thread_cache.misses++;
    return 0;
}

void print_cache_stats(void) {
    printf("Thread %lu : %d hits, %d misses\n",
           pthread_self(), thread_cache.hits, thread_cache.misses);
}
```

### 2. Profiling par thread

```c
#include <time.h>

typedef struct {
    const char *name;
    struct timespec start;
    double total_time;
    int call_count;
} ProfileEntry;

#define MAX_PROFILES 50

_Thread_local struct {
    ProfileEntry entries[MAX_PROFILES];
    int count;
} profiler = {.count = 0};

void profile_start(const char *name) {
    for (int i = 0; i < profiler.count; i++) {
        if (strcmp(profiler.entries[i].name, name) == 0) {
            clock_gettime(CLOCK_MONOTONIC, &profiler.entries[i].start);
            return;
        }
    }

    // Nouvelle entrée
    if (profiler.count < MAX_PROFILES) {
        ProfileEntry *entry = &profiler.entries[profiler.count++];
        entry->name = name;
        entry->total_time = 0.0;
        entry->call_count = 0;
        clock_gettime(CLOCK_MONOTONIC, &entry->start);
    }
}

void profile_end(const char *name) {
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);

    for (int i = 0; i < profiler.count; i++) {
        if (strcmp(profiler.entries[i].name, name) == 0) {
            ProfileEntry *entry = &profiler.entries[i];
            double elapsed = (end.tv_sec - entry->start.tv_sec) +
                            (end.tv_nsec - entry->start.tv_nsec) / 1e9;
            entry->total_time += elapsed;
            entry->call_count++;
            return;
        }
    }
}

void print_profile(void) {
    printf("\n=== Profiling Thread %lu ===\n", pthread_self());
    for (int i = 0; i < profiler.count; i++) {
        ProfileEntry *entry = &profiler.entries[i];
        printf("%s: %.6f s (%d calls, avg %.6f s)\n",
               entry->name, entry->total_time, entry->call_count,
               entry->total_time / entry->call_count);
    }
}

// Utilisation
void *worker(void *arg) {
    profile_start("task1");
    // Travail 1
    sleep(1);
    profile_end("task1");

    profile_start("task2");
    // Travail 2
    sleep(2);
    profile_end("task2");

    print_profile();
    return NULL;
}
```

### 3. Stack trace thread-local

```c
#define MAX_STACK_DEPTH 50

typedef struct {
    const char *functions[MAX_STACK_DEPTH];
    int depth;
} CallStack;

_Thread_local CallStack call_stack = {.depth = 0};

void stack_enter(const char *func) {
    if (call_stack.depth < MAX_STACK_DEPTH) {
        call_stack.functions[call_stack.depth++] = func;
    }
}

void stack_exit(void) {
    if (call_stack.depth > 0) {
        call_stack.depth--;
    }
}

void print_stack_trace(void) {
    printf("Stack trace for thread %lu:\n", pthread_self());
    for (int i = call_stack.depth - 1; i >= 0; i--) {
        printf("  [%d] %s\n", call_stack.depth - i - 1,
               call_stack.functions[i]);
    }
}

// Macros helper
#define ENTER() stack_enter(__func__)
#define EXIT() stack_exit()

// Utilisation
void fonction_c(void) {
    ENTER();
    print_stack_trace();
    EXIT();
}

void fonction_b(void) {
    ENTER();
    fonction_c();
    EXIT();
}

void fonction_a(void) {
    ENTER();
    fonction_b();
    EXIT();
}
```

---

## Performance et limitations

### Performance

**_Thread_local** :
- Accès très rapide (direct, comme variable locale)
- Pas d'overhead de synchronisation
- Optimisé par le compilateur

**pthread_key_t** :
- Légèrement plus lent (recherche dans table)
- Overhead minimal mais mesurable
- ~10-20 nanosecondes par accès

### Benchmark

```c
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define ITERATIONS 10000000

// Variable globale (avec mutex)
int global_var = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// TLS
_Thread_local int tls_var = 0;

// pthread_key
pthread_key_t key;

void benchmark_global(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);
        global_var++;
        pthread_mutex_unlock(&mutex);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Global (mutex) : %.3f s\n", elapsed);
}

void benchmark_tls(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < ITERATIONS; i++) {
        tls_var++;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("TLS            : %.3f s\n", elapsed);
}

void benchmark_key(void) {
    int *val = malloc(sizeof(int));
    *val = 0;
    pthread_setspecific(key, val);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < ITERATIONS; i++) {
        int *v = pthread_getspecific(key);
        (*v)++;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("pthread_key    : %.3f s\n", elapsed);

    free(val);
}

int main(void) {
    pthread_key_create(&key, free);

    printf("Benchmark (%d itérations) :\n", ITERATIONS);
    benchmark_global();
    benchmark_tls();
    benchmark_key();

    pthread_key_delete(&key);
    return 0;
}
```

**Résultats typiques** :
```
Benchmark (10000000 itérations) :
Global (mutex) : 2.347 s
TLS            : 0.023 s  ← ~100x plus rapide
pthread_key    : 0.045 s  ← ~50x plus rapide
```

### Limitations

**Nombre maximum** :
- TLS : Dépend du compilateur (généralement illimité)
- pthread_key : Limité (PTHREAD_KEYS_MAX, typiquement 1024)

**Taille** :
- TLS : Pas de limite (mais mémoire par thread)
- pthread_key : Stocke seulement un pointeur

**Exemple de vérification** :
```c
#include <limits.h>

printf("Maximum pthread_keys : %d\n", PTHREAD_KEYS_MAX);
```

---

## Erreurs courantes

### 1. Oublier d'initialiser

```c
// ❌ ERREUR : Utilisation avant initialisation
_Thread_local char *buffer = NULL;

void fonction(void) {
    strcpy(buffer, "test");  // CRASH : buffer est NULL
}

// ✅ CORRECT
void fonction(void) {
    if (buffer == NULL) {
        buffer = malloc(1024);
    }
    strcpy(buffer, "test");
}
```

### 2. Fuites mémoire

```c
// ❌ ERREUR : Fuite mémoire
pthread_key_t key;

void *thread_func(void *arg) {
    char *data = malloc(1024);
    pthread_setspecific(key, data);
    // Pas de free → fuite
    return NULL;
}

// ✅ CORRECT : Utiliser un destructeur
void cleanup(void *data) {
    free(data);
}

pthread_key_create(&key, cleanup);  // Destructeur automatique
```

### 3. Confusion TLS et globale

```c
// ❌ CONFUSION
int compteur = 0;  // Variable globale (partagée)

void *thread_func(void *arg) {
    compteur++;  // Race condition !
    return NULL;
}

// ✅ CORRECT
_Thread_local int compteur = 0;  // TLS (isolée)

void *thread_func(void *arg) {
    compteur++;  // Safe
    return NULL;
}
```

### 4. Accès depuis un autre thread

```c
// ❌ IMPOSSIBLE
_Thread_local int thread1_data = 0;

void *thread1(void *arg) {
    thread1_data = 42;
    return NULL;
}

void *thread2(void *arg) {
    printf("%d\n", thread1_data);  // Affiche 0, pas 42 !
    return NULL;
}
```

**Explication** : Chaque thread a sa propre copie. Thread2 voit sa propre copie (initialisée à 0).

### 5. Utilisation après fin du thread

```c
// ❌ DANGEREUX
_Thread_local char buffer[256];

char *get_buffer(void) {
    return buffer;  // Retourne un pointeur TLS
}

void *thread_func(void *arg) {
    char *buf = get_buffer();
    strcpy(buf, "test");
    return buf;  // ❌ Pointeur vers TLS qui va disparaître
}

int main(void) {
    pthread_t t;
    void *result;
    pthread_create(&t, NULL, thread_func, NULL);
    pthread_join(t, &result);

    printf("%s\n", (char *)result);  // ❌ Comportement indéfini
}
```

---

## Quand utiliser TLS ?

### ✅ Utilisez TLS pour :

1. **Éviter la synchronisation**
   ```c
   // Chaque thread a son buffer → Pas de mutex
   _Thread_local char format_buffer[256];
   ```

2. **Contexte de thread**
   ```c
   _Thread_local ThreadContext context;
   ```

3. **Caches locaux**
   ```c
   _Thread_local Cache thread_cache;
   ```

4. **État de bibliothèque**
   ```c
   _Thread_local int lib_initialized = 0;
   ```

5. **Générateurs aléatoires**
   ```c
   _Thread_local unsigned int seed;
   ```

### ❌ N'utilisez PAS TLS pour :

1. **Données devant être partagées**
   ```c
   // ❌ Mauvais usage
   _Thread_local int compteur_total;  // Chaque thread compte séparément
   ```

2. **Grandes structures**
   ```c
   // ❌ Gaspillage mémoire
   _Thread_local char huge_buffer[1000000];  // Par thread !
   ```

3. **Communication entre threads**
   ```c
   // ❌ Ne fonctionne pas
   _Thread_local Message message;  // Pas visible par autres threads
   ```

4. **Quand un mutex suffit**
   ```c
   // Si accès rare, mutex plus simple
   ```

### Tableau de décision

| Besoin | Solution |
|--------|----------|
| Protection données partagées | Mutex |
| Attente condition | Condition variable |
| Comptage ressources | Sémaphore |
| Isolation par thread | **TLS** |
| Communication threads | File + mutex |

---

## Récapitulatif

Dans cette section, vous avez appris :

✅ **Thread-Local Storage** :
- Chaque thread a sa propre copie
- Pas de partage entre threads
- Pas de synchronisation nécessaire

✅ **Deux méthodes** :
- `_Thread_local` (C11) : Simple, rapide
- `pthread_key_t` (POSIX) : Flexible, destructeur auto

✅ **_Thread_local** :
- Syntaxe : `_Thread_local type var = init;`
- Initialisation compile-time
- Types natifs, tableaux, structures
- Très performant

✅ **pthread_key_t** :
- `pthread_key_create()` : Créer clé
- `pthread_setspecific()` : Associer valeur
- `pthread_getspecific()` : Récupérer valeur
- `pthread_key_delete()` : Détruire clé
- Destructeur automatique

✅ **Cas d'usage** :
- errno thread-safe
- Buffers de formatage
- Contexte de thread
- Générateurs aléatoires
- Caches locaux

✅ **Performance** :
- TLS ~100x plus rapide que mutex
- pthread_key ~50x plus rapide que mutex
- Pas d'overhead de synchronisation

✅ **Erreurs à éviter** :
- Oublier initialisation dynamique
- Fuites mémoire (sans destructeur)
- Confusion TLS/globale
- Partage impossible entre threads
- Utilisation après fin du thread

✅ **Quand utiliser** :
- Éviter synchronisation
- Données isolées par thread
- Performance critique
- Pas de communication inter-threads

---

## Pour aller plus loin

Dans les prochaines sections :

- **18.13** : Barrières de threads (pthread_barrier)
- **19** : Signaux et threads
- **20** : Patterns avancés de programmation concurrente

**Conseil pratique** : Créez un système de logging thread-safe avec TLS :
- Buffer de log par thread
- Flush périodique vers fichier central
- Mesurez la différence de performance avec un mutex global

**Projet** : Implémentez un pool de workers avec :
- Contexte TLS par worker (ID, statistiques)
- Cache local des résultats
- Profiling automatique par thread
- Affichage des stats à la fin

---

📘 **Ressources utiles** :
- C11 Standard : Section 6.7.1 (Storage-class specifiers)
- `man pthread_key_create`
- `man pthread_getspecific`
- GCC : `-ftls-model` (options d'optimisation TLS)

💡 **Prochaine étape** : Section 18.13 - Barrières de threads

**Citation** : *"Thread-local storage: global scope, thread-local lifetime."* - Principe du TLS

⏭️ [Barrières de threads](/18-threads-et-concurrence/13-barrieres-threads.md)
