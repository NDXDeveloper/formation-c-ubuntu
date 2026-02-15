🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 18.2 Création et terminaison de threads

## Table des matières
- [Création de threads en détail](#cr%C3%A9ation-de-threads-en-d%C3%A9tail)
- [Gestion des erreurs de création](#gestion-des-erreurs-de-cr%C3%A9ation)
- [Création de threads multiples](#cr%C3%A9ation-de-threads-multiples)
- [Terminaison de threads](#terminaison-de-threads)
- [Récupération des valeurs de retour](#r%C3%A9cup%C3%A9ration-des-valeurs-de-retour)
- [pthread_join() en profondeur](#pthread_join-en-profondeur)
- [Le cas particulier du thread principal](#le-cas-particulier-du-thread-principal)
- [Cycle de vie complet d'un thread](#cycle-de-vie-complet-dun-thread)
- [Bonnes pratiques](#bonnes-pratiques)
- [Erreurs courantes à éviter](#erreurs-courantes-%C3%A0-%C3%A9viter)

---

## Création de threads en détail

### La fonction pthread_create() revisitée

Dans la section précédente, nous avons vu `pthread_create()` rapidement. Examinons-la maintenant en profondeur :

```c
int pthread_create(pthread_t *thread,
                   const pthread_attr_t *attr,
                   void *(*start_routine)(void *),
                   void *arg);
```

### Paramètre 1 : `pthread_t *thread`

C'est un **pointeur** vers une variable de type `pthread_t`. Cette variable recevra l'identifiant du thread créé.

```c
pthread_t mon_thread;  // Déclaration de la variable

// Passage de l'adresse à pthread_create
pthread_create(&mon_thread, NULL, ma_fonction, NULL);
              // ↑ On passe l'adresse avec &
```

**Pourquoi un pointeur ?** Parce que `pthread_create()` doit **modifier** cette variable pour y stocker l'ID du nouveau thread.

**Important** : Ne modifiez jamais directement le contenu d'une variable `pthread_t`. C'est un type **opaque** géré uniquement par les fonctions pthread.

### Paramètre 2 : `const pthread_attr_t *attr`

Les **attributs** du thread permettent de configurer :
- La taille de la pile (stack size)
- La politique de planification (scheduling policy)
- La priorité
- L'état détaché ou joinable
- Et bien d'autres paramètres...

**Pour débuter**, utilisez toujours `NULL` pour obtenir les attributs par défaut :

```c
pthread_create(&tid, NULL, func, NULL);
                    // ↑ Attributs par défaut
```

Nous verrons les attributs personnalisés dans les sections avancées.

### Paramètre 3 : `void *(*start_routine)(void *)`

C'est un **pointeur vers une fonction**. Cette syntaxe peut sembler complexe, décortiquons-la :

```c
void *(*start_routine)(void *)
└──┬─┘ └──────┬──────┘ └──┬─┘
   │          │           │
   │          │           └─ Prend un void* en paramètre
   │          └─ Nom du pointeur de fonction
   └─ Retourne un void*
```

**En clair** : C'est un pointeur vers une fonction qui :
- Prend un `void*` en paramètre
- Retourne un `void*`

**Exemples de fonctions valides** :

```c
// ✅ Signature correcte
void *ma_fonction(void *arg) {
    // Code du thread
    return NULL;
}

// ✅ Autre signature correcte
void *calculer_somme(void *parametres) {
    // Code du thread
    return NULL;
}

// ❌ Signature incorrecte (ne compile pas)
int ma_fonction(int x) {  // Mauvais type de retour et paramètre
    return 0;
}

// ❌ Signature incorrecte
void ma_fonction(void *arg) {  // Pas de valeur de retour
    printf("Hello\n");
}
```

**Utilisation** :

```c
pthread_t tid;  
pthread_create(&tid, NULL, ma_fonction, NULL);  
                          // ↑ Pas de parenthèses ni de &
                          // On passe le nom de la fonction directement
```

### Paramètre 4 : `void *arg`

C'est l'**argument** passé à la fonction du thread. Étant de type `void*`, vous pouvez passer n'importe quoi :

```c
// Exemple 1 : Passer un entier
int valeur = 42;  
pthread_create(&tid, NULL, func, &valeur);  

// Exemple 2 : Passer une chaîne
char *message = "Hello";  
pthread_create(&tid, NULL, func, message);  

// Exemple 3 : Passer une structure
struct Data {
    int id;
    char name[50];
};
struct Data data = {1, "Thread1"};  
pthread_create(&tid, NULL, func, &data);  

// Exemple 4 : Aucun argument
pthread_create(&tid, NULL, func, NULL);
```

**Dans la fonction du thread**, vous devrez **caster** le `void*` vers le bon type :

```c
void *func(void *arg) {
    // Récupérer un entier
    int valeur = *(int *)arg;
    printf("Valeur reçue : %d\n", valeur);

    return NULL;
}
```

**Valeur de retour** : `pthread_create()` retourne `0` en cas de succès, un code d'erreur sinon.

---

## Gestion des erreurs de création

### Codes d'erreur possibles

```c
int result = pthread_create(&tid, NULL, func, NULL);
```

Les codes d'erreur les plus courants :

| Code | Signification |
|------|---------------|
| `0` | Succès |
| `EAGAIN` | Ressources système insuffisantes (trop de threads) |
| `EINVAL` | Attributs invalides |
| `EPERM` | Pas les permissions nécessaires |

### Vérification des erreurs (méthode simple)

```c
pthread_t tid;  
int result = pthread_create(&tid, NULL, ma_fonction, NULL);  

if (result != 0) {
    fprintf(stderr, "Erreur pthread_create: code %d\n", result);
    exit(EXIT_FAILURE);
}
```

### Vérification avec message d'erreur explicite

Les fonctions pthread ne modifient **pas** `errno`, mais vous pouvez utiliser `strerror()` pour afficher un message lisible :

```c
#include <string.h>  // Pour strerror()

int result = pthread_create(&tid, NULL, func, NULL);  
if (result != 0) {  
    fprintf(stderr, "Erreur pthread_create: %s\n", strerror(result));
    exit(EXIT_FAILURE);
}
```

**Exemple de sortie** :
```
Erreur pthread_create: Resource temporarily unavailable
```

### Fonction utilitaire pour gérer les erreurs

Voici une fonction réutilisable pour simplifier la gestion des erreurs pthread :

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void pthread_error(const char *msg, int errnum) {
    fprintf(stderr, "%s: %s\n", msg, strerror(errnum));
    exit(EXIT_FAILURE);
}

// Utilisation
int result = pthread_create(&tid, NULL, func, NULL);  
if (result != 0) {  
    pthread_error("pthread_create", result);
}
```

---

## Création de threads multiples

### Créer plusieurs threads séquentiellement

```c
#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 5

void *print_hello(void *arg) {
    int thread_num = *(int *)arg;
    printf("Hello depuis le thread #%d (ID: %lu)\n",
           thread_num, pthread_self());
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];  // Tableau d'IDs de threads
    int thread_args[NUM_THREADS];    // Tableau d'arguments
    int i;

    // Création des threads
    for (i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i;  // Préparer l'argument

        int result = pthread_create(&threads[i], NULL,
                                    print_hello, &thread_args[i]);
        if (result != 0) {
            fprintf(stderr, "Erreur création thread %d\n", i);
            return 1;
        }
        printf("Thread %d créé\n", i);
    }

    // Attendre tous les threads
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Tous les threads sont terminés\n");
    return 0;
}
```

**Sortie possible** (l'ordre peut varier) :
```
Thread 0 créé  
Thread 1 créé  
Hello depuis le thread #0 (ID: 140234567890)  
Thread 2 créé  
Hello depuis le thread #1 (ID: 140234567891)  
Thread 3 créé  
Hello depuis le thread #2 (ID: 140234567892)  
Thread 4 créé  
Hello depuis le thread #3 (ID: 140234567893)  
Hello depuis le thread #4 (ID: 140234567894)  
Tous les threads sont terminés  
```

### ⚠️ Piège classique : Passer l'indice de boucle

**Code avec BUG** :

```c
// ❌ INCORRECT : Bug classique
for (int i = 0; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], NULL, print_hello, &i);
                                                  // ↑ DANGER !
}
```

**Problème** : Tous les threads reçoivent l'**adresse** de la même variable `i`. Quand les threads s'exécutent, `i` peut avoir changé de valeur !

**Résultat possible** :
```
Thread #5 (alors qu'on en a créé que 5, numérotés 0-4)  
Thread #5  
Thread #5  
Thread #2  
Thread #5  
```

**Solutions** :

**Solution 1** : Tableau d'arguments (recommandé)
```c
int thread_args[NUM_THREADS];  
for (int i = 0; i < NUM_THREADS; i++) {  
    thread_args[i] = i;
    pthread_create(&threads[i], NULL, print_hello, &thread_args[i]);
}
```

**Solution 2** : Allocation dynamique
```c
for (int i = 0; i < NUM_THREADS; i++) {
    int *arg = malloc(sizeof(int));
    *arg = i;
    pthread_create(&threads[i], NULL, print_hello, arg);
    // N'oubliez pas de free() dans la fonction du thread !
}
```

**Solution 3** : Cast direct (pour les petites valeurs)
```c
void *print_hello(void *arg) {
    int thread_num = (int)(long)arg;  // Cast du pointeur vers int
    printf("Thread #%d\n", thread_num);
    return NULL;
}

for (int i = 0; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], NULL, print_hello, (void *)(long)i);
                                                  // ↑ Cast direct
}
```

**⚠️ Attention** : La solution 3 fonctionne mais est moins portable et ne peut passer que des petites valeurs.

---

## Terminaison de threads

Un thread peut se terminer de **3 manières différentes** :

### 1. Retour normal de la fonction

La méthode la plus simple et recommandée :

```c
void *ma_fonction(void *arg) {
    printf("Début du thread\n");
    // ... travail ...
    printf("Fin du thread\n");

    return NULL;  // Le thread se termine ici
}
```

Vous pouvez retourner une valeur :

```c
void *calculer(void *arg) {
    int *resultat = malloc(sizeof(int));
    *resultat = 42;

    return resultat;  // Retourne un pointeur vers le résultat
}
```

### 2. Appel explicite à pthread_exit()

Équivalent à `return`, mais peut être appelé de n'importe où dans le thread (même dans des fonctions appelées) :

```c
void fonction_auxiliaire(void) {
    printf("Arrêt prématuré\n");
    pthread_exit(NULL);  // Termine le thread immédiatement
}

void *ma_fonction(void *arg) {
    printf("Début\n");
    fonction_auxiliaire();  // Le thread se termine ici
    printf("Jamais exécuté\n");  // ← Code jamais atteint
    return NULL;
}
```

**Utilisation typique** : Sortie anticipée en cas d'erreur :

```c
void *traiter_fichier(void *arg) {
    FILE *f = fopen("data.txt", "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur ouverture fichier\n");
        pthread_exit(NULL);  // Sortie immédiate
    }

    // Traitement du fichier...
    fclose(f);
    return NULL;
}
```

### 3. Annulation par un autre thread (pthread_cancel)

Un thread peut en annuler un autre avec `pthread_cancel()` :

```c
pthread_t tid;  
pthread_create(&tid, NULL, long_task, NULL);  

// Plus tard...
pthread_cancel(tid);  // Demande l'annulation du thread
```

**Note** : Nous verrons l'annulation en détail dans une section ultérieure. C'est un mécanisme complexe et délicat.

### Comparaison return vs pthread_exit()

```c
// Méthode 1 : return
void *thread1(void *arg) {
    printf("Thread 1\n");
    return NULL;  // ← Sortie normale
}

// Méthode 2 : pthread_exit()
void *thread2(void *arg) {
    printf("Thread 2\n");
    pthread_exit(NULL);  // ← Même effet
}

// Méthode 3 : Les deux (redondant)
void *thread3(void *arg) {
    printf("Thread 3\n");
    pthread_exit(NULL);
    return NULL;  // Jamais atteint
}
```

**Recommandation** : Utilisez `return` par défaut. Réservez `pthread_exit()` pour les sorties anticipées ou depuis des fonctions auxiliaires.

---

## Récupération des valeurs de retour

### Principe de base

```c
void *thread_func(void *arg) {
    int *result = malloc(sizeof(int));
    *result = 42;
    return result;  // Retourne un pointeur
}

int main(void) {
    pthread_t tid;
    void *retval;  // Variable pour récupérer la valeur

    pthread_create(&tid, NULL, thread_func, NULL);
    pthread_join(tid, &retval);  // ← Récupère la valeur

    int value = *(int *)retval;  // Cast et déréférencement
    printf("Résultat : %d\n", value);
    free(retval);  // Libération de la mémoire

    return 0;
}
```

**Points clés** :
1. Le thread alloue dynamiquement la mémoire pour le résultat
2. `pthread_join()` récupère le pointeur retourné
3. Le thread principal cast et déréférence le pointeur
4. **Important** : Le thread principal doit libérer la mémoire

### Exemple avec une structure

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int count;
    double average;
    char status[20];
} Result;

void *compute_stats(void *arg) {
    sleep(2);  // Simule un calcul long

    Result *res = malloc(sizeof(Result));
    if (res == NULL) {
        return NULL;  // Erreur d'allocation
    }

    res->count = 100;
    res->average = 75.5;
    snprintf(res->status, sizeof(res->status), "Completed");

    return res;
}

int main(void) {
    pthread_t tid;
    void *retval;

    printf("Démarrage du calcul...\n");
    pthread_create(&tid, NULL, compute_stats, NULL);

    printf("Attente du résultat...\n");
    pthread_join(tid, &retval);

    if (retval != NULL) {
        Result *result = (Result *)retval;
        printf("Résultats:\n");
        printf("  Count: %d\n", result->count);
        printf("  Average: %.2f\n", result->average);
        printf("  Status: %s\n", result->status);
        free(result);
    } else {
        printf("Erreur lors du calcul\n");
    }

    return 0;
}
```

### Retourner plusieurs valeurs

**Option 1 : Structure unique**
```c
typedef struct {
    int somme;
    int produit;
    int max;
} Resultats;

void *calculer(void *arg) {
    Resultats *res = malloc(sizeof(Resultats));
    res->somme = 10 + 20;
    res->produit = 10 * 20;
    res->max = 20;
    return res;
}
```

**Option 2 : Tableau dynamique**
```c
void *calculer(void *arg) {
    int *resultats = malloc(3 * sizeof(int));
    resultats[0] = 30;   // somme
    resultats[1] = 200;  // produit
    resultats[2] = 20;   // max
    return resultats;
}
```

### Que se passe-t-il si on ne récupère pas la valeur ?

```c
void *thread_func(void *arg) {
    int *val = malloc(sizeof(int));
    *val = 42;
    return val;
}

int main(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, thread_func, NULL);
    pthread_join(tid, NULL);  // ← On passe NULL : fuite mémoire !
    return 0;
}
```

**Problème** : La mémoire allouée dans le thread n'est jamais libérée → **fuite mémoire**.

**Solution** : Toujours récupérer et libérer :
```c
void *retval;  
pthread_join(tid, &retval);  
if (retval != NULL) {  
    free(retval);
}
```

---

## pthread_join() en profondeur

### Signature complète

```c
int pthread_join(pthread_t thread, void **retval);
```

### Comportement

`pthread_join()` est une fonction **bloquante** :
- Elle suspend l'exécution du thread appelant
- Elle attend que le thread spécifié se termine
- Elle récupère la valeur de retour (si `retval != NULL`)
- Elle libère les ressources du thread terminé

**Schéma temporel** :

```
Thread principal                Thread secondaire
================                =================
pthread_create() ──────────────→ démarre
       │                             │
       │                          travail...
       │                             │
pthread_join() ──[BLOQUÉ]            │
       │                          travail...
       │                             │
       │                          return/exit
       │                             │
   [DÉBLOQUÉ]←──────────────────── terminé
       │
récupération valeur
       │
suite du programme
```

### Exemple de timing

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

void *travail_long(void *arg) {
    printf("[Thread] Début à %ld\n", time(NULL));
    sleep(5);  // Simule 5 secondes de travail
    printf("[Thread] Fin à %ld\n", time(NULL));
    return NULL;
}

int main(void) {
    pthread_t tid;

    printf("[Main] Création thread à %ld\n", time(NULL));
    pthread_create(&tid, NULL, travail_long, NULL);

    printf("[Main] Avant join à %ld\n", time(NULL));
    pthread_join(tid, NULL);  // Bloque pendant 5 secondes
    printf("[Main] Après join à %ld\n", time(NULL));

    return 0;
}
```

**Sortie** :
```
[Main] Création thread à 1701234567
[Main] Avant join à 1701234567
[Thread] Début à 1701234567
[Thread] Fin à 1701234572
[Main] Après join à 1701234572
```

### Joindre plusieurs threads

Quand vous avez plusieurs threads, vous devez **tous** les joindre :

```c
pthread_t threads[5];

// Création
for (int i = 0; i < 5; i++) {
    pthread_create(&threads[i], NULL, worker, NULL);
}

// Attente de TOUS les threads
for (int i = 0; i < 5; i++) {
    pthread_join(threads[i], NULL);
}

// Ici, on est sûr que tous les threads sont terminés
printf("Tous les threads ont fini\n");
```

### Erreurs possibles de pthread_join()

| Code | Signification |
|------|---------------|
| `0` | Succès |
| `EINVAL` | Le thread n'est pas joinable (détaché) |
| `ESRCH` | Aucun thread avec cet ID |
| `EDEADLK` | Détection de deadlock (thread essaie de se joindre lui-même) |

**Exemple de deadlock** :
```c
void *thread_func(void *arg) {
    pthread_t self = pthread_self();
    pthread_join(self, NULL);  // ❌ ERREUR : EDEADLK
    return NULL;
}
```

### Peut-on joindre un thread plusieurs fois ?

**NON !** C'est un comportement indéfini :

```c
pthread_t tid;  
pthread_create(&tid, NULL, worker, NULL);  

pthread_join(tid, NULL);  // ✅ OK  
pthread_join(tid, NULL);  // ❌ Comportement indéfini !  
```

Après un `pthread_join()` réussi, le thread ID n'est plus valide.

---

## Le cas particulier du thread principal

### Le thread principal est spécial

Le thread qui exécute `main()` est appelé **thread principal**. Il a des propriétés spéciales :

```c
int main(void) {
    printf("ID du thread principal : %lu\n", pthread_self());

    pthread_t tid;
    pthread_create(&tid, NULL, worker, NULL);

    // Si main() se termine, que se passe-t-il ?
    return 0;  // ← Tous les threads sont tués !
}
```

**Comportement** : Quand `main()` fait `return`, **tout le processus se termine**, y compris les threads secondaires en cours d'exécution.

### Solution 1 : pthread_join()

```c
int main(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, worker, NULL);

    pthread_join(tid, NULL);  // Attend la fin du thread
    return 0;  // Maintenant on peut terminer proprement
}
```

### Solution 2 : pthread_exit() dans main()

```c
int main(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, worker, NULL);

    pthread_exit(NULL);  // main() se termine MAIS les threads continuent !

    // Code jamais atteint
    return 0;
}
```

**Différence clé** :
- `return 0;` dans `main()` → Tout le processus se termine
- `pthread_exit(NULL);` dans `main()` → Seulement le thread principal se termine, les autres continuent

**Exemple démonstratif** :

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *long_task(void *arg) {
    for (int i = 1; i <= 10; i++) {
        printf("Thread travaille... %d/10\n", i);
        sleep(1);
    }
    printf("Thread terminé\n");
    return NULL;
}

int main(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, long_task, NULL);

    printf("Main se termine avec pthread_exit\n");
    pthread_exit(NULL);  // Le thread continue à s'exécuter !

    // Jamais atteint
    printf("Cette ligne n'est jamais affichée\n");
}
```

**Sortie** :
```
Main se termine avec pthread_exit  
Thread travaille... 1/10  
Thread travaille... 2/10  
Thread travaille... 3/10  
...
Thread travaille... 10/10  
Thread terminé  
```

Le programme ne se termine que quand le dernier thread (ici `long_task`) se termine.

---

## Cycle de vie complet d'un thread

Voici le cycle de vie d'un thread depuis sa création jusqu'à sa terminaison :

```
┌─────────────────────────────────────────────────────────┐
│                  THREAD PRINCIPAL                       │
│                                                         │
│  pthread_t tid;                                         │
│  pthread_create(&tid, NULL, func, arg); ───┐            │
│                                            │            │
│                                            ▼            │
│                              ┌──────────────────────┐   │
│                              │  THREAD SECONDAIRE   │   │
│                              │                      │   │
│                              │  État: RUNNING       │   │
│                              │                      │   │
│                              │  func(arg) {         │   │
│                              │    // travail        │   │
│                              │    return val;       │   │
│                              │  }                   │   │
│                              │                      │   │
│                              │  État: TERMINATED    │   │
│                              └──────────────────────┘   │
│                                            │            │
│  void *retval;                             │            │
│  pthread_join(tid, &retval); ◄─────────────┘            │
│                   ↓                                     │
│  Ressources libérées, retval récupérée                  │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

**États d'un thread** :

1. **NEW** : Thread créé mais pas encore démarré (très bref, géré par le système)
2. **RUNNABLE** : Thread prêt à s'exécuter ou en cours d'exécution
3. **BLOCKED** : Thread en attente (I/O, mutex, etc.)
4. **TERMINATED** : Thread terminé, ressources en attente de libération
5. **ZOMBIE** : Thread terminé mais pas encore "joiné" (ressources non libérées)

### Thread zombie

Si vous créez un thread mais ne faites jamais `pthread_join()`, le thread devient un **zombie** :

```c
int main(void) {
    for (int i = 0; i < 1000; i++) {
        pthread_t tid;
        pthread_create(&tid, NULL, quick_task, NULL);
        // ❌ Pas de pthread_join() → Fuite de ressources !
    }
    // 1000 threads zombies, ressources non libérées
    return 0;
}
```

**Conséquence** : Fuite de ressources système (mémoire, descripteurs de threads, etc.).

**Solution** : Toujours joindre les threads OU les créer en mode détaché (voir section 18.4).

---

## Bonnes pratiques

### 1. Toujours vérifier les erreurs

```c
// ❌ Mauvais
pthread_create(&tid, NULL, func, NULL);  
pthread_join(tid, NULL);  

// ✅ Bon
int rc;  
rc = pthread_create(&tid, NULL, func, NULL);  
if (rc != 0) {  
    fprintf(stderr, "pthread_create: %s\n", strerror(rc));
    exit(EXIT_FAILURE);
}

rc = pthread_join(tid, NULL);  
if (rc != 0) {  
    fprintf(stderr, "pthread_join: %s\n", strerror(rc));
    exit(EXIT_FAILURE);
}
```

### 2. Gérer proprement la mémoire des valeurs de retour

```c
void *compute(void *arg) {
    int *result = malloc(sizeof(int));
    if (result == NULL) {
        return NULL;  // Indiquer l'erreur
    }
    *result = 42;
    return result;
}

int main(void) {
    pthread_t tid;
    void *retval;

    pthread_create(&tid, NULL, compute, NULL);
    pthread_join(tid, &retval);

    if (retval != NULL) {
        int value = *(int *)retval;
        printf("Résultat : %d\n", value);
        free(retval);  // Libération obligatoire
    }

    return 0;
}
```

### 3. Utiliser des structures pour passer plusieurs paramètres

```c
typedef struct {
    int start;
    int end;
    int *result;
} ThreadData;

void *sum_range(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int sum = 0;

    for (int i = data->start; i <= data->end; i++) {
        sum += i;
    }

    *(data->result) = sum;
    return NULL;
}

int main(void) {
    pthread_t tid;
    int result;
    ThreadData data = { .start = 1, .end = 100, .result = &result };

    pthread_create(&tid, NULL, sum_range, &data);
    pthread_join(tid, NULL);

    printf("Somme : %d\n", result);  // 5050
    return 0;
}
```

### 4. Ne pas passer de pointeurs vers des variables de pile

```c
// ❌ DANGER
void create_threads(void) {
    for (int i = 0; i < 5; i++) {
        int id = i;  // Variable locale
        pthread_t tid;
        pthread_create(&tid, NULL, worker, &id);  // ← BUG !
        // id est détruite à chaque itération
    }
}

// ✅ CORRECT
void create_threads(void) {
    static int ids[5];  // Variable statique ou globale
    for (int i = 0; i < 5; i++) {
        ids[i] = i;
        pthread_t tid;
        pthread_create(&tid, NULL, worker, &ids[i]);
    }
}
```

### 5. Initialiser les variables avant de créer des threads

```c
int compteur = 0;  // Variable partagée

void *incrementer(void *arg) {
    compteur++;  // Utilise la variable partagée
    return NULL;
}

int main(void) {
    compteur = 100;  // ✅ Initialiser AVANT pthread_create

    pthread_t tid;
    pthread_create(&tid, NULL, incrementer, NULL);
    pthread_join(tid, NULL);

    printf("Compteur : %d\n", compteur);
    return 0;
}
```

---

## Erreurs courantes à éviter

### 1. Oublier pthread_join()

```c
// ❌ Fuite de ressources
int main(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, worker, NULL);
    return 0;  // Le thread devient zombie
}
```

### 2. Utiliser le mauvais type de cast

```c
// ❌ Incorrect
void *func(void *arg) {
    int val = (int)arg;  // Cast direct : perte de précision possible
    return NULL;
}

// ✅ Correct (petites valeurs)
void *func(void *arg) {
    int val = (int)(long)arg;  // Double cast pour éviter warnings
    return NULL;
}

// ✅ Correct (recommandé)
void *func(void *arg) {
    int val = *(int *)arg;  // Déréférence du pointeur
    return NULL;
}
```

### 3. Retourner un pointeur vers une variable locale

```c
// ❌ BUG GRAVE
void *compute(void *arg) {
    int result = 42;
    return &result;  // result est détruite à la sortie de la fonction !
}

// ✅ Correct
void *compute(void *arg) {
    int *result = malloc(sizeof(int));
    *result = 42;
    return result;  // Mémoire allouée dynamiquement
}
```

### 4. Ne pas libérer la mémoire des valeurs de retour

```c
// ❌ Fuite mémoire
int main(void) {
    pthread_t tid;
    void *retval;

    pthread_create(&tid, NULL, compute, NULL);
    pthread_join(tid, &retval);

    // Oubli de free(retval) !
    return 0;
}
```

### 5. Créer trop de threads

```c
// ❌ Mauvaise pratique
for (int i = 0; i < 1000000; i++) {
    pthread_t tid;
    pthread_create(&tid, NULL, small_task, NULL);
}
```

**Problème** : Création de millions de threads → surcharge système.

**Solution** : Utiliser un **pool de threads** (nombre limité de threads réutilisables).

---

## Récapitulatif

Dans cette section, vous avez appris :

✅ **pthread_create() en détail** :
- Les 4 paramètres et leur signification
- Gestion des erreurs avec `strerror()`
- Création de threads multiples

✅ **Terminaison de threads** :
- `return` vs `pthread_exit()`
- Les 3 façons de terminer un thread
- Retourner des valeurs

✅ **pthread_join() en profondeur** :
- Comportement bloquant
- Récupération de valeurs de retour
- Gestion de plusieurs threads

✅ **Thread principal** :
- Différence `return` vs `pthread_exit()` dans `main()`
- Impact sur les threads secondaires

✅ **Cycle de vie** :
- États d'un thread
- Problème des threads zombies

✅ **Bonnes pratiques** :
- Vérification des erreurs
- Gestion mémoire
- Pièges à éviter

---

## Pour aller plus loin

Dans les prochaines sections :

- **18.3** : Passage de paramètres avancé (structures complexes, multiples arguments)
- **18.4** : Threads détachés vs joinable (et quand les utiliser)
- **18.5** : Race conditions et problèmes de concurrence
- **18.6** : Mutex et synchronisation

**Conseil pratique** : Avant de continuer, écrivez quelques programmes créant 3-5 threads avec passage de paramètres et récupération de valeurs de retour. Utilisez Valgrind pour vérifier l'absence de fuites mémoire.

---

📘 **Ressources utiles** :
- `man pthread_create`
- `man pthread_join`
- `man pthread_exit`

💡 **Prochaine étape** : Section 18.3 - Passage de paramètres

⏭️ [Passage de paramètres](/18-threads-et-concurrence/03-passage-parametres.md)
