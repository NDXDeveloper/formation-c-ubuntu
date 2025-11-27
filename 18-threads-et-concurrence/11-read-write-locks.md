🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 18.11 Read-write locks

## Table des matières
- [Introduction](#introduction)
- [Le problème avec les mutex simples](#le-probl%C3%A8me-avec-les-mutex-simples)
- [Qu'est-ce qu'un read-write lock ?](#quest-ce-quun-read-write-lock-)
- [Règles des read-write locks](#r%C3%A8gles-des-read-write-locks)
- [Initialisation et destruction](#initialisation-et-destruction)
- [Opérations de base](#op%C3%A9rations-de-base)
- [Exemple simple](#exemple-simple)
- [Cas d'usage : Cache de données](#cas-dusage--cache-de-donn%C3%A9es)
- [Politiques de priorité](#politiques-de-priorit%C3%A9)
- [Fonctions avec timeout et trylock](#fonctions-avec-timeout-et-trylock)
- [Performance : Mutex vs RWLock](#performance--mutex-vs-rwlock)
- [Patterns avancés](#patterns-avanc%C3%A9s)
- [Erreurs courantes](#erreurs-courantes)
- [Quand utiliser les read-write locks ?](#quand-utiliser-les-read-write-locks-)
- [Alternatives](#alternatives)

---

## Introduction

Jusqu'à présent, nous avons utilisé des **mutex** pour protéger les données partagées. Un mutex garantit qu'**un seul thread à la fois** peut accéder aux données, que ce soit pour lire ou écrire.

**Question** : Est-ce vraiment nécessaire de bloquer les lecteurs entre eux ?

**Réponse** : Non ! Plusieurs threads peuvent **lire simultanément** sans problème. Le problème survient uniquement quand quelqu'un **écrit**.

Les **read-write locks** (verrous lecteur-écrivain) optimisent cette situation en permettant :
- ✅ **Plusieurs lecteurs** simultanés
- ✅ **Un seul écrivain** à la fois (exclusif)
- ❌ **Pas de lecteur** pendant une écriture

**Analogie** : Une bibliothèque
- **Lecteurs** : Plusieurs personnes peuvent consulter des livres en même temps
- **Écrivain** : Quand le bibliothécaire réorganise les étagères, il faut fermer la bibliothèque

---

## Le problème avec les mutex simples

### Scénario : Base de données en cache

Imaginons un cache en mémoire avec :
- **90% de lectures** (consultations)
- **10% d'écritures** (mises à jour)

### Avec un mutex simple

```c
typedef struct {
    char data[1000];
    pthread_mutex_t mutex;
} Cache;

Cache cache;

// Lecture
void lire_cache(void) {
    pthread_mutex_lock(&cache.mutex);  // 🔒 Bloque TOUT LE MONDE

    // Lecture des données
    printf("Data: %s\n", cache.data);

    pthread_mutex_unlock(&cache.mutex);
}

// Écriture
void ecrire_cache(const char *nouvelles_donnees) {
    pthread_mutex_lock(&cache.mutex);

    strcpy(cache.data, nouvelles_donnees);

    pthread_mutex_unlock(&cache.mutex);
}
```

**Problème** :
```
Thread 1 (lecteur) : lock → LIT → unlock
Thread 2 (lecteur) : ⏳ BLOQUÉ (alors qu'il pourrait lire en même temps)
Thread 3 (lecteur) : ⏳ BLOQUÉ
Thread 4 (lecteur) : ⏳ BLOQUÉ
```

**Gaspillage** : Les lecteurs se bloquent mutuellement alors qu'ils pourraient tous lire en même temps !

### Avec un read-write lock

```c
typedef struct {
    char data[1000];
    pthread_rwlock_t rwlock;  // ← Read-Write Lock
} Cache;

Cache cache;

// Lecture
void lire_cache(void) {
    pthread_rwlock_rdlock(&cache.rwlock);  // 🔓 Plusieurs lecteurs OK

    printf("Data: %s\n", cache.data);

    pthread_rwlock_unlock(&cache.rwlock);
}

// Écriture
void ecrire_cache(const char *nouvelles_donnees) {
    pthread_rwlock_wrlock(&cache.rwlock);  // 🔒 Exclusif

    strcpy(cache.data, nouvelles_donnees);

    pthread_rwlock_unlock(&cache.rwlock);
}
```

**Avantage** :
```
Thread 1 (lecteur) : rdlock → LIT ──┐
Thread 2 (lecteur) : rdlock → LIT ──┤ Tous en même temps !
Thread 3 (lecteur) : rdlock → LIT ──┤
Thread 4 (lecteur) : rdlock → LIT ──┘
```

---

## Qu'est-ce qu'un read-write lock ?

### Définition

Un **read-write lock** (RWLock) est un mécanisme de synchronisation qui distingue deux types d'accès :

1. **Lecture (read)** : Accès partagé
   - Plusieurs threads peuvent lire simultanément
   - Aucune modification des données

2. **Écriture (write)** : Accès exclusif
   - Un seul thread peut écrire
   - Aucun autre thread (lecteur ou écrivain) ne peut accéder

### Visualisation

```
États possibles d'un RWLock :

État 1 : LIBRE
┌────────────────┐
│   UNLOCKED     │
│                │
└────────────────┘

État 2 : LECTURE (partagé)
┌────────────────┐
│  N Lecteurs    │
│  [R1][R2][R3]  │ ← Plusieurs lecteurs OK
└────────────────┘

État 3 : ÉCRITURE (exclusif)
┌────────────────┐
│  1 Écrivain    │
│      [W1]      │ ← Un seul, bloque tout
└────────────────┘
```

### Type POSIX

```c
#include <pthread.h>

pthread_rwlock_t mon_rwlock;
```

---

## Règles des read-write locks

### Règles d'accès

| Situation | Nouveau lecteur | Nouvel écrivain |
|-----------|-----------------|-----------------|
| **RWLock libre** | ✅ Accès immédiat | ✅ Accès immédiat |
| **N lecteurs actifs** | ✅ Peut se joindre | ❌ Doit attendre |
| **1 écrivain actif** | ❌ Doit attendre | ❌ Doit attendre |

### Tableau de décision

```
Demande          État actuel       Résultat
========         ===========       ========
rdlock()         Libre             ✅ OK (devient "lecture")
rdlock()         Lecture (N)       ✅ OK (devient "lecture N+1")
rdlock()         Écriture          ⏳ BLOQUE

wrlock()         Libre             ✅ OK (devient "écriture")
wrlock()         Lecture (N)       ⏳ BLOQUE
wrlock()         Écriture          ⏳ BLOQUE
```

### Propriété importante

**Compatibilité** :
- Lecture ↔ Lecture : ✅ Compatible
- Lecture ↔ Écriture : ❌ Incompatible
- Écriture ↔ Écriture : ❌ Incompatible

---

## Initialisation et destruction

### Initialisation statique

```c
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
```

**Avantages** :
- Simple et directe
- Pas besoin de fonction d'initialisation
- Idéale pour variables globales

### Initialisation dynamique

```c
pthread_rwlock_t rwlock;
pthread_rwlockattr_t attr;  // Attributs (optionnel)

// Initialisation simple
int result = pthread_rwlock_init(&rwlock, NULL);
if (result != 0) {
    fprintf(stderr, "pthread_rwlock_init: %s\n", strerror(result));
}

// Avec attributs (avancé)
pthread_rwlockattr_init(&attr);
// Configurer les attributs...
pthread_rwlock_init(&rwlock, &attr);
pthread_rwlockattr_destroy(&attr);
```

**Exemple avec structure** :

```c
typedef struct {
    int valeur;
    pthread_rwlock_t rwlock;
} Data;

Data *creer_data(void) {
    Data *d = malloc(sizeof(Data));
    if (d == NULL) return NULL;

    d->valeur = 0;

    if (pthread_rwlock_init(&d->rwlock, NULL) != 0) {
        free(d);
        return NULL;
    }

    return d;
}
```

### Destruction

```c
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
```

**Règles** :
- Obligatoire pour les RWLocks initialisés avec `pthread_rwlock_init()`
- Le RWLock doit être **libre** (aucun thread en lecture ou écriture)

**Exemple** :

```c
void detruire_data(Data *d) {
    pthread_rwlock_destroy(&d->rwlock);
    free(d);
}
```

---

## Opérations de base

### pthread_rwlock_rdlock() : Verrouillage en lecture

```c
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
```

**Comportement** :
- Si RWLock libre ou déjà en lecture : Accès immédiat
- Si RWLock en écriture : **Bloque** jusqu'à ce que l'écrivain libère

**Utilisation** :

```c
pthread_rwlock_rdlock(&rwlock);  // 🔓 Lecture partagée

// Lire les données (pas de modification)
int valeur = data.valeur;
printf("Valeur : %d\n", valeur);

pthread_rwlock_unlock(&rwlock);
```

### pthread_rwlock_wrlock() : Verrouillage en écriture

```c
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
```

**Comportement** :
- Si RWLock libre : Accès immédiat (mode exclusif)
- Si RWLock en lecture ou écriture : **Bloque**

**Utilisation** :

```c
pthread_rwlock_wrlock(&rwlock);  // 🔒 Écriture exclusive

// Modifier les données
data.valeur = 42;

pthread_rwlock_unlock(&rwlock);
```

### pthread_rwlock_unlock() : Déverrouillage

```c
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
```

**Comportement** :
- Libère le verrou (lecture ou écriture)
- Si des threads attendent : Réveille selon la politique de priorité

**Important** : Le même `unlock()` pour lecture ET écriture.

### Résumé visuel

```c
// Pattern lecteur
pthread_rwlock_rdlock(&rwlock);   // ← Peut être partagé
// LIRE (pas de modification)
pthread_rwlock_unlock(&rwlock);

// Pattern écrivain
pthread_rwlock_wrlock(&rwlock);   // ← Exclusif
// MODIFIER
pthread_rwlock_unlock(&rwlock);
```

---

## Exemple simple

### Cache de configuration

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    char config[256];
    int version;
    pthread_rwlock_t rwlock;
} ConfigCache;

ConfigCache cache = {
    .config = "config_initiale",
    .version = 1,
    .rwlock = PTHREAD_RWLOCK_INITIALIZER
};

// Lecteur : Lit la configuration
void *lecteur(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 5; i++) {
        pthread_rwlock_rdlock(&cache.rwlock);  // 🔓 Lecture partagée

        printf("[Lecteur %d] Config: %s (v%d)\n",
               id, cache.config, cache.version);

        pthread_rwlock_unlock(&cache.rwlock);

        usleep(100000);  // 100ms
    }

    return NULL;
}

// Écrivain : Met à jour la configuration
void *ecrivain(void *arg) {
    int id = *(int *)arg;

    sleep(1);  // Attendre un peu

    pthread_rwlock_wrlock(&cache.rwlock);  // 🔒 Écriture exclusive

    printf("[Écrivain %d] Mise à jour de la config...\n", id);
    snprintf(cache.config, sizeof(cache.config),
             "nouvelle_config_%d", id);
    cache.version++;
    sleep(2);  // Simule écriture longue

    printf("[Écrivain %d] Mise à jour terminée\n", id);

    pthread_rwlock_unlock(&cache.rwlock);

    return NULL;
}

int main(void) {
    pthread_t lecteurs[5], ecrivains[2];
    int ids_lect[5], ids_ecr[2];

    // Créer les lecteurs
    for (int i = 0; i < 5; i++) {
        ids_lect[i] = i;
        pthread_create(&lecteurs[i], NULL, lecteur, &ids_lect[i]);
    }

    // Créer les écrivains
    for (int i = 0; i < 2; i++) {
        ids_ecr[i] = i;
        pthread_create(&ecrivains[i], NULL, ecrivain, &ids_ecr[i]);
    }

    // Attendre
    for (int i = 0; i < 5; i++) {
        pthread_join(lecteurs[i], NULL);
    }
    for (int i = 0; i < 2; i++) {
        pthread_join(ecrivains[i], NULL);
    }

    pthread_rwlock_destroy(&cache.rwlock);

    return 0;
}
```

**Sortie typique** :
```
[Lecteur 0] Config: config_initiale (v1)
[Lecteur 1] Config: config_initiale (v1)  ← Lectures simultanées
[Lecteur 2] Config: config_initiale (v1)
[Lecteur 3] Config: config_initiale (v1)
[Lecteur 4] Config: config_initiale (v1)
[Écrivain 0] Mise à jour de la config...
[Écrivain 0] Mise à jour terminée
[Lecteur 0] Config: nouvelle_config_0 (v2)
[Lecteur 1] Config: nouvelle_config_0 (v2)
...
```

---

## Cas d'usage : Cache de données

### Implémentation complète d'un cache

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_ENTRIES 100

typedef struct {
    char key[50];
    char value[200];
    int valid;
} CacheEntry;

typedef struct {
    CacheEntry entries[MAX_ENTRIES];
    int count;
    pthread_rwlock_t rwlock;
} Cache;

// Initialiser le cache
void cache_init(Cache *cache) {
    cache->count = 0;
    for (int i = 0; i < MAX_ENTRIES; i++) {
        cache->entries[i].valid = 0;
    }
    pthread_rwlock_init(&cache->rwlock, NULL);
}

// Lire du cache (lecture partagée)
int cache_get(Cache *cache, const char *key, char *value, size_t size) {
    pthread_rwlock_rdlock(&cache->rwlock);  // 🔓 Lecture

    int found = 0;
    for (int i = 0; i < cache->count; i++) {
        if (cache->entries[i].valid &&
            strcmp(cache->entries[i].key, key) == 0) {
            strncpy(value, cache->entries[i].value, size - 1);
            value[size - 1] = '\0';
            found = 1;
            break;
        }
    }

    pthread_rwlock_unlock(&cache->rwlock);

    return found;
}

// Écrire dans le cache (écriture exclusive)
void cache_put(Cache *cache, const char *key, const char *value) {
    pthread_rwlock_wrlock(&cache->rwlock);  // 🔒 Écriture

    // Vérifier si la clé existe déjà
    int index = -1;
    for (int i = 0; i < cache->count; i++) {
        if (cache->entries[i].valid &&
            strcmp(cache->entries[i].key, key) == 0) {
            index = i;
            break;
        }
    }

    // Mise à jour ou ajout
    if (index >= 0) {
        // Mise à jour
        strncpy(cache->entries[index].value, value,
                sizeof(cache->entries[index].value) - 1);
    } else if (cache->count < MAX_ENTRIES) {
        // Ajout
        strncpy(cache->entries[cache->count].key, key,
                sizeof(cache->entries[cache->count].key) - 1);
        strncpy(cache->entries[cache->count].value, value,
                sizeof(cache->entries[cache->count].value) - 1);
        cache->entries[cache->count].valid = 1;
        cache->count++;
    }

    pthread_rwlock_unlock(&cache->rwlock);
}

// Supprimer du cache (écriture exclusive)
void cache_delete(Cache *cache, const char *key) {
    pthread_rwlock_wrlock(&cache->rwlock);  // 🔒 Écriture

    for (int i = 0; i < cache->count; i++) {
        if (cache->entries[i].valid &&
            strcmp(cache->entries[i].key, key) == 0) {
            cache->entries[i].valid = 0;
            break;
        }
    }

    pthread_rwlock_unlock(&cache->rwlock);
}

// Détruire le cache
void cache_destroy(Cache *cache) {
    pthread_rwlock_destroy(&cache->rwlock);
}

// Test
void *reader_thread(void *arg) {
    Cache *cache = (Cache *)arg;
    char value[200];

    for (int i = 0; i < 10; i++) {
        if (cache_get(cache, "user_1", value, sizeof(value))) {
            printf("[Lecteur] user_1 = %s\n", value);
        }
        usleep(50000);
    }

    return NULL;
}

void *writer_thread(void *arg) {
    Cache *cache = (Cache *)arg;

    sleep(1);
    cache_put(cache, "user_1", "Alice");
    printf("[Écrivain] Ajouté user_1\n");

    sleep(2);
    cache_put(cache, "user_1", "Bob");
    printf("[Écrivain] Mis à jour user_1\n");

    return NULL;
}

int main(void) {
    Cache cache;
    cache_init(&cache);

    // Ajouter des données initiales
    cache_put(&cache, "user_1", "Initial");

    pthread_t readers[5], writer;

    for (int i = 0; i < 5; i++) {
        pthread_create(&readers[i], NULL, reader_thread, &cache);
    }
    pthread_create(&writer, NULL, writer_thread, &cache);

    for (int i = 0; i < 5; i++) {
        pthread_join(readers[i], NULL);
    }
    pthread_join(writer, NULL);

    cache_destroy(&cache);

    return 0;
}
```

---

## Politiques de priorité

Les read-write locks peuvent avoir différentes **politiques de priorité** quand des threads attendent.

### 1. Priorité lecteurs (Reader-preference)

**Comportement** : Les lecteurs sont favorisés.

```
État : 1 écrivain actif

File d'attente :
[Lecteur 1] [Écrivain 2] [Lecteur 3] [Lecteur 4]

Écrivain libère :
→ Lecteur 1, 3, 4 passent TOUS (écrivain 2 attend)
```

**Avantage** : Maximise la concurrence en lecture
**Inconvénient** : L'écrivain peut attendre indéfiniment (**starvation**)

### 2. Priorité écrivains (Writer-preference)

**Comportement** : Les écrivains sont favorisés.

```
État : Lecteurs actifs

File d'attente :
[Écrivain 1] [Lecteur 2] [Lecteur 3]

Lecteurs libèrent :
→ Écrivain 1 passe (lecteurs 2, 3 attendent)
```

**Avantage** : Évite la starvation des écrivains
**Inconvénient** : Réduit la concurrence en lecture

### 3. Équitable (Fair / FIFO)

**Comportement** : Premier arrivé, premier servi.

```
File d'attente :
[Lecteur 1] [Écrivain 2] [Lecteur 3]

→ Lecteur 1 passe
→ Écrivain 2 passe
→ Lecteur 3 passe
```

**Avantage** : Pas de starvation
**Inconvénient** : Moins de concurrence

### Configuration de la politique

Sur Linux (glibc), le comportement par défaut est **writer-preference**.

**Configuration via attributs** :

```c
pthread_rwlockattr_t attr;
pthread_rwlockattr_init(&attr);

// Définir la politique (non portable, dépend du système)
// Sur Linux : PTHREAD_RWLOCK_PREFER_READER_NP
//             PTHREAD_RWLOCK_PREFER_WRITER_NP
//             PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP (défaut)

pthread_rwlockattr_setkind_np(&attr,
                               PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);

pthread_rwlock_init(&rwlock, &attr);
pthread_rwlockattr_destroy(&attr);
```

**Note** : Ces options sont spécifiques à Linux (suffixe `_NP` = non portable).

---

## Fonctions avec timeout et trylock

### pthread_rwlock_tryrdlock() : Lecture non-bloquante

```c
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
```

**Comportement** :
- Si lecture possible : Prend le verrou et retourne 0
- Sinon : Retourne immédiatement `EBUSY`

**Exemple** :

```c
if (pthread_rwlock_tryrdlock(&rwlock) == 0) {
    // Verrou obtenu
    printf("Lecture : %d\n", data);
    pthread_rwlock_unlock(&rwlock);
} else {
    // Occupé, faire autre chose
    printf("Cache occupé, utilise valeur par défaut\n");
}
```

### pthread_rwlock_trywrlock() : Écriture non-bloquante

```c
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
```

**Exemple** :

```c
if (pthread_rwlock_trywrlock(&rwlock) == 0) {
    // Verrou obtenu
    data = 42;
    pthread_rwlock_unlock(&rwlock);
} else {
    // Occupé, reporter la mise à jour
    printf("Mise à jour reportée\n");
}
```

### pthread_rwlock_timedrdlock() : Lecture avec timeout

```c
int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock,
                               const struct timespec *abs_timeout);
```

**Exemple** :

```c
struct timespec timeout;
clock_gettime(CLOCK_REALTIME, &timeout);
timeout.tv_sec += 5;  // 5 secondes

int result = pthread_rwlock_timedrdlock(&rwlock, &timeout);
if (result == 0) {
    // Verrou obtenu
    printf("Data: %d\n", data);
    pthread_rwlock_unlock(&rwlock);
} else if (result == ETIMEDOUT) {
    printf("Timeout : données non disponibles\n");
}
```

### pthread_rwlock_timedwrlock() : Écriture avec timeout

```c
int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock,
                               const struct timespec *abs_timeout);
```

---

## Performance : Mutex vs RWLock

### Benchmark théorique

**Scénario** : 10 threads, 90% lectures / 10% écritures

```c
// Avec Mutex
pthread_mutex_lock(&mutex);
// Lecture ou écriture
pthread_mutex_unlock(&mutex);

// Tous les threads se bloquent mutuellement
// → 10 accès séquentiels

// Avec RWLock
pthread_rwlock_rdlock(&rwlock);  // 90% du temps
// Lecture (9 threads peuvent lire ensemble)
pthread_rwlock_unlock(&rwlock);

pthread_rwlock_wrlock(&rwlock);  // 10% du temps
// Écriture (1 thread exclusif)
pthread_rwlock_unlock(&rwlock);

// Lectures concurrentes → ~9x plus rapide
```

### Mesure réelle

```c
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 10
#define ITERATIONS 100000
#define READ_RATIO 90  // 90% lectures

int data = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void *worker_mutex(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);

        if (i % 100 < READ_RATIO) {
            // Lecture
            int val = data;
            (void)val;  // Éviter warning
        } else {
            // Écriture
            data++;
        }

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *worker_rwlock(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        if (i % 100 < READ_RATIO) {
            // Lecture
            pthread_rwlock_rdlock(&rwlock);
            int val = data;
            (void)val;
            pthread_rwlock_unlock(&rwlock);
        } else {
            // Écriture
            pthread_rwlock_wrlock(&rwlock);
            data++;
            pthread_rwlock_unlock(&rwlock);
        }
    }
    return NULL;
}

double benchmark(void *(*func)(void *)) {
    pthread_t threads[NUM_THREADS];
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, func, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    return elapsed;
}

int main(void) {
    printf("Benchmark : %d threads, %d%% lectures\n",
           NUM_THREADS, READ_RATIO);

    double time_mutex = benchmark(worker_mutex);
    printf("Mutex    : %.3f secondes\n", time_mutex);

    data = 0;  // Reset

    double time_rwlock = benchmark(worker_rwlock);
    printf("RWLock   : %.3f secondes\n", time_rwlock);

    printf("Speedup  : %.2fx\n", time_mutex / time_rwlock);

    return 0;
}
```

**Résultats typiques** :
```
Benchmark : 10 threads, 90% lectures
Mutex    : 2.456 secondes
RWLock   : 0.823 secondes
Speedup  : 2.98x
```

**Observation** : Avec beaucoup de lectures, RWLock est **significativement plus rapide**.

### Quand RWLock est-il avantageux ?

| Ratio lectures/écritures | Nombre de threads | Gain RWLock |
|-------------------------|-------------------|-------------|
| 50/50 | 2 | ~0% (équivalent) |
| 80/20 | 4 | ~40-60% |
| 90/10 | 8+ | ~100-300% |
| 95/5 | 16+ | ~400%+ |

**Règle empirique** : RWLock utile si :
- ≥ 80% de lectures
- ≥ 4 threads lecteurs
- Section critique courte

---

## Patterns avancés

### Pattern 1 : Promotion lecture → écriture (Copy-on-Write)

Il n'existe **pas** de fonction pour "upgrader" un read-lock en write-lock. Il faut déverrouiller puis re-verrouiller :

```c
// ❌ Impossible de promouvoir directement
pthread_rwlock_rdlock(&rwlock);
// Lire...
// pthread_rwlock_upgrade(&rwlock);  // N'existe pas !

// ✅ Pattern Copy-on-Write
pthread_rwlock_rdlock(&rwlock);

// Lire les données
Data *data_copy = copier_data(data);

pthread_rwlock_unlock(&rwlock);

// Modifier la copie (hors du verrou)
modifier_data(data_copy);

// Écriture
pthread_rwlock_wrlock(&rwlock);
remplacer_data(data, data_copy);
pthread_rwlock_unlock(&rwlock);
```

### Pattern 2 : Cache avec expiration

```c
typedef struct {
    char value[200];
    time_t timestamp;
    int valid;
} CachedValue;

typedef struct {
    CachedValue cache;
    pthread_rwlock_t rwlock;
} Cache;

int cache_get(Cache *c, char *value, int timeout_sec) {
    pthread_rwlock_rdlock(&c->rwlock);

    time_t now = time(NULL);
    int expired = (now - c->cache.timestamp) > timeout_sec;

    if (c->cache.valid && !expired) {
        strcpy(value, c->cache.value);
        pthread_rwlock_unlock(&c->rwlock);
        return 1;  // Cache hit
    }

    pthread_rwlock_unlock(&c->rwlock);

    // Cache miss, recharger
    pthread_rwlock_wrlock(&c->rwlock);

    // Double-check (un autre thread a pu recharger)
    if (c->cache.valid && (time(NULL) - c->cache.timestamp) <= timeout_sec) {
        strcpy(value, c->cache.value);
        pthread_rwlock_unlock(&c->rwlock);
        return 1;
    }

    // Recharger
    recharger_cache(&c->cache);
    strcpy(value, c->cache.value);

    pthread_rwlock_unlock(&c->rwlock);
    return 1;
}
```

### Pattern 3 : RCU-like (Read-Copy-Update)

```c
typedef struct {
    Data *data;  // Pointeur vers données
    pthread_rwlock_t rwlock;
} RCUData;

// Lecteur : Accès très rapide
Data *lire_data(RCUData *rcu) {
    pthread_rwlock_rdlock(&rcu->rwlock);
    Data *ptr = rcu->data;  // Copie du pointeur
    pthread_rwlock_unlock(&rcu->rwlock);

    // Peut utiliser ptr (lecture seule)
    return ptr;
}

// Écrivain : Crée nouvelle version
void modifier_data(RCUData *rcu, Data *nouvelle_data) {
    pthread_rwlock_wrlock(&rcu->rwlock);

    Data *ancienne = rcu->data;
    rcu->data = nouvelle_data;  // Remplace le pointeur

    pthread_rwlock_unlock(&rcu->rwlock);

    // Libérer l'ancienne version après que tous les lecteurs aient fini
    // (nécessite mécanisme de grace period)
    free(ancienne);
}
```

---

## Erreurs courantes

### 1. Écrire en mode lecture

```c
// ❌ ERREUR : Modification avec rdlock
pthread_rwlock_rdlock(&rwlock);
data.valeur = 42;  // ← Race condition !
pthread_rwlock_unlock(&rwlock);

// ✅ CORRECT
pthread_rwlock_wrlock(&rwlock);
data.valeur = 42;
pthread_rwlock_unlock(&rwlock);
```

### 2. Deadlock avec upgrade

```c
// ❌ DEADLOCK : Tentative d'upgrade
pthread_rwlock_rdlock(&rwlock);
// ...
pthread_rwlock_wrlock(&rwlock);  // Bloque indéfiniment !
```

**Pourquoi ?** Le thread possède un read-lock et attend un write-lock. Mais le write-lock ne peut être accordé tant qu'il y a des lecteurs (dont lui-même).

### 3. Unlock sans lock correspondant

```c
// ❌ ERREUR
pthread_rwlock_unlock(&rwlock);  // Pas de lock avant !
```

### 4. Oublier unlock

```c
// ❌ FUITE
pthread_rwlock_rdlock(&rwlock);
if (erreur) {
    return;  // Oubli de unlock !
}
pthread_rwlock_unlock(&rwlock);

// ✅ CORRECT
pthread_rwlock_rdlock(&rwlock);
if (erreur) {
    pthread_rwlock_unlock(&rwlock);
    return;
}
pthread_rwlock_unlock(&rwlock);
```

### 5. RWLock récursif

Les RWLocks ne sont **pas récursifs** par défaut :

```c
// ❌ DEADLOCK possible
pthread_rwlock_rdlock(&rwlock);
fonction_qui_fait_rdlock();  // Deadlock potentiel
pthread_rwlock_unlock(&rwlock);
```

**Solution** : Éviter les appels imbriqués, ou documenter clairement.

---

## Quand utiliser les read-write locks ?

### ✅ Utilisez RWLock quand :

1. **Beaucoup de lectures, peu d'écritures**
   - Ratio ≥ 80/20 (lectures/écritures)
   - Exemple : Cache, configuration

2. **Plusieurs threads lecteurs**
   - Au moins 4+ threads
   - Sinon, overhead du RWLock annule les gains

3. **Section critique courte**
   - Lecture/écriture rapide
   - Pas d'I/O bloquantes

4. **Pas de promotion read → write**
   - Sinon, risque de deadlock

### ❌ N'utilisez PAS RWLock quand :

1. **Beaucoup d'écritures**
   - Ratio < 70/30
   - Un mutex simple est plus efficace

2. **Peu de threads**
   - 1-2 threads : overhead inutile

3. **Section critique longue**
   - I/O lentes, calculs complexes
   - Overhead de gestion du RWLock

4. **Modifications fréquentes avec lectures**
   - Pattern : lire, modifier légèrement, écrire
   - Mutex + condition variable plus adapté

### Tableau de décision

| Caractéristique | Mutex | RWLock |
|-----------------|-------|--------|
| % Lectures | < 70% | ≥ 80% |
| Threads lecteurs | 1-2 | 4+ |
| Section critique | Longue/Variable | Courte |
| Écritures | Fréquentes | Rares |
| Simplicité | ✅ Simple | ⚠️ Plus complexe |

---

## Alternatives

### 1. Atomics (C11)

Pour des lectures/écritures **très simples** :

```c
#include <stdatomic.h>

atomic_int compteur = 0;

// Lecture
int val = atomic_load(&compteur);

// Écriture
atomic_store(&compteur, 42);
```

**Avantages** :
- Pas de verrou (lock-free)
- Très rapide

**Limites** :
- Types simples uniquement
- Pas de structures complexes

### 2. RCU (Read-Copy-Update)

Optimisation extrême pour lectures très fréquentes :

**Principe** :
- Lecteurs : Aucun verrou (accès direct)
- Écrivains : Créent nouvelle version, remplacent pointeur

**Avantages** :
- Lectures ultra-rapides
- Scalabilité extrême

**Inconvénients** :
- Très complexe à implémenter correctement
- Nécessite gestion mémoire sophistiquée

### 3. Seqlock (Linux kernel)

**Principe** :
- Version counter
- Lecteurs vérifient cohérence
- Pas de blocage

**Limites** :
- Structures de petite taille
- Implémentation non triviale

---

## Récapitulatif

Dans cette section, vous avez appris :

✅ **Read-Write Locks** :
- Permettent lectures concurrentes
- Un seul écrivain (exclusif)
- Type `pthread_rwlock_t`

✅ **Règles** :
- Lecture ↔ Lecture : Compatible
- Lecture ↔ Écriture : Incompatible
- Écriture ↔ Écriture : Incompatible

✅ **Opérations** :
- `pthread_rwlock_rdlock()` : Lecture partagée
- `pthread_rwlock_wrlock()` : Écriture exclusive
- `pthread_rwlock_unlock()` : Libération
- `trylock()` et `timedlock()` : Non-bloquant/timeout

✅ **Politiques** :
- Reader-preference (starvation écrivains)
- Writer-preference (starvation lecteurs)
- Fair (FIFO)

✅ **Performance** :
- Gain significatif si ≥ 80% lectures
- Nécessite 4+ threads pour être rentable
- Section critique courte

✅ **Patterns** :
- Cache de données
- Configuration partagée
- Copy-on-Write

✅ **Erreurs** :
- Écrire avec rdlock
- Tentative d'upgrade (deadlock)
- Oubli unlock
- RWLock récursif

✅ **Quand utiliser** :
- Beaucoup de lectures (≥ 80%)
- Plusieurs threads lecteurs (4+)
- Sections critiques courtes
- Pas de promotion read → write

---

## Pour aller plus loin

Dans les prochaines sections :

- **18.12** : Thread-local storage (variables par thread)
- **18.13** : Barrières de threads (pthread_barrier)
- **19** : Mémoire partagée et IPC avancé

**Conseil pratique** : Implémentez un cache de DNS avec RWLock :
- Threads lecteurs : Résolvent des noms
- Thread writer : Met à jour le cache périodiquement
- Mesurez la différence de performance avec un mutex simple

**Projet** : Créez un système de base de données simple en mémoire :
- Table avec RWLock
- Opérations SELECT (rdlock) et INSERT/UPDATE (wrlock)
- Benchmark avec différents ratios lecture/écriture

---

📘 **Ressources utiles** :
- `man pthread_rwlock_rdlock`
- `man pthread_rwlock_wrlock`
- `man pthread_rwlock_init`
- Article : "Readers-Writer Problem" (Courtois et al.)

💡 **Prochaine étape** : Section 18.12 - Thread-local storage

**Citation** : *"Read-write locks are the Swiss Army knife of thread synchronization for read-heavy workloads."* - Principe de performance

⏭️ [Thread-local storage](/18-threads-et-concurrence/12-thread-local-storage.md)
