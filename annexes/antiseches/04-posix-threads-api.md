🔝 Retour au [Sommaire](/SOMMAIRE.md)

# B.4 POSIX Threads API - Antisèche

## 📌 Introduction

L'API POSIX threads (pthread) permet de créer et gérer des threads sous Linux. Un **thread** est un flux d'exécution léger au sein d'un processus qui partage la même mémoire que les autres threads du processus.

### Pourquoi utiliser les threads ?
- ✅ Exécution parallèle sur processeurs multi-cœurs
- ✅ Amélioration de la réactivité des applications
- ✅ Partage simple de données (mémoire commune)

### Compilation
Pour utiliser pthread, vous devez lier la bibliothèque avec `-pthread` :
```bash
gcc -pthread mon_programme.c -o mon_programme
```

### En-tête requis
```c
#include <pthread.h>
```

---

## 🧵 Gestion des Threads

### Création de thread

#### `pthread_create()`
Crée un nouveau thread qui exécute une fonction.

```c
int pthread_create(pthread_t *thread,
                   const pthread_attr_t *attr,
                   void *(*start_routine)(void*),
                   void *arg);
```

**Paramètres :**
- `thread` : Pointeur vers l'identifiant du thread créé
- `attr` : Attributs du thread (ou `NULL` pour valeurs par défaut)
- `start_routine` : Fonction que le thread va exécuter
- `arg` : Argument passé à la fonction

**Retour :** `0` en cas de succès, code d'erreur sinon

**Exemple :**
```c
void *ma_fonction(void *arg) {
    int id = *(int*)arg;
    printf("Thread %d en cours\n", id);
    return NULL;
}

int main() {
    pthread_t thread;
    int id = 1;

    if (pthread_create(&thread, NULL, ma_fonction, &id) != 0) {
        perror("Erreur création thread");
        return 1;
    }

    pthread_join(thread, NULL);  // Attendre la fin du thread
    return 0;
}
```

---

### Terminaison de thread

#### `pthread_exit()`
Termine le thread appelant.

```c
void pthread_exit(void *retval);
```

**Paramètres :**
- `retval` : Valeur de retour du thread (récupérable via `pthread_join`)

**Exemple :**
```c
void *ma_fonction(void *arg) {
    int resultat = 42;
    pthread_exit(&resultat);  // Termine le thread avec une valeur
}
```

---

#### `pthread_join()`
Attend la terminaison d'un thread spécifique (thread "joinable").

```c
int pthread_join(pthread_t thread, void **retval);
```

**Paramètres :**
- `thread` : Identifiant du thread à attendre
- `retval` : Pointeur pour récupérer la valeur de retour (ou `NULL`)

**Retour :** `0` en cas de succès, code d'erreur sinon

**Exemple :**
```c
pthread_t thread;
void *resultat;

pthread_create(&thread, NULL, ma_fonction, NULL);
pthread_join(thread, &resultat);  // Bloque jusqu'à la fin du thread
printf("Thread terminé avec résultat : %d\n", *(int*)resultat);
```

---

#### `pthread_detach()`
Détache un thread pour qu'il libère automatiquement ses ressources à sa terminaison.

```c
int pthread_detach(pthread_t thread);
```

**Paramètres :**
- `thread` : Identifiant du thread à détacher

**Retour :** `0` en cas de succès, code d'erreur sinon

⚠️ **Important :** Un thread détaché ne peut plus être "joint" avec `pthread_join()`.

**Exemple :**
```c
pthread_t thread;
pthread_create(&thread, NULL, ma_fonction, NULL);
pthread_detach(thread);  // Le thread se nettoie tout seul à la fin
```

---

#### `pthread_self()`
Retourne l'identifiant du thread appelant.

```c
pthread_t pthread_self(void);
```

**Exemple :**
```c
pthread_t mon_id = pthread_self();
printf("Mon ID de thread : %lu\n", (unsigned long)mon_id);
```

---

## 🔒 Synchronisation - Mutex

Un **mutex** (mutual exclusion) protège une section critique du code pour éviter que plusieurs threads y accèdent simultanément.

### Initialisation de mutex

#### Statique
```c
pthread_mutex_t mon_mutex = PTHREAD_MUTEX_INITIALIZER;
```

#### Dynamique avec `pthread_mutex_init()`
```c
int pthread_mutex_init(pthread_mutex_t *mutex,
                       const pthread_mutexattr_t *attr);
```

**Paramètres :**
- `mutex` : Pointeur vers le mutex à initialiser
- `attr` : Attributs du mutex (ou `NULL` pour valeurs par défaut)

**Exemple :**
```c
pthread_mutex_t mutex;
pthread_mutex_init(&mutex, NULL);
```

---

### Verrouillage et déverrouillage

#### `pthread_mutex_lock()`
Verrouille un mutex (bloque si déjà verrouillé).

```c
int pthread_mutex_lock(pthread_mutex_t *mutex);
```

**Exemple :**
```c
pthread_mutex_lock(&mutex);
// Section critique : un seul thread à la fois
compteur++;
pthread_mutex_unlock(&mutex);
```

---

#### `pthread_mutex_trylock()`
Tente de verrouiller un mutex sans bloquer.

```c
int pthread_mutex_trylock(pthread_mutex_t *mutex);
```

**Retour :**
- `0` si verrouillage réussi
- `EBUSY` si déjà verrouillé

**Exemple :**
```c
if (pthread_mutex_trylock(&mutex) == 0) {
    // Mutex acquis
    compteur++;
    pthread_mutex_unlock(&mutex);
} else {
    // Mutex déjà pris, faire autre chose
}
```

---

#### `pthread_mutex_unlock()`
Déverrouille un mutex.

```c
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```

---

#### `pthread_mutex_destroy()`
Détruit un mutex (libère les ressources).

```c
int pthread_mutex_destroy(pthread_mutex_t *mutex);
```

**Exemple complet :**
```c
pthread_mutex_t mutex;
int compteur_partage = 0;

void *incrementer(void *arg) {
    for (int i = 0; i < 1000; i++) {
        pthread_mutex_lock(&mutex);
        compteur_partage++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_mutex_init(&mutex, NULL);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, incrementer, NULL);
    pthread_create(&t2, NULL, incrementer, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Compteur final : %d\n", compteur_partage);  // Devrait être 2000

    pthread_mutex_destroy(&mutex);
    return 0;
}
```

---

## 🚦 Synchronisation - Variables de Condition

Les **variables de condition** permettent à un thread d'attendre qu'une condition devienne vraie.

### Initialisation

#### Statique
```c
pthread_cond_t ma_condition = PTHREAD_COND_INITIALIZER;
```

#### Dynamique avec `pthread_cond_init()`
```c
int pthread_cond_init(pthread_cond_t *cond,
                      const pthread_condattr_t *attr);
```

---

### Opérations

#### `pthread_cond_wait()`
Attend qu'une condition soit signalée (déverrouille le mutex pendant l'attente).

```c
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
```

**Paramètres :**
- `cond` : Variable de condition
- `mutex` : Mutex associé (doit être verrouillé avant l'appel)

**Exemple :**
```c
pthread_mutex_lock(&mutex);
while (!condition_verifiee) {
    pthread_cond_wait(&cond, &mutex);  // Attend le signal
}
// La condition est maintenant vraie
pthread_mutex_unlock(&mutex);
```

---

#### `pthread_cond_signal()`
Réveille **un** thread en attente sur la condition.

```c
int pthread_cond_signal(pthread_cond_t *cond);
```

**Exemple :**
```c
pthread_mutex_lock(&mutex);
condition_verifiee = 1;
pthread_cond_signal(&cond);  // Réveille un thread
pthread_mutex_unlock(&mutex);
```

---

#### `pthread_cond_broadcast()`
Réveille **tous** les threads en attente sur la condition.

```c
int pthread_cond_broadcast(pthread_cond_t *cond);
```

**Exemple :**
```c
pthread_mutex_lock(&mutex);
condition_verifiee = 1;
pthread_cond_broadcast(&cond);  // Réveille tous les threads
pthread_mutex_unlock(&mutex);
```

---

#### `pthread_cond_destroy()`
Détruit une variable de condition.

```c
int pthread_cond_destroy(pthread_cond_t *cond);
```

**Exemple complet (producteur-consommateur) :**
```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int buffer = 0;
int buffer_plein = 0;

void *producteur(void *arg) {
    pthread_mutex_lock(&mutex);
    buffer = 42;
    buffer_plein = 1;
    pthread_cond_signal(&cond);  // Signale que le buffer est prêt
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *consommateur(void *arg) {
    pthread_mutex_lock(&mutex);
    while (!buffer_plein) {
        pthread_cond_wait(&cond, &mutex);  // Attend les données
    }
    printf("Données reçues : %d\n", buffer);
    pthread_mutex_unlock(&mutex);
    return NULL;
}
```

---

## 🎯 Sémaphores POSIX

Les **sémaphores** sont des compteurs pour contrôler l'accès à des ressources.

### En-tête
```c
#include <semaphore.h>
```

### Initialisation

#### `sem_init()`
Initialise un sémaphore.

```c
int sem_init(sem_t *sem, int pshared, unsigned int value);
```

**Paramètres :**
- `sem` : Pointeur vers le sémaphore
- `pshared` : `0` pour threads d'un même processus, `non-0` pour inter-processus
- `value` : Valeur initiale du compteur

**Exemple :**
```c
sem_t semaphore;
sem_init(&semaphore, 0, 1);  // Initialise à 1 (équivalent mutex)
```

---

### Opérations

#### `sem_wait()`
Décrémente le sémaphore (bloque si valeur = 0).

```c
int sem_wait(sem_t *sem);
```

**Exemple :**
```c
sem_wait(&semaphore);  // Acquiert la ressource (ou attend)
// Section critique
sem_post(&semaphore);  // Libère la ressource
```

---

#### `sem_trywait()`
Tente de décrémenter sans bloquer.

```c
int sem_trywait(sem_t *sem);
```

**Retour :** `0` si succès, `-1` et `errno=EAGAIN` si sémaphore à 0

---

#### `sem_post()`
Incrémente le sémaphore (libère une ressource).

```c
int sem_post(sem_t *sem);
```

---

#### `sem_destroy()`
Détruit un sémaphore.

```c
int sem_destroy(sem_t *sem);
```

**Exemple complet :**
```c
#include <semaphore.h>

sem_t sem;

void *worker(void *arg) {
    sem_wait(&sem);  // Acquiert le sémaphore
    printf("Thread %d travaille\n", *(int*)arg);
    sleep(1);
    sem_post(&sem);  // Libère le sémaphore
    return NULL;
}

int main() {
    sem_init(&sem, 0, 2);  // Max 2 threads simultanés

    pthread_t threads[5];
    int ids[5] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++) {
        pthread_create(&threads[i], NULL, worker, &ids[i]);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&sem);
    return 0;
}
```

---

## 🔐 Read-Write Locks (Verrous Lecture-Écriture)

Permettent plusieurs lecteurs simultanés **ou** un seul écrivain.

### Initialisation

#### Statique
```c
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
```

#### Dynamique
```c
int pthread_rwlock_init(pthread_rwlock_t *rwlock,
                        const pthread_rwlockattr_t *attr);
```

---

### Opérations

#### `pthread_rwlock_rdlock()`
Acquiert un verrou en **lecture** (partagé).

```c
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
```

---

#### `pthread_rwlock_wrlock()`
Acquiert un verrou en **écriture** (exclusif).

```c
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
```

---

#### `pthread_rwlock_unlock()`
Libère le verrou (lecture ou écriture).

```c
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
```

---

#### `pthread_rwlock_destroy()`
Détruit un verrou lecture-écriture.

```c
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
```

**Exemple :**
```c
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
int donnee_partagee = 0;

void *lecteur(void *arg) {
    pthread_rwlock_rdlock(&rwlock);  // Plusieurs lecteurs OK
    printf("Lecture : %d\n", donnee_partagee);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

void *ecrivain(void *arg) {
    pthread_rwlock_wrlock(&rwlock);  // Exclusif
    donnee_partagee++;
    printf("Écriture : %d\n", donnee_partagee);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}
```

---

## 🧩 Thread-Local Storage (TLS)

Variables propres à chaque thread (non partagées).

### Déclaration avec `__thread`
```c
__thread int variable_locale_thread = 0;

void *fonction(void *arg) {
    variable_locale_thread = *(int*)arg;  // Chaque thread a sa propre copie
    printf("Thread %lu : %d\n", pthread_self(), variable_locale_thread);
    return NULL;
}
```

---

## 📊 Tableau récapitulatif

| **Fonction** | **Usage** | **Bloquant ?** |
|--------------|-----------|----------------|
| `pthread_create()` | Crée un thread | Non |
| `pthread_join()` | Attend un thread | Oui |
| `pthread_detach()` | Détache un thread | Non |
| `pthread_exit()` | Termine le thread courant | - |
| `pthread_mutex_lock()` | Verrouille mutex | Oui |
| `pthread_mutex_trylock()` | Tente verrouillage | Non |
| `pthread_mutex_unlock()` | Déverrouille mutex | Non |
| `pthread_cond_wait()` | Attend condition | Oui |
| `pthread_cond_signal()` | Signale à 1 thread | Non |
| `pthread_cond_broadcast()` | Signale à tous | Non |
| `sem_wait()` | Décrémente sémaphore | Oui |
| `sem_post()` | Incrémente sémaphore | Non |
| `pthread_rwlock_rdlock()` | Verrou lecture | Oui |
| `pthread_rwlock_wrlock()` | Verrou écriture | Oui |

---

## ⚠️ Pièges courants à éviter

### 1. Oublier de joindre ou détacher les threads
```c
// ❌ MAUVAIS : fuite de ressources
pthread_t thread;
pthread_create(&thread, NULL, fonction, NULL);
// Rien d'autre

// ✅ BON
pthread_create(&thread, NULL, fonction, NULL);
pthread_join(thread, NULL);  // OU pthread_detach(thread);
```

---

### 2. Deadlock avec les mutex
```c
// ❌ MAUVAIS : risque de deadlock
pthread_mutex_lock(&mutex1);
pthread_mutex_lock(&mutex2);  // Un autre thread peut avoir l'ordre inverse

// ✅ BON : Toujours verrouiller dans le même ordre
pthread_mutex_lock(&mutex1);
pthread_mutex_lock(&mutex2);
// ...
pthread_mutex_unlock(&mutex2);
pthread_mutex_unlock(&mutex1);
```

---

### 3. Condition sans boucle while
```c
// ❌ MAUVAIS : réveils parasites possibles
pthread_cond_wait(&cond, &mutex);

// ✅ BON : Toujours vérifier la condition dans une boucle
while (!condition_vraie) {
    pthread_cond_wait(&cond, &mutex);
}
```

---

### 4. Passer des variables locales aux threads
```c
// ❌ MAUVAIS : la variable locale disparaît !
void creer_thread() {
    int id = 42;
    pthread_t thread;
    pthread_create(&thread, NULL, fonction, &id);  // Danger !
    pthread_detach(thread);
}

// ✅ BON : Utiliser allocation dynamique ou variables globales
int *id = malloc(sizeof(int));
*id = 42;
pthread_create(&thread, NULL, fonction, id);
```

---

## 🎯 Bonnes pratiques

1. **Toujours vérifier les valeurs de retour**
   ```c
   if (pthread_mutex_lock(&mutex) != 0) {
       perror("Erreur lock");
       exit(1);
   }
   ```

2. **Initialiser et détruire proprement**
   - Chaque `pthread_mutex_init()` doit avoir son `pthread_mutex_destroy()`
   - Chaque `sem_init()` doit avoir son `sem_destroy()`

3. **Minimiser les sections critiques**
   - Verrouillez le moins longtemps possible
   - Évitez les appels bloquants dans les sections critiques

4. **Utiliser des outils de détection**
   - Compiler avec `-fsanitize=thread` (ThreadSanitizer)
   - Utiliser Valgrind avec Helgrind : `valgrind --tool=helgrind ./programme`

---

## 📚 Ressources complémentaires

- **Man pages essentielles** :
  ```bash
  man pthread_create
  man pthread_mutex_lock
  man sem_wait
  ```

- **Standards** : POSIX.1-2008 (IEEE Std 1003.1-2008)

- **Compilation** : N'oubliez jamais `-pthread` !

---

**💡 Astuce finale :** Les threads sont puissants mais complexes. Commencez simple avec un ou deux threads, puis augmentez progressivement la complexité. Le débogage multi-threadé est difficile, privilégiez les outils automatiques (sanitizers, Valgrind) dès le début !

⏭️ [CMake commandes principales](/annexes/antiseches/05-cmake-commandes.md)
