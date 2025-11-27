🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 18.5 Problèmes de concurrence : Race conditions

## Table des matières
- [Introduction](#introduction)
- [Qu'est-ce qu'une race condition ?](#quest-ce-quune-race-condition-)
- [Pourquoi les race conditions se produisent](#pourquoi-les-race-conditions-se-produisent)
- [Exemple classique : Le compteur partagé](#exemple-classique--le-compteur-partag%C3%A9)
- [Anatomie d'une race condition](#anatomie-dune-race-condition)
- [Types de race conditions](#types-de-race-conditions)
- [Conséquences des race conditions](#cons%C3%A9quences-des-race-conditions)
- [Détecter les race conditions](#d%C3%A9tecter-les-race-conditions)
- [Variables partagées vs variables locales](#variables-partag%C3%A9es-vs-variables-locales)
- [Opérations atomiques vs non-atomiques](#op%C3%A9rations-atomiques-vs-non-atomiques)
- [Exemples pratiques de race conditions](#exemples-pratiques-de-race-conditions)
- [Introduction aux solutions](#introduction-aux-solutions)
- [Bonnes pratiques de diagnostic](#bonnes-pratiques-de-diagnostic)

---

## Introduction

Bienvenue dans l'un des chapitres les plus importants de la programmation multi-threadée ! Les **race conditions** (conditions de course) sont le problème numéro 1 que vous rencontrerez avec les threads.

**Définition simple** : Une race condition se produit quand le résultat d'un programme dépend de l'ordre d'exécution de threads concurrents, et que cet ordre n'est pas contrôlé.

**Analogie** : Imaginez deux personnes essayant de mettre à jour le même document Word simultanément sans système de verrouillage. Qui gagne ? Quelle modification est conservée ? C'est le chaos !

Dans cette section, nous allons :
- Comprendre **pourquoi** les race conditions existent
- Voir des **exemples concrets** qui montrent le problème
- Apprendre à **détecter** ces bugs vicieux
- Introduire les **concepts** pour les résoudre (détaillés dans la section suivante)

---

## Qu'est-ce qu'une race condition ?

### Définition formelle

Une **race condition** se produit quand :

1. **Plusieurs threads** accèdent à une ressource partagée (variable, fichier, etc.)
2. **Au moins un thread modifie** cette ressource
3. **L'accès n'est pas synchronisé** (pas de protection)
4. **Le résultat dépend du timing** (ordre d'exécution imprévisible)

### Exemple du quotidien

Imaginez un compte bancaire partagé avec 100€ :

```
Vous (Thread 1)          Conjoint (Thread 2)
===============          ===================

Lire solde: 100€
Retirer 50€
Nouveau solde: 50€
                         Lire solde: 100€  ← LIT LA VIEILLE VALEUR !
                         Retirer 30€
                         Nouveau solde: 70€
Écrire: 50€
                         Écrire: 70€  ← ÉCRASE VOTRE RETRAIT !

Résultat final: 70€ au lieu de 20€ → 50€ ont disparu !
```

**C'est une race condition** : Le résultat dépend de qui lit/écrit en premier.

### En code C

```c
// Variable partagée entre threads
int solde = 100;

// Thread 1
void *retirer_50(void *arg) {
    int temp = solde;      // Lire
    temp = temp - 50;      // Calculer
    solde = temp;          // Écrire
    return NULL;
}

// Thread 2
void *retirer_30(void *arg) {
    int temp = solde;      // Lire
    temp = temp - 30;      // Calculer
    solde = temp;          // Écrire
    return NULL;
}

// Résultat imprévisible : peut-être 50, peut-être 70, peut-être 20 !
```

---

## Pourquoi les race conditions se produisent

### 1. Ordonnancement non déterministe

Le système d'exploitation (scheduler) décide **quand** chaque thread s'exécute. Vous ne pouvez **pas** prédire cet ordre :

```c
void *thread1(void *arg) {
    printf("A");
    return NULL;
}

void *thread2(void *arg) {
    printf("B");
    return NULL;
}

// Sorties possibles : AB, BA
// Vous ne pouvez pas garantir l'ordre !
```

### 2. Changement de contexte (Context Switch)

Le CPU peut **interrompre** un thread au milieu d'une opération pour exécuter un autre thread :

```
Thread 1                     Thread 2
========                     ========
x = x + 1;  ───┐
               │ Interrupted!
               │              x = x + 1;  ← Peut s'exécuter ici !
               │
               └→ Continue...
```

**Important** : Un changement de contexte peut survenir **n'importe quand**, même au milieu d'une instruction en C !

### 3. Opérations non-atomiques

La plupart des opérations en C ne sont **pas atomiques** (indivisibles). Par exemple, `x++` se décompose en 3 étapes :

```assembly
; x++ en assembleur (simplifié)
MOV  eax, [x]      ; 1. Lire x dans un registre
ADD  eax, 1        ; 2. Incrémenter le registre
MOV  [x], eax      ; 3. Écrire le résultat dans x
```

Un changement de contexte peut survenir **entre ces 3 instructions** !

### 4. Caches CPU et visibilité mémoire

Sur les systèmes multicœurs, chaque CPU a son propre cache. Les modifications d'un thread peuvent ne pas être **immédiatement visibles** aux autres threads :

```
CPU 1 (Thread 1)              CPU 2 (Thread 2)
================              ================
Cache: x = 5                  Cache: x = 5
x = 10
Cache: x = 10  ───────┐       Lit x
                      │       Cache: x = 5  ← Vieille valeur !
                      │
                      └─→ Synchronisation (éventuelle)
                              Cache: x = 10  ← Mise à jour
```

---

## Exemple classique : Le compteur partagé

### Code avec race condition

```c
#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 2
#define ITERATIONS 1000000

int compteur = 0;  // Variable globale partagée

void *incrementer(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        compteur++;  // ← RACE CONDITION !
    }
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];

    // Créer 2 threads qui incrémentent le compteur
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, incrementer, NULL);
    }

    // Attendre les threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Valeur attendue : %d\n", NUM_THREADS * ITERATIONS);
    printf("Valeur obtenue  : %d\n", compteur);

    return 0;
}
```

### Résultats possibles

**Exécution 1** :
```
Valeur attendue : 2000000
Valeur obtenue  : 1847392
```

**Exécution 2** :
```
Valeur attendue : 2000000
Valeur obtenue  : 1923871
```

**Exécution 3** :
```
Valeur attendue : 2000000
Valeur obtenue  : 2000000  ← Par chance !
```

**Pourquoi ?** Chaque exécution a un ordre d'ordonnancement différent, donc un résultat différent.

### Que se passe-t-il vraiment ?

Décomposons `compteur++` :

```c
compteur++;  // En réalité, c'est 3 opérations :

// 1. LIRE
temp = compteur;

// 2. MODIFIER
temp = temp + 1;

// 3. ÉCRIRE
compteur = temp;
```

**Scénario problématique** :

```
Temps    Thread 1                Thread 2           compteur
====     ========                ========           ========
t0       temp1 = compteur (0)                       0
t1                               temp2 = compteur (0)   0
t2       temp1 = temp1 + 1 (1)                      0
t3                               temp2 = temp2 + 1 (1)  0
t4       compteur = temp1 (1)                       1
t5                               compteur = temp2 (1)   1

Résultat : compteur = 1 au lieu de 2 !
Une incrémentation est perdue !
```

---

## Anatomie d'une race condition

### Les 3 ingrédients nécessaires

Pour qu'une race condition existe, il faut **les 3 conditions** suivantes :

```
┌─────────────────────────────────────┐
│ 1. Données partagées                │  ← Plusieurs threads accèdent
│    (variables globales, heap, etc.) │     à la même mémoire
└─────────────────────────────────────┘
              │
              ▼
┌─────────────────────────────────────┐
│ 2. Au moins une écriture            │  ← Un thread modifie les données
│    (modification des données)       │
└─────────────────────────────────────┘
              │
              ▼
┌─────────────────────────────────────┐
│ 3. Pas de synchronisation           │  ← Aucun mécanisme de protection
│    (accès non protégé)              │     (mutex, sémaphore, etc.)
└─────────────────────────────────────┘
```

**Si l'une des 3 conditions manque**, il n'y a **pas** de race condition :

- Pas de données partagées → Chaque thread a ses propres données → OK
- Que des lectures → Plusieurs threads peuvent lire sans problème → OK
- Synchronisation présente → Les accès sont ordonnés → OK

### Visualisation temporelle

```
Thread 1:  ──── LIRE ──── MODIFIER ──── ÉCRIRE ────
                 ↓           ↓            ↓
Thread 2:  ────────────── LIRE ─── MODIFIER ─── ÉCRIRE ──
                           ↑                      ↑
                           |                      └─ Écrase la valeur
                           └─ Lit une valeur périmée

                          RACE CONDITION !
```

---

## Types de race conditions

### 1. Read-Modify-Write (Lecture-Modification-Écriture)

Le plus courant. Exemple : `x++`, `x += 5`, `x = x * 2`

```c
int compteur = 0;

void *increment(void *arg) {
    compteur++;  // Lecture-Modification-Écriture
    return NULL;
}
```

**Problème** : Entre la lecture et l'écriture, un autre thread peut modifier la variable.

### 2. Check-Then-Act (Vérifier-Puis-Agir)

Vérifier une condition puis agir en fonction, mais la condition peut changer entre les deux.

```c
int stock = 10;

void *acheter(void *arg) {
    if (stock > 0) {          // ← VÉRIFIER
        sleep(1);             // Simulation de traitement
        stock--;              // ← AGIR
        printf("Achat OK\n");
    }
    return NULL;
}
```

**Problème** : Entre la vérification (`stock > 0`) et l'action (`stock--`), un autre thread peut modifier `stock`.

**Scénario** :
```
stock = 1

Thread 1: Vérifie (stock = 1 > 0) → Vrai
Thread 2: Vérifie (stock = 1 > 0) → Vrai
Thread 1: stock-- → stock = 0
Thread 2: stock-- → stock = -1 !  ← PROBLÈME !
```

### 3. Write-After-Write (Écriture après Écriture)

Deux threads écrivent sans coordination, l'un écrase l'autre.

```c
int resultat;

void *thread1(void *arg) {
    resultat = calcul1();  // Écrit
    return NULL;
}

void *thread2(void *arg) {
    resultat = calcul2();  // Écrit aussi
    return NULL;
}

// Lequel gagne ? Imprévisible !
```

### 4. Lost Update (Mise à jour perdue)

Variante de Read-Modify-Write où une mise à jour est complètement perdue.

```c
int solde = 100;

// Thread 1 : Ajoute 50
temp = solde;        // 100
temp = temp + 50;    // 150
solde = temp;        // 150

// Thread 2 : Enlève 30 (entrelaçé)
temp = solde;        // 100 (lit avant que T1 écrive)
temp = temp - 30;    // 70
solde = temp;        // 70 ← Écrase le +50 de T1 !

// Résultat : 70 au lieu de 120 (100 + 50 - 30)
```

---

## Conséquences des race conditions

### 1. Résultats incorrects

Le plus évident : Les calculs donnent de mauvais résultats.

```c
// Attendu : somme = 2000000
// Obtenu  : somme = 1847392
```

### 2. Corruption de données

Des structures de données deviennent incohérentes.

```c
typedef struct {
    int x;
    int y;
} Point;

Point p = {0, 0};

// Thread 1
p.x = 10;
p.y = 10;  // ← Interrupted ici

// Thread 2
printf("Point: (%d, %d)\n", p.x, p.y);
// Peut afficher : Point: (10, 0)  ← Incohérent !
```

### 3. Comportement non déterministe

Le programme produit des résultats différents à chaque exécution.

```bash
$ ./programme
Résultat: 1847392

$ ./programme
Résultat: 1923871

$ ./programme
Résultat: 2000000
```

**Problème majeur** : Impossible à déboguer de manière fiable !

### 4. Bugs difficiles à reproduire

Les race conditions sont **notoirement difficiles** à reproduire :

- Peuvent ne se manifester que sur certaines machines
- Peuvent disparaître quand on ajoute des `printf()` (changent le timing !)
- Peuvent ne survenir que sous forte charge
- Peuvent être masquées par le debugger

**Anecdote célèbre** : Le "Heisenbug" - un bug qui disparaît quand on essaie de l'observer !

### 5. Crash du programme

Dans les cas extrêmes, corruption de pointeurs ou d'index :

```c
int tableau[100];
int index = 0;

void *ajouter(void *arg) {
    tableau[index] = *(int *)arg;
    index++;  // ← RACE CONDITION
    return NULL;
}

// Si 2 threads incrémentent index simultanément :
// index peut sauter une valeur
// tableau[42] peut être écrit 2 fois
// tableau[43] jamais écrit
```

---

## Détecter les race conditions

### 1. Compilation avec warnings

Activez tous les warnings :

```bash
gcc -Wall -Wextra -pthread programme.c -o programme
```

Malheureusement, les warnings ne détectent **pas** les race conditions (le compilateur ne peut pas deviner l'ordonnancement runtime).

### 2. ThreadSanitizer (recommandé)

ThreadSanitizer (TSan) est un outil de détection de race conditions :

```bash
# Compilation avec TSan
gcc -fsanitize=thread -g -O1 -pthread programme.c -o programme

# Exécution
./programme
```

**Exemple de sortie** :

```
==================
WARNING: ThreadSanitizer: data race (pid=12345)
  Write of size 4 at 0x7b0400000000 by thread T2:
    #0 incrementer programme.c:12

  Previous write of size 4 at 0x7b0400000000 by thread T1:
    #0 incrementer programme.c:12

SUMMARY: ThreadSanitizer: data race programme.c:12 in incrementer
==================
```

**TSan vous dit** :
- Qu'il y a une race condition
- À quelle ligne de code
- Quels threads sont impliqués

### 3. Helgrind (Valgrind)

Helgrind est un outil de Valgrind pour détecter les problèmes de threading :

```bash
valgrind --tool=helgrind ./programme
```

**Exemple de sortie** :

```
==12345== Possible data race during write of size 4 at 0x30A010 by thread #2
==12345== Locks held: none
==12345==    at 0x400736: incrementer (programme.c:12)
==12345==
==12345== This conflicts with a previous write of size 4 by thread #1
==12345== Locks held: none
==12345==    at 0x400736: incrementer (programme.c:12)
```

### 4. Analyse statique

Des outils comme `cppcheck` ou `clang-tidy` peuvent détecter certains patterns dangereux :

```bash
cppcheck --enable=all --force programme.c
```

### 5. Tests de charge

Exécutez votre programme **plusieurs fois** avec un grand nombre d'itérations :

```bash
#!/bin/bash
for i in {1..100}; do
    echo "Exécution $i"
    ./programme
done
```

Si vous obtenez des résultats différents, vous avez probablement une race condition.

### 6. Inspection manuelle du code

Recherchez les patterns suivants :

```c
// ❌ Variable globale modifiée sans protection
int compteur = 0;
compteur++;  // Dans un thread

// ❌ Check-then-act
if (condition) {
    // Action basée sur condition
}

// ❌ Opérations non atomiques
x = x + 1;
array[index++] = value;
```

---

## Variables partagées vs variables locales

### Variables locales : SAFE

Les variables **locales** à un thread (dans la pile) sont **toujours sûres** :

```c
void *thread_func(void *arg) {
    int compteur = 0;  // ← Variable locale = pile du thread

    for (int i = 0; i < 1000000; i++) {
        compteur++;  // ✅ PAS de race condition
    }

    printf("Compteur local : %d\n", compteur);
    return NULL;
}

int main(void) {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, thread_func, NULL);
    pthread_create(&t2, NULL, thread_func, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Chaque thread a son propre compteur
    return 0;
}
```

**Résultat** : Toujours correct, car chaque thread a sa propre variable `compteur`.

### Variables globales/statiques : DANGER

Les variables **globales** ou **statiques** sont partagées entre tous les threads :

```c
int compteur_global = 0;  // ← Partagée entre tous les threads

void *thread_func(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        compteur_global++;  // ❌ RACE CONDITION
    }
    return NULL;
}
```

### Variables allouées dynamiquement

Dépend de **qui y accède** :

```c
// ✅ OK : Chaque thread a son propre malloc
void *thread_func(void *arg) {
    int *data = malloc(sizeof(int));
    *data = 42;
    // Seul ce thread accède à data
    free(data);
    return NULL;
}

// ❌ DANGER : Plusieurs threads partagent le même malloc
int *shared_data = malloc(sizeof(int));
*shared_data = 0;

void *thread_func(void *arg) {
    (*shared_data)++;  // ← RACE CONDITION
    return NULL;
}
```

### Tableau comparatif

| Type de variable | Localisation | Partagée ? | Race condition possible ? |
|------------------|--------------|-----------|---------------------------|
| Locale (auto) | Pile du thread | ❌ Non | ❌ Non |
| Paramètre fonction | Pile du thread | ❌ Non | ❌ Non |
| Statique locale | Data segment | ✅ Oui | ✅ Oui |
| Globale | Data segment | ✅ Oui | ✅ Oui |
| malloc (unique) | Heap | ❌ Non | ❌ Non |
| malloc (partagé) | Heap | ✅ Oui | ✅ Oui |

---

## Opérations atomiques vs non-atomiques

### Qu'est-ce qu'une opération atomique ?

Une opération **atomique** est une opération qui :
- S'exécute **complètement** ou **pas du tout**
- **Ne peut pas être interrompue** à mi-chemin
- Est **visible instantanément** aux autres threads

**Analogie** : Un atome est indivisible. Une opération atomique aussi.

### Opérations NON atomiques en C (la plupart)

```c
// ❌ NON atomique (3 instructions CPU)
compteur++;

// ❌ NON atomique
compteur += 5;

// ❌ NON atomique
if (x > 0) x--;

// ❌ NON atomique (même avec = !)
long long x = 123456789;  // Peut prendre plusieurs cycles CPU

// ❌ NON atomique
struct Point p;
p.x = 10;  // ← Peut être interrompu ici
p.y = 20;
```

### Opérations potentiellement atomiques

**Sur la plupart des architectures modernes** :

```c
// ✅ Lecture d'un int aligné (généralement atomique)
int x = variable;

// ✅ Écriture d'un int aligné (généralement atomique)
variable = 42;

// ⚠️ ATTENTION : "généralement" ≠ "toujours"
// L'atomicité dépend de l'architecture, de l'alignement, de la taille...
```

**Important** : Ne comptez **jamais** sur l'atomicité implicite ! Utilisez toujours une synchronisation explicite.

### Opérations vraiment atomiques (C11)

Depuis C11, il existe `<stdatomic.h>` :

```c
#include <stdatomic.h>

atomic_int compteur = 0;

void *thread_func(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        atomic_fetch_add(&compteur, 1);  // ✅ Vraiment atomique
    }
    return NULL;
}
```

Nous verrons cela en détail dans la section 18.10.

---

## Exemples pratiques de race conditions

### Exemple 1 : Liste chaînée partagée

```c
typedef struct Node {
    int data;
    struct Node *next;
} Node;

Node *head = NULL;  // Liste partagée

void *ajouter_element(void *arg) {
    int valeur = *(int *)arg;

    // ❌ RACE CONDITION sur plusieurs lignes
    Node *nouveau = malloc(sizeof(Node));
    nouveau->data = valeur;
    nouveau->next = head;  // ← Peut changer entre ici...
    head = nouveau;        // ← ...et ici !

    return NULL;
}
```

**Problème** : Entre la lecture de `head` et l'affectation, un autre thread peut modifier `head`. Résultat : perte d'éléments.

### Exemple 2 : Compte bancaire

```c
double solde = 1000.0;

void *retirer(void *arg) {
    double montant = *(double *)arg;

    // ❌ RACE CONDITION : Check-then-act
    if (solde >= montant) {
        sleep(1);  // Simule vérifications
        solde -= montant;
        printf("Retrait de %.2f OK\n", montant);
    } else {
        printf("Solde insuffisant\n");
    }

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

    printf("Solde final : %.2f\n", solde);
    // Peut être négatif !

    return 0;
}
```

**Résultat possible** :
```
Retrait de 600.00 OK
Retrait de 600.00 OK
Solde final : -200.00  ← Découvert non autorisé !
```

### Exemple 3 : Statistiques

```c
typedef struct {
    long total;
    int count;
    double moyenne;
} Stats;

Stats stats = {0, 0, 0.0};

void *ajouter_valeur(void *arg) {
    int valeur = *(int *)arg;

    // ❌ RACE CONDITION sur plusieurs variables
    stats.total += valeur;
    stats.count++;
    stats.moyenne = (double)stats.total / stats.count;

    return NULL;
}
```

**Problème** : Un autre thread peut lire entre les 3 affectations et voir un état incohérent :
```
Thread 1: total = 100, count = 5, moyenne = 20.0
Thread 2 commence...
Thread 2: total += 50 → total = 150
[Thread 3 lit ici: total=150, count=5, moyenne=20.0 ← Incohérent !]
Thread 2: count++ → count = 6
Thread 2: moyenne = 150/6 = 25.0
```

### Exemple 4 : Index de tableau

```c
int tableau[1000];
int prochain_index = 0;

void *ajouter(void *arg) {
    int valeur = *(int *)arg;

    // ❌ RACE CONDITION
    tableau[prochain_index] = valeur;  // ← Deux threads peuvent utiliser
    prochain_index++;                  //   le même index !

    return NULL;
}
```

**Problème** : Deux threads peuvent écrire au même index, perdant une valeur.

---

## Introduction aux solutions

Les race conditions sont un problème sérieux, mais il existe des solutions !

### 1. Mutex (Mutual Exclusion)

Un **mutex** (verrou) garantit qu'un seul thread à la fois peut accéder à une section critique :

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *incrementer(void *arg) {
    pthread_mutex_lock(&mutex);    // ← Verrouiller
    compteur++;                     // Section critique
    pthread_mutex_unlock(&mutex);  // ← Déverrouiller
    return NULL;
}
```

**Nous verrons cela en détail dans la section 18.6.**

### 2. Variables de condition

Pour attendre qu'une condition soit remplie :

```c
pthread_cond_t cond;
pthread_mutex_t mutex;

// Thread 1 : Attend
pthread_mutex_lock(&mutex);
while (!condition) {
    pthread_cond_wait(&cond, &mutex);
}
pthread_mutex_unlock(&mutex);

// Thread 2 : Signal
pthread_mutex_lock(&mutex);
condition = 1;
pthread_cond_signal(&cond);
pthread_mutex_unlock(&mutex);
```

**Détaillé dans la section 18.8.**

### 3. Sémaphores

Pour limiter le nombre de threads accédant à une ressource :

```c
sem_t semaphore;
sem_init(&semaphore, 0, 3);  // Max 3 threads simultanés

void *utiliser_ressource(void *arg) {
    sem_wait(&semaphore);    // Décrémenter (bloque si 0)
    // Utiliser la ressource
    sem_post(&semaphore);    // Incrémenter
    return NULL;
}
```

**Détaillé dans la section 18.9.**

### 4. Opérations atomiques (C11)

Pour les opérations simples, utiliser `<stdatomic.h>` :

```c
#include <stdatomic.h>

atomic_int compteur = 0;

void *incrementer(void *arg) {
    atomic_fetch_add(&compteur, 1);  // Atomique, pas de mutex
    return NULL;
}
```

**Détaillé dans la section 18.10.**

### 5. Design sans partage

La meilleure solution : **éviter le partage** quand possible !

```c
// Au lieu de partager un compteur global...
int compteur_global = 0;

// Chaque thread a son compteur local
void *worker(void *arg) {
    int compteur_local = 0;
    for (int i = 0; i < 1000000; i++) {
        compteur_local++;
    }

    // Fusionner à la fin (avec mutex si nécessaire)
    int *result = malloc(sizeof(int));
    *result = compteur_local;
    return result;
}
```

---

## Bonnes pratiques de diagnostic

### 1. Activer tous les warnings

```bash
gcc -Wall -Wextra -Werror -pthread programme.c -o programme
```

### 2. Utiliser ThreadSanitizer en développement

```bash
# Pendant le développement
gcc -fsanitize=thread -g -O1 -pthread programme.c -o programme_tsan
./programme_tsan

# En production (sans sanitizer)
gcc -O2 -pthread programme.c -o programme
```

### 3. Tests avec différents timings

```c
// Ajouter des sleep() aléatoires pour changer le timing
#include <unistd.h>
#include <stdlib.h>

void *thread_func(void *arg) {
    usleep(rand() % 1000);  // Sleep 0-1ms aléatoire
    // Code...
}
```

### 4. Exécutions multiples

```bash
#!/bin/bash
echo "Test de race conditions - 100 exécutions"
for i in {1..100}; do
    result=$(./programme | grep "Résultat" | awk '{print $2}')
    echo "Run $i: $result"
done | sort | uniq -c
```

**Si vous voyez plusieurs résultats différents, vous avez une race condition.**

### 5. Documentation des zones critiques

```c
// ⚠️ SECTION CRITIQUE : Accès à compteur_global
// Protéger avec mutex_compteur avant modification
int compteur_global = 0;
pthread_mutex_t mutex_compteur = PTHREAD_MUTEX_INITIALIZER;
```

### 6. Code review

Faites relire votre code par un pair, en se concentrant sur :
- Variables globales/statiques
- Opérations non atomiques
- Check-then-act patterns
- Absence de synchronisation

---

## Récapitulatif

Dans cette section, vous avez appris :

✅ **Qu'est-ce qu'une race condition** :
- Résultat dépendant de l'ordre d'exécution
- Accès concurrent non synchronisé
- Modification de données partagées

✅ **Pourquoi elles se produisent** :
- Ordonnancement non déterministe
- Changements de contexte
- Opérations non atomiques
- Caches CPU

✅ **Types de race conditions** :
- Read-Modify-Write
- Check-Then-Act
- Write-After-Write
- Lost Update

✅ **Conséquences** :
- Résultats incorrects
- Corruption de données
- Bugs non reproductibles
- Crashes possibles

✅ **Détection** :
- ThreadSanitizer (recommandé)
- Helgrind (Valgrind)
- Tests de charge
- Inspection manuelle

✅ **Variables sûres vs dangereuses** :
- Locales : sûres
- Globales/statiques : dangereuses
- Heap : dépend du partage

✅ **Opérations atomiques** :
- La plupart des opérations C ne sont PAS atomiques
- Utiliser `<stdatomic.h>` (C11) ou mutex

✅ **Solutions** (introduites) :
- Mutex (section suivante)
- Variables de condition
- Sémaphores
- Atomics
- Design sans partage

---

## Points clés à retenir

🔴 **Les race conditions sont difficiles à déboguer** : Elles sont non déterministes et peuvent disparaître quand on essaie de les observer.

🔴 **Ne comptez jamais sur la "chance"** : Si votre programme fonctionne parfois, ce n'est pas suffisant. Il doit fonctionner **toujours**.

🔴 **Tout accès concurrent à une donnée partagée modifiable doit être protégé** : Pas d'exception.

🔴 **Les race conditions ne sont pas détectées par le compilateur** : C'est un problème runtime qui nécessite des outils spécifiques.

🟢 **ThreadSanitizer est votre meilleur ami** : Utilisez-le systématiquement pendant le développement.

🟢 **La meilleure race condition est celle qu'on évite** : Concevez votre architecture pour minimiser le partage.

---

## Pour aller plus loin

Dans les prochaines sections :

- **18.6** : Mutex et sections critiques (LA solution principale aux race conditions)
- **18.7** : Deadlocks (un nouveau problème introduit par les mutex !)
- **18.8** : Variables de condition (synchronisation avancée)
- **18.9** : Sémaphores POSIX (contrôle d'accès)
- **18.10** : Atomics C11 (operations lock-free)

**Conseil pratique** : Prenez le temps de bien comprendre cette section avant de continuer. Les race conditions sont le problème fondamental du multithreading, et comprendre **pourquoi** elles se produisent est essentiel pour apprendre à les éviter.

**Exercice mental** : Regardez le code que vous avez écrit jusqu'ici et identifiez où des race conditions pourraient survenir. Même sans exécuter le code, essayez de visualiser les scénarios d'entrelacement problématiques.

---

📘 **Ressources utiles** :
- ThreadSanitizer : `gcc -fsanitize=thread`
- Helgrind : `valgrind --tool=helgrind`
- Article de référence : "The Art of Multiprocessor Programming"

💡 **Prochaine étape** : Section 18.6 - Mutex et sections critiques

**Citation** : *"In parallel programming, the race is not always to the swift, but sometimes to the lucky. Don't rely on luck."* - Adapté de Ecclesiastes 9:11

⏭️ [Mutex et sections critiques](/18-threads-et-concurrence/06-mutex.md)
