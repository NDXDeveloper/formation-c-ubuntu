🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 18.13 Barrières de threads

## Table des matières
- [Introduction](#introduction)
- [Le problème de la synchronisation par phases](#le-probl%C3%A8me-de-la-synchronisation-par-phases)
- [Qu'est-ce qu'une barrière ?](#quest-ce-quune-barri%C3%A8re-)
- [API pthread_barrier_t](#api-pthread_barrier_t)
- [Exemple simple](#exemple-simple)
- [Valeur de retour spéciale](#valeur-de-retour-sp%C3%A9ciale)
- [Calculs parallèles par phases](#calculs-parall%C3%A8les-par-phases)
- [Barrières réutilisables](#barri%C3%A8res-r%C3%A9utilisables)
- [Implémentation manuelle d'une barrière](#impl%C3%A9mentation-manuelle-dune-barri%C3%A8re)
- [Patterns avancés](#patterns-avanc%C3%A9s)
- [Barrières vs autres mécanismes](#barri%C3%A8res-vs-autres-m%C3%A9canismes)
- [Performance et considérations](#performance-et-consid%C3%A9rations)
- [Erreurs courantes](#erreurs-courantes)
- [Quand utiliser les barrières ?](#quand-utiliser-les-barri%C3%A8res-)

---

## Introduction

Dans les programmes parallèles, il est souvent nécessaire de **synchroniser** plusieurs threads à un point précis avant de continuer. C'est comme attendre que tous les membres d'une équipe soient prêts avant de commencer une activité.

**Analogie du sport** :
- Dans une course de relais, chaque coureur doit attendre que tous les coureurs de son équipe aient fini leur étape avant de passer à la suivante
- C'est exactement ce que fait une **barrière de threads**

**Exemple concret** :
```
Phase 1 : Chargement des données
↓
BARRIÈRE (attendre que tous aient chargé)
↓
Phase 2 : Traitement des données
↓
BARRIÈRE (attendre que tous aient fini)
↓
Phase 3 : Affichage des résultats
```

Dans cette section, nous allons apprendre :
- Ce qu'est une barrière de threads
- Comment utiliser `pthread_barrier_t`
- Les patterns classiques avec barrières
- Implémenter une barrière manuellement

---

## Le problème de la synchronisation par phases

### Scénario : Calcul parallèle en plusieurs étapes

Imaginons un programme qui calcule la température d'une grille en 2D, étape par étape :

```c
#define SIZE 100
#define NUM_THREADS 4

double grille[SIZE][SIZE];  
double nouvelle_grille[SIZE][SIZE];  

void *calculer_region(void *arg) {
    int id = *(int *)arg;
    int debut = id * (SIZE / NUM_THREADS);
    int fin = (id + 1) * (SIZE / NUM_THREADS);

    for (int iteration = 0; iteration < 1000; iteration++) {
        // Phase 1 : Calculer nouvelle valeur
        for (int i = debut; i < fin; i++) {
            for (int j = 0; j < SIZE; j++) {
                nouvelle_grille[i][j] = calculer_temperature(i, j);
            }
        }

        // ⚠️ PROBLÈME : Il faut attendre que TOUS les threads
        // aient fini Phase 1 avant de copier !

        // Phase 2 : Copier nouvelle vers ancienne
        for (int i = debut; i < fin; i++) {
            for (int j = 0; j < SIZE; j++) {
                grille[i][j] = nouvelle_grille[i][j];
            }
        }

        // ⚠️ PROBLÈME : Il faut attendre que TOUS les threads
        // aient copié avant de recommencer Phase 1 !
    }

    return NULL;
}
```

**Problème sans synchronisation** :
```
Thread 1: Phase 1 (rapide) → Phase 2 → Phase 1 de nouveau  
Thread 2: Phase 1 (lent)   → Toujours en Phase 1  
                            ↑
                Thread 1 lit des données pas encore calculées !
```

### Tentative avec mutex (❌ Insuffisant)

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  
int threads_finis_phase1 = 0;  

void *calculer_region(void *arg) {
    // Phase 1
    // ...

    // Attendre les autres (❌ Ne fonctionne pas bien)
    pthread_mutex_lock(&mutex);
    threads_finis_phase1++;
    pthread_mutex_unlock(&mutex);

    while (threads_finis_phase1 < NUM_THREADS) {
        // Busy-waiting → Gaspillage CPU
    }

    // Phase 2
    // ...
}
```

**Problèmes** :
- Busy-waiting (gaspillage CPU)
- Comment réinitialiser le compteur pour l'itération suivante ?
- Complexité croissante avec plusieurs phases

### Solution : Barrières (✅)

```c
pthread_barrier_t barriere;

void *calculer_region(void *arg) {
    for (int iteration = 0; iteration < 1000; iteration++) {
        // Phase 1
        // ...

        pthread_barrier_wait(&barriere);  // ✅ Attente efficace

        // Phase 2
        // ...

        pthread_barrier_wait(&barriere);  // ✅ Point de synchronisation
    }
    return NULL;
}
```

---

## Qu'est-ce qu'une barrière ?

### Définition

Une **barrière de threads** est un point de synchronisation où tous les threads doivent arriver avant que l'un d'eux puisse continuer.

**Principe** :
1. Chaque thread appelle `pthread_barrier_wait()`
2. Les N-1 premiers threads sont **bloqués**
3. Quand le Nième thread arrive, **tous sont libérés** simultanément
4. La barrière est automatiquement réinitialisée pour la prochaine utilisation

### Visualisation

```
Threads en exécution :
======================

Thread 1:  ════════════╗
                       ║
Thread 2:  ════════╗   ║
                   ║   ║
Thread 3:  ══════╗ ║   ║
                 ║ ║   ║
Thread 4:  ════╗ ║ ║   ║
               ║ ║ ║   ║
               ▼ ▼ ▼   ▼
           ┌─────────────┐
           │  BARRIÈRE   │ ← Tous attendent
           │             │
           │ 4 threads   │
           │ attendus    │
           └─────────────┘
                 │
      Tous arrivent simultanément
                 │
                 ▼
           Libération !
                 ║
Thread 1:        ║══════════════►  
Thread 2:        ║══════════════►  
Thread 3:        ║══════════════►  
Thread 4:        ║══════════════►  
```

### Comparaison avec autres mécanismes

**Sémaphore** : Compteur qui autorise N accès
```
Thread appelle sem_wait() → Si compteur > 0 : continue immédiatement
```

**Barrière** : Rendez-vous qui attend N threads
```
Thread appelle barrier_wait() → Bloque jusqu'à ce que N threads arrivent
```

**Analogie** :
- **Sémaphore** : Péage d'autoroute (N voitures peuvent passer)
- **Barrière** : Point de rassemblement (attend tout le groupe)

---

## API pthread_barrier_t

### Type

```c
#include <pthread.h>

pthread_barrier_t barriere;
```

### Initialisation

```c
int pthread_barrier_init(pthread_barrier_t *barrier,
                        const pthread_barrierattr_t *attr,
                        unsigned int count);
```

**Paramètres** :
- `barrier` : Pointeur vers la barrière à initialiser
- `attr` : Attributs (généralement NULL)
- `count` : **Nombre de threads** qui doivent atteindre la barrière

**Retour** : 0 en cas de succès, code d'erreur sinon

**Exemple** :
```c
pthread_barrier_t barriere;

// Initialiser pour 4 threads
if (pthread_barrier_init(&barriere, NULL, 4) != 0) {
    perror("pthread_barrier_init");
    exit(EXIT_FAILURE);
}
```

### Attente à la barrière

```c
int pthread_barrier_wait(pthread_barrier_t *barrier);
```

**Comportement** :
1. Le thread appelle `pthread_barrier_wait()`
2. Si moins de N threads ont appelé : **bloque**
3. Si c'est le Nième thread : **tous sont libérés**
4. La barrière est automatiquement réinitialisée

**Retour** :
- `0` : Pour N-1 threads
- `PTHREAD_BARRIER_SERIAL_THREAD` : Pour **un seul** thread (le "gagnant")

**Exemple** :
```c
void *thread_func(void *arg) {
    printf("Thread %lu : Avant barrière\n", pthread_self());

    pthread_barrier_wait(&barriere);  // Attendre les autres

    printf("Thread %lu : Après barrière\n", pthread_self());
    return NULL;
}
```

### Destruction

```c
int pthread_barrier_destroy(pthread_barrier_t *barrier);
```

**Règles** :
- Appeler après que tous les threads ont fini d'utiliser la barrière
- La barrière ne doit avoir aucun thread en attente

**Exemple** :
```c
// Attendre tous les threads
for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
}

// Détruire la barrière
pthread_barrier_destroy(&barriere);
```

---

## Exemple simple

### Programme complet

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 5

pthread_barrier_t barriere;

void *worker(void *arg) {
    int id = *(int *)arg;

    // Phase 1 : Préparation (durée variable)
    printf("[Thread %d] Phase 1 : Préparation...\n", id);
    sleep(id);  // Simule travail de durée différente
    printf("[Thread %d] Phase 1 terminée\n", id);

    // BARRIÈRE : Attendre que tous aient fini Phase 1
    printf("[Thread %d] Attente à la barrière...\n", id);
    pthread_barrier_wait(&barriere);

    // Phase 2 : Traitement (tous ensemble)
    printf("[Thread %d] Phase 2 : Traitement\n", id);

    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    // Initialiser la barrière pour NUM_THREADS threads
    pthread_barrier_init(&barriere, NULL, NUM_THREADS);

    printf("=== Démarrage de %d threads ===\n", NUM_THREADS);

    // Créer les threads
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, worker, &ids[i]);
    }

    // Attendre les threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Détruire la barrière
    pthread_barrier_destroy(&barriere);

    printf("=== Programme terminé ===\n");

    return 0;
}
```

**Sortie** :
```
=== Démarrage de 5 threads ===
[Thread 0] Phase 1 : Préparation...
[Thread 1] Phase 1 : Préparation...
[Thread 2] Phase 1 : Préparation...
[Thread 3] Phase 1 : Préparation...
[Thread 4] Phase 1 : Préparation...
[Thread 0] Phase 1 terminée
[Thread 0] Attente à la barrière...
[Thread 1] Phase 1 terminée
[Thread 1] Attente à la barrière...
[Thread 2] Phase 1 terminée
[Thread 2] Attente à la barrière...
[Thread 3] Phase 1 terminée
[Thread 3] Attente à la barrière...
[Thread 4] Phase 1 terminée
[Thread 4] Attente à la barrière...
[Thread 0] Phase 2 : Traitement  ← Tous libérés ensemble
[Thread 1] Phase 2 : Traitement
[Thread 2] Phase 2 : Traitement
[Thread 3] Phase 2 : Traitement
[Thread 4] Phase 2 : Traitement
=== Programme terminé ===
```

**Observation** : Bien que les threads finissent Phase 1 à des moments différents, ils commencent **tous** Phase 2 en même temps.

---

## Valeur de retour spéciale

### PTHREAD_BARRIER_SERIAL_THREAD

Quand tous les threads arrivent à la barrière, **un seul** thread reçoit la valeur de retour `PTHREAD_BARRIER_SERIAL_THREAD`. Les autres reçoivent 0.

**Utilité** : Permet à un thread de faire une tâche unique (comme afficher un résumé).

### Exemple

```c
void *worker(void *arg) {
    int id = *(int *)arg;

    // Travail Phase 1
    printf("[Thread %d] Travaille...\n", id);
    sleep(1);

    // Barrière
    int result = pthread_barrier_wait(&barriere);

    if (result == PTHREAD_BARRIER_SERIAL_THREAD) {
        // Ce thread est le "gagnant"
        printf("\n=== [Thread %d] Je suis le thread sériel ===\n", id);
        printf("=== Tous les threads ont passé la barrière ===\n\n");
    } else if (result == 0) {
        // Thread normal
        printf("[Thread %d] Passé la barrière\n", id);
    } else {
        // Erreur
        fprintf(stderr, "[Thread %d] Erreur barrière\n", id);
    }

    return NULL;
}
```

**Sortie typique** :
```
[Thread 0] Travaille...
[Thread 1] Travaille...
[Thread 2] Travaille...
[Thread 3] Travaille...

=== [Thread 2] Je suis le thread sériel ===
=== Tous les threads ont passé la barrière ===

[Thread 0] Passé la barrière
[Thread 1] Passé la barrière
[Thread 3] Passé la barrière
```

**Note** : Le thread "sériel" est choisi arbitrairement par l'implémentation.

### Use case : Statistiques

```c
int compteurs[NUM_THREADS];

void *worker(void *arg) {
    int id = *(int *)arg;

    // Chaque thread fait son travail
    compteurs[id] = faire_travail();

    // Barrière
    int result = pthread_barrier_wait(&barriere);

    if (result == PTHREAD_BARRIER_SERIAL_THREAD) {
        // Un seul thread calcule et affiche le total
        int total = 0;
        for (int i = 0; i < NUM_THREADS; i++) {
            total += compteurs[i];
        }
        printf("Total : %d\n", total);
    }

    return NULL;
}
```

---

## Calculs parallèles par phases

### Exemple : Simulation itérative

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define GRID_SIZE 100
#define NUM_THREADS 4
#define ITERATIONS 10

double grille[GRID_SIZE][GRID_SIZE];  
double nouvelle_grille[GRID_SIZE][GRID_SIZE];  
pthread_barrier_t barriere;  

void initialiser_grille(void) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grille[i][j] = (double)rand() / RAND_MAX * 100.0;
        }
    }
}

double calculer_moyenne(int i, int j) {
    // Moyenne des voisins (simulation simple)
    double somme = 0.0;
    int count = 0;

    for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
            int ni = i + di;
            int nj = j + dj;

            if (ni >= 0 && ni < GRID_SIZE &&
                nj >= 0 && nj < GRID_SIZE) {
                somme += grille[ni][nj];
                count++;
            }
        }
    }

    return somme / count;
}

void *simuler_region(void *arg) {
    int id = *(int *)arg;
    int debut = id * (GRID_SIZE / NUM_THREADS);
    int fin = (id + 1) * (GRID_SIZE / NUM_THREADS);

    for (int iter = 0; iter < ITERATIONS; iter++) {
        // Phase 1 : Calculer nouvelles valeurs
        for (int i = debut; i < fin; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                nouvelle_grille[i][j] = calculer_moyenne(i, j);
            }
        }

        // BARRIÈRE : Attendre que tous aient calculé
        pthread_barrier_wait(&barriere);

        // Phase 2 : Copier nouvelle vers ancienne
        for (int i = debut; i < fin; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                grille[i][j] = nouvelle_grille[i][j];
            }
        }

        // BARRIÈRE : Attendre que tous aient copié
        int result = pthread_barrier_wait(&barriere);

        // Un seul thread affiche
        if (result == PTHREAD_BARRIER_SERIAL_THREAD) {
            printf("Itération %d terminée\n", iter + 1);
        }
    }

    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    initialiser_grille();

    pthread_barrier_init(&barriere, NULL, NUM_THREADS);

    printf("Démarrage simulation (%dx%d, %d threads)\n",
           GRID_SIZE, GRID_SIZE, NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, simuler_region, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barriere);

    printf("Simulation terminée\n");

    return 0;
}
```

**Sortie** :
```
Démarrage simulation (100x100, 4 threads)  
Itération 1 terminée  
Itération 2 terminée  
Itération 3 terminée  
...
Itération 10 terminée  
Simulation terminée  
```

---

## Barrières réutilisables

### Réutilisation automatique

Une barrière est **automatiquement réinitialisée** après chaque synchronisation :

```c
void *worker(void *arg) {
    for (int i = 0; i < 100; i++) {
        // Travail

        pthread_barrier_wait(&barriere);  // ✅ Réutilisable
    }
    return NULL;
}
```

**Pas besoin** de réinitialiser manuellement entre chaque utilisation.

### Exemple : Pipeline à 3 étapes

```c
pthread_barrier_t barriere1, barriere2;

void *worker(void *arg) {
    int id = *(int *)arg;

    for (int iteration = 0; iteration < 10; iteration++) {
        // Étape 1
        printf("[%d] Étape 1\n", id);
        pthread_barrier_wait(&barriere1);

        // Étape 2
        printf("[%d] Étape 2\n", id);
        pthread_barrier_wait(&barriere2);

        // Étape 3
        printf("[%d] Étape 3\n", id);
    }

    return NULL;
}

int main(void) {
    pthread_t threads[4];
    int ids[4];

    pthread_barrier_init(&barriere1, NULL, 4);
    pthread_barrier_init(&barriere2, NULL, 4);

    for (int i = 0; i < 4; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, worker, &ids[i]);
    }

    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barriere1);
    pthread_barrier_destroy(&barriere2);

    return 0;
}
```

---

## Implémentation manuelle d'une barrière

### Avec mutex et condition variable

Pour comprendre comment fonctionne une barrière, implémentons-la manuellement :

```c
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;        // Nombre de threads attendus
    int arrived;      // Nombre de threads arrivés
    int generation;   // Numéro de génération (pour réutilisation)
} Barrier;

void barrier_init(Barrier *b, int count) {
    pthread_mutex_init(&b->mutex, NULL);
    pthread_cond_init(&b->cond, NULL);
    b->count = count;
    b->arrived = 0;
    b->generation = 0;
}

void barrier_wait(Barrier *b) {
    pthread_mutex_lock(&b->mutex);

    int my_generation = b->generation;  // Mémoriser la génération

    b->arrived++;

    if (b->arrived == b->count) {
        // Dernier thread arrivé
        b->arrived = 0;      // Réinitialiser pour la prochaine fois
        b->generation++;     // Incrémenter la génération
        pthread_cond_broadcast(&b->cond);  // Réveiller tout le monde
    } else {
        // Attendre
        while (my_generation == b->generation) {
            pthread_cond_wait(&b->cond, &b->mutex);
        }
    }

    pthread_mutex_unlock(&b->mutex);
}

void barrier_destroy(Barrier *b) {
    pthread_mutex_destroy(&b->mutex);
    pthread_cond_destroy(&b->cond);
}
```

**Explication** :
- `generation` : Permet de distinguer les utilisations successives de la barrière
- Le dernier thread réveille tous les autres avec `broadcast`
- Les threads attendent que la génération change

### Test de l'implémentation

```c
Barrier barriere;

void *test_worker(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 3; i++) {
        printf("[Thread %d] Itération %d, avant barrière\n", id, i);

        barrier_wait(&barriere);

        printf("[Thread %d] Itération %d, après barrière\n", id, i);
    }

    return NULL;
}

int main(void) {
    pthread_t threads[4];
    int ids[4];

    barrier_init(&barriere, 4);

    for (int i = 0; i < 4; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, test_worker, &ids[i]);
    }

    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    barrier_destroy(&barriere);

    return 0;
}
```

---

## Patterns avancés

### Pattern 1 : Fork-Join parallèle

```c
pthread_barrier_t barriere_debut, barriere_fin;  
double resultats[NUM_THREADS];  

void *worker(void *arg) {
    int id = *(int *)arg;

    while (1) {
        // Attendre le signal de départ (fork)
        pthread_barrier_wait(&barriere_debut);

        // Travail parallèle
        resultats[id] = calculer_quelque_chose(id);

        // Signaler la fin (join)
        pthread_barrier_wait(&barriere_fin);
    }

    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    pthread_barrier_init(&barriere_debut, NULL, NUM_THREADS + 1);  // +1 pour main
    pthread_barrier_init(&barriere_fin, NULL, NUM_THREADS + 1);

    // Créer les workers (persistent)
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, worker, &ids[i]);
    }

    // Boucle principale
    for (int tache = 0; tache < 10; tache++) {
        printf("Lancement tâche %d\n", tache);

        // Fork : Libérer les workers
        pthread_barrier_wait(&barriere_debut);

        // Join : Attendre la fin
        pthread_barrier_wait(&barriere_fin);

        // Traiter les résultats
        double total = 0.0;
        for (int i = 0; i < NUM_THREADS; i++) {
            total += resultats[i];
        }
        printf("Résultat tâche %d : %.2f\n", tache, total);
    }

    // Cleanup...

    return 0;
}
```

### Pattern 2 : Réduction parallèle avec barrières

```c
#define SIZE 1000000

int donnees[SIZE];  
int sommes_locales[NUM_THREADS];  
pthread_barrier_t barriere;  

void *calculer_somme(void *arg) {
    int id = *(int *)arg;
    int debut = id * (SIZE / NUM_THREADS);
    int fin = (id + 1) * (SIZE / NUM_THREADS);

    // Somme locale
    int somme = 0;
    for (int i = debut; i < fin; i++) {
        somme += donnees[i];
    }
    sommes_locales[id] = somme;

    // Barrière : Attendre que tous aient calculé leur somme
    int result = pthread_barrier_wait(&barriere);

    if (result == PTHREAD_BARRIER_SERIAL_THREAD) {
        // Un thread fait la réduction finale
        int total = 0;
        for (int i = 0; i < NUM_THREADS; i++) {
            total += sommes_locales[i];
        }
        printf("Somme totale : %d\n", total);
    }

    return NULL;
}
```

### Pattern 3 : Algorithme itératif avec convergence

```c
pthread_barrier_t barriere;  
double differences[NUM_THREADS];  
volatile int converge = 0;  

void *worker(void *arg) {
    int id = *(int *)arg;

    while (!converge) {
        // Itération
        double diff = faire_iteration(id);
        differences[id] = diff;

        // Barrière
        int result = pthread_barrier_wait(&barriere);

        if (result == PTHREAD_BARRIER_SERIAL_THREAD) {
            // Vérifier la convergence
            double max_diff = 0.0;
            for (int i = 0; i < NUM_THREADS; i++) {
                if (differences[i] > max_diff) {
                    max_diff = differences[i];
                }
            }

            if (max_diff < EPSILON) {
                converge = 1;
                printf("Convergence atteinte !\n");
            }
        }

        // Barrière pour que tous voient la décision
        pthread_barrier_wait(&barriere);
    }

    return NULL;
}
```

### Pattern 4 : Synchronisation maître-esclaves

```c
#define NUM_WORKERS 8

pthread_barrier_t barriere_debut, barriere_fin;  
int taches[NUM_WORKERS];  
int resultats[NUM_WORKERS];  

void *worker(void *arg) {
    int id = *(int *)arg;

    while (1) {
        // Attendre les instructions du maître
        pthread_barrier_wait(&barriere_debut);

        // Traiter la tâche
        resultats[id] = traiter(taches[id]);

        // Signaler la fin au maître
        pthread_barrier_wait(&barriere_fin);
    }

    return NULL;
}

void maitre(void) {
    for (int batch = 0; batch < 100; batch++) {
        // Préparer les tâches
        for (int i = 0; i < NUM_WORKERS; i++) {
            taches[i] = obtenir_tache();
        }

        // Lancer les workers
        pthread_barrier_wait(&barriere_debut);

        // Attendre les résultats
        pthread_barrier_wait(&barriere_fin);

        // Traiter les résultats
        for (int i = 0; i < NUM_WORKERS; i++) {
            sauvegarder(resultats[i]);
        }
    }
}

int main(void) {
    pthread_t workers[NUM_WORKERS];
    int ids[NUM_WORKERS];

    // +1 pour le thread maître (main)
    pthread_barrier_init(&barriere_debut, NULL, NUM_WORKERS + 1);
    pthread_barrier_init(&barriere_fin, NULL, NUM_WORKERS + 1);

    for (int i = 0; i < NUM_WORKERS; i++) {
        ids[i] = i;
        pthread_create(&workers[i], NULL, worker, &ids[i]);
    }

    maitre();  // Thread principal = maître

    // Cleanup...

    return 0;
}
```

---

## Barrières vs autres mécanismes

### Comparaison

| Mécanisme | Cas d'usage | Attente | Réutilisable |
|-----------|-------------|---------|--------------|
| **Mutex** | Protection données | Un seul à la fois | Oui |
| **Sémaphore** | Comptage ressources | N simultanés | Oui |
| **Condition variable** | Attente condition | Threads en attente | Oui |
| **Barrière** | Point de rendez-vous | N threads ensemble | Oui (auto) |

### Quand utiliser quoi ?

**Mutex** :
```c
// Protection de données partagées
pthread_mutex_lock(&mutex);  
compteur++;  
pthread_mutex_unlock(&mutex);  
```

**Condition variable** :
```c
// Attendre qu'une condition devienne vraie
pthread_mutex_lock(&mutex);  
while (!condition) {  
    pthread_cond_wait(&cond, &mutex);
}
pthread_mutex_unlock(&mutex);
```

**Barrière** :
```c
// Synchroniser N threads à un point précis
pthread_barrier_wait(&barriere);
```

### Équivalences

**Barrière = Mutex + Condition + Compteur** :
```c
// Barrière avec pthread_barrier_wait()
pthread_barrier_wait(&barriere);

// Équivalent manuel
pthread_mutex_lock(&mutex);  
arrived++;  
if (arrived == count) {  
    arrived = 0;
    pthread_cond_broadcast(&cond);
} else {
    pthread_cond_wait(&cond, &mutex);
}
pthread_mutex_unlock(&mutex);
```

---

## Performance et considérations

### Overhead

**Coût d'une barrière** :
- Lock/unlock d'un mutex
- Mise en sommeil (si pas le dernier)
- Réveil de tous les threads (si dernier)

**Benchmark** :
```c
#include <time.h>

#define ITERATIONS 10000

void benchmark_barriere(void) {
    pthread_barrier_t b;
    pthread_barrier_init(&b, NULL, NUM_THREADS);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < ITERATIONS; i++) {
        pthread_barrier_wait(&b);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Temps : %.3f s (%.2f µs/barrière)\n",
           elapsed, elapsed * 1e6 / ITERATIONS);

    pthread_barrier_destroy(&b);
}
```

**Résultats typiques** :
```
4 threads : ~1-2 µs par barrière
8 threads : ~2-4 µs par barrière
16 threads : ~4-8 µs par barrière
```

### Scalabilité

**Temps d'attente** augmente avec le nombre de threads :

```
2 threads  : ██░░░░░░░░  Rapide
4 threads  : ████░░░░░░  Acceptable
8 threads  : ██████░░░░  Moyen
16 threads : ████████░░  Lent
32 threads : ██████████  Très lent
```

**Recommandations** :
- Limiter à 8-16 threads pour performance optimale
- Minimiser le nombre de barrières par itération
- Grouper les synchronisations si possible

### Fausse attente (False Sharing)

**Problème** : Si les threads modifient des données proches en mémoire :

```c
// ❌ Mauvais : False sharing possible
int donnees[NUM_THREADS];  // Contigus en mémoire

void *worker(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 1000; i++) {
        donnees[id]++;  // Peut causer false sharing
        pthread_barrier_wait(&b);
    }
}

// ✅ Mieux : Padding
typedef struct {
    int valeur;
    char padding[64 - sizeof(int)];  // Cache line = 64 bytes
} DonneePadded;

DonneePadded donnees[NUM_THREADS];
```

---

## Erreurs courantes

### 1. Mauvais nombre de threads

```c
// ❌ ERREUR : Barrière initialisée pour 5 threads
pthread_barrier_init(&barriere, NULL, 5);

// Mais seulement 3 threads créés
for (int i = 0; i < 3; i++) {
    pthread_create(&threads[i], NULL, worker, NULL);
}

// Les 3 threads seront BLOQUÉS indéfiniment
```

**Solution** : S'assurer que le nombre correspond exactement.

### 2. Oublier un thread

```c
// ❌ ERREUR : Le thread principal ne participe pas
pthread_barrier_init(&barriere, NULL, NUM_THREADS + 1);

for (int i = 0; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], NULL, worker, &ids[i]);
}

// Oubli de : pthread_barrier_wait(&barriere);
// Les workers sont BLOQUÉS

for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);  // Deadlock !
}
```

### 3. Barrières imbriquées (attention)

```c
// ⚠️ DANGEREUX
void *worker(void *arg) {
    pthread_barrier_wait(&barriere1);

    if (condition) {
        pthread_barrier_wait(&barriere2);  // Tous ne passent pas ici !
    }

    pthread_barrier_wait(&barriere1);  // Deadlock possible
}
```

**Solution** : S'assurer que tous les threads suivent le même chemin.

### 4. Destruction prématurée

```c
// ❌ ERREUR
pthread_barrier_destroy(&barriere);  // Détruit trop tôt

for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);  // Threads utilisent encore barriere
}

// ✅ CORRECT : Attendre d'abord
for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
}

pthread_barrier_destroy(&barriere);  // Maintenant OK
```

### 5. Nombre dynamique de threads

```c
// ❌ PROBLÈME : Le nombre change
int num_threads = 4;  
pthread_barrier_init(&barriere, NULL, num_threads);  

// Plus tard...
num_threads = 6;  // La barrière attend toujours 4 !

// ✅ SOLUTION : Recréer la barrière
pthread_barrier_destroy(&barriere);  
pthread_barrier_init(&barriere, NULL, num_threads);  
```

---

## Quand utiliser les barrières ?

### ✅ Utilisez barrières pour :

1. **Algorithmes par phases**
   ```c
   for (iteration) {
       phase1();
       barrier_wait();  // Synchroniser
       phase2();
       barrier_wait();
   }
   ```

2. **Calculs parallèles itératifs**
   - Simulations physiques
   - Résolution d'équations
   - Traitement d'images par région

3. **Fork-Join patterns**
   - Distribuer tâches
   - Collecter résultats

4. **Tests de performance**
   ```c
   // Démarrer tous les threads ensemble
   pthread_barrier_wait(&start_barrier);
   benchmark();
   pthread_barrier_wait(&end_barrier);
   ```

### ❌ N'utilisez PAS barrières pour :

1. **Protection de données**
   → Utilisez mutex

2. **Signalisation simple**
   → Utilisez condition variable

3. **Comptage de ressources**
   → Utilisez sémaphore

4. **Nombre variable de threads**
   → Utilisez condition variable

5. **Synchronisation rare**
   → Overhead inutile

### Tableau de décision

| Situation | Solution |
|-----------|----------|
| Protéger compteur++ | Mutex |
| Attendre queue non vide | Condition variable |
| Limiter à N connexions | Sémaphore |
| **Tous attendent avant phase 2** | **Barrière** |
| Synchronisation maître-workers | Barrière (fork-join) |

---

## Récapitulatif

Dans cette section, vous avez appris :

✅ **Barrières** :
- Point de synchronisation pour N threads
- Tous doivent arriver avant que l'un continue
- Réutilisable automatiquement

✅ **API pthread_barrier_t** :
- `pthread_barrier_init()` : Initialiser (avec count)
- `pthread_barrier_wait()` : Attendre au point de rendez-vous
- `pthread_barrier_destroy()` : Détruire

✅ **Valeur de retour** :
- `0` : Pour N-1 threads
- `PTHREAD_BARRIER_SERIAL_THREAD` : Pour 1 thread (le "gagnant")
- Utile pour tâches uniques

✅ **Patterns** :
- Calculs par phases
- Fork-Join
- Réduction parallèle
- Maître-esclaves

✅ **Implémentation manuelle** :
- Mutex + condition variable + compteur + génération
- Comprend le mécanisme interne

✅ **Performance** :
- ~1-8 µs par barrière selon nombre de threads
- Scalabilité limitée (optimal : 8-16 threads)
- Attention au false sharing

✅ **Erreurs à éviter** :
- Mauvais nombre de threads
- Oublier un thread
- Barrières imbriquées conditionnelles
- Destruction prématurée
- Nombre dynamique non géré

✅ **Quand utiliser** :
- Algorithmes par phases
- Synchronisation de groupe
- Fork-Join patterns
- Pas pour protection données

---

## Pour aller plus loin

**Ressources** :
- `man pthread_barrier_init`
- `man pthread_barrier_wait`
- Livre : "The Art of Multiprocessor Programming"

**Projets suggérés** :
1. **Simulation de chaleur 2D** : Grille avec diffusion thermique par itérations
2. **Tri parallèle** : Merge sort parallèle avec barrières entre phases
3. **Rendu d'image** : Découpe par régions, synchronisation entre passes

**Conseil pratique** : Testez toujours avec différents nombres de threads pour vérifier la synchronisation correcte.

💡 **Prochaine étape** : Explorez les patterns avancés de parallélisme et considérez des alternatives comme OpenMP pour la parallélisation de boucles.

**Citation** : *"A barrier is a rendezvous point where threads wait for each other."* - Maurice Herlihy

⏭️ [Mémoire partagée et IPC avancé](/19-memoire-partagee-ipc/README.md)
