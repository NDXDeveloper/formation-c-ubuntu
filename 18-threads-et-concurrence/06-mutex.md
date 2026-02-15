🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 18.6 Mutex et sections critiques

## Table des matières
- [Introduction](#introduction)
- [Qu'est-ce qu'un mutex ?](#quest-ce-quun-mutex-)
- [Concept de section critique](#concept-de-section-critique)
- [Initialisation des mutex](#initialisation-des-mutex)
- [Verrouillage et déverrouillage](#verrouillage-et-d%C3%A9verrouillage)
- [Exemple : Résoudre le compteur partagé](#exemple--r%C3%A9soudre-le-compteur-partag%C3%A9)
- [Les variantes de lock](#les-variantes-de-lock)
- [Destruction des mutex](#destruction-des-mutex)
- [Résolution des exemples de la section précédente](#r%C3%A9solution-des-exemples-de-la-section-pr%C3%A9c%C3%A9dente)
- [Bonnes pratiques](#bonnes-pratiques)
- [Erreurs courantes et pièges](#erreurs-courantes-et-pi%C3%A8ges)
- [Performance et overhead](#performance-et-overhead)
- [Granularité du verrouillage](#granularit%C3%A9-du-verrouillage)
- [Mutex multiples et complexité](#mutex-multiples-et-complexit%C3%A9)

---

## Introduction

Dans la section précédente, nous avons vu les **race conditions** - le cauchemar du programmeur multi-threadé. Maintenant, découvrons la solution principale : les **mutex** (Mutual Exclusion).

**Problème rappelé** :
```c
int compteur = 0;  // Partagé

void *incrementer(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        compteur++;  // ❌ Race condition
    }
    return NULL;
}
// Résultat : incorrect (1847392 au lieu de 2000000)
```

**Solution avec mutex** :
```c
int compteur = 0;  
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  

void *incrementer(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        pthread_mutex_lock(&mutex);    // 🔒 Verrouiller
        compteur++;                     // ✅ Section critique protégée
        pthread_mutex_unlock(&mutex);  // 🔓 Déverrouiller
    }
    return NULL;
}
// Résultat : toujours correct (2000000)
```

Dans cette section, nous allons apprendre :
- Ce qu'est un mutex et comment il fonctionne
- Comment l'utiliser correctement
- Les pièges à éviter
- Les bonnes pratiques de verrouillage

---

## Qu'est-ce qu'un mutex ?

### Définition

Un **mutex** (Mutual Exclusion = Exclusion Mutuelle) est un mécanisme de synchronisation qui garantit qu'**un seul thread à la fois** peut exécuter une section de code critique.

**Analogie simple** : Imaginez des toilettes avec une seule clé. Tant que quelqu'un a la clé (le verrou), personne d'autre ne peut entrer. Quand la personne sort, elle rend la clé, et la prochaine personne peut entrer.

```
Thread 1                Thread 2                Thread 3
========                ========                ========
Prend la clé 🔑         Attend...               Attend...  
Entre 🚪                Attend...               Attend...  
Utilise 🚽              Attend...               Attend...  
Sort et rend clé        Prend la clé 🔑         Attend...  
Continue                Entre 🚪                Attend...  
                        Utilise 🚽              Attend...
                        Sort et rend clé        Prend la clé 🔑
                        Continue                Entre 🚪
                                                Utilise 🚽
                                                Sort et rend clé
                                                Continue
```

### Principe de fonctionnement

Un mutex a **deux états** :
- **Verrouillé** (locked) : Un thread possède le mutex
- **Déverrouillé** (unlocked) : Le mutex est disponible

**Opérations** :
1. **Lock (verrouiller)** : Prendre le mutex. Si déjà pris, **attendre** qu'il soit libéré.
2. **Unlock (déverrouiller)** : Libérer le mutex. Un autre thread en attente peut alors le prendre.

### Visualisation

```
État initial : Mutex DÉVERROUILLÉ
┌────────────┐
│ UNLOCKED   │
└────────────┘

Thread 1 appelle lock()
┌────────────┐
│  LOCKED    │ ← Thread 1 possède le mutex
└────────────┘

Thread 2 appelle lock()
┌────────────┐
│  LOCKED    │ ← Thread 1 possède toujours
│            │
│ Thread 2   │ ← Thread 2 est BLOQUÉ, attend
│  (wait)    │
└────────────┘

Thread 1 appelle unlock()
┌────────────┐
│  LOCKED    │ ← Thread 2 prend le mutex automatiquement
└────────────┘

Thread 2 appelle unlock()
┌────────────┐
│ UNLOCKED   │ ← Disponible à nouveau
└────────────┘
```

### Le type pthread_mutex_t

En POSIX threads, un mutex est représenté par le type `pthread_mutex_t` :

```c
pthread_mutex_t mon_mutex;
```

C'est un type **opaque** - vous ne devez jamais accéder directement à son contenu, seulement via les fonctions pthread.

---

## Concept de section critique

### Définition

Une **section critique** (critical section) est une portion de code qui :
- Accède à des **ressources partagées**
- Doit être exécutée de manière **atomique** (sans interruption par d'autres threads)
- Doit être **protégée** par un mutex

### Identification des sections critiques

**Règle d'or** : Toute partie de code qui lit **ou** modifie une variable partagée est une section critique.

```c
// Variable partagée
int compteur = 0;

void *worker(void *arg) {
    // ↓ DÉBUT SECTION CRITIQUE
    compteur++;  // Lecture + Modification + Écriture
    // ↑ FIN SECTION CRITIQUE
    return NULL;
}
```

### Pattern de protection

Le pattern standard pour protéger une section critique :

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_function(void *arg) {
    // Code non critique (pas de variables partagées)
    int local_var = calcul_local();

    // ── SECTION CRITIQUE ──
    pthread_mutex_lock(&mutex);      // 🔒 ENTRÉE

    // Accès aux ressources partagées
    variable_partagee = local_var;
    autre_variable_partagee++;

    pthread_mutex_unlock(&mutex);    // 🔓 SORTIE
    // ── FIN SECTION CRITIQUE ──

    // Code non critique
    printf("Résultat : %d\n", local_var);

    return NULL;
}
```

**Principe** : Minimiser la taille de la section critique pour maximiser la concurrence.

### Règles importantes

1. **Toujours déverrouiller** : Chaque `lock()` doit avoir son `unlock()` correspondant
2. **Pas de blocage dans la section** : Ne faites pas d'I/O lentes, de `sleep()`, ou d'appels bloquants
3. **Courte durée** : Gardez la section critique aussi petite que possible
4. **Un seul mutex par ressource** : Chaque ressource partagée a son mutex dédié (ou groupe logique)

---

## Initialisation des mutex

Il existe **deux méthodes** pour initialiser un mutex.

### Méthode 1 : Initialisation statique (recommandée pour mutex globaux)

```c
pthread_mutex_t mon_mutex = PTHREAD_MUTEX_INITIALIZER;
```

**Avantages** :
- Simple et directe
- Pas besoin de fonction d'initialisation
- Pas besoin de vérification d'erreur
- Idéale pour les variables globales ou statiques

**Exemple complet** :

```c
#include <stdio.h>
#include <pthread.h>

int compteur = 0;  
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // ← Initialisation statique  

void *incrementer(void *arg) {
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&mutex);
        compteur++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, incrementer, NULL);
    pthread_create(&t2, NULL, incrementer, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Compteur : %d\n", compteur);  // Toujours 200000
    return 0;
}
```

### Méthode 2 : Initialisation dynamique

```c
pthread_mutex_t mon_mutex;  
int result = pthread_mutex_init(&mon_mutex, NULL);  
```

**Signature** :
```c
int pthread_mutex_init(pthread_mutex_t *mutex,
                       const pthread_mutexattr_t *attr);
```

**Paramètres** :
- `mutex` : Pointeur vers le mutex à initialiser
- `attr` : Attributs du mutex (NULL pour attributs par défaut)

**Retour** :
- `0` en cas de succès
- Code d'erreur sinon

**Exemple** :

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

int main(void) {
    pthread_mutex_t mutex;

    // Initialisation
    int result = pthread_mutex_init(&mutex, NULL);
    if (result != 0) {
        fprintf(stderr, "pthread_mutex_init: %s\n", strerror(result));
        return EXIT_FAILURE;
    }

    // Utilisation du mutex...
    pthread_mutex_lock(&mutex);
    // Section critique
    pthread_mutex_unlock(&mutex);

    // Destruction (obligatoire avec init dynamique)
    pthread_mutex_destroy(&mutex);

    return EXIT_SUCCESS;
}
```

**Quand utiliser l'initialisation dynamique ?**

- Mutex dans une structure allouée dynamiquement
- Mutex avec attributs personnalisés
- Création de mutex au runtime

**Exemple avec structure** :

```c
typedef struct {
    int valeur;
    pthread_mutex_t mutex;
} Compteur;

Compteur *creer_compteur(void) {
    Compteur *c = malloc(sizeof(Compteur));
    if (c == NULL) return NULL;

    c->valeur = 0;

    // Initialisation dynamique obligatoire
    if (pthread_mutex_init(&c->mutex, NULL) != 0) {
        free(c);
        return NULL;
    }

    return c;
}

void detruire_compteur(Compteur *c) {
    pthread_mutex_destroy(&c->mutex);
    free(c);
}
```

---

## Verrouillage et déverrouillage

### pthread_mutex_lock()

**Signature** :
```c
int pthread_mutex_lock(pthread_mutex_t *mutex);
```

**Comportement** :
- Si le mutex est **déverrouillé** : Le prend et continue immédiatement
- Si le mutex est **verrouillé** par un autre thread : **Bloque** jusqu'à ce qu'il soit libéré
- Si le mutex est déjà **possédé par le même thread** : Comportement indéfini (généralement deadlock)

**Retour** :
- `0` en cas de succès
- Code d'erreur en cas de problème

**Exemple** :

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_func(void *arg) {
    printf("Thread %lu essaie de prendre le mutex...\n", pthread_self());

    pthread_mutex_lock(&mutex);  // Peut bloquer ici

    printf("Thread %lu A le mutex !\n", pthread_self());

    // Section critique
    sleep(2);  // Simule du travail

    printf("Thread %lu libère le mutex\n", pthread_self());
    pthread_mutex_unlock(&mutex);

    return NULL;
}
```

### pthread_mutex_unlock()

**Signature** :
```c
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```

**Comportement** :
- Libère le mutex
- Réveille un thread en attente (s'il y en a)
- Ne doit être appelé **que** par le thread qui possède le mutex

**Retour** :
- `0` en cas de succès
- Code d'erreur en cas de problème (`EPERM` si le thread ne possède pas le mutex)

**Important** : Toujours déverrouiller dans le même thread qui a verrouillé !

```c
// ❌ ERREUR : Déverrouiller dans un autre thread
void *thread1(void *arg) {
    pthread_mutex_lock(&mutex);
    return NULL;
}

void *thread2(void *arg) {
    pthread_mutex_unlock(&mutex);  // ❌ ERREUR : thread2 ne possède pas le mutex
    return NULL;
}
```

### Pattern lock/unlock avec gestion d'erreur

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *worker(void *arg) {
    int result;

    // Verrouillage
    result = pthread_mutex_lock(&mutex);
    if (result != 0) {
        fprintf(stderr, "Lock failed: %s\n", strerror(result));
        return NULL;
    }

    // ── SECTION CRITIQUE ──
    // Accès aux ressources partagées
    // ──────────────────────

    // Déverrouillage
    result = pthread_mutex_unlock(&mutex);
    if (result != 0) {
        fprintf(stderr, "Unlock failed: %s\n", strerror(result));
        return NULL;
    }

    return NULL;
}
```

**En pratique**, pour un code plus lisible, beaucoup de programmeurs omettent la vérification d'erreur pour `lock()`/`unlock()` (sauf en cas de debugging) :

```c
pthread_mutex_lock(&mutex);
// Section critique
pthread_mutex_unlock(&mutex);
```

---

## Exemple : Résoudre le compteur partagé

Reprenons l'exemple problématique de la section 18.5 et corrigeons-le avec un mutex.

### Code SANS mutex (bugué)

```c
#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 2
#define ITERATIONS 1000000

int compteur = 0;  // ❌ Non protégé

void *incrementer(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        compteur++;  // ❌ Race condition
    }
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, incrementer, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Attendu : %d\n", NUM_THREADS * ITERATIONS);
    printf("Obtenu  : %d\n", compteur);  // Faux !

    return 0;
}
```

**Sortie** :
```
Attendu : 2000000  
Obtenu  : 1847392  ← Incorrect !  
```

### Code AVEC mutex (corrigé)

```c
#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 2
#define ITERATIONS 1000000

int compteur = 0;  
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // ✅ Mutex  

void *incrementer(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);      // 🔒 Verrouiller
        compteur++;                       // ✅ Section critique protégée
        pthread_mutex_unlock(&mutex);    // 🔓 Déverrouiller
    }
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, incrementer, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Attendu : %d\n", NUM_THREADS * ITERATIONS);
    printf("Obtenu  : %d\n", compteur);  // ✅ Toujours correct !

    return 0;
}
```

**Sortie** :
```
Attendu : 2000000  
Obtenu  : 2000000  ← Correct !  
```

**Compilation et test** :
```bash
gcc -Wall -pthread compteur.c -o compteur

# Test avec ThreadSanitizer (aucun warning maintenant)
gcc -fsanitize=thread -pthread compteur.c -o compteur_tsan
./compteur_tsan
```

### Que se passe-t-il ?

**Diagramme temporel** :

```
SANS MUTEX :
============
Thread 1:  LIRE(0) ── INCR(1) ── ÉCRIRE(1)  
Thread 2:        LIRE(0) ── INCR(1) ── ÉCRIRE(1)  
                 ↑                      ↑
            Lit l'ancienne           Écrase T1
            valeur (0)

Résultat : 1 au lieu de 2


AVEC MUTEX :
===========
Thread 1:  LOCK ── LIRE(0) ── INCR(1) ── ÉCRIRE(1) ── UNLOCK  
Thread 2:         [BLOQUÉ................................] LOCK ── LIRE(1) ── ...  
                                                          ↑
                                                    Lit la bonne valeur

Résultat : Toujours correct !
```

---

## Les variantes de lock

### pthread_mutex_trylock()

Tente de prendre le mutex **sans bloquer**.

**Signature** :
```c
int pthread_mutex_trylock(pthread_mutex_t *mutex);
```

**Retour** :
- `0` : Succès, mutex pris
- `EBUSY` : Mutex déjà verrouillé (n'attend pas)
- Autre code d'erreur

**Utilisation** :

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_func(void *arg) {
    int result = pthread_mutex_trylock(&mutex);

    if (result == 0) {
        // ✅ Mutex obtenu
        printf("Mutex obtenu, travail...\n");
        // Section critique
        pthread_mutex_unlock(&mutex);
    } else if (result == EBUSY) {
        // ❌ Mutex occupé, faire autre chose
        printf("Mutex occupé, je fais autre chose\n");
        // Travail alternatif
    } else {
        // Erreur
        fprintf(stderr, "trylock error: %s\n", strerror(result));
    }

    return NULL;
}
```

**Cas d'usage** :
- Éviter de bloquer indéfiniment
- Implémenter une file d'attente de tâches (si occupé, passer à la suivante)
- Polling avec timeout manuel

### pthread_mutex_timedlock()

Tente de prendre le mutex avec un **timeout**.

**Signature** :
```c
int pthread_mutex_timedlock(pthread_mutex_t *mutex,
                           const struct timespec *abs_timeout);
```

**Paramètres** :
- `mutex` : Le mutex
- `abs_timeout` : Temps absolu maximum d'attente (pas relatif !)

**Retour** :
- `0` : Succès
- `ETIMEDOUT` : Timeout écoulé
- Autre code d'erreur

**Exemple** :

```c
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <errno.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_func(void *arg) {
    struct timespec timeout;

    // Calculer le temps absolu : maintenant + 2 secondes
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 2;

    printf("Thread essaie de prendre le mutex (max 2s)...\n");

    int result = pthread_mutex_timedlock(&mutex, &timeout);

    if (result == 0) {
        printf("Mutex obtenu !\n");
        // Section critique
        pthread_mutex_unlock(&mutex);
    } else if (result == ETIMEDOUT) {
        printf("Timeout : mutex non disponible après 2s\n");
    } else {
        fprintf(stderr, "Erreur: %s\n", strerror(result));
    }

    return NULL;
}

int main(void) {
    pthread_t t1, t2;

    // Thread 1 prend le mutex et le garde 5 secondes
    pthread_mutex_lock(&mutex);

    pthread_create(&t1, NULL, thread_func, NULL);
    pthread_create(&t2, NULL, thread_func, NULL);

    sleep(5);  // Garder le mutex 5 secondes
    pthread_mutex_unlock(&mutex);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
```

**Sortie** :
```
Thread essaie de prendre le mutex (max 2s)...  
Thread essaie de prendre le mutex (max 2s)...  
Timeout : mutex non disponible après 2s  
Timeout : mutex non disponible après 2s  
```

---

## Destruction des mutex

### pthread_mutex_destroy()

Détruit un mutex initialisé dynamiquement.

**Signature** :
```c
int pthread_mutex_destroy(pthread_mutex_t *mutex);
```

**Règles** :
- Obligatoire pour les mutex créés avec `pthread_mutex_init()`
- Pas nécessaire pour les mutex avec `PTHREAD_MUTEX_INITIALIZER`
- Le mutex doit être **déverrouillé** avant destruction
- Ne doit **plus** être utilisé après destruction

**Exemple** :

```c
pthread_mutex_t mutex;

// Initialisation
pthread_mutex_init(&mutex, NULL);

// Utilisation
pthread_mutex_lock(&mutex);
// ...
pthread_mutex_unlock(&mutex);

// Destruction (obligatoire)
pthread_mutex_destroy(&mutex);
```

**Avec structure** :

```c
typedef struct {
    int data;
    pthread_mutex_t mutex;
} Resource;

Resource *create_resource(void) {
    Resource *r = malloc(sizeof(Resource));
    if (r == NULL) return NULL;

    r->data = 0;
    if (pthread_mutex_init(&r->mutex, NULL) != 0) {
        free(r);
        return NULL;
    }

    return r;
}

void destroy_resource(Resource *r) {
    pthread_mutex_destroy(&r->mutex);  // ✅ Destruction avant free
    free(r);
}
```

---

## Résolution des exemples de la section précédente

Reprenons les exemples problématiques de la section 18.5 et corrigeons-les avec des mutex.

### Exemple 1 : Liste chaînée partagée (CORRIGÉ)

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

Node *head = NULL;  
pthread_mutex_t mutex_liste = PTHREAD_MUTEX_INITIALIZER;  // ✅ Mutex  

void *ajouter_element(void *arg) {
    int valeur = *(int *)arg;

    Node *nouveau = malloc(sizeof(Node));
    if (nouveau == NULL) return NULL;

    nouveau->data = valeur;

    pthread_mutex_lock(&mutex_liste);      // 🔒 VERROUILLER
    nouveau->next = head;
    head = nouveau;
    pthread_mutex_unlock(&mutex_liste);    // 🔓 DÉVERROUILLER

    return NULL;
}

int main(void) {
    pthread_t threads[10];
    int valeurs[10];

    for (int i = 0; i < 10; i++) {
        valeurs[i] = i + 1;
        pthread_create(&threads[i], NULL, ajouter_element, &valeurs[i]);
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    // Compter les éléments
    pthread_mutex_lock(&mutex_liste);
    int count = 0;
    Node *current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    pthread_mutex_unlock(&mutex_liste);

    printf("Nombre d'éléments : %d\n", count);  // Toujours 10

    return 0;
}
```

### Exemple 2 : Compte bancaire (CORRIGÉ)

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

double solde = 1000.0;  
pthread_mutex_t mutex_compte = PTHREAD_MUTEX_INITIALIZER;  // ✅ Mutex  

void *retirer(void *arg) {
    double montant = *(double *)arg;

    pthread_mutex_lock(&mutex_compte);  // 🔒 Protection complète

    if (solde >= montant) {
        printf("Vérification OK pour %.2f€\n", montant);
        sleep(1);  // Simule vérifications
        solde -= montant;
        printf("Retrait de %.2f€ effectué\n", montant);
    } else {
        printf("Solde insuffisant pour %.2f€\n", montant);
    }

    pthread_mutex_unlock(&mutex_compte);  // 🔓 Déverrouiller

    return NULL;
}

int main(void) {
    pthread_t t1, t2;
    double montant1 = 600.0;
    double montant2 = 600.0;

    pthread_create(&t1, NULL, retirer, &montant1);
    pthread_create(&t2, NULL, retirer, &montant2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Solde final : %.2f€\n", solde);  // 400€ ou 1000€ (jamais négatif)

    return 0;
}
```

**Sortie** :
```
Vérification OK pour 600.00€  
Retrait de 600.00€ effectué  
Solde insuffisant pour 600.00€  
Solde final : 400.00€  
```

### Exemple 3 : Statistiques (CORRIGÉ)

```c
#include <stdio.h>
#include <pthread.h>

typedef struct {
    long total;
    int count;
    double moyenne;
    pthread_mutex_t mutex;  // ✅ Mutex intégré
} Stats;

Stats stats;

void init_stats(void) {
    stats.total = 0;
    stats.count = 0;
    stats.moyenne = 0.0;
    pthread_mutex_init(&stats.mutex, NULL);
}

void destroy_stats(void) {
    pthread_mutex_destroy(&stats.mutex);
}

void *ajouter_valeur(void *arg) {
    int valeur = *(int *)arg;

    pthread_mutex_lock(&stats.mutex);  // 🔒 Tout protégé

    stats.total += valeur;
    stats.count++;
    stats.moyenne = (double)stats.total / stats.count;

    pthread_mutex_unlock(&stats.mutex);  // 🔓

    return NULL;
}

void afficher_stats(void) {
    pthread_mutex_lock(&stats.mutex);
    printf("Total: %ld, Count: %d, Moyenne: %.2f\n",
           stats.total, stats.count, stats.moyenne);
    pthread_mutex_unlock(&stats.mutex);
}

int main(void) {
    pthread_t threads[10];
    int valeurs[10] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

    init_stats();

    for (int i = 0; i < 10; i++) {
        pthread_create(&threads[i], NULL, ajouter_valeur, &valeurs[i]);
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    afficher_stats();  // Toujours cohérent

    destroy_stats();
    return 0;
}
```

---

## Bonnes pratiques

### 1. Minimiser la section critique

Plus la section critique est courte, plus la concurrence est élevée.

```c
// ❌ MAUVAIS : Section critique trop grande
pthread_mutex_lock(&mutex);  
int result = calcul_complexe();  // Prend 1 seconde  
variable_partagee = result;  
pthread_mutex_unlock(&mutex);  

// ✅ BON : Calcul en dehors de la section critique
int result = calcul_complexe();  // Calcul local  
pthread_mutex_lock(&mutex);  
variable_partagee = result;  // Seulement l'écriture protégée  
pthread_mutex_unlock(&mutex);  
```

### 2. Toujours déverrouiller

Chaque `lock()` doit avoir son `unlock()`.

```c
// ❌ DANGEREUX
pthread_mutex_lock(&mutex);  
if (erreur) {  
    return NULL;  // ← Oubli de unlock !
}
pthread_mutex_unlock(&mutex);

// ✅ BON : Unlock dans tous les chemins
pthread_mutex_lock(&mutex);  
if (erreur) {  
    pthread_mutex_unlock(&mutex);
    return NULL;
}
pthread_mutex_unlock(&mutex);

// ✅ MEILLEUR : Goto cleanup
pthread_mutex_lock(&mutex);  
if (erreur) {  
    goto cleanup;
}
// ...
cleanup:
    pthread_mutex_unlock(&mutex);
    return NULL;
```

### 3. Un mutex par ressource logique

Chaque ressource (ou groupe logique) doit avoir son propre mutex.

```c
// ✅ BON : Mutex séparés pour ressources indépendantes
int compteur_a = 0;  
pthread_mutex_t mutex_a = PTHREAD_MUTEX_INITIALIZER;  

int compteur_b = 0;  
pthread_mutex_t mutex_b = PTHREAD_MUTEX_INITIALIZER;  

void *modifier_a(void *arg) {
    pthread_mutex_lock(&mutex_a);
    compteur_a++;
    pthread_mutex_unlock(&mutex_a);
    return NULL;
}

void *modifier_b(void *arg) {
    pthread_mutex_lock(&mutex_b);  // Pas de conflit avec A
    compteur_b++;
    pthread_mutex_unlock(&mutex_b);
    return NULL;
}
```

### 4. Documenter les invariants

```c
// Compteur global
// Invariant : Toujours >= 0
// Protection : mutex_compteur
int compteur = 0;  
pthread_mutex_t mutex_compteur = PTHREAD_MUTEX_INITIALIZER;  

void incrementer(void) {
    pthread_mutex_lock(&mutex_compteur);
    compteur++;  // Maintient l'invariant
    pthread_mutex_unlock(&mutex_compteur);
}
```

### 5. Ordre de verrouillage cohérent

Quand vous devez prendre plusieurs mutex, utilisez **toujours le même ordre**.

```c
pthread_mutex_t mutex_a = PTHREAD_MUTEX_INITIALIZER;  
pthread_mutex_t mutex_b = PTHREAD_MUTEX_INITIALIZER;  

// ✅ BON : Toujours A puis B
void *thread1(void *arg) {
    pthread_mutex_lock(&mutex_a);
    pthread_mutex_lock(&mutex_b);
    // ...
    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);
    return NULL;
}

void *thread2(void *arg) {
    pthread_mutex_lock(&mutex_a);  // ← Même ordre
    pthread_mutex_lock(&mutex_b);
    // ...
    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);
    return NULL;
}
```

**Pourquoi ?** Éviter les deadlocks (section 18.7).

---

## Erreurs courantes et pièges

### 1. Oublier de déverrouiller

```c
// ❌ BUG : Mutex jamais libéré
void *thread_func(void *arg) {
    pthread_mutex_lock(&mutex);

    if (condition) {
        return NULL;  // ← OUBLI : unlock manquant !
    }

    pthread_mutex_unlock(&mutex);
    return NULL;
}

// Conséquence : Les autres threads sont bloqués indéfiniment (deadlock)
```

### 2. Verrouiller deux fois le même mutex

```c
// ❌ DEADLOCK : Le thread se bloque lui-même
pthread_mutex_lock(&mutex);  
pthread_mutex_lock(&mutex);  // ← Bloque indéfiniment  
pthread_mutex_unlock(&mutex);  
pthread_mutex_unlock(&mutex);  
```

### 3. Déverrouiller un mutex non verrouillé

```c
// ❌ ERREUR : Comportement indéfini
pthread_mutex_unlock(&mutex);  // Pas de lock() avant !
```

### 4. Déverrouiller dans le mauvais thread

```c
// ❌ ERREUR : Chaque thread doit unlock son propre lock
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread1(void *arg) {
    pthread_mutex_lock(&mutex);
    // ...
    return NULL;  // Ne déverrouille pas
}

void *thread2(void *arg) {
    pthread_mutex_unlock(&mutex);  // ❌ thread2 n'a pas fait le lock
    return NULL;
}
```

### 5. Accès non protégé

```c
// ❌ RACE CONDITION : Lecture non protégée
int valeur;  
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  

void *writer(void *arg) {
    pthread_mutex_lock(&mutex);
    valeur = 42;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *reader(void *arg) {
    printf("%d\n", valeur);  // ❌ Lecture sans lock !
    return NULL;
}

// ✅ CORRECT : Lecture aussi protégée
void *reader_correct(void *arg) {
    pthread_mutex_lock(&mutex);
    printf("%d\n", valeur);
    pthread_mutex_unlock(&mutex);
    return NULL;
}
```

### 6. Section critique trop grande

```c
// ❌ MAUVAIS : Tout est dans la section critique
pthread_mutex_lock(&mutex);  
sleep(5);              // Bloque les autres threads 5 secondes !  
variable = calcul();   // Calcul long  
ecrire_fichier();      // I/O lente  
pthread_mutex_unlock(&mutex);  

// ✅ BON : Minimiser la section
int result = calcul();        // Local  
ecrire_fichier_temp();        // I/O en dehors  
pthread_mutex_lock(&mutex);  
variable = result;             // Seulement l'écriture  
pthread_mutex_unlock(&mutex);  
```

---

## Performance et overhead

### Coût d'un mutex

Le verrouillage/déverrouillage d'un mutex a un **coût** :

**Cas non contesté** (mutex disponible) :
- ~25 nanosecondes sur CPU moderne
- Principalement des instructions CPU (atomic operations)

**Cas contesté** (mutex occupé) :
- ~1-2 microsecondes (appel système)
- Le thread est mis en sommeil
- Changement de contexte

### Exemple de mesure

```c
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define ITERATIONS 1000000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  
int compteur = 0;  

int main(void) {
    struct timespec start, end;

    // Sans mutex
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < ITERATIONS; i++) {
        compteur++;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    long ns_sans = (end.tv_sec - start.tv_sec) * 1000000000L +
                   (end.tv_nsec - start.tv_nsec);

    // Avec mutex
    compteur = 0;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);
        compteur++;
        pthread_mutex_unlock(&mutex);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    long ns_avec = (end.tv_sec - start.tv_sec) * 1000000000L +
                   (end.tv_nsec - start.tv_nsec);

    printf("Sans mutex : %ld ns (%.2f ns/op)\n",
           ns_sans, (double)ns_sans / ITERATIONS);
    printf("Avec mutex : %ld ns (%.2f ns/op)\n",
           ns_avec, (double)ns_avec / ITERATIONS);
    printf("Overhead   : %.2fx\n", (double)ns_avec / ns_sans);

    return 0;
}
```

**Sortie typique** :
```
Sans mutex : 2456789 ns (2.46 ns/op)  
Avec mutex : 25678901 ns (25.68 ns/op)  
Overhead   : 10.45x  
```

Le mutex est ~10x plus lent, mais reste très rapide en absolu (~25 ns).

### Quand optimiser

**Ne pas optimiser prématurément** ! Les mutex sont suffisamment rapides pour la plupart des cas.

**Optimiser si** :
- Contention très élevée (beaucoup de threads en attente)
- Section critique exécutée des millions de fois par seconde
- Profiling montre que les mutex sont un bottleneck

**Techniques d'optimisation** :
- Réduire la granularité (mutex par ressource)
- Utiliser des atomics pour opérations simples (section 18.10)
- Lock-free data structures (avancé)
- Read-write locks si beaucoup de lectures (section 18.11)

---

## Granularité du verrouillage

La **granularité** détermine combien de ressources sont protégées par un mutex.

### Granularité grossière (Coarse-grained)

**Un seul mutex** pour tout.

```c
// Granularité grossière
typedef struct {
    int compteur_a;
    int compteur_b;
    int compteur_c;
} Data;

Data data = {0, 0, 0};  
pthread_mutex_t mutex_global = PTHREAD_MUTEX_INITIALIZER;  

void incrementer_a(void) {
    pthread_mutex_lock(&mutex_global);
    data.compteur_a++;
    pthread_mutex_unlock(&mutex_global);
}

void incrementer_b(void) {
    pthread_mutex_lock(&mutex_global);  // ← Bloque même si A est modifié
    data.compteur_b++;
    pthread_mutex_unlock(&mutex_global);
}
```

**Avantages** :
- Simple à implémenter
- Moins de chance de deadlock
- Moins de mutex à gérer

**Inconvénients** :
- Faible concurrence
- Threads bloqués même pour ressources indépendantes

### Granularité fine (Fine-grained)

**Un mutex par ressource**.

```c
// Granularité fine
typedef struct {
    int compteur_a;
    pthread_mutex_t mutex_a;

    int compteur_b;
    pthread_mutex_t mutex_b;

    int compteur_c;
    pthread_mutex_t mutex_c;
} Data;

Data data;

void init_data(void) {
    data.compteur_a = 0;
    pthread_mutex_init(&data.mutex_a, NULL);

    data.compteur_b = 0;
    pthread_mutex_init(&data.mutex_b, NULL);

    data.compteur_c = 0;
    pthread_mutex_init(&data.mutex_c, NULL);
}

void incrementer_a(void) {
    pthread_mutex_lock(&data.mutex_a);
    data.compteur_a++;
    pthread_mutex_unlock(&data.mutex_a);
}

void incrementer_b(void) {
    pthread_mutex_lock(&data.mutex_b);  // ← Pas de conflit avec A
    data.compteur_b++;
    pthread_mutex_unlock(&data.mutex_b);
}
```

**Avantages** :
- Meilleure concurrence
- Threads ne se bloquent que si même ressource

**Inconvénients** :
- Plus complexe
- Risque de deadlock si plusieurs mutex
- Plus de mémoire (un mutex par ressource)

### Trouver le bon équilibre

```c
// Granularité moyenne : Mutex par groupe logique
typedef struct {
    // Groupe 1 : Compteurs
    int compteur_a;
    int compteur_b;
    pthread_mutex_t mutex_compteurs;

    // Groupe 2 : Statistiques
    double somme;
    int nombre;
    pthread_mutex_t mutex_stats;
} Data;
```

**Règle générale** :
- Débutez avec granularité grossière
- Affinez si profiling montre de la contention
- Groupez les ressources accédées ensemble

---

## Mutex multiples et complexité

### Exemple avec 2 mutex

```c
pthread_mutex_t mutex_source = PTHREAD_MUTEX_INITIALIZER;  
pthread_mutex_t mutex_dest = PTHREAD_MUTEX_INITIALIZER;  

int compte_source = 100;  
int compte_dest = 0;  

void transferer(int montant) {
    // Prendre les deux mutex dans un ordre cohérent
    pthread_mutex_lock(&mutex_source);
    pthread_mutex_lock(&mutex_dest);

    compte_source -= montant;
    compte_dest += montant;

    pthread_mutex_unlock(&mutex_dest);
    pthread_mutex_unlock(&mutex_source);
}
```

**Important** : Toujours prendre les mutex dans le **même ordre** pour éviter les deadlocks (voir section 18.7).

### Cas complexe : Fonction helper

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  
int valeur = 0;  

void modifier_valeur(int delta) {
    pthread_mutex_lock(&mutex);
    valeur += delta;
    pthread_mutex_unlock(&mutex);
}

void operation_complexe(void) {
    // ❌ PROBLÈME : Double lock si appel interne
    pthread_mutex_lock(&mutex);

    modifier_valeur(10);  // ← Tente de re-verrouiller !

    pthread_mutex_unlock(&mutex);
}

// ✅ SOLUTION : Version interne sans lock
void modifier_valeur_interne(int delta) {
    valeur += delta;  // Assume le mutex déjà pris
}

void modifier_valeur(int delta) {
    pthread_mutex_lock(&mutex);
    modifier_valeur_interne(delta);
    pthread_mutex_unlock(&mutex);
}

void operation_complexe(void) {
    pthread_mutex_lock(&mutex);
    modifier_valeur_interne(10);  // Version sans lock
    pthread_mutex_unlock(&mutex);
}
```

---

## Récapitulatif

Dans cette section, vous avez appris :

✅ **Les mutex** :
- Mécanisme d'exclusion mutuelle
- Un seul thread à la fois dans la section critique
- Type `pthread_mutex_t`

✅ **Sections critiques** :
- Portion de code accédant aux ressources partagées
- Doit être protégée par mutex
- À minimiser pour la performance

✅ **Initialisation** :
- Statique : `PTHREAD_MUTEX_INITIALIZER`
- Dynamique : `pthread_mutex_init()`

✅ **Utilisation** :
- `pthread_mutex_lock()` : Verrouiller (bloquant)
- `pthread_mutex_unlock()` : Déverrouiller
- `pthread_mutex_trylock()` : Tentative non bloquante
- `pthread_mutex_timedlock()` : Avec timeout

✅ **Destruction** :
- `pthread_mutex_destroy()` pour init dynamique

✅ **Bonnes pratiques** :
- Minimiser la section critique
- Toujours déverrouiller
- Un mutex par ressource logique
- Ordre cohérent pour mutex multiples

✅ **Erreurs à éviter** :
- Oublier unlock
- Double lock
- Déverrouiller sans avoir verrouillé
- Section critique trop grande

✅ **Performance** :
- Overhead ~10x mais absolu faible (~25 ns)
- Optimiser uniquement si nécessaire

✅ **Granularité** :
- Grossière : simple mais peu concurrent
- Fine : complexe mais très concurrent
- Trouver l'équilibre

---

## Pour aller plus loin

Dans les prochaines sections :

- **18.7** : Deadlocks (blocages mutuels entre threads)
- **18.8** : Variables de condition (synchronisation avancée)
- **18.9** : Sémaphores POSIX (contrôle d'accès multiple)
- **18.10** : Atomics C11 (opérations lock-free)
- **18.11** : Read-write locks (optimisation lecture/écriture)

**Conseil pratique** : Reprenez tous vos programmes multi-threadés précédents et ajoutez des mutex pour protéger les accès aux variables partagées. Testez avec ThreadSanitizer pour vérifier qu'il n'y a plus de race conditions.

**Projet** : Créez un programme avec un tableau partagé où plusieurs threads ajoutent des éléments simultanément. Protégez les accès avec un mutex et vérifiez que tous les éléments sont bien ajoutés.

---

📘 **Ressources utiles** :
- `man pthread_mutex_lock`
- `man pthread_mutex_init`
- `man pthread_mutex_destroy`
- Test de concurrence : `stress --cpu 4 --io 2 --vm 1`

💡 **Prochaine étape** : Section 18.7 - Deadlocks

**Citation** : *"Lock data, not code."* - Principe de conception multi-threadée

⏭️ [Deadlocks](/18-threads-et-concurrence/07-deadlocks.md)
