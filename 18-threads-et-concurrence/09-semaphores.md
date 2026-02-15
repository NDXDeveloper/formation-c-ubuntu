🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 18.9 Sémaphores POSIX

## Table des matières
- [Introduction](#introduction)
- [Qu'est-ce qu'un sémaphore ?](#quest-ce-quun-s%C3%A9maphore-)
- [Sémaphores vs Mutex](#s%C3%A9maphores-vs-mutex)
- [Types de sémaphores](#types-de-s%C3%A9maphores)
- [Sémaphores anonymes (sem_t)](#s%C3%A9maphores-anonymes-sem_t)
- [Opérations de base](#op%C3%A9rations-de-base)
- [Sémaphores binaires](#s%C3%A9maphores-binaires)
- [Sémaphores compteurs](#s%C3%A9maphores-compteurs)
- [Pattern producteur-consommateur](#pattern-producteur-consommateur)
- [Limitation de ressources concurrentes](#limitation-de-ressources-concurrentes)
- [Barrières de synchronisation](#barri%C3%A8res-de-synchronisation)
- [Sémaphores nommés](#s%C3%A9maphores-nomm%C3%A9s)
- [sem_timedwait() et sem_trywait()](#sem_timedwait-et-sem_trywait)
- [POSIX vs System V](#posix-vs-system-v)
- [Erreurs courantes](#erreurs-courantes)
- [Bonnes pratiques](#bonnes-pratiques)
- [Quand utiliser les sémaphores ?](#quand-utiliser-les-s%C3%A9maphores-)

---

## Introduction

Nous avons vu les **mutex** (exclusion mutuelle) et les **variables de condition** (attente de condition). Maintenant, découvrons les **sémaphores** - un mécanisme de synchronisation plus ancien mais toujours très utile.

**Historique** : Les sémaphores ont été inventés par Edsger Dijkstra en 1965, bien avant les mutex et variables de condition.

**Cas d'usage** : Les sémaphores sont particulièrement utiles pour :
- Limiter le nombre de threads accédant à une ressource
- Synchroniser des producteurs et consommateurs
- Implémenter des barrières de synchronisation
- Gérer des pools de ressources

Dans cette section, nous allons apprendre à :
- Comprendre le concept de sémaphore
- Utiliser les sémaphores POSIX
- Distinguer sémaphores binaires et compteurs
- Implémenter des patterns classiques

---

## Qu'est-ce qu'un sémaphore ?

### Définition

Un **sémaphore** est un compteur partagé entre threads qui contrôle l'accès à une ressource partagée.

**Analogie simple** : Imaginez une bibliothèque avec 5 salles d'étude.

- **Compteur** : Nombre de salles disponibles (initialement 5)
- **sem_wait()** : Prendre une salle (décrémenter le compteur)
  - Si compteur > 0 : Prend une salle, continue
  - Si compteur = 0 : Attend qu'une salle se libère
- **sem_post()** : Libérer une salle (incrémenter le compteur)
  - Compteur++, réveille un thread en attente

```
État initial : Compteur = 5 (5 salles libres)

Étudiant 1 arrive → sem_wait() → Compteur = 4 (prend salle 1)
Étudiant 2 arrive → sem_wait() → Compteur = 3 (prend salle 2)
Étudiant 3 arrive → sem_wait() → Compteur = 2 (prend salle 3)
Étudiant 4 arrive → sem_wait() → Compteur = 1 (prend salle 4)
Étudiant 5 arrive → sem_wait() → Compteur = 0 (prend salle 5)
Étudiant 6 arrive → sem_wait() → BLOQUÉ (aucune salle libre)

Étudiant 1 part → sem_post() → Compteur = 1 → Étudiant 6 entre
```

### Concept mathématique

Un sémaphore est défini par :
- **Valeur** : Un entier non-négatif (≥ 0)
- **Opération P** (Proberen = tester) : `sem_wait()` - Décrémenter
- **Opération V** (Verhogen = incrémenter) : `sem_post()` - Incrémenter

**Règle fondamentale** : La valeur ne peut **jamais** devenir négative. Si un thread tente de décrémenter quand la valeur est 0, il **bloque**.

### Principe de fonctionnement

```
┌─────────────────────────────────────┐
│       SÉMAPHORE (valeur = 3)        │
├─────────────────────────────────────┤
│                                     │
│  sem_wait() ────► Valeur-- (2)      │
│                   Si valeur >= 0    │
│                   → Continue        │
│                   Si valeur < 0     │
│                   → Bloque          │
│                                     │
│  sem_post() ────► Valeur++ (3)      │
│                   Réveille un       │
│                   thread bloqué     │
└─────────────────────────────────────┘
```

---

## Sémaphores vs Mutex

### Différences clés

| Caractéristique | Mutex | Sémaphore |
|-----------------|-------|-----------|
| **Concept** | Verrou (lock/unlock) | Compteur |
| **Valeur** | Booléen (verrouillé/libre) | Entier ≥ 0 |
| **Propriétaire** | Le thread qui lock doit unlock | Pas de notion de propriétaire |
| **Libération** | Même thread unlock | N'importe quel thread peut post |
| **Usage principal** | Protection de données | Signalisation, comptage |
| **Accès simultanés** | 1 seul thread | N threads (si valeur = N) |

### Exemple comparatif

**Avec Mutex** :
```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_func(void *arg) {
    pthread_mutex_lock(&mutex);    // Un seul thread à la fois
    // Section critique
    pthread_mutex_unlock(&mutex);  // Doit être le même thread
    return NULL;
}
```

**Avec Sémaphore** :
```c
sem_t semaphore;  
sem_init(&semaphore, 0, 3);  // 3 threads simultanés max  

void *thread_func(void *arg) {
    sem_wait(&semaphore);  // Décrémenter (max 3 en même temps)
    // Section critique (jusqu'à 3 threads)
    sem_post(&semaphore);  // Incrémenter (n'importe quel thread)
    return NULL;
}
```

### Quand utiliser quoi ?

**Utilisez un Mutex quand** :
- Vous protégez des données partagées
- Un seul thread à la fois doit accéder
- Besoin de garanties de propriétaire

**Utilisez un Sémaphore quand** :
- Vous comptez des ressources
- Plusieurs threads peuvent accéder simultanément
- Signalisation entre threads (producteur/consommateur)

---

## Types de sémaphores

Il existe **deux types** de sémaphores POSIX :

### 1. Sémaphores anonymes (sem_t)

**Caractéristiques** :
- Partagés entre threads d'un **même processus**
- Ou entre processus (via mémoire partagée)
- Stockés dans une variable `sem_t`

**Utilisation** : Synchronisation intra-processus (le plus courant)

### 2. Sémaphores nommés

**Caractéristiques** :
- Identifiés par un **nom** (comme un fichier)
- Partagés entre **plusieurs processus** indépendants
- Persistent même après fermeture du programme

**Utilisation** : Communication inter-processus (IPC)

**Dans cette section**, nous nous concentrerons principalement sur les **sémaphores anonymes**, les plus utilisés pour le multithreading.

---

## Sémaphores anonymes (sem_t)

### Déclaration

```c
#include <semaphore.h>

sem_t mon_semaphore;
```

### Initialisation

```c
int sem_init(sem_t *sem, int pshared, unsigned int value);
```

**Paramètres** :
- `sem` : Pointeur vers le sémaphore à initialiser
- `pshared` :
  - `0` : Partagé entre threads du **même processus** (le plus courant)
  - `non-zéro` : Partagé entre **processus** (nécessite mémoire partagée)
- `value` : Valeur initiale du sémaphore (≥ 0)

**Retour** :
- `0` : Succès
- `-1` : Erreur (vérifier `errno`)

**Exemple** :

```c
sem_t sem;

// Initialisation pour threads (pshared = 0)
if (sem_init(&sem, 0, 1) != 0) {
    perror("sem_init");
    exit(EXIT_FAILURE);
}
```

### Destruction

```c
int sem_destroy(sem_t *sem);
```

**Règles** :
- Obligatoire après utilisation
- Aucun thread ne doit être bloqué sur le sémaphore
- Libère les ressources système

**Exemple** :

```c
sem_destroy(&sem);
```

### Exemple complet d'initialisation

```c
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

sem_t semaphore;

int main(void) {
    // Initialisation : valeur = 3 (3 threads max simultanés)
    if (sem_init(&semaphore, 0, 3) != 0) {
        perror("sem_init failed");
        return EXIT_FAILURE;
    }

    printf("Sémaphore initialisé avec valeur = 3\n");

    // Utilisation du sémaphore...

    // Destruction
    sem_destroy(&semaphore);

    return EXIT_SUCCESS;
}
```

---

## Opérations de base

### sem_wait() : Décrémenter (P)

```c
int sem_wait(sem_t *sem);
```

**Comportement** :
1. Si valeur > 0 : Décrémenter et continuer immédiatement
2. Si valeur = 0 : **Bloquer** jusqu'à ce qu'un autre thread fasse `sem_post()`

**Retour** :
- `0` : Succès
- `-1` : Erreur

**Analogie** : Prendre un jeton de la pile. S'il n'y en a plus, attendre.

**Exemple** :

```c
sem_t sem;  
sem_init(&sem, 0, 2);  // Valeur initiale = 2  

// Thread 1
sem_wait(&sem);  // Valeur = 1, continue  
printf("Thread 1 : Accès ressource\n");  

// Thread 2
sem_wait(&sem);  // Valeur = 0, continue  
printf("Thread 2 : Accès ressource\n");  

// Thread 3
sem_wait(&sem);  // Valeur = 0, BLOQUE jusqu'à un post()
```

### sem_post() : Incrémenter (V)

```c
int sem_post(sem_t *sem);
```

**Comportement** :
1. Incrémenter la valeur
2. Si des threads sont bloqués : Réveiller **un** thread

**Retour** :
- `0` : Succès
- `-1` : Erreur

**Analogie** : Remettre un jeton dans la pile. Si quelqu'un attend, il le prend.

**Exemple** :

```c
// Thread qui libère
sem_post(&sem);  // Valeur++, réveille un thread bloqué  
printf("Ressource libérée\n");  
```

### sem_getvalue() : Obtenir la valeur

```c
int sem_getvalue(sem_t *sem, int *valeur);
```

**Comportement** : Lit la valeur actuelle du sémaphore.

**⚠️ Attention** : La valeur peut changer **immédiatement** après la lecture. Utiliser avec précaution.

**Exemple** :

```c
int valeur;  
sem_getvalue(&sem, &valeur);  
printf("Valeur actuelle du sémaphore : %d\n", valeur);  
```

### Exemple complet

```c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t sem;

void *worker(void *arg) {
    int id = *(int *)arg;

    printf("Thread %d : Demande accès\n", id);

    sem_wait(&sem);  // Décrémenter (bloquer si 0)

    printf("Thread %d : Accès obtenu, travaille...\n", id);
    sleep(2);  // Simule travail

    printf("Thread %d : Libère l'accès\n", id);

    sem_post(&sem);  // Incrémenter

    return NULL;
}

int main(void) {
    pthread_t threads[5];
    int ids[5];

    // Initialisation : max 2 threads simultanés
    sem_init(&sem, 0, 2);

    // Créer 5 threads
    for (int i = 0; i < 5; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, worker, &ids[i]);
    }

    // Attendre
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&sem);

    return 0;
}
```

**Sortie typique** :
```
Thread 0 : Demande accès  
Thread 0 : Accès obtenu, travaille...  
Thread 1 : Demande accès  
Thread 1 : Accès obtenu, travaille...  
Thread 2 : Demande accès  
Thread 3 : Demande accès  
Thread 4 : Demande accès  
Thread 0 : Libère l'accès  
Thread 2 : Accès obtenu, travaille...  
Thread 1 : Libère l'accès  
Thread 3 : Accès obtenu, travaille...  
...
```

**Observation** : Seulement 2 threads à la fois dans la section critique.

---

## Sémaphores binaires

Un **sémaphore binaire** a une valeur de **0 ou 1**. Il se comporte comme un mutex.

### Initialisation

```c
sem_t sem_binaire;  
sem_init(&sem_binaire, 0, 1);  // Valeur = 1 (libre)  
```

### Utilisation comme mutex

```c
int compteur = 0;  
sem_t mutex_sem;  

void *incrementer(void *arg) {
    for (int i = 0; i < 100000; i++) {
        sem_wait(&mutex_sem);    // Lock
        compteur++;               // Section critique
        sem_post(&mutex_sem);    // Unlock
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;

    sem_init(&mutex_sem, 0, 1);  // Binaire : 1

    pthread_create(&t1, NULL, incrementer, NULL);
    pthread_create(&t2, NULL, incrementer, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Compteur : %d\n", compteur);  // 200000

    sem_destroy(&mutex_sem);
    return 0;
}
```

### Différence avec pthread_mutex_t

| Caractéristique | pthread_mutex_t | Sémaphore binaire |
|-----------------|-----------------|-------------------|
| **Propriétaire** | Oui (thread qui lock) | Non |
| **Unlock** | Même thread | N'importe quel thread |
| **Récursif** | Peut être (attribut) | Non |
| **Performance** | Optimisé pour lock | Optimisé pour signalisation |
| **Sémantique** | Protection de données | Signalisation |

**Recommandation** : Utilisez `pthread_mutex_t` pour protéger des données, sémaphore binaire pour la signalisation.

---

## Sémaphores compteurs

Un **sémaphore compteur** a une valeur > 1. Il permet à **plusieurs threads** d'accéder simultanément.

### Exemple : Pool de connexions

```c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_CONNEXIONS 3
#define NUM_CLIENTS 10

sem_t pool_connexions;

void *client(void *arg) {
    int id = *(int *)arg;

    printf("Client %d : Demande connexion\n", id);

    sem_wait(&pool_connexions);  // Prendre une connexion

    printf("Client %d : Connexion obtenue, utilise...\n", id);
    sleep(2);  // Utilise la connexion

    printf("Client %d : Libère connexion\n", id);

    sem_post(&pool_connexions);  // Libérer la connexion

    return NULL;
}

int main(void) {
    pthread_t clients[NUM_CLIENTS];
    int ids[NUM_CLIENTS];

    // Pool de 3 connexions
    sem_init(&pool_connexions, 0, MAX_CONNEXIONS);

    printf("Pool de %d connexions disponibles\n", MAX_CONNEXIONS);

    for (int i = 0; i < NUM_CLIENTS; i++) {
        ids[i] = i;
        pthread_create(&clients[i], NULL, client, &ids[i]);
        usleep(100000);  // Étaler les demandes
    }

    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(clients[i], NULL);
    }

    sem_destroy(&pool_connexions);

    return 0;
}
```

**Sortie** :
```
Pool de 3 connexions disponibles  
Client 0 : Demande connexion  
Client 0 : Connexion obtenue, utilise...  
Client 1 : Demande connexion  
Client 1 : Connexion obtenue, utilise...  
Client 2 : Demande connexion  
Client 2 : Connexion obtenue, utilise...  
Client 3 : Demande connexion  ← Bloqué (pool plein)  
Client 0 : Libère connexion  
Client 3 : Connexion obtenue, utilise...  ← Débloqué  
...
```

### Cas d'usage typiques

**Sémaphore compteur** (valeur > 1) :
- Pool de connexions réseau
- Pool de threads workers
- Limitation de threads simultanés
- Slots de parking (N places)
- Licences logicielles (N utilisateurs max)

---

## Pattern producteur-consommateur

Les sémaphores sont **idéaux** pour implémenter le pattern producteur-consommateur.

### Version simple

```c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];  
int in = 0, out = 0;  

sem_t empty;  // Nombre de cases vides  
sem_t full;   // Nombre de cases pleines  

void *producteur(void *arg) {
    for (int i = 0; i < 20; i++) {
        sem_wait(&empty);  // Attendre une case vide

        buffer[in] = i;
        printf("Produit : %d (position %d)\n", i, in);
        in = (in + 1) % BUFFER_SIZE;

        sem_post(&full);   // Signaler une case pleine

        usleep(100000);  // 100ms
    }
    return NULL;
}

void *consommateur(void *arg) {
    for (int i = 0; i < 20; i++) {
        sem_wait(&full);   // Attendre une case pleine

        int item = buffer[out];
        printf("Consommé : %d (position %d)\n", item, out);
        out = (out + 1) % BUFFER_SIZE;

        sem_post(&empty);  // Signaler une case vide

        usleep(200000);  // 200ms (plus lent)
    }
    return NULL;
}

int main(void) {
    pthread_t prod, cons;

    // Initialisation
    sem_init(&empty, 0, BUFFER_SIZE);  // BUFFER_SIZE cases vides
    sem_init(&full, 0, 0);              // 0 cases pleines

    pthread_create(&prod, NULL, producteur, NULL);
    pthread_create(&cons, NULL, consommateur, NULL);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
```

**Principe** :
- `empty` compte les **cases vides** (initialement BUFFER_SIZE)
- `full` compte les **cases pleines** (initialement 0)
- Producteur : `wait(empty)`, produit, `post(full)`
- Consommateur : `wait(full)`, consomme, `post(empty)`

### Version avec mutex pour protection

Si plusieurs producteurs/consommateurs accèdent au buffer, il faut protéger `in` et `out` :

```c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define N_PROD 2
#define N_CONS 2

int buffer[BUFFER_SIZE];  
int in = 0, out = 0;  

sem_t empty;  
sem_t full;  
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  

void *producteur(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 10; i++) {
        int item = id * 100 + i;

        sem_wait(&empty);

        pthread_mutex_lock(&mutex);  // Protéger in
        buffer[in] = item;
        printf("Prod %d : item %d (pos %d)\n", id, item, in);
        in = (in + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&mutex);

        sem_post(&full);

        usleep(rand() % 100000);
    }
    return NULL;
}

void *consommateur(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 10; i++) {
        sem_wait(&full);

        pthread_mutex_lock(&mutex);  // Protéger out
        int item = buffer[out];
        printf("Cons %d : item %d (pos %d)\n", id, item, out);
        out = (out + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&mutex);

        sem_post(&empty);

        usleep(rand() % 150000);
    }
    return NULL;
}

int main(void) {
    pthread_t prods[N_PROD], cons[N_CONS];
    int ids_prod[N_PROD], ids_cons[N_CONS];

    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

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

    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
```

---

## Limitation de ressources concurrentes

Les sémaphores sont parfaits pour limiter le nombre de threads utilisant une ressource.

### Exemple : Limitation de requêtes simultanées

```c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_CONCURRENT 3
#define NUM_REQUESTS 10

sem_t limiter;

void traiter_requete(int id) {
    printf("[Thread %d] Traitement de la requête...\n", id);
    sleep(2);  // Simule traitement
    printf("[Thread %d] Requête traitée\n", id);
}

void *worker(void *arg) {
    int id = *(int *)arg;

    printf("Requête %d : En attente d'un slot\n", id);

    sem_wait(&limiter);  // Obtenir un slot

    printf("Requête %d : Slot obtenu\n", id);
    traiter_requete(id);

    sem_post(&limiter);  // Libérer le slot

    return NULL;
}

int main(void) {
    pthread_t threads[NUM_REQUESTS];
    int ids[NUM_REQUESTS];

    // Limiter à 3 requêtes simultanées
    sem_init(&limiter, 0, MAX_CONCURRENT);

    printf("=== Serveur : max %d requêtes simultanées ===\n", MAX_CONCURRENT);

    for (int i = 0; i < NUM_REQUESTS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, worker, &ids[i]);
        usleep(100000);  // Étaler les arrivées
    }

    for (int i = 0; i < NUM_REQUESTS; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&limiter);

    printf("=== Toutes les requêtes traitées ===\n");

    return 0;
}
```

### Exemple : Rate limiting

```c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#define RATE_LIMIT 5  // 5 requêtes par seconde max

sem_t rate_limiter;

void *refill_tokens(void *arg) {
    while (1) {
        sleep(1);  // Chaque seconde

        // Remettre les tokens (max RATE_LIMIT)
        for (int i = 0; i < RATE_LIMIT; i++) {
            int val;
            sem_getvalue(&rate_limiter, &val);
            if (val < RATE_LIMIT) {
                sem_post(&rate_limiter);
            }
        }

        printf("[Refill] Tokens rechargés\n");
    }
    return NULL;
}

void *client(void *arg) {
    int id = *(int *)arg;

    while (1) {
        sem_wait(&rate_limiter);  // Consommer un token

        printf("Client %d : Requête envoyée à %ld\n",
               id, time(NULL));

        usleep(100000);  // Envoi toutes les 100ms
    }

    return NULL;
}

int main(void) {
    pthread_t refill, clients[3];
    int ids[3];

    sem_init(&rate_limiter, 0, RATE_LIMIT);

    pthread_create(&refill, NULL, refill_tokens, NULL);

    for (int i = 0; i < 3; i++) {
        ids[i] = i;
        pthread_create(&clients[i], NULL, client, &ids[i]);
    }

    pthread_join(refill, NULL);  // Tourne indéfiniment

    return 0;
}
```

---

## Barrières de synchronisation

Une **barrière** force tous les threads à attendre que tous soient arrivés avant de continuer.

### Implémentation avec sémaphores

```c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_THREADS 5

int threads_arrived = 0;  
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  
sem_t barrier_sem;  

void barrier(int id) {
    pthread_mutex_lock(&mutex);

    threads_arrived++;
    printf("Thread %d arrivé à la barrière (%d/%d)\n",
           id, threads_arrived, NUM_THREADS);

    if (threads_arrived == NUM_THREADS) {
        // Dernier arrivé, libérer tout le monde
        printf("=== Barrière franchie, libération ===\n");

        for (int i = 0; i < NUM_THREADS - 1; i++) {
            sem_post(&barrier_sem);
        }
    }

    pthread_mutex_unlock(&mutex);

    if (threads_arrived < NUM_THREADS) {
        // Pas le dernier, attendre
        sem_wait(&barrier_sem);
    }
}

void *worker(void *arg) {
    int id = *(int *)arg;

    // Phase 1
    printf("Thread %d : Phase 1\n", id);
    sleep(rand() % 3);

    // Attendre que tous finissent phase 1
    barrier(id);

    // Phase 2 (tous ensemble)
    printf("Thread %d : Phase 2\n", id);

    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    sem_init(&barrier_sem, 0, 0);

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, worker, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&barrier_sem);

    return 0;
}
```

**Note** : POSIX fournit aussi `pthread_barrier_t` pour cela, mais l'implémenter avec sémaphores est instructif.

---

## Sémaphores nommés

Les sémaphores **nommés** permettent la communication entre **processus indépendants**.

### Création et ouverture

```c
#include <semaphore.h>
#include <fcntl.h>

// Créer ou ouvrir un sémaphore nommé
sem_t *sem_open(const char *name, int oflag, ...);

// oflag peut être :
// O_CREAT : Créer si n'existe pas
// O_EXCL : Erreur si existe déjà (avec O_CREAT)

// Exemple
sem_t *sem = sem_open("/mon_semaphore", O_CREAT, 0644, 1);  
if (sem == SEM_FAILED) {  
    perror("sem_open");
    exit(EXIT_FAILURE);
}
```

### Fermeture et suppression

```c
// Fermer dans un processus
int sem_close(sem_t *sem);

// Supprimer du système (tous processus)
int sem_unlink(const char *name);
```

### Exemple : Synchronisation inter-processus

**Processus 1 (producteur)** :
```c
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main(void) {
    // Ouvrir sémaphore nommé
    sem_t *sem = sem_open("/sync_sem", O_CREAT, 0644, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    printf("Producteur : Préparation des données...\n");
    sleep(3);

    printf("Producteur : Données prêtes, signal !\n");
    sem_post(sem);  // Signaler au consommateur

    sem_close(sem);

    return 0;
}
```

**Processus 2 (consommateur)** :
```c
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>

int main(void) {
    // Ouvrir le même sémaphore
    sem_t *sem = sem_open("/sync_sem", 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    printf("Consommateur : Attente des données...\n");
    sem_wait(sem);  // Attendre le signal

    printf("Consommateur : Données reçues !\n");

    sem_close(sem);
    sem_unlink("/sync_sem");  // Nettoyer

    return 0;
}
```

**Exécution** :
```bash
# Terminal 1
./consommateur
Consommateur : Attente des données...

# Terminal 2
./producteur
Producteur : Préparation des données...  
Producteur : Données prêtes, signal !  

# Terminal 1 (débloqué)
Consommateur : Données reçues !
```

---

## sem_timedwait() et sem_trywait()

### sem_trywait() : Tentative non-bloquante

```c
int sem_trywait(sem_t *sem);
```

**Comportement** :
- Si valeur > 0 : Décrémenter et retourner 0
- Si valeur = 0 : Retourner -1 immédiatement (errno = EAGAIN)

**Exemple** :

```c
if (sem_trywait(&sem) == 0) {
    // Sémaphore obtenu
    printf("Accès obtenu\n");
    // Travail...
    sem_post(&sem);
} else {
    // Sémaphore occupé
    printf("Occupé, je fais autre chose\n");
    // Travail alternatif
}
```

### sem_timedwait() : Attente avec timeout

```c
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
```

**Paramètres** :
- `sem` : Sémaphore
- `abs_timeout` : Temps **absolu** maximum (comme `pthread_cond_timedwait`)

**Retour** :
- `0` : Succès
- `-1` : Erreur (errno = ETIMEDOUT si timeout)

**Exemple** :

```c
#include <stdio.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>

sem_t sem;

int main(void) {
    sem_init(&sem, 0, 0);  // Valeur = 0 (bloqué)

    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 5;  // 5 secondes

    printf("Attente avec timeout de 5 secondes...\n");

    if (sem_timedwait(&sem, &timeout) == 0) {
        printf("Sémaphore obtenu\n");
    } else if (errno == ETIMEDOUT) {
        printf("TIMEOUT : Pas de signal après 5 secondes\n");
    }

    sem_destroy(&sem);

    return 0;
}
```

---

## POSIX vs System V

Il existe **deux APIs** de sémaphores sous Linux :

### POSIX Sémaphores (sem_*)

**Caractéristiques** :
- API moderne et portable
- Fonctions : `sem_init()`, `sem_wait()`, `sem_post()`
- Header : `<semaphore.h>`
- Link : `-pthread`

**Avantages** :
- Plus simple à utiliser
- Portable (POSIX standard)
- Meilleure intégration avec pthreads

### System V Sémaphores (semget, semop)

**Caractéristiques** :
- API plus ancienne
- Fonctions : `semget()`, `semop()`, `semctl()`
- Header : `<sys/sem.h>`
- Plus complexe (tableaux de sémaphores, opérations atomiques multiples)

**Avantages** :
- Opérations atomiques sur plusieurs sémaphores
- Persiste après crash (peut être un inconvénient)

### Comparaison

| Critère | POSIX | System V |
|---------|-------|----------|
| **API** | Simple | Complexe |
| **Portabilité** | Bonne | Unix seulement |
| **Threading** | Excellent | Moyen |
| **IPC** | Bon | Excellent |
| **Recommandation** | ✅ Pour threads | Pour IPC avancé |

**Dans ce tutoriel**, nous utilisons **POSIX sémaphores** car ils sont plus simples et mieux adaptés au multithreading.

---

## Erreurs courantes

### 1. Oublier sem_destroy()

```c
// ❌ Fuite de ressources
sem_t sem;  
sem_init(&sem, 0, 1);  
// Utilisation...
// Pas de sem_destroy() !

// ✅ Correct
sem_t sem;  
sem_init(&sem, 0, 1);  
// Utilisation...
sem_destroy(&sem);
```

### 2. Valeur initiale incorrecte

```c
// ❌ Bloque immédiatement
sem_t sem;  
sem_init(&sem, 0, 0);  // Valeur = 0  
sem_wait(&sem);  // BLOQUE (personne ne fera post)  

// ✅ Correct pour mutex
sem_init(&sem, 0, 1);  // Valeur = 1 (disponible)
```

### 3. Post sans wait correspondant

```c
// ❌ Valeur croît indéfiniment
for (int i = 0; i < 1000; i++) {
    sem_post(&sem);  // Valeur = 1000 !
}
// Pas de wait() → Valeur très grande

// ✅ Équilibré
sem_wait(&sem);  // Prendre
// Travail
sem_post(&sem);  // Rendre
```

### 4. Confusion entre sem_wait et pthread_mutex_lock

```c
// ❌ Mélange d'APIs
sem_t sem;  
pthread_mutex_lock(&sem);  // Erreur de type !  

// ✅ Correct
sem_wait(&sem);
```

### 5. Deadlock avec ordre incorrect

```c
// Producteur-consommateur mal fait
// ❌ DEADLOCK potentiel
sem_wait(&mutex);    // Prendre mutex  
sem_wait(&full);     // Attendre full → DEADLOCK si buffer vide  
// Buffer plein, mais mutex verrouillé !

// ✅ CORRECT : Sémaphore avant mutex
sem_wait(&full);     // Attendre données  
sem_wait(&mutex);    // Puis prendre mutex  
// Consommer
sem_post(&mutex);  
sem_post(&empty);  
```

---

## Bonnes pratiques

### 1. Nommer clairement les sémaphores

```c
// ❌ Ambigu
sem_t s1, s2;

// ✅ Clair
sem_t empty_slots;  
sem_t filled_slots;  
sem_t mutex_sem;  
```

### 2. Documenter la valeur initiale

```c
// Initialisation du sémaphore
// Valeur = POOL_SIZE : nombre de ressources disponibles
sem_init(&pool_sem, 0, POOL_SIZE);
```

### 3. Pattern wait/post symétrique

```c
// ✅ BON : Chaque wait a son post
sem_wait(&sem);
// Section critique
sem_post(&sem);
```

### 4. Vérifier les retours

```c
// ✅ Toujours vérifier
if (sem_wait(&sem) != 0) {
    perror("sem_wait");
    // Gérer l'erreur
}
```

### 5. Utiliser le bon mécanisme

```c
// Pour protection de données → Mutex
pthread_mutex_t mutex;

// Pour signalisation/comptage → Sémaphore
sem_t semaphore;

// Pour attente de condition complexe → Condition variable
pthread_cond_t cond;
```

---

## Quand utiliser les sémaphores ?

### ✅ Utilisez sémaphores pour :

1. **Limiter ressources concurrentes**
   ```c
   sem_t pool;
   sem_init(&pool, 0, MAX_CONNECTIONS);
   ```

2. **Producteur-consommateur**
   ```c
   sem_t empty, full;
   sem_init(&empty, 0, BUFFER_SIZE);
   sem_init(&full, 0, 0);
   ```

3. **Signalisation entre threads**
   ```c
   sem_t ready_signal;
   sem_init(&ready_signal, 0, 0);
   // Thread 1: sem_post(&ready_signal);
   // Thread 2: sem_wait(&ready_signal);
   ```

4. **Comptage de ressources**
   ```c
   sem_t available_licenses;
   sem_init(&available_licenses, 0, N_LICENSES);
   ```

### ❌ N'utilisez PAS sémaphores pour :

1. **Protection de structures complexes**
   → Utilisez mutex + conditions

2. **Attente de conditions complexes**
   → Utilisez pthread_cond_wait()

3. **Deadlock prevention**
   → Les sémaphores peuvent aussi deadlock

4. **Synchronisation fine**
   → Utilisez atomics (C11)

---

## Récapitulatif

Dans cette section, vous avez appris :

✅ **Sémaphores** :
- Compteur partagé entre threads
- Opérations P (wait) et V (post)
- Valeur toujours ≥ 0

✅ **Types** :
- Anonymes (`sem_t`) : Threads d'un processus
- Nommés (`sem_open`) : Communication inter-processus

✅ **Opérations** :
- `sem_init()` : Initialiser
- `sem_wait()` : Décrémenter (bloquer si 0)
- `sem_post()` : Incrémenter
- `sem_destroy()` : Détruire
- `sem_trywait()` : Non-bloquant
- `sem_timedwait()` : Avec timeout

✅ **Sémaphores binaires** :
- Valeur 0 ou 1
- Comportement mutex-like
- Mais pas de notion de propriétaire

✅ **Sémaphores compteurs** :
- Valeur > 1
- Permet N threads simultanés
- Idéal pour pools de ressources

✅ **Patterns classiques** :
- Producteur-consommateur
- Limitation de ressources
- Barrières de synchronisation
- Rate limiting

✅ **vs Mutex** :
- Sémaphore : Comptage, signalisation
- Mutex : Protection de données
- Pas toujours interchangeables

✅ **Bonnes pratiques** :
- Nommage clair
- Documentation
- Vérification des erreurs
- Choisir le bon mécanisme

---

## Pour aller plus loin

Dans les prochaines sections :

- **18.10** : Atomics C11 (opérations lock-free)
- **18.11** : Read-write locks (optimisation lecture/écriture)
- **18.12** : Thread-local storage (variables par thread)
- **18.13** : Barrières de threads (pthread_barrier)

**Conseil pratique** : Implémentez un serveur web simple qui limite le nombre de connexions simultanées avec un sémaphore. Testez avec plusieurs clients et observez le comportement.

**Projet** : Créez un système de téléchargement avec :
- Un sémaphore pour limiter les téléchargements simultanés
- Des threads producteurs qui ajoutent des URLs
- Des threads consommateurs qui téléchargent
- Utilisez sémaphores + mutex pour la synchronisation

---

📘 **Ressources utiles** :
- `man sem_init`
- `man sem_wait`
- `man sem_post`
- `man sem_overview` : Vue d'ensemble complète

💡 **Prochaine étape** : Section 18.10 - Atomics C11

**Citation** : *"Semaphores are like traffic lights for your threads."* - Analogie classique

⏭️ [Concurrence lock-free avec les Atomics (C11)](/18-threads-et-concurrence/10-atomics.md)
