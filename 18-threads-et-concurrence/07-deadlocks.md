🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 18.7 Deadlocks

## Table des matières
- [Introduction](#introduction)
- [Qu'est-ce qu'un deadlock ?](#quest-ce-quun-deadlock-)
- [Les quatre conditions de Coffman](#les-quatre-conditions-de-coffman)
- [Deadlock avec un seul mutex](#deadlock-avec-un-seul-mutex)
- [Deadlock avec plusieurs mutex](#deadlock-avec-plusieurs-mutex)
- [Détection des deadlocks](#d%C3%A9tection-des-deadlocks)
- [Prévention des deadlocks](#pr%C3%A9vention-des-deadlocks)
- [Stratégies d'évitement](#strat%C3%A9gies-d%C3%A9vitement)
- [Timeouts et trylock](#timeouts-et-trylock)
- [Exemples pratiques](#exemples-pratiques)
- [Outils de diagnostic](#outils-de-diagnostic)
- [Deadlock vs Livelock vs Starvation](#deadlock-vs-livelock-vs-starvation)

---

## Introduction

Dans la section précédente, nous avons appris à utiliser les **mutex** pour résoudre les race conditions. Les mutex sont essentiels, mais ils introduisent un **nouveau problème** : les **deadlocks** (interblocages).

**Ironie tragique** : La solution à un problème (race conditions) crée un nouveau problème (deadlocks) !

**Analogie routière** : Imaginez un carrefour à 4 voies où 4 voitures arrivent simultanément. Chacune attend que les autres bougent avant de passer. Résultat : **blocage total**, personne ne peut avancer. C'est un deadlock.

```
      ↑
      │
      │ Voiture C
      │
←──B──┼──D──→
      │
      │ Voiture A
      │
      ↓

Chaque voiture attend celle de droite → Blocage circulaire !
```

Dans cette section, nous allons apprendre à :
- Comprendre ce qu'est un deadlock
- Identifier les situations à risque
- Détecter les deadlocks
- Les prévenir et les éviter

---

## Qu'est-ce qu'un deadlock ?

### Définition

Un **deadlock** (interblocage) se produit quand **deux ou plusieurs threads** sont bloqués **indéfiniment**, chacun attendant qu'un autre libère une ressource.

**Caractéristique principale** : Aucun thread ne peut progresser. Le système est **gelé**.

### Exemple simple avec 2 threads

```c
pthread_mutex_t mutex_A = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_B = PTHREAD_MUTEX_INITIALIZER;

// Thread 1
void *thread1(void *arg) {
    pthread_mutex_lock(&mutex_A);     // 🔒 Prend A
    sleep(1);                          // Simulation
    pthread_mutex_lock(&mutex_B);     // ⏳ Attend B (possédé par T2)

    // Section critique

    pthread_mutex_unlock(&mutex_B);
    pthread_mutex_unlock(&mutex_A);
    return NULL;
}

// Thread 2
void *thread2(void *arg) {
    pthread_mutex_lock(&mutex_B);     // 🔒 Prend B
    sleep(1);                          // Simulation
    pthread_mutex_lock(&mutex_A);     // ⏳ Attend A (possédé par T1)

    // Section critique

    pthread_mutex_unlock(&mutex_A);
    pthread_mutex_unlock(&mutex_B);
    return NULL;
}

// RÉSULTAT : Deadlock ! Les deux threads sont bloqués indéfiniment
```

### Visualisation temporelle

```
Temps    Thread 1                    Thread 2
======   ========================    ========================
t0       lock(mutex_A) ✅
t1                                   lock(mutex_B) ✅
t2       Essaie lock(mutex_B)        Essaie lock(mutex_A)
         [BLOQUÉ - attend T2]        [BLOQUÉ - attend T1]
t3       [BLOQUÉ]                    [BLOQUÉ]
t4       [BLOQUÉ]                    [BLOQUÉ]
...      [BLOQUÉ POUR TOUJOURS]      [BLOQUÉ POUR TOUJOURS]
```

**Cercle vicieux** :
```
Thread 1 possède A, attend B
         ↓              ↑
         └──────────────┘
                ↑
                │
         ┌──────────────┐
         ↓              │
Thread 2 possède B, attend A
```

### Conséquences

1. **Programme gelé** : Les threads impliqués ne progressent plus
2. **Ressources bloquées** : Les ressources verrouillées ne sont jamais libérées
3. **Effet domino** : D'autres threads attendant ces ressources sont aussi bloqués
4. **Nécessite redémarrage** : Souvent, seul un redémarrage peut résoudre le deadlock

---

## Les quatre conditions de Coffman

Le chercheur Edward Coffman a identifié **4 conditions nécessaires** pour qu'un deadlock se produise. Si **l'une** de ces conditions est absente, le deadlock est **impossible**.

### 1. Exclusion mutuelle (Mutual Exclusion)

**Définition** : Une ressource ne peut être utilisée que par un seul thread à la fois.

**Exemple** : Un mutex ne peut être possédé que par un seul thread.

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// ✅ Condition présente : Mutex = exclusion mutuelle
pthread_mutex_lock(&mutex);   // Seul un thread peut le posséder
```

**Comment l'éliminer ?** : Impossible pour les mutex (c'est leur raison d'être).

### 2. Hold and Wait (Possession et attente)

**Définition** : Un thread possède au moins une ressource et attend d'en acquérir d'autres.

**Exemple** :

```c
pthread_mutex_lock(&mutex_A);     // Possède A
pthread_mutex_lock(&mutex_B);     // Attend B (tout en gardant A)
```

**Comment l'éliminer ?** : Acquérir toutes les ressources en une seule fois atomique.

### 3. No Preemption (Pas de préemption)

**Définition** : Les ressources ne peuvent pas être arrachées de force à un thread. Seul le thread qui les possède peut les libérer.

**Exemple** : On ne peut pas "voler" un mutex à un thread.

**Comment l'éliminer ?** : Mécanisme de timeout ou de priorité (complexe).

### 4. Circular Wait (Attente circulaire)

**Définition** : Il existe une chaîne circulaire de threads, chacun attendant une ressource possédée par le suivant.

**Exemple** :

```
T1 possède A, attend B
   ↓              ↑
T2 possède B, attend C
   ↓              ↑
T3 possède C, attend A  ← Boucle !
```

**Comment l'éliminer ?** : Imposer un ordre d'acquisition des ressources (voir plus loin).

### Résumé visuel

```
┌─────────────────────────────────────────────────────┐
│  DEADLOCK = Les 4 conditions SIMULTANÉMENT          │
├─────────────────────────────────────────────────────┤
│  1. Exclusion mutuelle     ✅ (mutex par nature)    │
│  2. Hold and Wait          ✅ (garde A, attend B)   │
│  3. No Preemption          ✅ (pas de "vol")        │
│  4. Circular Wait          ✅ (T1→T2→T1)            │
└─────────────────────────────────────────────────────┘
                    ↓
          BRISER UNE condition
                    ↓
            PAS DE DEADLOCK
```

---

## Deadlock avec un seul mutex

### Auto-deadlock (Self-deadlock)

Un thread peut créer un deadlock avec **lui-même** en essayant de verrouiller deux fois le même mutex.

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_func(void *arg) {
    pthread_mutex_lock(&mutex);      // ✅ OK : Prend le mutex

    // ... code ...

    pthread_mutex_lock(&mutex);      // ❌ DEADLOCK : Essaie de re-verrouiller

    // Jamais atteint
    pthread_mutex_unlock(&mutex);
    pthread_mutex_unlock(&mutex);

    return NULL;
}
```

**Pourquoi ?** Le thread attend que le mutex soit libéré, mais c'est **lui-même** qui le possède. Attente infinie.

**Visualisation** :

```
Thread essaie lock(mutex)
   ↓
Mutex possédé par... Thread lui-même !
   ↓
Thread attend que mutex soit libéré
   ↓
Mais seul Thread peut libérer mutex
   ↓
DEADLOCK
```

### Cas fréquent : Appels de fonctions imbriqués

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void fonction_interne(void) {
    pthread_mutex_lock(&mutex);
    // Travail...
    pthread_mutex_unlock(&mutex);
}

void fonction_externe(void) {
    pthread_mutex_lock(&mutex);

    fonction_interne();  // ❌ DEADLOCK : Tente de re-verrouiller

    pthread_mutex_unlock(&mutex);
}
```

**Solution 1 : Version interne sans lock**

```c
void fonction_interne_sans_lock(void) {
    // Assume que le mutex est déjà possédé
    // Travail...
}

void fonction_interne(void) {
    pthread_mutex_lock(&mutex);
    fonction_interne_sans_lock();
    pthread_mutex_unlock(&mutex);
}

void fonction_externe(void) {
    pthread_mutex_lock(&mutex);
    fonction_interne_sans_lock();  // ✅ OK : Pas de double lock
    pthread_mutex_unlock(&mutex);
}
```

**Solution 2 : Mutex récursifs**

Les mutex **récursifs** permettent au même thread de verrouiller plusieurs fois :

```c
pthread_mutex_t mutex;
pthread_mutexattr_t attr;

// Initialisation avec attribut récursif
pthread_mutexattr_init(&attr);
pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
pthread_mutex_init(&mutex, &attr);
pthread_mutexattr_destroy(&attr);

// Maintenant OK
pthread_mutex_lock(&mutex);
pthread_mutex_lock(&mutex);  // ✅ OK : Même thread
pthread_mutex_unlock(&mutex);
pthread_mutex_unlock(&mutex);

pthread_mutex_destroy(&mutex);
```

**⚠️ Attention** : Les mutex récursifs peuvent masquer des problèmes de conception. À utiliser avec parcimonie.

---

## Deadlock avec plusieurs mutex

### Exemple classique : Transfert bancaire

Deux comptes, chacun avec son mutex :

```c
typedef struct {
    int solde;
    pthread_mutex_t mutex;
} Compte;

Compte compte_A;
Compte compte_B;

// Thread 1 : A → B
void *transfert_A_vers_B(void *arg) {
    pthread_mutex_lock(&compte_A.mutex);  // 🔒 Prend A
    sleep(1);  // Simule traitement
    pthread_mutex_lock(&compte_B.mutex);  // ⏳ Attend B

    compte_A.solde -= 100;
    compte_B.solde += 100;

    pthread_mutex_unlock(&compte_B.mutex);
    pthread_mutex_unlock(&compte_A.mutex);
    return NULL;
}

// Thread 2 : B → A (ORDRE INVERSÉ !)
void *transfert_B_vers_A(void *arg) {
    pthread_mutex_lock(&compte_B.mutex);  // 🔒 Prend B
    sleep(1);  // Simule traitement
    pthread_mutex_lock(&compte_A.mutex);  // ⏳ Attend A

    compte_B.solde -= 50;
    compte_A.solde += 50;

    pthread_mutex_unlock(&compte_A.mutex);
    pthread_mutex_unlock(&compte_B.mutex);
    return NULL;
}

// RÉSULTAT : DEADLOCK
```

**Diagramme** :

```
T1: A→B          T2: B→A
    ↓               ↓
Lock(A) ✅      Lock(B) ✅
    ↓               ↓
Wait(B) ⏳      Wait(A) ⏳
    ↓               ↓
[BLOQUÉ]        [BLOQUÉ]
```

### Deadlock à 3 threads

Plus complexe, mais même principe :

```c
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m3 = PTHREAD_MUTEX_INITIALIZER;

void *thread1(void *arg) {
    pthread_mutex_lock(&m1);
    sleep(1);
    pthread_mutex_lock(&m2);  // Attend m2
    // ...
    pthread_mutex_unlock(&m2);
    pthread_mutex_unlock(&m1);
    return NULL;
}

void *thread2(void *arg) {
    pthread_mutex_lock(&m2);
    sleep(1);
    pthread_mutex_lock(&m3);  // Attend m3
    // ...
    pthread_mutex_unlock(&m3);
    pthread_mutex_unlock(&m2);
    return NULL;
}

void *thread3(void *arg) {
    pthread_mutex_lock(&m3);
    sleep(1);
    pthread_mutex_lock(&m1);  // Attend m1 → Boucle !
    // ...
    pthread_mutex_unlock(&m1);
    pthread_mutex_unlock(&m3);
    return NULL;
}
```

**Chaîne circulaire** :

```
T1 possède m1, attend m2
    ↓              ↑
T2 possède m2, attend m3
    ↓              ↑
T3 possède m3, attend m1  ← CERCLE VICIEUX
```

---

## Détection des deadlocks

### Symptômes

**Programme gelé** :
- Les threads ne progressent plus
- CPU faible (threads en attente)
- Le programme ne répond plus

**Pas de message d'erreur** : Les threads sont simplement bloqués en attente.

### Vérification manuelle

**Avec GDB** :

```bash
# Lancer le programme
./programme

# Dans un autre terminal
pidof programme  # Obtenir le PID

# Attacher GDB
gdb -p <PID>

# Lister les threads
(gdb) info threads

# Examiner chaque thread
(gdb) thread 1
(gdb) bt  # Backtrace

(gdb) thread 2
(gdb) bt
```

**Exemple de backtrace montrant un deadlock** :

```
Thread 1 (Thread 0x7ffff7fc1740):
#0  __lll_lock_wait () at pthread_mutex_lock.c:103
#1  pthread_mutex_lock () at pthread_mutex_lock.c:87
#2  thread1 () at programme.c:45
    → Bloqué sur mutex_B

Thread 2 (Thread 0x7ffff6fc0700):
#0  __lll_lock_wait () at pthread_mutex_lock.c:103
#1  pthread_mutex_lock () at pthread_mutex_lock.c:87
#2  thread2 () at programme.c:67
    → Bloqué sur mutex_A
```

### Helgrind (Valgrind)

Helgrind peut détecter des **potentiels** deadlocks :

```bash
valgrind --tool=helgrind ./programme
```

**Exemple de sortie** :

```
==12345== Thread #1: lock order "0x30A010 before 0x30A040" violated
==12345==
==12345== Observed (incorrect) order is: acquisition of lock at 0x30A040
==12345==    at 0x4C2D87C: pthread_mutex_lock (hg_intercepts.c:593)
==12345==    by 0x400836: thread2 (programme.c:67)
==12345==
==12345==  followed by a later acquisition of lock at 0x30A010
==12345==    at 0x4C2D87C: pthread_mutex_lock (hg_intercepts.c:593)
==12345==    by 0x400856: thread2 (programme.c:68)
```

Helgrind vous dit : "Attention, ordre d'acquisition incohérent → risque de deadlock"

### ThreadSanitizer

Compile avec `-fsanitize=thread` :

```bash
gcc -fsanitize=thread -g -pthread programme.c -o programme
./programme
```

TSan peut détecter certains deadlocks et ordre d'acquisition problématiques.

### Logs personnalisés

Ajoutez des logs pour tracer l'acquisition de mutex :

```c
void debug_lock(pthread_mutex_t *mutex, const char *name, int line) {
    printf("[Thread %lu] Essaie de prendre %s (ligne %d)\n",
           pthread_self(), name, line);
    pthread_mutex_lock(mutex);
    printf("[Thread %lu] A obtenu %s\n", pthread_self(), name);
}

#define LOCK(m) debug_lock(&m, #m, __LINE__)

// Utilisation
LOCK(mutex_A);
LOCK(mutex_B);
```

**Sortie en cas de deadlock** :

```
[Thread 12345] Essaie de prendre mutex_A (ligne 45)
[Thread 12345] A obtenu mutex_A
[Thread 67890] Essaie de prendre mutex_B (ligne 67)
[Thread 67890] A obtenu mutex_B
[Thread 12345] Essaie de prendre mutex_B (ligne 46)
[Thread 67890] Essaie de prendre mutex_A (ligne 68)
[Silence... deadlock]
```

---

## Prévention des deadlocks

Rappel : Il suffit de **briser une** des 4 conditions de Coffman.

### Stratégie 1 : Ordre d'acquisition global

**Principe** : Imposer un ordre **global cohérent** pour acquérir les mutex.

**Briser** : Condition 4 (Circular Wait)

```c
// ❌ DEADLOCK possible
void *thread1(void *arg) {
    pthread_mutex_lock(&mutex_A);
    pthread_mutex_lock(&mutex_B);
    // ...
}

void *thread2(void *arg) {
    pthread_mutex_lock(&mutex_B);  // Ordre différent !
    pthread_mutex_lock(&mutex_A);
    // ...
}

// ✅ PAS DE DEADLOCK : Ordre cohérent
void *thread1(void *arg) {
    pthread_mutex_lock(&mutex_A);  // Toujours A puis B
    pthread_mutex_lock(&mutex_B);
    // ...
}

void *thread2(void *arg) {
    pthread_mutex_lock(&mutex_A);  // Même ordre : A puis B
    pthread_mutex_lock(&mutex_B);
    // ...
}
```

**Règle** : Définissez un ordre (alphabétique, par adresse mémoire, etc.) et respectez-le **partout**.

### Stratégie 2 : Verrouillage atomique

**Principe** : Acquérir tous les mutex en une seule fois atomique.

**Briser** : Condition 2 (Hold and Wait)

```c
pthread_mutex_t mutex_global = PTHREAD_MUTEX_INITIALIZER;

void *thread_func(void *arg) {
    // Protéger l'acquisition de tous les mutex
    pthread_mutex_lock(&mutex_global);

    pthread_mutex_lock(&mutex_A);
    pthread_mutex_lock(&mutex_B);

    pthread_mutex_unlock(&mutex_global);  // Libérer le "meta-mutex"

    // Section critique avec A et B

    pthread_mutex_unlock(&mutex_B);
    pthread_mutex_unlock(&mutex_A);

    return NULL;
}
```

**Inconvénient** : Sérialise l'acquisition → Réduit la concurrence.

### Stratégie 3 : Trylock avec backoff

**Principe** : Essayer de prendre les mutex. Si échec, **libérer tout** et réessayer.

**Briser** : Condition 2 (Hold and Wait)

```c
void *thread_func(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex_A);

        // Essayer de prendre B
        if (pthread_mutex_trylock(&mutex_B) == 0) {
            // ✅ Succès : On a A et B
            break;
        }

        // ❌ Échec : Libérer A et réessayer
        pthread_mutex_unlock(&mutex_A);

        // Backoff (éviter livelock)
        usleep(rand() % 1000);
    }

    // Section critique avec A et B

    pthread_mutex_unlock(&mutex_B);
    pthread_mutex_unlock(&mutex_A);

    return NULL;
}
```

**Important** : Le backoff aléatoire évite que deux threads réessayent exactement en même temps (livelock).

### Stratégie 4 : Ressource ordering (Tri des mutex)

**Exemple : Transfert bancaire**

```c
void transfert(Compte *source, Compte *dest, int montant) {
    pthread_mutex_t *premier, *second;

    // Trier les mutex par adresse mémoire
    if (&source->mutex < &dest->mutex) {
        premier = &source->mutex;
        second = &dest->mutex;
    } else {
        premier = &dest->mutex;
        second = &source->mutex;
    }

    // Toujours verrouiller dans le même ordre
    pthread_mutex_lock(premier);
    pthread_mutex_lock(second);

    source->solde -= montant;
    dest->solde += montant;

    pthread_mutex_unlock(second);
    pthread_mutex_unlock(premier);
}
```

**Avantage** : Fonctionne même si les comptes sont passés dans un ordre différent.

```c
// Ces deux appels utilisent le même ordre interne
transfert(&compte_A, &compte_B, 100);
transfert(&compte_B, &compte_A, 50);
```

---

## Stratégies d'évitement

### Éviter les mutex multiples

**Meilleure solution** : Concevoir le système pour éviter les mutex multiples.

```c
// ❌ Design nécessitant 2 mutex
typedef struct {
    int solde;
    pthread_mutex_t mutex;
} Compte;

// ✅ Design avec 1 mutex pour tout
typedef struct {
    int solde_A;
    int solde_B;
    pthread_mutex_t mutex;  // Un seul mutex
} SystemeBancaire;

void transfert(SystemeBancaire *sys, int montant) {
    pthread_mutex_lock(&sys->mutex);
    sys->solde_A -= montant;
    sys->solde_B += montant;
    pthread_mutex_unlock(&sys->mutex);
}
```

### Réduire la portée des mutex

**Minimiser** le temps pendant lequel plusieurs mutex sont détenus :

```c
// ❌ Garde les deux mutex longtemps
pthread_mutex_lock(&mutex_A);
pthread_mutex_lock(&mutex_B);
calcul_long();  // Garde A et B
pthread_mutex_unlock(&mutex_B);
pthread_mutex_unlock(&mutex_A);

// ✅ Garde les mutex le minimum de temps
int temp_A, temp_B;

pthread_mutex_lock(&mutex_A);
temp_A = variable_A;
pthread_mutex_unlock(&mutex_A);

pthread_mutex_lock(&mutex_B);
temp_B = variable_B;
pthread_mutex_unlock(&mutex_B);

int result = calcul_long(temp_A, temp_B);

pthread_mutex_lock(&mutex_A);
variable_A = result;
pthread_mutex_unlock(&mutex_A);
```

### Lock-free data structures

Pour les opérations simples, utiliser des **atomics** (section 18.10) :

```c
#include <stdatomic.h>

atomic_int compteur = 0;

void *incrementer(void *arg) {
    atomic_fetch_add(&compteur, 1);  // Pas de mutex !
    return NULL;
}
```

---

## Timeouts et trylock

### pthread_mutex_timedlock()

Évite les attentes infinies avec un timeout :

```c
#include <time.h>

void *thread_func(void *arg) {
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 5;  // 5 secondes max

    int result = pthread_mutex_timedlock(&mutex, &timeout);

    if (result == 0) {
        // Mutex obtenu
        // Section critique
        pthread_mutex_unlock(&mutex);
    } else if (result == ETIMEDOUT) {
        // Timeout : probable deadlock
        fprintf(stderr, "ATTENTION : Timeout détecté (deadlock ?)\n");
        // Action corrective
    }

    return NULL;
}
```

### Boucle trylock intelligente

```c
void *thread_func(void *arg) {
    int tentatives = 0;
    const int MAX_TENTATIVES = 100;

    while (tentatives < MAX_TENTATIVES) {
        if (pthread_mutex_trylock(&mutex) == 0) {
            // ✅ Mutex obtenu
            // Section critique
            pthread_mutex_unlock(&mutex);
            return NULL;
        }

        // Mutex occupé, attendre un peu
        tentatives++;
        usleep(1000);  // 1 ms
    }

    // Échec après 100 tentatives
    fprintf(stderr, "ERREUR : Impossible d'obtenir le mutex\n");
    return NULL;
}
```

---

## Exemples pratiques

### Exemple 1 : Dining Philosophers (Problème des philosophes)

Problème classique illustrant les deadlocks.

**Setup** : 5 philosophes autour d'une table, 5 fourchettes (une entre chaque philosophe).
Chaque philosophe doit prendre **2 fourchettes** (gauche et droite) pour manger.

```c
#define N_PHILO 5

pthread_mutex_t fourchettes[N_PHILO];

void *philosophe(void *arg) {
    int id = *(int *)arg;
    int gauche = id;
    int droite = (id + 1) % N_PHILO;

    while (1) {
        // Penser
        printf("Philosophe %d pense\n", id);
        sleep(1);

        // ❌ DEADLOCK si tous prennent gauche simultanément
        printf("Philosophe %d veut manger\n", id);
        pthread_mutex_lock(&fourchettes[gauche]);
        pthread_mutex_lock(&fourchettes[droite]);

        // Manger
        printf("Philosophe %d mange\n", id);
        sleep(2);

        pthread_mutex_unlock(&fourchettes[droite]);
        pthread_mutex_unlock(&fourchettes[gauche]);
    }

    return NULL;
}
```

**Problème** : Si tous prennent leur fourchette de gauche en même temps, deadlock !

**Solution 1 : Ordre global**

```c
void *philosophe_safe(void *arg) {
    int id = *(int *)arg;
    int gauche = id;
    int droite = (id + 1) % N_PHILO;

    // Toujours prendre la fourchette la plus petite en premier
    int premiere = (gauche < droite) ? gauche : droite;
    int seconde = (gauche < droite) ? droite : gauche;

    while (1) {
        printf("Philosophe %d pense\n", id);
        sleep(1);

        pthread_mutex_lock(&fourchettes[premiere]);  // ✅ Ordre cohérent
        pthread_mutex_lock(&fourchettes[seconde]);

        printf("Philosophe %d mange\n", id);
        sleep(2);

        pthread_mutex_unlock(&fourchettes[seconde]);
        pthread_mutex_unlock(&fourchettes[premiere]);
    }

    return NULL;
}
```

**Solution 2 : Limiter le nombre**

```c
sem_t limite;  // Sémaphore

void init(void) {
    sem_init(&limite, 0, N_PHILO - 1);  // Max N-1 philosophes mangent
}

void *philosophe_limite(void *arg) {
    int id = *(int *)arg;
    int gauche = id;
    int droite = (id + 1) % N_PHILO;

    while (1) {
        printf("Philosophe %d pense\n", id);
        sleep(1);

        sem_wait(&limite);  // Limiter à N-1

        pthread_mutex_lock(&fourchettes[gauche]);
        pthread_mutex_lock(&fourchettes[droite]);

        printf("Philosophe %d mange\n", id);
        sleep(2);

        pthread_mutex_unlock(&fourchettes[droite]);
        pthread_mutex_unlock(&fourchettes[gauche]);

        sem_post(&limite);
    }

    return NULL;
}
```

### Exemple 2 : Reader-Writer avec deadlock potentiel

```c
pthread_mutex_t mutex_read = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_write = PTHREAD_MUTEX_INITIALIZER;

void *reader(void *arg) {
    pthread_mutex_lock(&mutex_read);

    // Si writer a mutex_write et attend mutex_read...
    pthread_mutex_lock(&mutex_write);  // Potentiel deadlock

    // Lecture

    pthread_mutex_unlock(&mutex_write);
    pthread_mutex_unlock(&mutex_read);

    return NULL;
}

void *writer(void *arg) {
    pthread_mutex_lock(&mutex_write);

    // Si reader a mutex_read et attend mutex_write...
    pthread_mutex_lock(&mutex_read);  // Potentiel deadlock

    // Écriture

    pthread_mutex_unlock(&mutex_read);
    pthread_mutex_unlock(&mutex_write);

    return NULL;
}
```

**Solution** : Utiliser un seul mutex ou des read-write locks (section 18.11).

---

## Outils de diagnostic

### 1. GDB avec info threads

```bash
gdb ./programme

(gdb) run
# Programme se bloque

(gdb) info threads
(gdb) thread apply all bt  # Backtrace de tous les threads
```

### 2. Helgrind (Valgrind)

```bash
valgrind --tool=helgrind --log-file=helgrind.log ./programme
```

Analyse `helgrind.log` pour les violations d'ordre de lock.

### 3. ThreadSanitizer

```bash
gcc -fsanitize=thread -g -O1 -pthread programme.c -o programme
./programme
```

### 4. Deadlock detector custom

```c
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define TIMEOUT_SEC 5

typedef struct {
    pthread_mutex_t mutex;
    time_t last_lock;
    pthread_t owner;
    char name[32];
} MonitoredMutex;

void monitored_lock(MonitoredMutex *m) {
    time_t start = time(NULL);

    while (pthread_mutex_trylock(&m->mutex) != 0) {
        if (time(NULL) - start > TIMEOUT_SEC) {
            fprintf(stderr, "WARNING: Deadlock détecté sur %s\n", m->name);
            fprintf(stderr, "Possédé par thread %lu\n", m->owner);
            // Action corrective ou abort
            abort();
        }
        usleep(10000);  // 10ms
    }

    m->owner = pthread_self();
    m->last_lock = time(NULL);
}

void monitored_unlock(MonitoredMutex *m) {
    m->owner = 0;
    pthread_mutex_unlock(&m->mutex);
}
```

### 5. Logs structurés

```c
void log_lock_attempt(pthread_t tid, const char *mutex_name) {
    printf("[%ld] Thread %lu ESSAIE %s\n", time(NULL), tid, mutex_name);
}

void log_lock_acquired(pthread_t tid, const char *mutex_name) {
    printf("[%ld] Thread %lu OBTENU %s\n", time(NULL), tid, mutex_name);
}

void log_lock_released(pthread_t tid, const char *mutex_name) {
    printf("[%ld] Thread %lu LIBÉRÉ %s\n", time(NULL), tid, mutex_name);
}
```

---

## Deadlock vs Livelock vs Starvation

### Deadlock

**Définition** : Blocage permanent. Aucun thread ne progresse.

**Symptôme** : Programme gelé.

```c
// T1 et T2 bloqués indéfiniment
T1: lock(A), wait(B)
T2: lock(B), wait(A)
```

### Livelock

**Définition** : Les threads changent d'état mais ne progressent pas. Ils "dansent" sans avancer.

**Symptôme** : CPU élevé mais pas de progrès.

**Exemple** :

```c
// Deux threads essayent d'être polis
void *thread1(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex_A);

        if (pthread_mutex_trylock(&mutex_B) != 0) {
            pthread_mutex_unlock(&mutex_A);  // "Après vous"
            continue;  // Réessayer
        }

        // Travail
        break;
    }
    return NULL;
}

void *thread2(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex_B);

        if (pthread_mutex_trylock(&mutex_A) != 0) {
            pthread_mutex_unlock(&mutex_B);  // "Non, après vous"
            continue;  // Réessayer
        }

        // Travail
        break;
    }
    return NULL;
}

// Les deux threads libèrent et réessayent sans fin
```

**Solution** : Backoff aléatoire.

### Starvation

**Définition** : Un thread n'obtient jamais la ressource (toujours devancé par d'autres).

**Symptôme** : Certains threads progressent, d'autres jamais.

**Exemple** :

```c
// Thread basse priorité jamais servi
void *low_priority(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);  // N'obtient jamais le mutex
        // ...
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

// Threads haute priorité monopolisent
void *high_priority(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        // Travail long
        pthread_mutex_unlock(&mutex);
        // Pas de pause → reprend immédiatement
    }
    return NULL;
}
```

**Solution** : Équité (fair scheduling) ou sémaphores avec file FIFO.

### Comparaison

| Problème | Threads progressent ? | CPU | Solution |
|----------|----------------------|-----|----------|
| **Deadlock** | ❌ Non | Bas | Ordre de lock, timeouts |
| **Livelock** | ❌ Non (mais actifs) | Haut | Backoff aléatoire |
| **Starvation** | ⚠️ Certains oui | Variable | Équité, priorités |

---

## Récapitulatif

Dans cette section, vous avez appris :

✅ **Deadlock** :
- Blocage permanent de threads
- Attente circulaire de ressources
- Programme gelé

✅ **4 conditions de Coffman** :
- Exclusion mutuelle
- Hold and Wait
- No Preemption
- Circular Wait
- Briser une condition → Pas de deadlock

✅ **Types de deadlocks** :
- Auto-deadlock (1 thread, 1 mutex)
- Deadlock classique (2+ threads, 2+ mutex)
- Chaîne circulaire (3+ threads)

✅ **Détection** :
- GDB (info threads, backtrace)
- Helgrind (ordre de lock)
- ThreadSanitizer
- Logs personnalisés

✅ **Prévention** :
- Ordre d'acquisition global
- Verrouillage atomique
- Trylock avec backoff
- Resource ordering

✅ **Évitement** :
- Design avec moins de mutex
- Timeouts
- Lock-free structures

✅ **Problèmes associés** :
- Livelock (actif mais pas de progrès)
- Starvation (thread jamais servi)

---

## Règles d'or anti-deadlock

1. **Toujours verrouiller les mutex dans le même ordre**
2. **Minimiser le nombre de mutex détenus simultanément**
3. **Garder les mutex le moins longtemps possible**
4. **Utiliser trylock ou timedlock pour détecter les problèmes**
5. **Tester avec Helgrind/ThreadSanitizer**
6. **Concevoir pour éviter les mutex multiples quand possible**

---

## Pour aller plus loin

Dans les prochaines sections :

- **18.8** : Variables de condition (synchronisation avancée)
- **18.9** : Sémaphores POSIX (contrôle d'accès multiples)
- **18.10** : Atomics C11 (opérations lock-free)
- **18.11** : Read-write locks (optimisation lecture/écriture)

**Conseil pratique** : Reprenez vos programmes avec mutex et :
1. Vérifiez l'ordre d'acquisition des mutex
2. Testez avec Helgrind
3. Ajoutez des timeouts pour détecter les deadlocks
4. Simplifiez le design si possible pour éviter les mutex multiples

**Projet** : Implémentez le problème des philosophes avec les 3 solutions présentées. Testez chacune et observez les différences.

---

📘 **Ressources utiles** :
- `man pthread_mutex_timedlock`
- Helgrind : `valgrind --tool=helgrind`
- Article : "Dining Philosophers Problem" (Dijkstra)
- Livre : "The Little Book of Semaphores" (Allen Downey)

💡 **Prochaine étape** : Section 18.8 - Variables de condition

**Citation** : *"The only truly safe way to avoid deadlock is to avoid locks."* - Herb Sutter (mais ce n'est pas toujours pratique !)

⏭️ [Variables de condition](/18-threads-et-concurrence/08-variables-condition.md)
