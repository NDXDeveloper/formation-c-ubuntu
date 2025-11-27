🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 18.8 Variables de condition

## Table des matières
- [Introduction](#introduction)
- [Le problème du busy-waiting](#le-probl%C3%A8me-du-busy-waiting)
- [Qu'est-ce qu'une variable de condition ?](#quest-ce-quune-variable-de-condition-)
- [Initialisation et destruction](#initialisation-et-destruction)
- [pthread_cond_wait() : Attendre une condition](#pthread_cond_wait--attendre-une-condition)
- [pthread_cond_signal() : Signaler un thread](#pthread_cond_signal--signaler-un-thread)
- [pthread_cond_broadcast() : Signaler tous les threads](#pthread_cond_broadcast--signaler-tous-les-threads)
- [Pattern producteur-consommateur](#pattern-producteur-consommateur)
- [Spurious wakeups et le piège du if](#spurious-wakeups-et-le-pi%C3%A8ge-du-if)
- [Exemples pratiques](#exemples-pratiques)
- [pthread_cond_timedwait()](#pthread_cond_timedwait)
- [Erreurs courantes](#erreurs-courantes)
- [Bonnes pratiques](#bonnes-pratiques)
- [Variables de condition vs autres mécanismes](#variables-de-condition-vs-autres-m%C3%A9canismes)

---

## Introduction

Jusqu'à présent, nous avons vu les **mutex** pour protéger les données partagées et éviter les race conditions. Mais les mutex seuls ne suffisent pas toujours. Parfois, un thread doit **attendre qu'une condition soit remplie** avant de continuer.

**Exemple** : Un thread consommateur doit attendre qu'un producteur ait mis des données disponibles.

**Question** : Comment faire attendre un thread jusqu'à ce qu'une condition soit vraie ?

**Mauvaise solution** : Vérifier en boucle (busy-waiting)
```c
// ❌ TRÈS INEFFICACE
while (!condition_remplie) {
    // Vérifier sans arrêt → Gaspille CPU
}
```

**Bonne solution** : Utiliser des **variables de condition** (condition variables)
```c
// ✅ EFFICACE
pthread_cond_wait(&cond, &mutex);  // S'endort jusqu'au signal
```

Dans cette section, nous allons apprendre à :
- Faire attendre un thread efficacement
- Réveiller un ou plusieurs threads
- Implémenter des patterns classiques (producteur-consommateur)
- Éviter les pièges des variables de condition

---

## Le problème du busy-waiting

### Scénario : File d'attente

Imaginons une file d'attente partagée entre threads :

```c
typedef struct {
    int items[100];
    int count;
    pthread_mutex_t mutex;
} Queue;

Queue queue = {.count = 0};
```

**Producteur** : Ajoute des éléments
**Consommateur** : Retire des éléments (doit attendre si vide)

### Tentative 1 : Attente active (❌ Mauvais)

```c
void *consommateur(void *arg) {
    while (1) {
        pthread_mutex_lock(&queue.mutex);

        // Attendre qu'il y ait des éléments
        while (queue.count == 0) {
            pthread_mutex_unlock(&queue.mutex);
            // Busy-waiting : Vérifier en boucle
            // ❌ Gaspille énormément de CPU !
            pthread_mutex_lock(&queue.mutex);
        }

        // Consommer un élément
        int item = queue.items[--queue.count];

        pthread_mutex_unlock(&queue.mutex);

        printf("Consommé : %d\n", item);
    }
    return NULL;
}
```

**Problèmes** :
1. **CPU à 100%** : Le thread vérifie la condition sans arrêt
2. **Inefficace** : Gaspille des ressources
3. **Délai** : Petite fenêtre entre unlock et lock où un autre thread peut agir

### Tentative 2 : Attente avec sleep (❌ Mieux mais pas idéal)

```c
void *consommateur(void *arg) {
    while (1) {
        pthread_mutex_lock(&queue.mutex);

        if (queue.count == 0) {
            pthread_mutex_unlock(&queue.mutex);
            sleep(1);  // Attendre 1 seconde
            continue;
        }

        int item = queue.items[--queue.count];
        pthread_mutex_unlock(&queue.mutex);

        printf("Consommé : %d\n", item);
    }
    return NULL;
}
```

**Problèmes** :
1. **Latence** : Peut attendre jusqu'à 1 seconde après qu'un élément est disponible
2. **Compromis impossible** : Sleep court = CPU gaspillé, sleep long = latence élevée
3. **Pas scalable** : Avec beaucoup de threads, ça ne marche pas

### Solution : Variables de condition (✅ Correct)

```c
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *consommateur(void *arg) {
    while (1) {
        pthread_mutex_lock(&queue.mutex);

        // Attendre efficacement
        while (queue.count == 0) {
            pthread_cond_wait(&cond, &queue.mutex);  // ✅ S'endort
        }

        int item = queue.items[--queue.count];
        pthread_mutex_unlock(&queue.mutex);

        printf("Consommé : %d\n", item);
    }
    return NULL;
}

void *producteur(void *arg) {
    for (int i = 0; i < 100; i++) {
        pthread_mutex_lock(&queue.mutex);

        queue.items[queue.count++] = i;

        pthread_cond_signal(&cond);  // ✅ Réveille le consommateur

        pthread_mutex_unlock(&queue.mutex);
    }
    return NULL;
}
```

**Avantages** :
1. **Pas de CPU gaspillé** : Le thread est vraiment endormi
2. **Latence minimale** : Réveil immédiat quand condition remplie
3. **Scalable** : Fonctionne avec beaucoup de threads

---

## Qu'est-ce qu'une variable de condition ?

### Définition

Une **variable de condition** (condition variable) est un mécanisme de synchronisation qui permet à un thread de :
- **S'endormir** en attendant qu'une condition devienne vraie
- **Être réveillé** par un autre thread quand la condition est remplie

**Analogie** : Une salle d'attente chez le médecin
- **Patients (threads)** : S'assoient et attendent (`pthread_cond_wait`)
- **Secrétaire (producteur)** : Appelle le prochain patient (`pthread_cond_signal`)
- **Liste de patients** : La condition à vérifier

### Type et déclaration

```c
pthread_cond_t ma_condition;
```

**Important** : Une variable de condition est **toujours** associée à un **mutex**.

### Le trio : Condition + Mutex + Donnée partagée

```c
// Données partagées
int stock = 0;

// Mutex pour protéger les données
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Variable de condition pour signaler les changements
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
```

**Règle** :
- Le **mutex** protège les données
- La **variable de condition** signale les changements d'état

### Principe de fonctionnement

```
Thread en attente                   Thread qui signale
=================                   ===================

1. Lock mutex                       1. Lock mutex
2. Vérifier condition               2. Modifier les données
3. Si fausse:                       3. Unlock mutex
   - cond_wait()                    4. Signal condition
     → Unlock mutex automatiquement
     → S'endort
     → Attend signal                 ← Signal envoyé
     → Réveillé
     → Re-lock mutex automatiquement
4. Condition vraie
5. Travailler
6. Unlock mutex
```

**Point clé** : `pthread_cond_wait()` fait **deux opérations atomiquement** :
1. Libère le mutex
2. Met le thread en sommeil

Puis, au réveil :
1. Re-prend le mutex
2. Retourne

---

## Initialisation et destruction

### Initialisation statique

```c
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
```

**Avantages** :
- Simple
- Pas besoin de fonction d'initialisation
- Idéale pour variables globales

### Initialisation dynamique

```c
pthread_cond_t cond;
pthread_condattr_t attr;  // Attributs (optionnel)

// Initialisation simple
int result = pthread_cond_init(&cond, NULL);
if (result != 0) {
    fprintf(stderr, "pthread_cond_init: %s\n", strerror(result));
}

// Avec attributs (avancé)
pthread_condattr_init(&attr);
// Configurer attr...
pthread_cond_init(&cond, &attr);
pthread_condattr_destroy(&attr);
```

**Quand l'utiliser ?**
- Variables de condition dans des structures allouées dynamiquement
- Besoin d'attributs spéciaux

**Exemple avec structure** :

```c
typedef struct {
    int data;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Resource;

Resource *create_resource(void) {
    Resource *r = malloc(sizeof(Resource));
    if (r == NULL) return NULL;

    r->data = 0;

    if (pthread_mutex_init(&r->mutex, NULL) != 0) {
        free(r);
        return NULL;
    }

    if (pthread_cond_init(&r->cond, NULL) != 0) {
        pthread_mutex_destroy(&r->mutex);
        free(r);
        return NULL;
    }

    return r;
}
```

### Destruction

```c
int pthread_cond_destroy(pthread_cond_t *cond);
```

**Règles** :
- Obligatoire pour les variables de condition initialisées avec `pthread_cond_init()`
- Optionnel pour `PTHREAD_COND_INITIALIZER`
- Aucun thread ne doit être en attente sur la condition

**Exemple** :

```c
void destroy_resource(Resource *r) {
    pthread_cond_destroy(&r->cond);
    pthread_mutex_destroy(&r->mutex);
    free(r);
}
```

---

## pthread_cond_wait() : Attendre une condition

### Signature

```c
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
```

**Paramètres** :
- `cond` : Variable de condition sur laquelle attendre
- `mutex` : Mutex qui **doit être verrouillé** par le thread appelant

**Retour** :
- `0` : Succès
- Code d'erreur sinon

### Comportement

`pthread_cond_wait()` fait **atomiquement** :

1. **Libère** le mutex
2. **Met le thread en sommeil** sur la condition
3. **Attend** d'être réveillé par `signal()` ou `broadcast()`
4. Quand réveillé : **Re-prend** le mutex
5. **Retourne**

**Visualisation** :

```
Avant wait :
┌─────────────────┐
│ Thread possède  │
│ le mutex        │
└─────────────────┘
        │
        ▼
pthread_cond_wait()
        │
        ├─► Libère mutex  ────┐
        │                     │ Atomique
        └─► S'endort     ─────┘

[Thread endormi, mutex libéré]

Signal reçu !
        │
        ▼
pthread_cond_wait()
        │
        ├─► Reprend mutex ────┐
        │                     │ Atomique
        └─► Retourne     ─────┘

Après wait :
┌─────────────────┐
│ Thread possède  │
│ à nouveau       │
│ le mutex        │
└─────────────────┘
```

### Pattern d'utilisation OBLIGATOIRE

```c
pthread_mutex_lock(&mutex);

while (!condition) {  // ← TOUJOURS while, PAS if
    pthread_cond_wait(&cond, &mutex);
}

// Condition est maintenant vraie, mutex verrouillé
// Faire le travail...

pthread_mutex_unlock(&mutex);
```

**⚠️ CRITIQUE** : Utilisez **TOUJOURS** `while`, jamais `if` (voir section spurious wakeups).

### Exemple complet

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int ready = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *waiter(void *arg) {
    printf("Waiter: J'attends que ready devienne 1...\n");

    pthread_mutex_lock(&mutex);

    while (!ready) {  // Tant que pas prêt
        printf("Waiter: Condition fausse, je m'endors\n");
        pthread_cond_wait(&cond, &mutex);
        printf("Waiter: Je suis réveillé, je vérifie la condition\n");
    }

    printf("Waiter: Condition vraie ! Je continue\n");

    pthread_mutex_unlock(&mutex);

    return NULL;
}

void *signaler(void *arg) {
    sleep(2);  // Simuler du travail

    printf("Signaler: Je prépare les données\n");

    pthread_mutex_lock(&mutex);
    ready = 1;  // Changer la condition
    pthread_mutex_unlock(&mutex);

    printf("Signaler: J'envoie le signal\n");
    pthread_cond_signal(&cond);  // Réveiller le waiter

    return NULL;
}

int main(void) {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, waiter, NULL);
    pthread_create(&t2, NULL, signaler, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
```

**Sortie** :
```
Waiter: J'attends que ready devienne 1...
Waiter: Condition fausse, je m'endors
Signaler: Je prépare les données
Signaler: J'envoie le signal
Waiter: Je suis réveillé, je vérifie la condition
Waiter: Condition vraie ! Je continue
```

---

## pthread_cond_signal() : Signaler un thread

### Signature

```c
int pthread_cond_signal(pthread_cond_t *cond);
```

**Comportement** : Réveille **au moins un** thread en attente sur la condition.

**Si aucun thread n'attend** : Ne fait rien (le signal est "perdu").

### Utilisation

```c
pthread_mutex_lock(&mutex);

// Modifier les données
ready = 1;

pthread_mutex_unlock(&mutex);  // Libérer AVANT signal (recommandé)

pthread_cond_signal(&cond);  // Réveiller un thread
```

**Question** : Signal avant ou après unlock ?

**Deux approches valides** :

```c
// Approche 1 : Signal APRÈS unlock (recommandé)
pthread_mutex_lock(&mutex);
ready = 1;
pthread_mutex_unlock(&mutex);
pthread_cond_signal(&cond);  // Thread réveillé peut prendre mutex immédiatement

// Approche 2 : Signal AVANT unlock (fonctionne aussi)
pthread_mutex_lock(&mutex);
ready = 1;
pthread_cond_signal(&cond);  // Thread réveillé attend sur mutex
pthread_mutex_unlock(&mutex);
```

**Recommandation** : Signal après unlock pour éviter un réveil suivi d'un blocage immédiat.

### Exemple : Producteur simple

```c
void *producteur(void *arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex);

        buffer[count++] = i;
        printf("Produit : %d\n", i);

        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);  // Signaler qu'il y a des données

        sleep(1);
    }
    return NULL;
}
```

---

## pthread_cond_broadcast() : Signaler tous les threads

### Signature

```c
int pthread_cond_broadcast(pthread_cond_t *cond);
```

**Comportement** : Réveille **TOUS** les threads en attente sur la condition.

### Différence avec signal()

```c
// pthread_cond_signal() : Réveille 1 thread
pthread_cond_signal(&cond);

// pthread_cond_broadcast() : Réveille TOUS les threads
pthread_cond_broadcast(&cond);
```

### Quand utiliser broadcast() ?

**Utilisez `broadcast()` quand** :
1. **Plusieurs threads** peuvent satisfaire la condition
2. **État global change** (ex : arrêt du programme)
3. **Tous doivent réagir** au changement

**Exemple 1 : Arrêt de programme**

```c
int terminate = 0;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *worker(void *arg) {
    pthread_mutex_lock(&mutex);

    while (!terminate) {
        pthread_cond_wait(&cond, &mutex);
    }

    pthread_mutex_unlock(&mutex);
    printf("Worker terminé\n");
    return NULL;
}

void shutdown(void) {
    pthread_mutex_lock(&mutex);
    terminate = 1;
    pthread_mutex_unlock(&mutex);

    pthread_cond_broadcast(&cond);  // Réveiller TOUS les workers
}
```

**Exemple 2 : Barrière de threads**

```c
int threads_arrives = 0;
int n_threads = 5;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *worker(void *arg) {
    // Phase 1
    printf("Thread %ld : Phase 1\n", pthread_self());

    // Attendre que tous arrivent
    pthread_mutex_lock(&mutex);
    threads_arrives++;

    if (threads_arrives < n_threads) {
        // Pas tous arrivés, attendre
        while (threads_arrives < n_threads) {
            pthread_cond_wait(&cond, &mutex);
        }
    } else {
        // Dernier arrivé, réveiller tout le monde
        pthread_cond_broadcast(&cond);
    }

    pthread_mutex_unlock(&mutex);

    // Phase 2 (tous les threads exécutent ensemble)
    printf("Thread %ld : Phase 2\n", pthread_self());

    return NULL;
}
```

### Comparaison visuelle

```
SIGNAL :
========
Threads en attente : [T1] [T2] [T3] [T4]
                      ↑
Signal envoyé ────────┘
Résultat : T1 réveillé, T2/T3/T4 dorment encore


BROADCAST :
===========
Threads en attente : [T1] [T2] [T3] [T4]
                      ↑    ↑    ↑    ↑
Broadcast envoyé ─────┴────┴────┴────┘
Résultat : TOUS réveillés
```

---

## Pattern producteur-consommateur

Le pattern producteur-consommateur est l'utilisation classique des variables de condition.

### Version simple (1 producteur, 1 consommateur)

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 10

typedef struct {
    int buffer[BUFFER_SIZE];
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond_not_empty;
    pthread_cond_t cond_not_full;
} Buffer;

Buffer buf = {
    .count = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .cond_not_empty = PTHREAD_COND_INITIALIZER,
    .cond_not_full = PTHREAD_COND_INITIALIZER
};

void *producteur(void *arg) {
    for (int i = 0; i < 20; i++) {
        pthread_mutex_lock(&buf.mutex);

        // Attendre que le buffer ne soit pas plein
        while (buf.count == BUFFER_SIZE) {
            printf("Producteur: Buffer plein, j'attends\n");
            pthread_cond_wait(&buf.cond_not_full, &buf.mutex);
        }

        // Produire
        buf.buffer[buf.count++] = i;
        printf("Produit : %d (count=%d)\n", i, buf.count);

        pthread_cond_signal(&buf.cond_not_empty);  // Signaler au consommateur

        pthread_mutex_unlock(&buf.mutex);

        usleep(100000);  // 100ms
    }
    return NULL;
}

void *consommateur(void *arg) {
    for (int i = 0; i < 20; i++) {
        pthread_mutex_lock(&buf.mutex);

        // Attendre que le buffer ne soit pas vide
        while (buf.count == 0) {
            printf("Consommateur: Buffer vide, j'attends\n");
            pthread_cond_wait(&buf.cond_not_empty, &buf.mutex);
        }

        // Consommer
        int item = buf.buffer[--buf.count];
        printf("Consommé : %d (count=%d)\n", item, buf.count);

        pthread_cond_signal(&buf.cond_not_full);  // Signaler au producteur

        pthread_mutex_unlock(&buf.mutex);

        usleep(200000);  // 200ms (consomme plus lentement)
    }
    return NULL;
}

int main(void) {
    pthread_t prod, cons;

    pthread_create(&prod, NULL, producteur, NULL);
    pthread_create(&cons, NULL, consommateur, NULL);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    return 0;
}
```

**Sortie typique** :
```
Produit : 0 (count=1)
Produit : 1 (count=2)
Consommé : 1 (count=1)
Produit : 2 (count=2)
Produit : 3 (count=3)
Consommé : 2 (count=2)
...
Produit : 9 (count=10)
Producteur: Buffer plein, j'attends
Consommé : 9 (count=9)
Produit : 10 (count=10)
...
```

### Version multi-producteurs/consommateurs

```c
#define N_PROD 3
#define N_CONS 2

void *producteur(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&buf.mutex);

        while (buf.count == BUFFER_SIZE) {
            pthread_cond_wait(&buf.cond_not_full, &buf.mutex);
        }

        buf.buffer[buf.count++] = id * 100 + i;
        printf("Prod %d : item %d\n", id, id * 100 + i);

        pthread_cond_signal(&buf.cond_not_empty);  // Un consommateur

        pthread_mutex_unlock(&buf.mutex);

        usleep(rand() % 100000);
    }

    return NULL;
}

void *consommateur(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 15; i++) {  // 3*10 / 2 = 15 par consommateur
        pthread_mutex_lock(&buf.mutex);

        while (buf.count == 0) {
            pthread_cond_wait(&buf.cond_not_empty, &buf.mutex);
        }

        int item = buf.buffer[--buf.count];
        printf("Cons %d : item %d\n", id, item);

        pthread_cond_signal(&buf.cond_not_full);  // Un producteur

        pthread_mutex_unlock(&buf.mutex);

        usleep(rand() % 150000);
    }

    return NULL;
}

int main(void) {
    pthread_t prods[N_PROD], cons[N_CONS];
    int ids_prod[N_PROD], ids_cons[N_CONS];

    for (int i = 0; i < N_PROD; i++) {
        ids_prod[i] = i;
        pthread_create(&prods[i], NULL, producteur, &ids_prod[i]);
    }

    for (int i = 0; i < N_CONS; i++) {
        ids_cons[i] = i;
        pthread_create(&cons[i], NULL, consommateur, &ids_cons[i]);
    }

    for (int i = 0; i < N_PROD; i++) {
        pthread_join(prods[i], NULL);
    }

    for (int i = 0; i < N_CONS; i++) {
        pthread_join(cons[i], NULL);
    }

    return 0;
}
```

---

## Spurious wakeups et le piège du if

### Qu'est-ce qu'un spurious wakeup ?

Un **spurious wakeup** (réveil intempestif) se produit quand `pthread_cond_wait()` retourne **sans qu'un signal ait été envoyé**.

**Causes** :
- Implémentation système
- Signaux UNIX interceptés
- Optimisations du système

**Important** : C'est un comportement **normal** et **documenté** de POSIX threads.

### Le piège du if

```c
// ❌ ERREUR : Utiliser if
pthread_mutex_lock(&mutex);

if (count == 0) {  // ← Vérification unique
    pthread_cond_wait(&cond, &mutex);
}

// Ici, count peut TOUJOURS être 0 !
int item = buffer[--count];  // ← CRASH possible

pthread_mutex_unlock(&mutex);
```

**Problème** : Si spurious wakeup, la condition peut toujours être fausse au retour de `wait()`.

### Solution : TOUJOURS utiliser while

```c
// ✅ CORRECT : Utiliser while
pthread_mutex_lock(&mutex);

while (count == 0) {  // ← Vérification en boucle
    pthread_cond_wait(&cond, &mutex);
}

// Ici, count est GARANTI > 0
int item = buffer[--count];  // ✅ Safe

pthread_mutex_unlock(&mutex);
```

**Pourquoi ça marche ?**

```
1. while vérifie la condition
2. Si fausse → wait()
3. Réveil (signal ou spurious)
4. while vérifie À NOUVEAU
5. Si encore fausse → retour au wait()
6. Si vraie → sortie de la boucle
```

### Autre raison : Threads multiples

Même sans spurious wakeup, plusieurs threads peuvent être réveillés :

```c
// Scénario avec 2 consommateurs

Consommateur 1: wait() sur count == 0
Consommateur 2: wait() sur count == 0

Producteur: count++, signal()

// Signal réveille C1
// Mais pendant que C1 se réveille, C2 peut aussi se réveiller
// (ou broadcast est utilisé)

C1: Consomme l'item, count = 0
C2: Se réveille, count == 0 !
```

**Avec while** :

```c
C1: while(count == 0) wait()  → Réveil, count = 1, sort du while
    Consomme, count = 0

C2: while(count == 0) wait()  → Réveil, count = 0, retourne au wait()
```

### Règle d'or

```c
// ❌ JAMAIS ÇA
if (condition) {
    pthread_cond_wait(&cond, &mutex);
}

// ✅ TOUJOURS ÇA
while (!condition) {
    pthread_cond_wait(&cond, &mutex);
}
```

---

## Exemples pratiques

### Exemple 1 : Attendre qu'un thread soit prêt

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int thread_ready = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *worker(void *arg) {
    // Initialisation longue
    printf("Worker: Initialisation...\n");
    sleep(3);

    // Signaler que le thread est prêt
    pthread_mutex_lock(&mutex);
    thread_ready = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    printf("Worker: Prêt !\n");

    // Travail principal
    sleep(5);

    return NULL;
}

int main(void) {
    pthread_t tid;

    pthread_create(&tid, NULL, worker, NULL);

    // Attendre que le worker soit prêt
    pthread_mutex_lock(&mutex);
    while (!thread_ready) {
        printf("Main: Attente du worker...\n");
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    printf("Main: Worker prêt, je peux continuer\n");

    // Faire d'autres choses...

    pthread_join(tid, NULL);
    return 0;
}
```

### Exemple 2 : File de tâches

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_TASKS 100

typedef struct {
    void (*fonction)(int);
    int arg;
} Task;

typedef struct {
    Task tasks[MAX_TASKS];
    int count;
    int shutdown;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} TaskQueue;

TaskQueue queue = {
    .count = 0,
    .shutdown = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .cond = PTHREAD_COND_INITIALIZER
};

void executer_tache(int n) {
    printf("Tâche %d exécutée par thread %lu\n", n, pthread_self());
}

void ajouter_tache(void (*fonction)(int), int arg) {
    pthread_mutex_lock(&queue.mutex);

    if (queue.count < MAX_TASKS) {
        queue.tasks[queue.count].fonction = fonction;
        queue.tasks[queue.count].arg = arg;
        queue.count++;

        pthread_cond_signal(&queue.cond);
    }

    pthread_mutex_unlock(&queue.mutex);
}

void *worker(void *arg) {
    while (1) {
        pthread_mutex_lock(&queue.mutex);

        // Attendre une tâche ou shutdown
        while (queue.count == 0 && !queue.shutdown) {
            pthread_cond_wait(&queue.cond, &queue.mutex);
        }

        // Vérifier shutdown
        if (queue.shutdown && queue.count == 0) {
            pthread_mutex_unlock(&queue.mutex);
            break;
        }

        // Prendre une tâche
        Task task = queue.tasks[--queue.count];

        pthread_mutex_unlock(&queue.mutex);

        // Exécuter (en dehors du mutex)
        task.fonction(task.arg);
    }

    printf("Worker %lu terminé\n", pthread_self());
    return NULL;
}

int main(void) {
    pthread_t workers[4];

    // Créer les workers
    for (int i = 0; i < 4; i++) {
        pthread_create(&workers[i], NULL, worker, NULL);
    }

    // Ajouter des tâches
    for (int i = 0; i < 20; i++) {
        ajouter_tache(executer_tache, i);
    }

    // Attendre un peu
    sleep(2);

    // Demander l'arrêt
    pthread_mutex_lock(&queue.mutex);
    queue.shutdown = 1;
    pthread_cond_broadcast(&queue.cond);  // Réveiller tous les workers
    pthread_mutex_unlock(&queue.mutex);

    // Attendre les workers
    for (int i = 0; i < 4; i++) {
        pthread_join(workers[i], NULL);
    }

    return 0;
}
```

### Exemple 3 : Read-Write avec priorité lecteurs

```c
typedef struct {
    int readers;       // Nombre de lecteurs actifs
    int writer;        // 1 si écrivain actif
    int waiting_writers;  // Écrivains en attente
    pthread_mutex_t mutex;
    pthread_cond_t cond_read;
    pthread_cond_t cond_write;
} RWLock;

void rwlock_init(RWLock *rw) {
    rw->readers = 0;
    rw->writer = 0;
    rw->waiting_writers = 0;
    pthread_mutex_init(&rw->mutex, NULL);
    pthread_cond_init(&rw->cond_read, NULL);
    pthread_cond_init(&rw->cond_write, NULL);
}

void read_lock(RWLock *rw) {
    pthread_mutex_lock(&rw->mutex);

    // Attendre s'il y a un écrivain ou des écrivains en attente
    while (rw->writer || rw->waiting_writers > 0) {
        pthread_cond_wait(&rw->cond_read, &rw->mutex);
    }

    rw->readers++;

    pthread_mutex_unlock(&rw->mutex);
}

void read_unlock(RWLock *rw) {
    pthread_mutex_lock(&rw->mutex);

    rw->readers--;

    if (rw->readers == 0) {
        pthread_cond_signal(&rw->cond_write);  // Réveiller un écrivain
    }

    pthread_mutex_unlock(&rw->mutex);
}

void write_lock(RWLock *rw) {
    pthread_mutex_lock(&rw->mutex);

    rw->waiting_writers++;

    // Attendre qu'il n'y ait ni lecteurs ni écrivain
    while (rw->readers > 0 || rw->writer) {
        pthread_cond_wait(&rw->cond_write, &rw->mutex);
    }

    rw->waiting_writers--;
    rw->writer = 1;

    pthread_mutex_unlock(&rw->mutex);
}

void write_unlock(RWLock *rw) {
    pthread_mutex_lock(&rw->mutex);

    rw->writer = 0;

    // Réveiller tout le monde (lecteurs et écrivains)
    pthread_cond_broadcast(&rw->cond_read);
    pthread_cond_signal(&rw->cond_write);

    pthread_mutex_unlock(&rw->mutex);
}
```

---

## pthread_cond_timedwait()

Attendre avec un **timeout** pour éviter les blocages infinis.

### Signature

```c
int pthread_cond_timedwait(pthread_cond_t *cond,
                          pthread_mutex_t *mutex,
                          const struct timespec *abstime);
```

**Paramètres** :
- `cond` : Variable de condition
- `mutex` : Mutex (doit être verrouillé)
- `abstime` : Temps **absolu** maximum (pas relatif)

**Retour** :
- `0` : Condition signalée
- `ETIMEDOUT` : Timeout écoulé
- Autre code d'erreur

### Exemple

```c
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

int ready = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *waiter(void *arg) {
    struct timespec timeout;

    // Calculer temps absolu : maintenant + 5 secondes
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 5;

    pthread_mutex_lock(&mutex);

    printf("Attente avec timeout de 5 secondes...\n");

    while (!ready) {
        int result = pthread_cond_timedwait(&cond, &mutex, &timeout);

        if (result == ETIMEDOUT) {
            printf("TIMEOUT : Aucun signal reçu après 5 secondes\n");
            pthread_mutex_unlock(&mutex);
            return NULL;
        }
    }

    printf("Signal reçu à temps !\n");

    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(void) {
    pthread_t tid;

    pthread_create(&tid, NULL, waiter, NULL);

    // Ne jamais signaler → timeout
    pthread_join(tid, NULL);

    return 0;
}
```

**Sortie** :
```
Attente avec timeout de 5 secondes...
TIMEOUT : Aucun signal reçu après 5 secondes
```

---

## Erreurs courantes

### 1. Utiliser if au lieu de while

```c
// ❌ ERREUR
if (count == 0) {
    pthread_cond_wait(&cond, &mutex);
}

// ✅ CORRECT
while (count == 0) {
    pthread_cond_wait(&cond, &mutex);
}
```

### 2. Oublier le mutex

```c
// ❌ ERREUR : wait sans mutex verrouillé
pthread_cond_wait(&cond, &mutex);  // Crash ou comportement indéfini

// ✅ CORRECT
pthread_mutex_lock(&mutex);
while (!condition) {
    pthread_cond_wait(&cond, &mutex);
}
pthread_mutex_unlock(&mutex);
```

### 3. Signal sans protéger les données

```c
// ❌ MAUVAIS : Race condition
ready = 1;  // Pas protégé !
pthread_cond_signal(&cond);

// ✅ CORRECT
pthread_mutex_lock(&mutex);
ready = 1;
pthread_mutex_unlock(&mutex);
pthread_cond_signal(&cond);
```

### 4. Attendre la mauvaise condition

```c
// ❌ ERREUR : Attend sur la mauvaise condition
pthread_mutex_lock(&mutex);
while (count == 0) {
    pthread_cond_wait(&cond_not_full, &mutex);  // Mauvaise condition !
}
pthread_mutex_unlock(&mutex);
```

### 5. Deadlock avec cond_wait

```c
// ❌ DEADLOCK : wait sans le bon mutex
pthread_mutex_lock(&mutex_A);
pthread_cond_wait(&cond, &mutex_B);  // Attend sur mutex_B mais possède A !
pthread_mutex_unlock(&mutex_A);
```

---

## Bonnes pratiques

### 1. Toujours while, jamais if

```c
// ✅ Pattern à suivre systématiquement
pthread_mutex_lock(&mutex);

while (!condition) {
    pthread_cond_wait(&cond, &mutex);
}

// Travail...

pthread_mutex_unlock(&mutex);
```

### 2. Encapsuler dans des fonctions

```c
void buffer_put(Buffer *buf, int item) {
    pthread_mutex_lock(&buf->mutex);

    while (buf->count == BUFFER_SIZE) {
        pthread_cond_wait(&buf->cond_not_full, &buf->mutex);
    }

    buf->items[buf->count++] = item;

    pthread_cond_signal(&buf->cond_not_empty);

    pthread_mutex_unlock(&buf->mutex);
}

int buffer_get(Buffer *buf) {
    pthread_mutex_lock(&buf->mutex);

    while (buf->count == 0) {
        pthread_cond_wait(&buf->cond_not_empty, &buf->mutex);
    }

    int item = buf->items[--buf->count];

    pthread_cond_signal(&buf->cond_not_full);

    pthread_mutex_unlock(&buf->mutex);

    return item;
}
```

### 3. Une condition par prédicat logique

```c
// ✅ BON : Conditions séparées
pthread_cond_t cond_not_empty;  // Pour "buffer non vide"
pthread_cond_t cond_not_full;   // Pour "buffer non plein"

// ❌ MOINS BON : Une seule condition pour tout
pthread_cond_t cond;  // Utilisée pour les deux → broadcast nécessaire
```

### 4. Signal après unlock (recommandé)

```c
// ✅ RECOMMANDÉ
pthread_mutex_lock(&mutex);
ready = 1;
pthread_mutex_unlock(&mutex);
pthread_cond_signal(&cond);  // Signal après unlock

// ✅ FONCTIONNE AUSSI (mais moins efficace)
pthread_mutex_lock(&mutex);
ready = 1;
pthread_cond_signal(&cond);
pthread_mutex_unlock(&mutex);
```

### 5. Documenter les invariants

```c
// Buffer circulaire
// Invariants :
//   - 0 <= count <= BUFFER_SIZE
//   - count == 0 → buffer vide
//   - count == BUFFER_SIZE → buffer plein
// Protection : mutex
// Signalisation :
//   - cond_not_empty : signalée quand count > 0
//   - cond_not_full : signalée quand count < BUFFER_SIZE
```

---

## Variables de condition vs autres mécanismes

### Variables de condition vs Busy-waiting

| Critère | Busy-waiting | Variables de condition |
|---------|--------------|------------------------|
| **CPU** | 100% (vérification continue) | 0% (thread endormi) |
| **Latence** | Faible | Très faible |
| **Scalabilité** | ❌ Mauvaise | ✅ Excellente |
| **Use case** | Jamais (sauf spin lock très court) | Attente de condition |

### Variables de condition vs Sémaphores

| Critère | Sémaphores | Variables de condition |
|---------|------------|------------------------|
| **Complexité** | Simple (compteur) | Plus complexe (+ mutex) |
| **Flexibilité** | Limitée | Très flexible |
| **Prédicat** | Compteur seul | N'importe quelle condition |
| **Broadcast** | Non | Oui |
| **Use case** | Comptage de ressources | Conditions complexes |

### Variables de condition vs Sleep

| Critère | Sleep | Variables de condition |
|---------|-------|------------------------|
| **Précision** | ❌ Faible (1s, 100ms...) | ✅ Réveil immédiat |
| **CPU** | Faible mais non-zero | Zero |
| **Fiabilité** | ❌ Peut manquer événements | ✅ Fiable |

---

## Récapitulatif

Dans cette section, vous avez appris :

✅ **Variables de condition** :
- Mécanisme pour faire attendre efficacement un thread
- Toujours associées à un mutex
- Type `pthread_cond_t`

✅ **Fonctions principales** :
- `pthread_cond_wait()` : Attendre (libère mutex, s'endort, reprend mutex)
- `pthread_cond_signal()` : Réveiller un thread
- `pthread_cond_broadcast()` : Réveiller tous les threads
- `pthread_cond_timedwait()` : Attendre avec timeout

✅ **Pattern obligatoire** :
```c
pthread_mutex_lock(&mutex);
while (!condition) {  // TOUJOURS while
    pthread_cond_wait(&cond, &mutex);
}
// Travail
pthread_mutex_unlock(&mutex);
```

✅ **Spurious wakeups** :
- Réveils sans signal
- Pourquoi utiliser while au lieu de if
- Vérifier la condition à chaque réveil

✅ **Producteur-consommateur** :
- Pattern classique
- Deux conditions (not_empty, not_full)
- Multi-threads possible

✅ **Bonnes pratiques** :
- while, pas if
- Encapsulation dans fonctions
- Une condition par prédicat
- Signal après unlock
- Documentation des invariants

✅ **Erreurs à éviter** :
- if au lieu de while
- Oublier le mutex
- Signal sans protéger les données
- Mauvaise condition
- Deadlock

---

## Pour aller plus loin

Dans les prochaines sections :

- **18.9** : Sémaphores POSIX (contrôle d'accès multiples)
- **18.10** : Atomics C11 (opérations lock-free)
- **18.11** : Read-write locks (optimisation lecture/écriture)
- **18.12** : Thread-local storage (variables par thread)

**Conseil pratique** : Implémentez une file de tâches avec variables de condition. Créez plusieurs workers qui attendent des tâches, et un main qui ajoute des tâches. Testez avec shutdown propre en utilisant broadcast.

**Projet** : Créez un système de cache avec expiration : des threads producteurs ajoutent des entrées, des threads consommateurs les lisent, et un thread cleaner supprime les entrées expirées. Utilisez des variables de condition pour synchroniser l'accès.

---

📘 **Ressources utiles** :
- `man pthread_cond_wait`
- `man pthread_cond_signal`
- `man pthread_cond_broadcast`
- Livre : "Programming with POSIX Threads" (David Butenhof)

💡 **Prochaine étape** : Section 18.9 - Sémaphores POSIX

**Citation** : *"Condition variables are the Swiss Army knife of synchronization."* - David Butenhof

⏭️ [Sémaphores POSIX](/18-threads-et-concurrence/09-semaphores.md)
