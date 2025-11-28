🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 35.3 Investigation de race conditions

## Introduction

Une **race condition** (condition de concurrence) est un bug qui survient lorsque plusieurs threads accèdent simultanément à une même ressource partagée (variable, structure, fichier) et qu'au moins un des accès est une écriture. Le résultat final dépend alors de l'**ordre d'exécution** des threads, qui est imprévisible.

C'est comme si deux personnes essayaient d'écrire dans le même cahier en même temps : le résultat final sera chaotique et dépendra de qui écrit quand.

### Pourquoi les race conditions sont-elles redoutables ?

Les race conditions sont parmi les bugs **les plus difficiles à diagnostiquer** car :

1. **Non-déterministes** : Le bug peut apparaître une fois sur mille exécutions
2. **Sensibles au timing** : Ajouter un `printf` peut "masquer" le bug (effet Heisenbug)
3. **Dépendantes de la charge** : Ne se manifestent qu'en production sous forte charge
4. **Difficiles à reproduire** : Conditions exactes rarement réunies en développement
5. **Conséquences variées** : Corruption de données, crashs, deadlocks, résultats incorrects

**Exemple concret en production** : Une banque en ligne a une race condition sur le calcul du solde. Résultat : un client effectue deux retraits simultanés de 100€ avec un solde de 150€. Sans protection, les deux retraits peuvent passer (solde = -50€).

---

## Comprendre les race conditions

### Anatomie d'une race condition simple

```c
#include <pthread.h>
#include <stdio.h>

int counter = 0;  // Variable partagée

void* increment(void* arg) {
    for (int i = 0; i < 100000; i++) {
        counter++;  // ❌ RACE CONDITION
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Counter: %d\n", counter);  // Devrait être 200000
    return 0;
}
```

**Résultat attendu** : 200 000
**Résultat réel** : 187 432 (varie à chaque exécution)

### Pourquoi counter++ n'est pas atomique

L'opération `counter++` se décompose en 3 instructions assembleur :

```asm
mov eax, [counter]    # 1. Lire la valeur
add eax, 1            # 2. Incrémenter
mov [counter], eax    # 3. Écrire la nouvelle valeur
```

**Scénario de race condition** :

| Temps | Thread 1 | Thread 2 | Valeur de counter |
|-------|----------|----------|-------------------|
| t0    | Lit counter (0) | | 0 |
| t1    | | Lit counter (0) | 0 |
| t2    | Incrémente (1) | | 0 |
| t3    | | Incrémente (1) | 0 |
| t4    | Écrit 1 | | 1 |
| t5    | | Écrit 1 | 1 ❌ |

**Résultat** : Deux incrémentations, mais counter vaut 1 au lieu de 2. Une incrémentation a été **perdue**.

### Types de race conditions

#### 1. Read-Modify-Write

L'exemple ci-dessus : lecture, modification, écriture sans protection.

```c
// ❌ RACE : Incrément non atomique
counter++;

// ✅ SOLUTION : Mutex
pthread_mutex_lock(&mutex);
counter++;
pthread_mutex_unlock(&mutex);
```

#### 2. Check-Then-Act

Vérifier une condition puis agir en fonction, mais la condition peut changer entre-temps.

```c
// ❌ RACE : La condition peut changer entre le if et le malloc
if (cache == NULL) {
    cache = malloc(sizeof(Data));  // Peut être exécuté par 2 threads
}
```

#### 3. Write-Write

Deux threads écrivent simultanément dans la même variable.

```c
// Thread 1
config.timeout = 30;

// Thread 2
config.timeout = 60;

// ❌ RACE : Quelle valeur finale ? Dépend du timing
```

#### 4. Write-Read (Visibility)

Un thread écrit, un autre lit, mais la nouvelle valeur n'est pas encore visible.

```c
// Thread 1
data_ready = 1;  // ❌ Peut ne pas être visible immédiatement pour Thread 2

// Thread 2
if (data_ready) {
    process_data();  // Peut voir l'ancienne valeur (0)
}
```

---

## Détection avec ThreadSanitizer (TSan)

**ThreadSanitizer** est l'outil de référence pour détecter les race conditions. Il fait partie de la famille des sanitizers comme AddressSanitizer.

### Compilation avec TSan

```bash
gcc -g -fsanitize=thread -o myapp myapp.c -lpthread
```

**Important** : TSan nécessite :
- `-g` pour les symboles de débogage
- `-lpthread` si vous utilisez pthread
- Ne pas combiner avec ASan (incompatibles)

### Exemple d'analyse

Reprenons notre code avec race condition :

```c
// race_example.c
#include <pthread.h>
#include <stdio.h>

int counter = 0;

void* increment(void* arg) {
    for (int i = 0; i < 100000; i++) {
        counter++;
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("Counter: %d\n", counter);
    return 0;
}
```

**Compilation et exécution** :

```bash
$ gcc -g -fsanitize=thread -o race_example race_example.c -lpthread
$ ./race_example
```

**Sortie TSan** :

```
==================
WARNING: ThreadSanitizer: data race (pid=12345)
  Write of size 4 at 0x7b0400000000 by thread T2:
    #0 increment race_example.c:7 (race_example+0x12ab)

  Previous write of size 4 at 0x7b0400000000 by thread T1:
    #0 increment race_example.c:7 (race_example+0x12ab)

  Location is global 'counter' of size 4 at 0x7b0400000000 (race_example+0x0000004040a0)

  Thread T2 (tid=12347, running) created by main thread at:
    #0 pthread_create <null>:0 (race_example+0x42ab3)
    #1 main race_example.c:15 (race_example+0x131d)

  Thread T1 (tid=12346, running) created by main thread at:
    #0 pthread_create <null>:0 (race_example+0x42ab3)
    #1 main race_example.c:14 (race_example+0x130c)

SUMMARY: ThreadSanitizer: data race race_example.c:7 in increment
==================
Counter: 178432
ThreadSanitizer: reported 1 warnings
```

### Interpréter la sortie TSan

**Points clés à identifier** :

1. **Type de problème** : `data race`
2. **Où** : `race_example.c:7` (la ligne `counter++`)
3. **Qui** : Thread T1 et Thread T2
4. **Quoi** : `Write of size 4` (deux écritures simultanées)
5. **Variable impliquée** : `global 'counter'`

---

## Détection avec Valgrind Helgrind

**Helgrind** est un outil Valgrind spécialisé dans la détection de problèmes de concurrence.

### Utilisation

```bash
# Compiler normalement (avec -g pour les symboles)
gcc -g -o race_example race_example.c -lpthread

# Exécuter avec Helgrind
valgrind --tool=helgrind ./race_example
```

**Sortie Helgrind** :

```
==12345== Helgrind, a thread error detector
==12345== Using Valgrind-3.15.0
==12345==
==12345== Possible data race during write of size 4 at 0x309014 by thread #3
==12345==    at 0x1087AB: increment (race_example.c:7)
==12345==    by 0x4C34DB6: mythread_wrapper (hg_intercepts.c:389)
==12345==    by 0x4E4B608: start_thread (pthread_create.c:477)
==12345==  This conflicts with a previous write of size 4 by thread #2
==12345==    at 0x1087AB: increment (race_example.c:7)
==12345==    by 0x4C34DB6: mythread_wrapper (hg_intercepts.c:389)
==12345==    by 0x4E4B608: start_thread (pthread_create.c:477)
==12345==  Address 0x309014 is 0 bytes inside data symbol "counter"
```

### TSan vs Helgrind : Comparaison

| Critère | ThreadSanitizer | Helgrind |
|---------|-----------------|----------|
| **Vitesse** | Rapide (2-5x) | Très lent (20-50x) |
| **Recompilation** | Requise | Non requise |
| **Détection** | Excellente | Très bonne |
| **Faux positifs** | Peu | Quelques-uns |
| **Deadlocks** | Non | ✅ Oui |
| **Order violations** | ✅ Oui | ✅ Oui |
| **Utilisation** | Développement quotidien | Analyse approfondie |

**Recommandation** : Utilisez TSan par défaut, Helgrind pour les cas complexes.

---

## Techniques d'investigation

### 1. Réduction du problème

Les race conditions sont difficiles à déboguer. Il faut d'abord **isoler** le problème.

#### Technique : Augmenter les itérations

Plus d'itérations = plus de chances de déclencher la race.

```c
// Au lieu de 10 itérations
for (int i = 0; i < 10; i++) { ... }

// Essayer avec 1 000 000
for (int i = 0; i < 1000000; i++) { ... }
```

#### Technique : Stress testing

Créer plus de threads que de cœurs CPU :

```c
#define NUM_THREADS 100  // Sur une machine à 8 cœurs

pthread_t threads[NUM_THREADS];
for (int i = 0; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], NULL, worker, NULL);
}
```

### 2. Logging stratégique

Ajouter des logs peut **masquer** la race (effet Heisenbug), mais peut aussi donner des indices.

```c
// ❌ MAL : fprintf peut synchroniser involontairement
void* worker(void* arg) {
    fprintf(stderr, "Thread %lu: before increment\n", pthread_self());
    counter++;
    fprintf(stderr, "Thread %lu: after increment\n", pthread_self());
    return NULL;
}

// ✅ MIEUX : Logger dans un buffer lock-free puis dumper à la fin
#define LOG_SIZE 10000
struct {
    unsigned long tid;
    int value;
} log_buffer[LOG_SIZE];
atomic_int log_index = 0;

void log_event(int value) {
    int idx = atomic_fetch_add(&log_index, 1);
    if (idx < LOG_SIZE) {
        log_buffer[idx].tid = pthread_self();
        log_buffer[idx].value = value;
    }
}
```

### 3. Insertion de délais (sleep)

Forcer un ordre d'exécution spécifique pour reproduire la race :

```c
#include <unistd.h>

void* thread1(void* arg) {
    if (cache == NULL) {
        usleep(1000);  // Délai pour laisser thread2 entrer dans le if
        cache = malloc(sizeof(Data));
    }
    return NULL;
}
```

**Attention** : Cette technique ne fonctionne que pour reproduire, pas pour corriger !

### 4. Analyse de la timeline

Créer une timeline des événements pour visualiser la race :

```c
#include <time.h>

void log_with_timestamp(const char *event) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    printf("[%ld.%09ld] Thread %lu: %s\n",
           ts.tv_sec, ts.tv_nsec, pthread_self(), event);
}

void* worker(void* arg) {
    log_with_timestamp("START");
    log_with_timestamp("BEFORE_LOCK");
    pthread_mutex_lock(&mutex);
    log_with_timestamp("ACQUIRED_LOCK");
    // ... section critique ...
    pthread_mutex_unlock(&mutex);
    log_with_timestamp("RELEASED_LOCK");
    return NULL;
}
```

---

## Cas pratiques d'investigation

### Cas 1 : Race sur un compteur de références

**Symptôme** : Segfault aléatoire lors de la libération d'un objet partagé.

**Code problématique** :

```c
typedef struct {
    int ref_count;
    char *data;
} SharedObject;

SharedObject* create_object() {
    SharedObject *obj = malloc(sizeof(SharedObject));
    obj->ref_count = 1;
    obj->data = malloc(1024);
    return obj;
}

void retain(SharedObject *obj) {
    obj->ref_count++;  // ❌ RACE CONDITION
}

void release(SharedObject *obj) {
    obj->ref_count--;  // ❌ RACE CONDITION
    if (obj->ref_count == 0) {
        free(obj->data);
        free(obj);
    }
}
```

**Diagnostic avec TSan** :

```bash
$ gcc -g -fsanitize=thread -o shared shared.c -lpthread
$ ./shared
WARNING: ThreadSanitizer: data race
  Write of size 4 at 0x7b04... by thread T2:
    #0 retain shared.c:14
  Previous write of size 4 at 0x7b04... by thread T1:
    #0 retain shared.c:14
```

**Analyse** : Deux threads incrémentent `ref_count` simultanément. Les incrémentations peuvent être perdues.

**Scénario catastrophe** :
1. Thread 1 lit `ref_count = 2`
2. Thread 2 lit `ref_count = 2`
3. Thread 1 écrit `ref_count = 1` (release)
4. Thread 2 écrit `ref_count = 1` (release)
5. Thread 1 voit `ref_count = 1`, décrémente → 0, libère l'objet
6. Thread 2 voit `ref_count = 0`, tente de décrémenter → use-after-free

**Solution** : Utiliser des opérations atomiques

```c
#include <stdatomic.h>

typedef struct {
    atomic_int ref_count;  // ✅ Type atomique
    char *data;
} SharedObject;

void retain(SharedObject *obj) {
    atomic_fetch_add(&obj->ref_count, 1);  // ✅ Atomique
}

void release(SharedObject *obj) {
    if (atomic_fetch_sub(&obj->ref_count, 1) == 1) {  // ✅ Atomique
        free(obj->data);
        free(obj);
    }
}
```

### Cas 2 : Race sur un flag de statut

**Symptôme** : Un worker thread ne se termine jamais, même après avoir reçu un signal d'arrêt.

**Code problématique** :

```c
int should_stop = 0;  // Variable partagée

void* worker(void* arg) {
    while (!should_stop) {  // ❌ RACE : Lecture non synchronisée
        process_item();
    }
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, worker, NULL);

    sleep(5);
    should_stop = 1;  // ❌ RACE : Écriture non synchronisée

    pthread_join(thread, NULL);  // Peut bloquer indéfiniment
    return 0;
}
```

**Problème** : Sans synchronisation, le compilateur peut optimiser la boucle du worker :

```c
// Optimisation par le compilateur
int temp = should_stop;  // Lecture une seule fois
while (!temp) {          // Boucle infinie si temp = 0
    process_item();
}
```

**Solution 1** : Utiliser `volatile` (minimal)

```c
volatile int should_stop = 0;  // Force la relecture à chaque itération
```

**Solution 2** : Utiliser des atomiques (mieux)

```c
#include <stdatomic.h>

atomic_bool should_stop = false;

void* worker(void* arg) {
    while (!atomic_load(&should_stop)) {  // ✅ Lecture atomique
        process_item();
    }
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, worker, NULL);

    sleep(5);
    atomic_store(&should_stop, true);  // ✅ Écriture atomique

    pthread_join(thread, NULL);
    return 0;
}
```

### Cas 3 : Double-checked locking

**Symptôme** : Initialisation singleton qui crashe sporadiquement.

**Code problématique** :

```c
Config *global_config = NULL;
pthread_mutex_t config_mutex = PTHREAD_MUTEX_INITIALIZER;

Config* get_config() {
    if (global_config == NULL) {  // ❌ Check non protégé
        pthread_mutex_lock(&config_mutex);
        if (global_config == NULL) {
            global_config = malloc(sizeof(Config));
            init_config(global_config);
        }
        pthread_mutex_unlock(&config_mutex);
    }
    return global_config;  // ❌ Peut retourner un pointeur partiellement initialisé
}
```

**Problème** : Réordonnancement des instructions par le compilateur/CPU :

```c
// Ce que vous écrivez
global_config = malloc(sizeof(Config));
init_config(global_config);

// Ce que le CPU peut exécuter
global_config = <adresse>;  // Assignation visible AVANT l'initialisation
init_config(global_config);
```

Résultat : Un autre thread voit `global_config != NULL` mais l'objet n'est pas encore initialisé.

**Solution** : Utiliser `pthread_once` (pattern idiomatique)

```c
Config *global_config = NULL;
pthread_once_t config_once = PTHREAD_ONCE_INIT;

void init_config_once() {
    global_config = malloc(sizeof(Config));
    init_config(global_config);
}

Config* get_config() {
    pthread_once(&config_once, init_config_once);  // ✅ Thread-safe
    return global_config;
}
```

### Cas 4 : Race sur une liste partagée

**Symptôme** : Corruption de liste chaînée, segfaults aléatoires.

**Code problématique** :

```c
typedef struct Node {
    int data;
    struct Node *next;
} Node;

Node *head = NULL;

void add_item(int value) {
    Node *new_node = malloc(sizeof(Node));
    new_node->data = value;
    new_node->next = head;  // ❌ RACE
    head = new_node;        // ❌ RACE
}
```

**Diagnostic avec TSan** :

```
WARNING: ThreadSanitizer: data race
  Write of size 8 at 0x7b04... by thread T2:
    #0 add_item list.c:12
  Previous write of size 8 at 0x7b04... by thread T1:
    #0 add_item list.c:12
```

**Scénario de corruption** :

| Thread 1 | Thread 2 | État de head |
|----------|----------|--------------|
| `new_node1->next = head` (NULL) | | NULL |
| | `new_node2->next = head` (NULL) | NULL |
| `head = new_node1` | | new_node1 |
| | `head = new_node2` | new_node2 ❌ |

**Résultat** : new_node1 est perdu (fuite mémoire) et new_node2 pointe vers NULL au lieu de new_node1.

**Solution** : Protéger toute la section critique

```c
pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;

void add_item(int value) {
    Node *new_node = malloc(sizeof(Node));
    new_node->data = value;

    pthread_mutex_lock(&list_mutex);   // ✅ Lock
    new_node->next = head;
    head = new_node;
    pthread_mutex_unlock(&list_mutex); // ✅ Unlock
}
```

---

## Solutions aux race conditions

### 1. Mutex (Mutual Exclusion)

Protection classique avec verrous.

```c
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int shared_data = 0;

void* worker(void* arg) {
    pthread_mutex_lock(&mutex);
    // Section critique : un seul thread à la fois
    shared_data++;
    pthread_mutex_unlock(&mutex);
    return NULL;
}
```

**Avantages** : Simple, fiable
**Inconvénients** : Overhead, risque de deadlock

### 2. Atomiques (C11)

Opérations garanties atomiques sans verrous.

```c
#include <stdatomic.h>

atomic_int counter = 0;

void* worker(void* arg) {
    atomic_fetch_add(&counter, 1);  // Atomique, pas de lock
    return NULL;
}
```

**Avantages** : Rapide, pas de deadlock
**Inconvénients** : Limité aux opérations simples

### 3. Read-Write Locks

Permettent plusieurs lecteurs simultanés, mais un seul écrivain.

```c
#include <pthread.h>

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
int shared_data = 0;

void* reader(void* arg) {
    pthread_rwlock_rdlock(&rwlock);  // Plusieurs readers OK
    int value = shared_data;
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

void* writer(void* arg) {
    pthread_rwlock_wrlock(&rwlock);  // Exclusif
    shared_data++;
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}
```

**Avantages** : Optimisé pour lectures fréquentes
**Inconvénients** : Plus complexe

### 4. Thread-Local Storage

Chaque thread a sa propre copie de la variable.

```c
__thread int thread_local_counter = 0;  // GCC/Clang
// ou
_Thread_local int thread_local_counter = 0;  // C11

void* worker(void* arg) {
    thread_local_counter++;  // Pas de race, chaque thread a sa copie
    return NULL;
}
```

**Avantages** : Aucune synchronisation nécessaire
**Inconvénients** : Données non partagées

### 5. Message Passing

Éviter le partage de données, communiquer par messages.

```c
// Producer
void* producer(void* arg) {
    while (running) {
        Message *msg = create_message();
        queue_push(msg);  // Queue thread-safe
    }
    return NULL;
}

// Consumer
void* consumer(void* arg) {
    while (running) {
        Message *msg = queue_pop();  // Queue thread-safe
        process_message(msg);
        free_message(msg);
    }
    return NULL;
}
```

**Avantages** : Découplage, moins de bugs
**Inconvénients** : Overhead de la queue

---

## Prévention des race conditions

### 1. Design sans partage

**Principe** : Si possible, évitez le partage de données entre threads.

```c
// ❌ MAL : Données partagées
int global_result = 0;

void* worker(void* arg) {
    global_result += compute();  // Race
    return NULL;
}

// ✅ MIEUX : Chaque thread retourne son résultat
void* worker(void* arg) {
    int *result = malloc(sizeof(int));
    *result = compute();
    return result;  // Pas de partage
}

int main() {
    pthread_t threads[NUM_THREADS];
    int total = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        int *result;
        pthread_join(threads[i], (void**)&result);
        total += *result;  // Agrégation dans le thread principal
        free(result);
    }

    printf("Total: %d\n", total);
}
```

### 2. Immutabilité

Les données en lecture seule ne créent jamais de races.

```c
// Configuration partagée, mais jamais modifiée après l'initialisation
const Config *global_config = NULL;

void init() {
    Config *cfg = malloc(sizeof(Config));
    load_config(cfg);
    global_config = cfg;  // Assignation une seule fois
}

void* worker(void* arg) {
    // Lecture seule, pas de race
    int timeout = global_config->timeout;
    return NULL;
}
```

### 3. Principe de responsabilité unique

Chaque thread est responsable de ses propres données.

```c
typedef struct {
    int thread_id;
    int *local_data;  // Données privées à ce thread
} ThreadContext;

void* worker(void* arg) {
    ThreadContext *ctx = (ThreadContext*)arg;

    // Travail uniquement sur ctx->local_data
    for (int i = 0; i < 1000; i++) {
        ctx->local_data[i] = compute(ctx->thread_id, i);
    }

    return NULL;
}
```

### 4. Granularité des locks

Minimiser la taille des sections critiques.

```c
// ❌ MAL : Lock trop large
pthread_mutex_lock(&mutex);
int value = shared_data;
int result = expensive_computation(value);  // Calcul long sous lock
shared_data = result;
pthread_mutex_unlock(&mutex);

// ✅ MIEUX : Lock minimal
pthread_mutex_lock(&mutex);
int value = shared_data;
pthread_mutex_unlock(&mutex);

int result = expensive_computation(value);  // Calcul hors lock

pthread_mutex_lock(&mutex);
shared_data = result;
pthread_mutex_unlock(&mutex);
```

### 5. Convention d'ordre de lock

Pour éviter les deadlocks et raisonner sur les races.

```c
// Convention : Toujours locker dans l'ordre : mutex_A puis mutex_B

// ✅ CORRECT
void transfer_a_to_b() {
    pthread_mutex_lock(&mutex_A);
    pthread_mutex_lock(&mutex_B);
    // ... transfert ...
    pthread_mutex_unlock(&mutex_B);
    pthread_mutex_unlock(&mutex_A);
}

void transfer_b_to_a() {
    pthread_mutex_lock(&mutex_A);  // Même ordre
    pthread_mutex_lock(&mutex_B);
    // ... transfert ...
    pthread_mutex_unlock(&mutex_B);
    pthread_mutex_unlock(&mutex_A);
}
```

---

## Outils avancés

### 1. rr (Record and Replay)

**rr** de Mozilla permet d'enregistrer une exécution et de la rejouer de manière déterministe.

```bash
# Installation
sudo apt install rr

# Enregistrement de l'exécution
rr record ./myapp

# Rejeu avec GDB
rr replay
(rr) continue
(rr) reverse-continue  # Remonter dans le temps !
```

**Avantage énorme** : Vous pouvez revenir en arrière pour comprendre ce qui s'est passé avant la race.

### 2. Intel Inspector

Outil commercial puissant pour détecter les races et deadlocks.

```bash
inspxe-cl -collect ti3 -- ./myapp
inspxe-cl -report problems -report-output report.txt
```

### 3. Clang Thread Safety Analysis

Annotations pour vérifier statiquement la protection des données.

```c
#include <pthread.h>

// Annotations Clang
int shared_data __attribute__((guarded_by(mutex)));
pthread_mutex_t mutex;

void access_data() __attribute__((requires_capability(mutex))) {
    shared_data++;  // ✅ Compilateur vérifie que mutex est locké
}

int main() {
    access_data();  // ⚠️ Avertissement : mutex non locké

    pthread_mutex_lock(&mutex);
    access_data();  // ✅ OK
    pthread_mutex_unlock(&mutex);
}
```

Compiler avec :
```bash
clang -Wthread-safety -o myapp myapp.c -lpthread
```

---

## Debugging avec GDB

### Techniques spécifiques aux races

#### 1. Watchpoints sur données partagées

```gdb
# Arrêter quand une variable est modifiée
(gdb) watch shared_data
(gdb) run

# GDB s'arrête à chaque modification
Hardware watchpoint 1: shared_data
Old value = 0
New value = 1
```

#### 2. Thread-specific breakpoints

```gdb
# Breakpoint uniquement pour le thread 3
(gdb) break increment thread 3
(gdb) info threads
(gdb) thread 3
(gdb) bt
```

#### 3. Conditional breakpoints

```gdb
# Arrêter seulement quand counter atteint une valeur suspecte
(gdb) break increment if counter > 195000 && counter < 205000
```

---

## Checklist d'investigation

### Phase 1 : Détection

- [ ] Compiler avec ThreadSanitizer (`-fsanitize=thread`)
- [ ] Exécuter les tests avec TSan actif
- [ ] Noter toutes les races détectées (même si non critiques)
- [ ] Tester avec Helgrind si TSan ne détecte rien

### Phase 2 : Reproduction

- [ ] Augmenter le nombre d'itérations
- [ ] Augmenter le nombre de threads
- [ ] Tester sous forte charge CPU
- [ ] Utiliser rr pour capturer une exécution

### Phase 3 : Analyse

- [ ] Identifier la variable partagée
- [ ] Tracer tous les accès (lectures et écritures)
- [ ] Déterminer la fenêtre temporelle de la race
- [ ] Comprendre les conséquences (corruption, crash, résultat incorrect)

### Phase 4 : Correction

- [ ] Choisir la solution appropriée (mutex, atomic, redesign)
- [ ] Implémenter la protection
- [ ] Vérifier qu'aucune nouvelle race n'est introduite
- [ ] Re-tester avec TSan (doit être clean)

### Phase 5 : Validation

- [ ] Tests de charge prolongés
- [ ] Validation en environnement de staging
- [ ] Monitoring en production

---

## Cas d'étude : Race condition en production

### Contexte

Un serveur web multi-threaded crashe aléatoirement une fois par semaine en production, mais jamais en développement.

### Investigation

**Étape 1** : Activer core dumps et attendre le prochain crash.

```bash
ulimit -c unlimited
```

**Étape 2** : Analyser le core dump avec GDB.

```gdb
$ gdb ./webserver core.12345
(gdb) bt
#0  0x00007f8b in process_request (req=0x0)
#1  0x00007f9c in worker_thread
```

→ `req` est NULL, mais pourquoi ?

**Étape 3** : Examiner le code de gestion des requêtes.

```c
typedef struct {
    int socket;
    char *buffer;
} Request;

Request *pending_requests[MAX_REQUESTS];
int request_count = 0;

void enqueue_request(Request *req) {
    pending_requests[request_count++] = req;  // ❌ RACE
}

Request* dequeue_request() {
    if (request_count == 0) return NULL;
    return pending_requests[--request_count];  // ❌ RACE
}
```

**Étape 4** : Reproduire avec TSan.

```bash
$ gcc -g -fsanitize=thread -o webserver webserver.c -lpthread
$ ./load_test.sh  # Script de test de charge

WARNING: ThreadSanitizer: data race
  Write of size 4 at 0x... by thread T5:
    #0 enqueue_request webserver.c:45
  Previous read of size 4 at 0x... by thread T3:
    #0 dequeue_request webserver.c:52
```

**Étape 5** : Correction avec mutex.

```c
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

void enqueue_request(Request *req) {
    pthread_mutex_lock(&queue_mutex);
    if (request_count < MAX_REQUESTS) {
        pending_requests[request_count++] = req;
    }
    pthread_mutex_unlock(&queue_mutex);
}

Request* dequeue_request() {
    Request *req = NULL;
    pthread_mutex_lock(&queue_mutex);
    if (request_count > 0) {
        req = pending_requests[--request_count];
    }
    pthread_mutex_unlock(&queue_mutex);
    return req;
}
```

**Étape 6** : Validation avec tests prolongés.

```bash
$ gcc -g -fsanitize=thread -o webserver webserver.c -lpthread
$ ./load_test.sh
# Aucun warning TSan après 24h de tests ✅
```

---

## Résumé

Les race conditions sont parmi les bugs les plus insidieux en programmation concurrente. Points essentiels :

### Détection
- **ThreadSanitizer** : Outil de référence, rapide et précis
- **Helgrind** : Alternative pour analyse approfondie
- **rr** : Record & replay pour débogage déterministe

### Compréhension
- Les races surviennent sur les **accès concurrents** à des données partagées
- L'opération `i++` n'est **pas atomique**
- Les lectures non protégées sont aussi dangereuses que les écritures

### Solutions
- **Mutex** : Protection classique, fiable mais avec overhead
- **Atomiques** : Pour opérations simples, très rapide
- **Design sans partage** : Meilleure solution quand possible

### Prévention
- Minimiser le partage de données entre threads
- Protéger **toutes** les données partagées
- Tester systématiquement avec TSan
- Code review axé sur la concurrence

### En production
- Activer core dumps pour capturer les crashes
- Monitorer les comportements anormaux
- Tests de charge réguliers
- Déployer progressivement les corrections

**La règle d'or** : En présence de threads, toute donnée partagée DOIT être protégée. Aucune exception.

---


⏭️ [Debugging de deadlocks](/35-debugging-code-complexe/04-debugging-deadlocks.md)
