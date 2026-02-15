🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 18.4 Threads détachés vs joinable

## Table des matières
- [Introduction](#introduction)
- [Threads joinable (par défaut)](#threads-joinable-par-d%C3%A9faut)
- [Threads détachés](#threads-d%C3%A9tach%C3%A9s)
- [Différences clés](#diff%C3%A9rences-cl%C3%A9s)
- [Détacher un thread avec pthread_detach()](#d%C3%A9tacher-un-thread-avec-pthread_detach)
- [Créer un thread directement détaché](#cr%C3%A9er-un-thread-directement-d%C3%A9tach%C3%A9)
- [Quand utiliser des threads détachés ?](#quand-utiliser-des-threads-d%C3%A9tach%C3%A9s-)
- [Quand utiliser des threads joinable ?](#quand-utiliser-des-threads-joinable-)
- [Gestion des ressources](#gestion-des-ressources)
- [Exemples pratiques](#exemples-pratiques)
- [Pièges et erreurs courantes](#pi%C3%A8ges-et-erreurs-courantes)
- [Bonnes pratiques](#bonnes-pratiques)

---

## Introduction

Chaque thread créé avec `pthread_create()` peut être dans **deux états différents** concernant sa terminaison :

1. **Joinable** (par défaut) : Un autre thread doit appeler `pthread_join()` pour récupérer ses ressources
2. **Détaché** (detached) : Le thread libère automatiquement ses ressources à sa terminaison

Cette distinction est **cruciale** pour :
- Éviter les fuites de ressources
- Optimiser la gestion mémoire
- Concevoir correctement l'architecture de votre application

**Analogie** :

Imaginez des employés dans une entreprise :

- **Thread joinable** = Employé qui doit remettre un rapport à son manager avant de partir. Le manager attend ce rapport (`pthread_join()`).
- **Thread détaché** = Employé autonome qui fait son travail et part sans rapport. Personne ne l'attend.

---

## Threads joinable (par défaut)

### Qu'est-ce qu'un thread joinable ?

Par défaut, tous les threads créés avec `pthread_create()` sont **joinable**. Cela signifie :

- Un autre thread (généralement le thread principal) **doit** appeler `pthread_join()` pour libérer ses ressources
- Le thread peut retourner une valeur récupérable via `pthread_join()`
- Les ressources du thread (pile, structures internes) ne sont pas libérées automatiquement à sa terminaison

### Exemple basique

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *tache(void *arg) {
    printf("Thread démarre\n");
    sleep(2);
    printf("Thread se termine\n");
    return NULL;
}

int main(void) {
    pthread_t tid;

    printf("Création du thread joinable (par défaut)\n");
    pthread_create(&tid, NULL, tache, NULL);

    printf("Thread principal attend...\n");
    pthread_join(tid, NULL);  // ← OBLIGATOIRE pour libérer les ressources

    printf("Thread principal terminé\n");
    return 0;
}
```

**Sortie** :
```
Création du thread joinable (par défaut)  
Thread principal attend...  
Thread démarre  
Thread se termine  
Thread principal terminé  
```

### Cycle de vie d'un thread joinable

```
Création                  Exécution              Terminaison
   │                          │                       │
   ▼                          ▼                       ▼
┌──────────┐            ┌──────────┐           ┌──────────────┐
│ JOINABLE │  ───────→  │ RUNNING  │  ───────→ │  TERMINATED  │
└──────────┘            └──────────┘           └──────────────┘
                                                       │
                                              pthread_join() requis
                                                       │
                                                       ▼
                                               ┌────────────────┐
                                               │ Ressources     │
                                               │ libérées       │
                                               └────────────────┘
```

### État "zombie" des threads joinable

Si vous ne faites **jamais** `pthread_join()` sur un thread joinable, il devient un **thread zombie** :

```c
// ❌ PROBLÈME : Fuite de ressources
int main(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, tache, NULL);

    sleep(5);  // Le thread se termine

    // Pas de pthread_join() → Le thread est terminé mais ses ressources
    // ne sont pas libérées → Thread ZOMBIE

    return 0;  // Fuite de ressources
}
```

**Conséquence** : Les ressources du thread (pile, descripteur, etc.) restent allouées et ne sont jamais libérées.

### Récupération de valeur

L'avantage des threads joinable est de pouvoir **récupérer une valeur de retour** :

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *calculer_carre(void *arg) {
    int n = *(int *)arg;
    int *resultat = malloc(sizeof(int));
    *resultat = n * n;
    return resultat;
}

int main(void) {
    pthread_t tid;
    int nombre = 7;
    void *retval;

    pthread_create(&tid, NULL, calculer_carre, &nombre);
    pthread_join(tid, &retval);  // Récupère la valeur

    int carre = *(int *)retval;
    printf("Carré de %d = %d\n", nombre, carre);
    free(retval);

    return 0;
}
```

---

## Threads détachés

### Qu'est-ce qu'un thread détaché ?

Un thread **détaché** (detached) est un thread qui :

- **Libère automatiquement** ses ressources dès qu'il se termine
- **Ne peut pas** être joint avec `pthread_join()` (retourne une erreur)
- **Ne peut pas** retourner de valeur récupérable
- Est complètement **indépendant** après sa création

**Analogie** : Un thread détaché est comme un feu d'artifice - une fois lancé, il fait son travail et disparaît tout seul, sans que personne ait besoin de le "nettoyer".

### Cycle de vie d'un thread détaché

```
Création                  Exécution              Terminaison
   │                          │                       │
   ▼                          ▼                       ▼
┌──────────┐            ┌──────────┐           ┌──────────────┐
│ DETACHED │  ───────→  │ RUNNING  │  ───────→ │  TERMINATED  │
└──────────┘            └──────────┘           └──────────────┘
                                                       │
                                                       │ Automatique
                                                       ▼
                                               ┌────────────────┐
                                               │ Ressources     │
                                               │ LIBÉRÉES AUTO  │
                                               └────────────────┘
```

### Exemple basique

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *tache_detachee(void *arg) {
    int id = *(int *)arg;
    printf("Thread détaché %d démarre\n", id);
    sleep(2);
    printf("Thread détaché %d se termine\n", id);
    return NULL;  // Ressources libérées automatiquement
}

int main(void) {
    pthread_t tid;
    int id = 1;

    pthread_create(&tid, NULL, tache_detachee, &id);

    // Détacher le thread
    pthread_detach(tid);

    // Pas besoin de pthread_join() !
    printf("Thread principal continue son travail\n");

    sleep(3);  // Attendre que le thread détaché se termine
    printf("Thread principal terminé\n");

    return 0;
}
```

**Sortie** :
```
Thread principal continue son travail  
Thread détaché 1 démarre  
Thread détaché 1 se termine  
Thread principal terminé  
```

---

## Différences clés

| Caractéristique | Thread Joinable | Thread Détaché |
|-----------------|-----------------|----------------|
| **État par défaut** | ✅ Oui | ❌ Non (doit être explicitement détaché) |
| **pthread_join() requis** | ✅ Oui, obligatoire | ❌ Non, interdit |
| **Libération ressources** | Manuel via `pthread_join()` | Automatique à la terminaison |
| **Retour de valeur** | ✅ Possible via `pthread_join()` | ❌ Impossible |
| **Thread zombie possible** | ✅ Oui, si pas de join | ❌ Non |
| **Synchronisation** | ✅ `pthread_join()` bloque | ❌ Aucune synchronisation native |
| **Use case typique** | Calculs avec résultat attendu | Tâches de fond indépendantes |

### Tableau comparatif visuel

```
JOINABLE                          DÉTACHÉ
========                          =======

Main thread                       Main thread
    │                                 │
    ├─ pthread_create()               ├─ pthread_create()
    │       │                         │       │
    │       ├─ Worker démarre         │       ├─ Worker démarre
    │       │    travail...           │       │    travail...
    │       │    travail...           │       │    travail...
    │       │                         │       │
    │  pthread_join() ◄──┐            │       │
    │    [BLOQUÉ]        │            │   Continue...
    │                    │            │       │
    │                Worker           │   Continue...
    │                termine          │       │
    │                    │            │       Worker
    │    [DÉBLOQUÉ] ─────┘            │       termine
    │                                 │         │
    │  Ressources libérées            │    [AUTO-LIBÉRATION]
    │                                 │
  Continue                          Continue
```

---

## Détacher un thread avec pthread_detach()

### Syntaxe

```c
int pthread_detach(pthread_t thread);
```

**Paramètre** :
- `thread` : ID du thread à détacher

**Retour** :
- `0` en cas de succès
- Code d'erreur en cas d'échec (`EINVAL`, `ESRCH`)

### Utilisation

Vous pouvez détacher un thread de **deux manières** :

#### 1. Depuis le thread principal (après création)

```c
pthread_t tid;  
pthread_create(&tid, NULL, worker, NULL);  

// Détacher immédiatement après la création
int result = pthread_detach(tid);  
if (result != 0) {  
    fprintf(stderr, "Erreur pthread_detach: %s\n", strerror(result));
}
```

#### 2. Depuis le thread lui-même

```c
void *worker(void *arg) {
    // Le thread se détache lui-même
    pthread_detach(pthread_self());

    printf("Je suis maintenant détaché\n");
    // Travail...

    return NULL;  // Libération automatique
}

int main(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, worker, NULL);

    // Pas de pthread_join() nécessaire
    sleep(2);  // Laisser le temps au thread de travailler
    return 0;
}
```

### Exemple complet : Détachement après création

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

void *tache_independante(void *arg) {
    int id = *(int *)arg;

    for (int i = 1; i <= 5; i++) {
        printf("Thread %d : étape %d/5\n", id, i);
        sleep(1);
    }

    printf("Thread %d terminé\n", id);
    return NULL;
}

int main(void) {
    pthread_t tid;
    int id = 1;

    printf("Création du thread\n");
    pthread_create(&tid, NULL, tache_independante, &id);

    printf("Détachement du thread\n");
    pthread_detach(tid);

    printf("Thread principal continue (pas de join nécessaire)\n");

    // Attendre suffisamment pour que le thread se termine
    sleep(6);

    printf("Programme terminé\n");
    return 0;
}
```

### Erreurs possibles

**Détacher deux fois le même thread** :

```c
pthread_t tid;  
pthread_create(&tid, NULL, worker, NULL);  

pthread_detach(tid);  // ✅ OK  
pthread_detach(tid);  // ❌ ERREUR : EINVAL (thread déjà détaché)  
```

**Joindre un thread détaché** :

```c
pthread_t tid;  
pthread_create(&tid, NULL, worker, NULL);  
pthread_detach(tid);  

pthread_join(tid, NULL);  // ❌ ERREUR : EINVAL (thread détaché)
```

**Détacher après avoir joint** :

```c
pthread_t tid;  
pthread_create(&tid, NULL, worker, NULL);  

pthread_join(tid, NULL);  // Thread terminé et ressources libérées  
pthread_detach(tid);      // ❌ ERREUR : ESRCH (thread n'existe plus)  
```

---

## Créer un thread directement détaché

Au lieu de créer un thread puis le détacher, vous pouvez créer un thread **directement détaché** en utilisant les **attributs**.

### Étapes

1. Initialiser une structure `pthread_attr_t`
2. Définir l'état détaché
3. Créer le thread avec ces attributs
4. Détruire les attributs

### Exemple complet

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

void *worker(void *arg) {
    int id = *(int *)arg;
    printf("Thread %d démarre (créé détaché)\n", id);
    sleep(2);
    printf("Thread %d termine\n", id);
    return NULL;
}

int main(void) {
    pthread_t tid;
    pthread_attr_t attr;
    int id = 1;
    int result;

    // 1. Initialiser les attributs
    result = pthread_attr_init(&attr);
    if (result != 0) {
        fprintf(stderr, "Erreur pthread_attr_init: %s\n", strerror(result));
        return 1;
    }

    // 2. Définir l'état détaché
    result = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (result != 0) {
        fprintf(stderr, "Erreur pthread_attr_setdetachstate: %s\n",
                strerror(result));
        pthread_attr_destroy(&attr);
        return 1;
    }

    // 3. Créer le thread avec les attributs
    result = pthread_create(&tid, &attr, worker, &id);
    if (result != 0) {
        fprintf(stderr, "Erreur pthread_create: %s\n", strerror(result));
        pthread_attr_destroy(&attr);
        return 1;
    }

    // 4. Détruire les attributs (plus besoin)
    pthread_attr_destroy(&attr);

    printf("Thread détaché créé\n");

    // Pas de pthread_join() !
    sleep(3);

    return 0;
}
```

### Fonction utilitaire réutilisable

```c
#include <pthread.h>
#include <string.h>

/**
 * Crée un thread détaché
 *
 * @param thread Pointeur vers pthread_t pour stocker l'ID
 * @param start_routine Fonction du thread
 * @param arg Argument à passer au thread
 * @return 0 en cas de succès, code d'erreur sinon
 */
int creer_thread_detache(pthread_t *thread,
                        void *(*start_routine)(void *),
                        void *arg) {
    pthread_attr_t attr;
    int result;

    // Initialiser
    result = pthread_attr_init(&attr);
    if (result != 0) return result;

    // Détaché
    result = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (result != 0) {
        pthread_attr_destroy(&attr);
        return result;
    }

    // Créer
    result = pthread_create(thread, &attr, start_routine, arg);

    // Nettoyer
    pthread_attr_destroy(&attr);

    return result;
}

// Utilisation
int main(void) {
    pthread_t tid;
    int id = 1;

    int result = creer_thread_detache(&tid, worker, &id);
    if (result != 0) {
        fprintf(stderr, "Erreur: %s\n", strerror(result));
        return 1;
    }

    // Thread détaché créé !
    sleep(3);
    return 0;
}
```

### États disponibles

```c
// État détaché
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

// État joinable (par défaut)
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
```

---

## Quand utiliser des threads détachés ?

### ✅ Utilisez des threads détachés quand :

#### 1. Tâches de fond sans résultat attendu

```c
void *logger(void *arg) {
    pthread_detach(pthread_self());

    while (1) {
        // Écrire dans un fichier de log
        sleep(60);  // Toutes les minutes
    }

    return NULL;
}
```

**Exemples** :
- Services de logging
- Monitoring en arrière-plan
- Nettoyage périodique
- Envoi de métriques

#### 2. Serveur gérant plusieurs clients

```c
void *gerer_client(void *arg) {
    pthread_detach(pthread_self());

    int socket = *(int *)arg;
    free(arg);

    // Traiter les requêtes du client
    // ...

    close(socket);
    return NULL;
}

void serveur(void) {
    while (1) {
        int *client_sock = malloc(sizeof(int));
        *client_sock = accept(...);

        pthread_t tid;
        pthread_create(&tid, NULL, gerer_client, client_sock);
        // Pas de join → le thread se nettoie tout seul
    }
}
```

**Avantages** :
- Pas besoin de garder trace des threads créés
- Pas de `pthread_join()` massif
- Libération automatique des ressources

#### 3. Fire-and-forget tasks

Tâches qui doivent juste être lancées et oubliées :

```c
void *envoyer_notification(void *arg) {
    pthread_detach(pthread_self());

    char *message = (char *)arg;

    // Envoyer email/SMS/notification
    send_notification(message);

    free(message);
    return NULL;
}

void action_utilisateur(void) {
    char *msg = malloc(100);
    strcpy(msg, "Action effectuée");

    pthread_t tid;
    pthread_create(&tid, NULL, envoyer_notification, msg);
    // Continue sans attendre
}
```

#### 4. Workers dans un pool de threads

```c
#define NUM_WORKERS 10

void *worker_permanent(void *arg) {
    pthread_detach(pthread_self());

    while (1) {
        Task *task = get_task_from_queue();
        if (task == NULL) break;

        execute_task(task);
    }

    return NULL;
}

void creer_pool(void) {
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_t tid;
        pthread_create(&tid, NULL, worker_permanent, NULL);
        // Les workers sont détachés
    }
}
```

---

## Quand utiliser des threads joinable ?

### ✅ Utilisez des threads joinable quand :

#### 1. Vous avez besoin d'un résultat

```c
void *calculer_pi(void *arg) {
    double *pi = malloc(sizeof(double));
    *pi = 3.14159265359;  // Calcul complexe en réalité
    return pi;
}

int main(void) {
    pthread_t tid;
    void *result;

    pthread_create(&tid, NULL, calculer_pi, NULL);
    pthread_join(tid, &result);  // Récupère le résultat

    double pi = *(double *)result;
    printf("PI = %.10f\n", pi);
    free(result);

    return 0;
}
```

#### 2. Synchronisation nécessaire

Vous devez attendre la fin d'un thread avant de continuer :

```c
void *preparer_donnees(void *arg) {
    // Préparation longue
    sleep(5);
    return NULL;
}

int main(void) {
    pthread_t tid;

    pthread_create(&tid, NULL, preparer_donnees, NULL);

    printf("Préparation en cours...\n");
    pthread_join(tid, NULL);  // ATTENDRE la fin

    printf("Données prêtes, on continue\n");
    // Utiliser les données préparées

    return 0;
}
```

#### 3. Nombre limité de threads

Quand vous créez peu de threads et pouvez facilement les gérer :

```c
#define NUM_WORKERS 4

int main(void) {
    pthread_t threads[NUM_WORKERS];

    // Créer
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    // Attendre tous
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Tous les workers ont terminé\n");
    return 0;
}
```

#### 4. Parallélisme structuré

Pattern fork-join où vous divisez le travail puis rassemblez les résultats :

```c
typedef struct {
    int debut;
    int fin;
    long resultat;
} Segment;

void *calculer_segment(void *arg) {
    Segment *seg = (Segment *)arg;
    long somme = 0;

    for (int i = seg->debut; i < seg->fin; i++) {
        somme += i;
    }

    seg->resultat = somme;
    return NULL;
}

int main(void) {
    pthread_t threads[4];
    Segment segments[4];

    // Fork : Créer les threads
    for (int i = 0; i < 4; i++) {
        segments[i].debut = i * 25;
        segments[i].fin = (i + 1) * 25;
        pthread_create(&threads[i], NULL, calculer_segment, &segments[i]);
    }

    // Join : Attendre et collecter
    long total = 0;
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
        total += segments[i].resultat;
    }

    printf("Total : %ld\n", total);
    return 0;
}
```

---

## Gestion des ressources

### Threads joinable : Fuite si pas de join

```c
// ❌ FUITE DE RESSOURCES
void creer_beaucoup_de_threads(void) {
    for (int i = 0; i < 1000; i++) {
        pthread_t tid;
        pthread_create(&tid, NULL, quick_task, NULL);
        // Pas de pthread_join() → 1000 threads zombies !
    }
}
```

**Symptômes** :
- Augmentation de la mémoire utilisée
- Impossible de créer de nouveaux threads (limite système atteinte)
- `pthread_create()` retourne `EAGAIN`

**Solution** : Toujours joindre OU détacher

```c
// ✅ SOLUTION 1 : Joindre
for (int i = 0; i < 1000; i++) {
    pthread_t tid;
    pthread_create(&tid, NULL, quick_task, NULL);
    pthread_join(tid, NULL);  // Attendre chaque thread
}

// ✅ SOLUTION 2 : Détacher
for (int i = 0; i < 1000; i++) {
    pthread_t tid;
    pthread_create(&tid, NULL, quick_task, NULL);
    pthread_detach(tid);  // Libération auto
}
```

### Threads détachés : Pas de fuite

```c
// ✅ PAS DE FUITE
void creer_beaucoup_de_threads(void) {
    for (int i = 0; i < 1000; i++) {
        pthread_t tid;
        pthread_create(&tid, NULL, quick_task, NULL);
        pthread_detach(tid);  // Libération automatique à la fin
    }
}
```

**Important** : Les threads détachés libèrent leurs **ressources système** automatiquement, mais pas la mémoire que vous allouez avec `malloc()` !

```c
void *worker(void *arg) {
    pthread_detach(pthread_self());

    char *data = malloc(1000);
    // Travail...
    free(data);  // ← TOUJOURS nécessaire !

    return NULL;  // Ressources du thread libérées auto
}
```

---

## Exemples pratiques

### Exemple 1 : Serveur simple avec threads détachés

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    int client_id;
    char requete[100];
} ClientData;

void *traiter_requete(void *arg) {
    pthread_detach(pthread_self());

    ClientData *client = (ClientData *)arg;

    printf("[Client %d] Requête reçue: %s\n",
           client->client_id, client->requete);

    // Simulation de traitement
    sleep(2);

    printf("[Client %d] Requête traitée\n", client->client_id);

    free(client);
    return NULL;
}

int main(void) {
    printf("Serveur démarré\n");

    // Simuler l'arrivée de 5 clients
    for (int i = 1; i <= 5; i++) {
        ClientData *client = malloc(sizeof(ClientData));
        client->client_id = i;
        snprintf(client->requete, sizeof(client->requete),
                "GET /data/%d", i);

        pthread_t tid;
        pthread_create(&tid, NULL, traiter_requete, client);
        // Pas de join : le serveur continue à accepter des connexions

        sleep(1);  // Simuler délai entre clients
    }

    printf("En attente de terminaison des requêtes...\n");
    sleep(5);  // Laisser le temps aux threads de finir

    printf("Serveur arrêté\n");
    return 0;
}
```

### Exemple 2 : Calcul parallèle avec threads joinable

```c
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 4
#define ARRAY_SIZE 100

typedef struct {
    int *tableau;
    int debut;
    int fin;
    long somme;
} ThreadWork;

void *calculer_somme(void *arg) {
    ThreadWork *work = (ThreadWork *)arg;
    work->somme = 0;

    for (int i = work->debut; i < work->fin; i++) {
        work->somme += work->tableau[i];
    }

    return NULL;
}

int main(void) {
    int tableau[ARRAY_SIZE];
    pthread_t threads[NUM_THREADS];
    ThreadWork work[NUM_THREADS];

    // Initialiser le tableau
    for (int i = 0; i < ARRAY_SIZE; i++) {
        tableau[i] = i + 1;
    }

    int chunk = ARRAY_SIZE / NUM_THREADS;

    // Créer les threads (joinable par défaut)
    for (int i = 0; i < NUM_THREADS; i++) {
        work[i].tableau = tableau;
        work[i].debut = i * chunk;
        work[i].fin = (i + 1) * chunk;
        work[i].somme = 0;

        pthread_create(&threads[i], NULL, calculer_somme, &work[i]);
    }

    // Joindre et récupérer les résultats
    long total = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);  // ← Obligatoire
        total += work[i].somme;
    }

    printf("Somme totale : %ld\n", total);
    printf("Attendu : %d\n", ARRAY_SIZE * (ARRAY_SIZE + 1) / 2);

    return 0;
}
```

### Exemple 3 : Auto-détachement dans le thread

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *tache_autonome(void *arg) {
    // Le thread se détache lui-même
    int result = pthread_detach(pthread_self());
    if (result != 0) {
        fprintf(stderr, "Erreur détachement\n");
        return NULL;
    }

    int id = *(int *)arg;
    free(arg);  // Libérer l'argument alloué dynamiquement

    printf("Thread %d : Je suis autonome !\n", id);

    for (int i = 1; i <= 3; i++) {
        printf("Thread %d : étape %d\n", id, i);
        sleep(1);
    }

    printf("Thread %d : Terminé (auto-nettoyage)\n", id);
    return NULL;
}

int main(void) {
    for (int i = 1; i <= 3; i++) {
        int *id = malloc(sizeof(int));
        *id = i;

        pthread_t tid;
        pthread_create(&tid, NULL, tache_autonome, id);
        // Le thread se détache lui-même, pas besoin de join
    }

    printf("Main : Tous les threads lancés\n");

    sleep(5);  // Laisser le temps aux threads de finir

    printf("Main : Terminé\n");
    return 0;
}
```

---

## Pièges et erreurs courantes

### 1. Joindre un thread détaché

```c
pthread_t tid;  
pthread_create(&tid, NULL, worker, NULL);  
pthread_detach(tid);  

// ❌ ERREUR : EINVAL
pthread_join(tid, NULL);  // Le thread est détaché !
```

**Message d'erreur** :
```
pthread_join: Invalid argument
```

### 2. Détacher deux fois

```c
pthread_t tid;  
pthread_create(&tid, NULL, worker, NULL);  

pthread_detach(tid);  // ✅ OK  
pthread_detach(tid);  // ❌ ERREUR : EINVAL  
```

### 3. Utiliser pthread_t après détachement

```c
pthread_t tid;  
pthread_create(&tid, NULL, worker, NULL);  
pthread_detach(tid);  

// ❌ DANGER : tid peut ne plus être valide
// Le thread peut avoir déjà terminé et tid recyclé
```

**Important** : Après `pthread_detach()`, ne conservez pas et ne réutilisez pas `tid`.

### 4. Oublier de détacher dans une boucle

```c
// ❌ FUITE si les threads ne sont pas détachés
for (int i = 0; i < 10000; i++) {
    pthread_t tid;
    pthread_create(&tid, NULL, short_task, NULL);
    // Ni join ni detach → Fuite !
}
```

**Solution** :

```c
// ✅ Détacher chaque thread
for (int i = 0; i < 10000; i++) {
    pthread_t tid;
    pthread_create(&tid, NULL, short_task, NULL);
    pthread_detach(tid);
}
```

### 5. Accès à des variables locales

```c
// ❌ BUG avec thread détaché
void lancer_thread(void) {
    int valeur = 42;
    pthread_t tid;

    pthread_create(&tid, NULL, worker, &valeur);
    pthread_detach(tid);

    // La fonction retourne immédiatement
    // valeur est détruite alors que le thread peut encore l'utiliser !
}
```

**Solution** : Allocation dynamique

```c
// ✅ Correct
void lancer_thread(void) {
    int *valeur = malloc(sizeof(int));
    *valeur = 42;

    pthread_t tid;
    pthread_create(&tid, NULL, worker, valeur);
    pthread_detach(tid);

    // Le thread libérera valeur quand il aura fini
}

void *worker(void *arg) {
    // Pas de pthread_detach() ici : déjà détaché dans lancer_thread()

    int val = *(int *)arg;
    free(arg);  // Libérer l'argument

    // Travail avec val...
    return NULL;
}
```

### 6. Terminer main() avec des threads détachés actifs

```c
int main(void) {
    for (int i = 0; i < 10; i++) {
        pthread_t tid;
        pthread_create(&tid, NULL, long_task, NULL);
        pthread_detach(tid);
    }

    return 0;  // ❌ main() se termine immédiatement
    // Tous les threads détachés sont tués !
}
```

**Solution** : Attendre suffisamment ou utiliser `pthread_exit()`

```c
// Solution 1 : Sleep
int main(void) {
    for (int i = 0; i < 10; i++) {
        pthread_t tid;
        pthread_create(&tid, NULL, long_task, NULL);
        pthread_detach(tid);
    }

    sleep(10);  // Attendre que les threads finissent
    return 0;
}

// Solution 2 : pthread_exit dans main
int main(void) {
    for (int i = 0; i < 10; i++) {
        pthread_t tid;
        pthread_create(&tid, NULL, long_task, NULL);
        pthread_detach(tid);
    }

    pthread_exit(NULL);  // main se termine mais threads continuent
}
```

---

## Bonnes pratiques

### 1. Détacher immédiatement si pas de join prévu

```c
// ✅ Bon : Détacher immédiatement après création
pthread_t tid;  
pthread_create(&tid, NULL, worker, NULL);  
pthread_detach(tid);  

// Ou créer directement détaché avec attributs
```

### 2. Documenter le comportement

```c
/**
 * Traite une requête client en arrière-plan.
 *
 * @param client Données client (libérées par le thread)
 * @return NULL
 *
 * Note: Ce thread se détache automatiquement.
 */
void *traiter_client(void *client) {
    pthread_detach(pthread_self());
    // ...
}
```

### 3. Choisir le bon type selon le use case

```c
// Pour calculs avec résultat → Joinable
pthread_t tid;  
pthread_create(&tid, NULL, calculer_pi, NULL);  
void *result;  
pthread_join(tid, &result);  // Récupérer le résultat  

// Pour tâches de fond → Détaché
pthread_t tid;  
pthread_create(&tid, NULL, logger, NULL);  
pthread_detach(tid);  // Fire and forget  
```

### 4. Utiliser une fonction helper pour threads détachés

```c
int create_detached_thread(pthread_t *thread,
                          void *(*func)(void *),
                          void *arg) {
    pthread_attr_t attr;
    int result;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    result = pthread_create(thread, &attr, func, arg);

    pthread_attr_destroy(&attr);
    return result;
}
```

### 5. Gérer proprement la mémoire

```c
void *worker(void *arg) {
    // Auto-détachement
    pthread_detach(pthread_self());

    // Récupérer et libérer l'argument
    MyData *data = (MyData *)arg;

    // Travail...

    // Libération AVANT le retour
    free(data);

    return NULL;
}
```

---

## Récapitulatif

Dans cette section, vous avez appris :

✅ **Threads joinable** :
- État par défaut
- Nécessitent `pthread_join()` pour libérer les ressources
- Peuvent retourner des valeurs
- Risque de threads zombies si pas de join

✅ **Threads détachés** :
- Libération automatique des ressources
- Pas de `pthread_join()` possible
- Pas de valeur de retour récupérable
- Idéaux pour tâches de fond

✅ **pthread_detach()** :
- Détacher un thread après création
- Peut être appelé par le thread lui-même
- Ne peut pas être annulé

✅ **Attributs pthread** :
- Créer directement un thread détaché
- `pthread_attr_setdetachstate()`
- Plus propre que détacher après création

✅ **Quand utiliser quoi** :
- Joinable : résultats attendus, synchronisation
- Détaché : fire-and-forget, serveurs, tâches de fond

✅ **Pièges à éviter** :
- Joindre un thread détaché
- Détacher deux fois
- Fuites de ressources avec joinable
- Variables locales avec détaché

---

## Pour aller plus loin

Dans les prochaines sections :

- **18.5** : Race conditions (problèmes de concurrence avec données partagées)
- **18.6** : Mutex et sections critiques (protéger les données partagées)
- **18.7** : Deadlocks (situations de blocage mutuel)

**Conseil pratique** : Créez un mini-serveur qui accepte des "requêtes" (nombres aléatoires) et les traite dans des threads détachés. Puis créez un programme de calcul parallèle utilisant des threads joinable pour diviser un tableau en segments.

---

📘 **Ressources utiles** :
- `man pthread_detach`
- `man pthread_attr_setdetachstate`
- `man pthread_join`

💡 **Prochaine étape** : Section 18.5 - Race conditions

⏭️ [Problèmes de concurrence : Race conditions](/18-threads-et-concurrence/05-race-conditions.md)
