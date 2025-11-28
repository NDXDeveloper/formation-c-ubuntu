🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 35.4 Debugging de deadlocks

## Introduction

Un **deadlock** (étreinte mortelle ou interblocage) est une situation où deux threads (ou plus) se bloquent mutuellement indéfiniment, chacun attendant que l'autre libère une ressource. C'est comme deux voitures qui se font face sur un pont à sens unique : aucune ne peut avancer ni reculer.

### Analogie de la vie réelle

Imaginez deux personnes qui veulent échanger des documents :
- Alice a le document A et attend le document B
- Bob a le document B et attend le document A

Si elles refusent toutes les deux de donner leur document avant de recevoir l'autre, elles resteront bloquées **indéfiniment**. C'est exactement ce qui se passe dans un deadlock informatique.

### Pourquoi les deadlocks sont-ils critiques ?

Les deadlocks sont des bugs **catastrophiques** en production car :

1. **Blocage total** : L'application devient complètement figée
2. **Indétectable par l'utilisateur** : L'application semble juste "ne plus répondre"
3. **Requiert un redémarrage** : Seul moyen de récupérer
4. **Perte de données** : Si des transactions sont en cours
5. **Difficiles à reproduire** : Dépendent du timing exact

**Impact en production** :
- Serveur web : Tous les workers bloqués, aucune nouvelle requête traitée
- Base de données : Transactions bloquées, timeouts clients
- Service critique : Indisponibilité totale du service

---

## Les quatre conditions de Coffman

Pour qu'un deadlock se produise, **quatre conditions** doivent être réunies simultanément. On les appelle les **conditions de Coffman** :

### 1. Exclusion mutuelle (Mutual Exclusion)

Les ressources ne peuvent être utilisées que par un seul thread à la fois.

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// ✅ Un seul thread peut avoir le mutex à la fois
```

### 2. Détention et attente (Hold and Wait)

Un thread détient une ressource et attend d'en acquérir d'autres.

```c
pthread_mutex_lock(&mutex_A);  // Thread détient A
pthread_mutex_lock(&mutex_B);  // Thread attend B (tout en détenant A)
```

### 3. Non-préemption (No Preemption)

Les ressources ne peuvent pas être arrachées de force à un thread qui les détient.

```c
// Un mutex ne peut pas être "volé" à un thread qui l'a locké
// Le thread doit le libérer volontairement avec pthread_mutex_unlock
```

### 4. Attente circulaire (Circular Wait)

Une chaîne circulaire de threads existe, où chaque thread attend une ressource détenue par le suivant.

```
Thread 1 détient A, attend B
Thread 2 détient B, attend A
```

**Pour prévenir les deadlocks, il suffit de casser une de ces quatre conditions.**

---

## Types de deadlocks

### 1. Deadlock simple (2 threads, 2 mutex)

Le cas classique : deux threads qui verrouillent deux mutex dans un ordre différent.

```c
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t mutex_A = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_B = PTHREAD_MUTEX_INITIALIZER;

void* thread1_func(void* arg) {
    printf("Thread 1: Locking A...\n");
    pthread_mutex_lock(&mutex_A);
    sleep(1);  // Simuler du travail

    printf("Thread 1: Locking B...\n");
    pthread_mutex_lock(&mutex_B);  // ❌ Bloque ici si thread2 a déjà B

    printf("Thread 1: Got both locks!\n");

    pthread_mutex_unlock(&mutex_B);
    pthread_mutex_unlock(&mutex_A);
    return NULL;
}

void* thread2_func(void* arg) {
    printf("Thread 2: Locking B...\n");
    pthread_mutex_lock(&mutex_B);
    sleep(1);  // Simuler du travail

    printf("Thread 2: Locking A...\n");
    pthread_mutex_lock(&mutex_A);  // ❌ Bloque ici si thread1 a déjà A

    printf("Thread 2: Got both locks!\n");

    pthread_mutex_unlock(&mutex_A);
    pthread_mutex_unlock(&mutex_B);
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, thread1_func, NULL);
    pthread_create(&t2, NULL, thread2_func, NULL);

    pthread_join(t1, NULL);  // ❌ Bloque indéfiniment
    pthread_join(t2, NULL);

    return 0;
}
```

**Exécution** :
```
Thread 1: Locking A...
Thread 2: Locking B...
Thread 1: Locking B...
Thread 2: Locking A...
[Blocage infini - aucun thread ne progresse]
```

**Diagramme du deadlock** :
```
Thread 1:  [A] ──wait──> [B]
                           ↑
                          owns
                           |
Thread 2:  [B] ──wait──> [A]
```

### 2. Deadlock circulaire (3+ threads, 3+ mutex)

Plus complexe : une chaîne circulaire de dépendances.

```c
// Thread 1: A -> B
// Thread 2: B -> C
// Thread 3: C -> A
// Résultat : Cycle A -> B -> C -> A
```

### 3. Self-deadlock

Un thread tente de verrouiller deux fois le même mutex non-récursif.

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void function() {
    pthread_mutex_lock(&mutex);
    // ... code ...
    pthread_mutex_lock(&mutex);  // ❌ Deadlock avec soi-même
}
```

### 4. Deadlock avec condition variables

Mauvaise utilisation des variables de condition.

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* waiter(void* arg) {
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond, &mutex);  // Attend un signal
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_t t;
    pthread_create(&t, NULL, waiter, NULL);

    // ❌ Oubli de signaler la condition
    // Le thread waiter attendra indéfiniment

    pthread_join(t, NULL);
    return 0;
}
```

---

## Détection avec Valgrind Helgrind

**Helgrind** est l'outil de référence pour détecter les deadlocks.

### Utilisation basique

```bash
# Compiler normalement
gcc -g -o deadlock deadlock.c -lpthread

# Exécuter avec Helgrind
valgrind --tool=helgrind ./deadlock
```

### Exemple de sortie

Pour le code de deadlock simple ci-dessus :

```
==12345== Helgrind, a thread error detector
==12345==
==12345== Thread #1: lock order "0x309018 before 0x309050" violated
==12345==    at 0x4C34F93: pthread_mutex_lock (hg_intercepts.c:802)
==12345==    by 0x108B3C: thread1_func (deadlock.c:14)
==12345==    by 0x4C34DB6: mythread_wrapper (hg_intercepts.c:389)
==12345==
==12345== Required order was established by acquisition of lock at 0x309050
==12345==    at 0x4C34F93: pthread_mutex_lock (hg_intercepts.c:802)
==12345==    by 0x108B8E: thread2_func (deadlock.c:28)
==12345==    by 0x4C34DB6: mythread_wrapper (hg_intercepts.c:389)
==12345==
==12345== followed by a later acquisition of lock at 0x309018
==12345==    at 0x4C34F93: pthread_mutex_lock (hg_intercepts.c:802)
==12345==    by 0x108BA9: thread2_func (deadlock.c:31)
==12345==    by 0x4C34DB6: mythread_wrapper (hg_intercepts.c:389)
==12345==
==12345== Lock at 0x309018 was first observed at:
==12345==    at 0x4C34F93: pthread_mutex_lock (hg_intercepts.c:802)
==12345==    by 0x108B21: thread1_func (deadlock.c:11)
```

**Interprétation** :
- "lock order violated" : Ordre de verrouillage incohérent détecté
- `mutex_A` (0x309018) et `mutex_B` (0x309050) verrouillés dans des ordres différents
- Helgrind pointe les lignes exactes du code

---

## Détection manuelle avec timeout

Si vous suspectez un deadlock mais n'avez pas accès à Helgrind, utilisez des timeouts.

### Méthode 1 : pthread_mutex_timedlock

```c
#include <time.h>
#include <errno.h>

int safe_lock_with_timeout(pthread_mutex_t *mutex, int seconds) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += seconds;

    int result = pthread_mutex_timedlock(mutex, &ts);
    if (result == ETIMEDOUT) {
        fprintf(stderr, "⚠️ Timeout trying to acquire lock! Possible deadlock.\n");
        return -1;
    }
    return 0;
}

void* thread_func(void* arg) {
    if (safe_lock_with_timeout(&mutex_A, 5) != 0) {
        return NULL;  // Abandon si timeout
    }

    if (safe_lock_with_timeout(&mutex_B, 5) != 0) {
        pthread_mutex_unlock(&mutex_A);
        return NULL;
    }

    // Section critique

    pthread_mutex_unlock(&mutex_B);
    pthread_mutex_unlock(&mutex_A);
    return NULL;
}
```

### Méthode 2 : Watchdog thread

Un thread qui surveille l'activité et alerte si blocage détecté.

```c
#include <time.h>

time_t last_activity = 0;

void* watchdog(void* arg) {
    while (1) {
        sleep(10);
        time_t now = time(NULL);
        if (now - last_activity > 30) {
            fprintf(stderr, "⚠️ No activity for 30s! Possible deadlock.\n");
            // Optionnel : générer un core dump pour analyse
            abort();
        }
    }
    return NULL;
}

void update_activity() {
    last_activity = time(NULL);
}
```

---

## Investigation avec GDB

Lorsqu'un processus est bloqué en production, GDB peut vous aider à diagnostiquer le deadlock.

### Attacher GDB à un processus vivant

```bash
# Trouver le PID du processus bloqué
ps aux | grep myapp

# Attacher GDB
sudo gdb -p 12345
```

### Commandes essentielles

```gdb
# 1. Voir tous les threads
(gdb) info threads
  Id   Target Id         Frame
* 1    Thread 0x7f... (LWP 12345) 0x7f... in __pthread_mutex_lock
  2    Thread 0x7f... (LWP 12346) 0x7f... in __pthread_mutex_lock
  3    Thread 0x7f... (LWP 12347) 0x7f... in sem_wait

# 2. Examiner chaque thread bloqué
(gdb) thread 1
(gdb) bt
#0  __pthread_mutex_lock () at pthread_mutex_lock.c:81
#1  0x0000555555555234 in thread1_func (arg=0x0) at deadlock.c:14
#2  0x00007ffff7bb76ba in start_thread () from /lib/x86_64-linux-gnu/libpthread.so.0

# 3. Voir sur quel mutex le thread est bloqué
(gdb) frame 1
(gdb) print mutex_B
$1 = {__data = {__lock = 1, __owner = 12346, ...}}

# Le mutex est détenu par le thread 12346 (LWP)

# 4. Vérifier le thread 2
(gdb) thread 2
(gdb) bt
#0  __pthread_mutex_lock () at pthread_mutex_lock.c:81
#1  0x0000555555555289 in thread2_func (arg=0x0) at deadlock.c:31

(gdb) frame 1
(gdb) print mutex_A
$2 = {__data = {__lock = 1, __owner = 12345, ...}}

# Le mutex est détenu par le thread 12345
```

**Conclusion** :
- Thread 1 (12345) détient `mutex_A`, attend `mutex_B`
- Thread 2 (12346) détient `mutex_B`, attend `mutex_A`
- **Deadlock confirmé !**

### Script GDB pour automatiser l'analyse

Créez un fichier `detect_deadlock.gdb` :

```gdb
# detect_deadlock.gdb
set pagination off

echo ===== THREAD INFORMATION =====\n
info threads

echo \n===== BACKTRACES =====\n
thread apply all bt

echo \n===== MUTEX OWNERS =====\n
# Script personnalisé pour afficher les propriétaires de mutex
# (Nécessite adaptation selon votre code)

quit
```

Utilisation :
```bash
gdb -batch -x detect_deadlock.gdb -p 12345 > deadlock_report.txt
```

---

## Cas pratiques d'investigation

### Cas 1 : Deadlock par ordre inversé (Classic)

**Symptôme** : Application figée après quelques heures de fonctionnement.

**Code problématique** :

```c
typedef struct {
    pthread_mutex_t lock;
    int balance;
} Account;

Account account_A = {PTHREAD_MUTEX_INITIALIZER, 1000};
Account account_B = {PTHREAD_MUTEX_INITIALIZER, 2000};

void transfer(Account *from, Account *to, int amount) {
    pthread_mutex_lock(&from->lock);
    pthread_mutex_lock(&to->lock);

    from->balance -= amount;
    to->balance += amount;

    pthread_mutex_unlock(&to->lock);
    pthread_mutex_unlock(&from->lock);
}

// Thread 1: transfer(&account_A, &account_B, 100);
// Thread 2: transfer(&account_B, &account_A, 50);
// ❌ DEADLOCK si les deux s'exécutent simultanément
```

**Diagnostic avec Helgrind** :

```bash
$ valgrind --tool=helgrind ./bank
Thread #1: lock order "0x309018 before 0x309050" violated
```

**Solution 1** : Ordre global de verrouillage

```c
void transfer(Account *from, Account *to, int amount) {
    // Toujours locker dans l'ordre d'adresse mémoire
    Account *first = (from < to) ? from : to;
    Account *second = (from < to) ? to : from;

    pthread_mutex_lock(&first->lock);
    pthread_mutex_lock(&second->lock);

    from->balance -= amount;
    to->balance += amount;

    pthread_mutex_unlock(&second->lock);
    pthread_mutex_unlock(&first->lock);
}
```

**Solution 2** : Un seul mutex global

```c
pthread_mutex_t transfer_lock = PTHREAD_MUTEX_INITIALIZER;

void transfer(Account *from, Account *to, int amount) {
    pthread_mutex_lock(&transfer_lock);

    from->balance -= amount;
    to->balance += amount;

    pthread_mutex_unlock(&transfer_lock);
}
```

### Cas 2 : Deadlock avec callbacks

**Symptôme** : Blocage lors de l'appel d'une callback depuis une section critique.

**Code problématique** :

```c
typedef void (*Callback)(void);

pthread_mutex_t data_lock = PTHREAD_MUTEX_INITIALIZER;
Callback notify_callback = NULL;

void set_data(int value) {
    pthread_mutex_lock(&data_lock);
    // ... mise à jour data ...

    if (notify_callback) {
        notify_callback();  // ❌ Callback peut essayer de locker data_lock
    }

    pthread_mutex_unlock(&data_lock);
}

void my_callback() {
    pthread_mutex_lock(&data_lock);  // ❌ DEADLOCK si appelé depuis set_data
    // ... accès data ...
    pthread_mutex_unlock(&data_lock);
}

int main() {
    notify_callback = my_callback;
    set_data(42);  // ❌ Deadlock
    return 0;
}
```

**Solution 1** : Mutex récursif

```c
pthread_mutexattr_t attr;
pthread_mutexattr_init(&attr);
pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
pthread_mutex_init(&data_lock, &attr);

// Maintenant le même thread peut locker plusieurs fois
```

**Solution 2** : Appeler callback hors section critique

```c
void set_data(int value) {
    pthread_mutex_lock(&data_lock);
    // ... mise à jour data ...
    Callback cb = notify_callback;  // Copie locale
    pthread_mutex_unlock(&data_lock);

    if (cb) {
        cb();  // ✅ Appelé hors lock
    }
}
```

### Cas 3 : Deadlock avec condition variable

**Symptôme** : Un thread consumer ne progresse jamais.

**Code problématique** :

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int ready = 0;

void* producer(void* arg) {
    sleep(1);
    ready = 1;
    pthread_cond_signal(&cond);  // ❌ Signal sans tenir le mutex
    return NULL;
}

void* consumer(void* arg) {
    pthread_mutex_lock(&mutex);
    while (!ready) {
        pthread_cond_wait(&cond, &mutex);  // ❌ Peut rater le signal
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}
```

**Problème** : Le signal peut être envoyé AVANT que le consumer n'entre dans `pthread_cond_wait`. Le signal est alors perdu et le consumer attend indéfiniment.

**Solution** : Signaler sous mutex

```c
void* producer(void* arg) {
    sleep(1);

    pthread_mutex_lock(&mutex);   // ✅ Locker avant signal
    ready = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    return NULL;
}
```

### Cas 4 : Deadlock dans des bibliothèques tierces

**Symptôme** : Deadlock dans du code que vous ne contrôlez pas (malloc, printf, etc.).

**Exemple** :

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void log_message(const char *msg) {
    pthread_mutex_lock(&mutex);
    printf("%s\n", msg);  // ❌ printf peut locker un mutex interne
    pthread_mutex_unlock(&mutex);
}

void signal_handler(int sig) {
    log_message("Signal received");  // ❌ DEADLOCK si signal arrive pendant log_message
}
```

**Problème** : Si le signal interrompt `printf` qui détient déjà son mutex interne, le handler tente de re-locker ce mutex → deadlock.

**Solution** : Ne jamais appeler de fonctions non async-signal-safe dans un signal handler

```c
// Utiliser write() au lieu de printf() dans les handlers
void signal_handler(int sig) {
    const char msg[] = "Signal received\n";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);  // ✅ Async-signal-safe
}
```

---

## Solutions et prévention

### 1. Ordre global de verrouillage

**Principe** : Toujours acquérir les mutex dans le même ordre, peu importe le thread.

```c
// ❌ MAL : Ordre variable
void transfer_bad(Account *from, Account *to, int amount) {
    pthread_mutex_lock(&from->lock);
    pthread_mutex_lock(&to->lock);
    // ...
}

// ✅ BIEN : Ordre fixe (par adresse mémoire)
void transfer_good(Account *from, Account *to, int amount) {
    Account *first = (from < to) ? from : to;
    Account *second = (from < to) ? to : from;

    pthread_mutex_lock(&first->lock);
    pthread_mutex_lock(&second->lock);
    // ...
    pthread_mutex_unlock(&second->lock);
    pthread_mutex_unlock(&first->lock);
}
```

### 2. Timeout et retry

Abandonner l'acquisition si elle prend trop de temps.

```c
int try_transfer(Account *from, Account *to, int amount) {
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 1;  // Timeout de 1 seconde

    if (pthread_mutex_timedlock(&from->lock, &timeout) != 0) {
        return -1;  // Échec
    }

    if (pthread_mutex_timedlock(&to->lock, &timeout) != 0) {
        pthread_mutex_unlock(&from->lock);
        return -1;  // Échec
    }

    // Transfert
    from->balance -= amount;
    to->balance += amount;

    pthread_mutex_unlock(&to->lock);
    pthread_mutex_unlock(&from->lock);
    return 0;  // Succès
}

// Appel avec retry
void transfer_with_retry(Account *from, Account *to, int amount) {
    int retries = 0;
    while (try_transfer(from, to, amount) != 0) {
        if (++retries > 10) {
            fprintf(stderr, "Transfer failed after 10 retries\n");
            return;
        }
        usleep(1000);  // Attendre 1ms avant de réessayer
    }
}
```

### 3. Lock-free data structures

Éviter complètement les mutex avec des structures lock-free.

```c
#include <stdatomic.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

typedef struct {
    atomic_uintptr_t head;
} LockFreeStack;

void push(LockFreeStack *stack, Node *node) {
    Node *old_head;
    do {
        old_head = (Node*)atomic_load(&stack->head);
        node->next = old_head;
    } while (!atomic_compare_exchange_weak(&stack->head,
                                           (uintptr_t*)&old_head,
                                           (uintptr_t)node));
}

// Pas de mutex = pas de deadlock possible
```

### 4. Reduce lock scope

Minimiser le code exécuté sous lock.

```c
// ❌ MAL : Lock trop large
pthread_mutex_lock(&mutex);
int value = shared_data;
int result = expensive_function(value);  // Calcul long sous lock
shared_data = result;
pthread_mutex_unlock(&mutex);

// ✅ BIEN : Lock minimal
pthread_mutex_lock(&mutex);
int value = shared_data;
pthread_mutex_unlock(&mutex);

int result = expensive_function(value);  // Calcul hors lock

pthread_mutex_lock(&mutex);
shared_data = result;
pthread_mutex_unlock(&mutex);
```

### 5. Reader-Writer Locks

Permettre plusieurs lecteurs simultanés.

```c
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void read_data() {
    pthread_rwlock_rdlock(&rwlock);  // Lecture partagée
    // ... lecture ...
    pthread_rwlock_unlock(&rwlock);
}

void write_data() {
    pthread_rwlock_wrlock(&rwlock);  // Écriture exclusive
    // ... écriture ...
    pthread_rwlock_unlock(&rwlock);
}
```

### 6. Avoid nested locks

Si possible, ne jamais avoir besoin de plusieurs mutex simultanément.

```c
// ❌ MAL : Locks imbriqués
void operation() {
    pthread_mutex_lock(&mutex_A);
    pthread_mutex_lock(&mutex_B);
    // ...
    pthread_mutex_unlock(&mutex_B);
    pthread_mutex_unlock(&mutex_A);
}

// ✅ MIEUX : Redesign pour éviter les locks imbriqués
void operation() {
    pthread_mutex_lock(&global_operation_lock);
    // Accès atomique à toutes les ressources
    pthread_mutex_unlock(&global_operation_lock);
}
```

---

## Outils avancés

### 1. Deadlock detection runtime (libdispatch)

Sur macOS/iOS, la libdispatch détecte automatiquement les deadlocks.

```c
#include <dispatch/dispatch.h>

dispatch_queue_t queue = dispatch_queue_create("com.example.queue", NULL);

dispatch_sync(queue, ^{
    dispatch_sync(queue, ^{
        // ❌ DEADLOCK détecté et rapporté automatiquement
    });
});
```

### 2. Lock order checker (Google's ThreadSanitizer)

TSan peut détecter les violations d'ordre même sans deadlock réel.

```bash
gcc -g -fsanitize=thread -o myapp myapp.c -lpthread
./myapp

# TSan avertit sur les lock order violations
# même si le deadlock ne se produit pas cette fois
```

### 3. Static analysis (Clang Thread Safety Analysis)

Annotations pour vérifier statiquement l'absence de deadlock.

```c
pthread_mutex_t mutex_A __attribute__((capability("mutex")));
pthread_mutex_t mutex_B __attribute__((capability("mutex")));

void function_A() __attribute__((requires_capability(mutex_A)))
                  __attribute__((requires_capability(mutex_B))) {
    // Compilateur vérifie que les deux mutex sont tenus
}
```

### 4. Runtime monitoring

Instrumenter le code pour tracer les acquisitions de locks.

```c
#define DEBUG_LOCKS

#ifdef DEBUG_LOCKS
#define LOCK(m) do { \
    printf("[%lu] Locking %s at %s:%d\n", pthread_self(), #m, __FILE__, __LINE__); \
    pthread_mutex_lock(m); \
} while(0)

#define UNLOCK(m) do { \
    pthread_mutex_unlock(m); \
    printf("[%lu] Unlocked %s at %s:%d\n", pthread_self(), #m, __FILE__, __LINE__); \
} while(0)
#else
#define LOCK(m) pthread_mutex_lock(m)
#define UNLOCK(m) pthread_mutex_unlock(m)
#endif

// Utilisation
LOCK(&mutex_A);
LOCK(&mutex_B);
// ...
UNLOCK(&mutex_B);
UNLOCK(&mutex_A);
```

---

## Testing pour deadlocks

### 1. Stress testing

Augmenter la probabilité de deadlock avec beaucoup de threads.

```c
#define NUM_THREADS 1000
#define NUM_ITERATIONS 10000

void* stress_test(void* arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        transfer(&account_A, &account_B, rand() % 100);
        transfer(&account_B, &account_A, rand() % 100);
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, stress_test, NULL);
    }

    // Si deadlock, programme ne terminera jamais
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("✅ No deadlock detected\n");
    return 0;
}
```

### 2. Chaos engineering

Injecter des délais aléatoires pour forcer des interleaving rares.

```c
#include <stdlib.h>
#include <unistd.h>

void chaos_delay() {
    if (rand() % 100 < 10) {  // 10% de chance
        usleep(rand() % 1000);  // Délai 0-1ms
    }
}

void critical_operation() {
    pthread_mutex_lock(&mutex_A);
    chaos_delay();  // Augmente les chances d'interleaving

    pthread_mutex_lock(&mutex_B);
    chaos_delay();

    // ... opération ...

    pthread_mutex_unlock(&mutex_B);
    pthread_mutex_unlock(&mutex_A);
}
```

### 3. Automated testing avec timeout

Détecter automatiquement les deadlocks dans les tests.

```bash
#!/bin/bash
# test_deadlock.sh

timeout 10s ./myapp &
PID=$!

sleep 10
if ps -p $PID > /dev/null; then
    echo "❌ DEADLOCK DETECTED: Process still running after 10s"
    kill -9 $PID
    exit 1
else
    echo "✅ No deadlock"
    exit 0
fi
```

---

## Debugging en production

### Stratégie quand le serveur est figé

#### 1. Capturer l'état avant de tuer le processus

```bash
# Générer un core dump sans tuer le processus
sudo gcore -o /tmp/deadlock 12345

# Analyser plus tard
gdb ./myapp /tmp/deadlock.12345
```

#### 2. Tracer les appels système

```bash
# Voir ce que font tous les threads
sudo strace -p 12345 -f

# Si tous les threads sont en futex_wait, c'est un deadlock
```

#### 3. Logs de locks (si instrumenté)

Si vous avez ajouté du logging sur les locks :

```bash
# Trouver le dernier lock acquis par chaque thread
grep "Locking" /var/log/myapp.log | tail -100

# Exemple de sortie:
# [Thread 12345] Locking mutex_A at main.c:45
# [Thread 12346] Locking mutex_B at worker.c:23
# [Thread 12345] Locking mutex_B at main.c:47  ← Bloqué ici
# [Thread 12346] Locking mutex_A at worker.c:25  ← Bloqué ici
```

---

## Cas d'étude : Deadlock intermittent en production

### Contexte

Un service REST multi-threaded se fige aléatoirement une fois par semaine en production. Impossible de reproduire en développement.

### Investigation

**Étape 1** : Capturer un core dump lors du prochain blocage.

Script de monitoring :
```bash
#!/bin/bash
# monitor_deadlock.sh

while true; do
    # Vérifier si le processus répond
    timeout 5s curl http://localhost:8080/health > /dev/null 2>&1

    if [ $? -ne 0 ]; then
        echo "⚠️ Service not responding! Capturing core dump..."
        PID=$(pidof myservice)
        sudo gcore -o /tmp/deadlock $PID

        # Redémarrer le service
        sudo systemctl restart myservice

        echo "Core dump saved to /tmp/deadlock.$PID"
        exit 0
    fi

    sleep 30
done
```

**Étape 2** : Analyser le core dump avec GDB.

```gdb
$ gdb ./myservice /tmp/deadlock.12345
(gdb) info threads
  Id   Target Id         Frame
* 1    Thread 0x7f8... (LWP 12345) pthread_mutex_lock at mutex.c:81
  2    Thread 0x7f9... (LWP 12346) pthread_mutex_lock at mutex.c:81
  3    Thread 0x7fa... (LWP 12347) epoll_wait at epoll.c:123
  4    Thread 0x7fb... (LWP 12348) pthread_cond_wait at cond.c:45

(gdb) thread 1
(gdb) bt
#0  pthread_mutex_lock () at mutex.c:81
#1  0x555555556234 in handle_request (req=0x...) at handler.c:123
#2  0x555555556445 in worker_thread (arg=0x...) at server.c:67

(gdb) print *(pthread_mutex_t*)$rdi
$1 = {__data = {__owner = 12346, ...}}

(gdb) thread 2
(gdb) bt
#0  pthread_mutex_lock () at mutex.c:81
#1  0x555555556389 in get_cached_data () at cache.c:45
#2  0x555555556278 in handle_request (req=0x...) at handler.c:134

(gdb) print *(pthread_mutex_t*)$rdi
$2 = {__data = {__owner = 12345, ...}}
```

**Diagnostic** :
- Thread 1 détient `cache_lock`, attend `request_lock`
- Thread 2 détient `request_lock`, attend `cache_lock`
- **Deadlock confirmé !**

**Étape 3** : Examiner le code.

```c
// handler.c
void handle_request(Request *req) {
    pthread_mutex_lock(&request_lock);
    // ... traitement ...
    Data *data = get_cached_data();  // ❌ Appelle cache.c qui lock cache_lock
    // ...
    pthread_mutex_unlock(&request_lock);
}

// cache.c
Data* get_cached_data() {
    pthread_mutex_lock(&cache_lock);
    // ... dans certains cas, invalide le cache ...
    if (need_refresh) {
        refresh_cache();  // ❌ Appelle handler.c qui lock request_lock
    }
    pthread_mutex_unlock(&cache_lock);
}
```

**Solution** : Refactoriser pour éliminer la dépendance circulaire.

```c
// handler.c
void handle_request(Request *req) {
    pthread_mutex_lock(&request_lock);
    bool need_refresh = check_if_refresh_needed();
    pthread_mutex_unlock(&request_lock);

    if (need_refresh) {
        refresh_cache();  // ✅ Appelé hors lock
    }

    pthread_mutex_lock(&request_lock);
    Data *data = get_cached_data_nolock();  // Version sans lock
    // ...
    pthread_mutex_unlock(&request_lock);
}
```

**Étape 4** : Validation avec stress tests.

```bash
# Compiler avec Helgrind
gcc -g -o myservice myservice.c -lpthread

# Lancer sous Helgrind avec charge
valgrind --tool=helgrind ./myservice &
./load_test.sh  # Envoie 10000 requêtes simultanées

# Résultat : Aucune violation détectée ✅
```

---

## Checklist de prévention

### Au design

- [ ] Minimiser le nombre de mutex nécessaires
- [ ] Documenter l'ordre de verrouillage
- [ ] Éviter les appels de fonctions sous lock qui peuvent re-locker
- [ ] Préférer les structures lock-free quand possible

### À l'implémentation

- [ ] Toujours acquérir plusieurs mutex dans le même ordre
- [ ] Utiliser des mutex récursifs si nécessaire
- [ ] Libérer les locks dans l'ordre inverse de l'acquisition
- [ ] Timeout sur les acquisitions critiques

### Au testing

- [ ] Tester avec Helgrind sur tous les chemins d'exécution
- [ ] Stress tests avec beaucoup de threads
- [ ] Tests avec délais aléatoires (chaos engineering)
- [ ] Scripts de détection automatique avec timeout

### En production

- [ ] Monitoring de la responsivité du service
- [ ] Core dumps automatiques en cas de blocage
- [ ] Logs détaillés des acquisitions de locks (si overhead acceptable)
- [ ] Mécanisme de redémarrage automatique

---

## Résumé

Les deadlocks sont des bugs critiques mais évitables. Points essentiels :

### Détection
- **Helgrind** : Outil de référence pour détecter les deadlocks potentiels
- **GDB** : Investigation de processus bloqués en production
- **Timeout** : Détection manuelle avec `pthread_mutex_timedlock`

### Causes courantes
- Ordre de verrouillage inversé entre threads
- Callbacks qui re-lockent le même mutex
- Signaux non-thread-safe dans les condition variables
- Appels à des fonctions externes sous lock

### Solutions
- **Ordre global** : Toujours locker dans le même ordre
- **Timeout et retry** : Abandonner et réessayer
- **Lock-free** : Éviter complètement les mutex
- **Simplification** : Réduire le nombre de mutex

### Prévention
- Design: minimiser le partage et les locks imbriqués
- Testing: Helgrind + stress tests systématiques
- Code review: vérifier l'ordre de verrouillage
- Monitoring: détecter rapidement les blocages en production

**La règle d'or** : Si vous devez acquérir plusieurs mutex, définissez et respectez un ordre global strict. Aucune exception.

**En cas de doute** : Préférez un seul mutex global plutôt que plusieurs mutex avec des risques de deadlock.

---


⏭️ [Performance debugging](/35-debugging-code-complexe/05-performance-debugging.md)
