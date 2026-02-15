🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 18.1 Introduction à pthread

## Table des matières
- [Qu'est-ce qu'un thread ?](#quest-ce-quun-thread-)
- [Processus vs Threads](#processus-vs-threads)
- [Pourquoi utiliser les threads ?](#pourquoi-utiliser-les-threads-)
- [La bibliothèque POSIX Threads (pthread)](#la-biblioth%C3%A8que-posix-threads-pthread)
- [Installation et compilation](#installation-et-compilation)
- [Premier exemple : Hello World multi-threadé](#premier-exemple--hello-world-multi-thread%C3%A9)
- [Anatomie d'un programme pthread](#anatomie-dun-programme-pthread)
- [Concepts clés à retenir](#concepts-cl%C3%A9s-%C3%A0-retenir)
- [Quand utiliser les threads ?](#quand-utiliser-les-threads-)
- [Points d'attention pour débutants](#points-dattention-pour-d%C3%A9butants)

---

## Qu'est-ce qu'un thread ?

Un **thread** (ou fil d'exécution) est la plus petite unité d'exécution qu'un système d'exploitation peut planifier. Imaginez votre programme comme une usine :

- **Sans threads** : Vous avez un seul ouvrier qui fait toutes les tâches l'une après l'autre (séquentiel)
- **Avec threads** : Vous avez plusieurs ouvriers qui peuvent travailler simultanément sur différentes tâches (parallèle)

### Exemple concret

Prenons un programme qui doit :
1. Télécharger un fichier depuis Internet
2. Compresser des images
3. Répondre aux clics de l'utilisateur

**Sans threads** :
```
[Téléchargement...] → [Compression...] → [Interface bloquée !]
```
L'utilisateur doit attendre que tout soit fini avant de pouvoir cliquer.

**Avec threads** :
```
Thread 1: [Téléchargement...]  
Thread 2: [Compression...]  
Thread 3: [Interface réactive !]  
```
Tout se passe en même temps, l'interface reste fluide.

---

## Processus vs Threads

Il est essentiel de bien comprendre la différence entre un **processus** et un **thread**.

### Processus
Un processus est un programme en cours d'exécution qui possède :
- Son propre espace mémoire (isolé des autres processus)
- Ses propres ressources système (fichiers ouverts, variables d'environnement, etc.)
- Au moins un thread (le thread principal)

```
┌─────────────────────────────────┐
│      PROCESSUS A                │
│  ┌───────────────────────────┐  │
│  │   Espace mémoire propre   │  │
│  │   (pile, tas, variables)  │  │
│  └───────────────────────────┘  │
│          Thread principal       │
└─────────────────────────────────┘

┌─────────────────────────────────┐
│      PROCESSUS B                │
│  ┌───────────────────────────┐  │
│  │   Espace mémoire propre   │  │
│  │   (complètement séparé)   │  │
│  └───────────────────────────┘  │
│          Thread principal       │
└─────────────────────────────────┘
```

### Threads
Les threads d'un même processus partagent :
- ✅ Le même espace mémoire (variables globales, tas)
- ✅ Les mêmes fichiers ouverts
- ✅ Les mêmes ressources système

Mais chaque thread a :
- ❌ Sa propre pile d'exécution
- ❌ Ses propres registres CPU
- ❌ Son propre compteur de programme (PC)

```
┌──────────────────────────────────────────┐
│            PROCESSUS                     │
│  ┌────────────────────────────────────┐  │
│  │  MÉMOIRE PARTAGÉE                  │  │
│  │  (variables globales, tas)         │  │
│  └────────────────────────────────────┘  │
│                                          │
│  Thread 1        Thread 2      Thread 3  │
│  [pile #1]       [pile #2]     [pile #3] │
│     ▼               ▼              ▼     │
│  fonction_A()   fonction_B()  fonction_C │
└──────────────────────────────────────────┘
```

### Comparaison rapide

| Critère | Processus | Thread |
|---------|-----------|--------|
| **Création** | Lourd (fork) | Léger (pthread_create) |
| **Mémoire** | Isolée | Partagée |
| **Communication** | IPC complexe | Variables partagées |
| **Temps de création** | ~millisecondes | ~microsecondes |
| **Changement de contexte** | Coûteux | Rapide |
| **Isolation** | Forte (crash = 1 processus) | Faible (crash = tout le processus) |

---

## Pourquoi utiliser les threads ?

### 1. **Performance sur multicœurs**
Les processeurs modernes ont plusieurs cœurs. Les threads permettent d'utiliser tous les cœurs simultanément.

```c
// Sans threads : utilise 1 cœur
for (int i = 0; i < 1000000; i++) {
    calcul_complexe(i);
}
// Temps : 10 secondes sur 1 cœur

// Avec 4 threads : utilise 4 cœurs
// Temps : ~2.5 secondes (4x plus rapide théoriquement)
```

### 2. **Réactivité**
Les threads permettent de séparer les tâches longues des tâches interactives.

**Exemple : Serveur web**
```
Thread principal  → Accepte les nouvelles connexions (rapide)  
Thread worker 1   → Traite requête client 1 (peut être lent)  
Thread worker 2   → Traite requête client 2 (peut être lent)  
Thread worker 3   → Traite requête client 3 (peut être lent)  
```

Le serveur reste réactif même si une requête prend du temps.

### 3. **Modélisation naturelle**
Certains problèmes se modélisent naturellement avec plusieurs fils d'exécution :
- Serveur de chat : 1 thread par utilisateur connecté
- Jeu vidéo : 1 thread pour le rendu, 1 pour la physique, 1 pour l'IA
- Pipeline de traitement : 1 thread par étape

---

## La bibliothèque POSIX Threads (pthread)

**POSIX Threads** (ou **pthreads**) est la bibliothèque standard pour la programmation multi-threadée sur les systèmes UNIX/Linux. Elle est :

- ✅ **Portable** : Fonctionne sur Linux, macOS, BSD, Solaris, etc.
- ✅ **Standardisée** : Définie par la norme POSIX (IEEE 1003.1c)
- ✅ **Complète** : Threads, mutex, variables de condition, sémaphores, etc.
- ✅ **Intégrée** : Fait partie de la libc (glibc sur Linux)

### Pourquoi pthread et pas autre chose ?

Il existe d'autres solutions pour faire du multithreading en C :
- **OpenMP** : Plus simple mais moins flexible (pragmas)
- **C11 threads (threads.h)** : Standard C11 mais moins adopté, moins de fonctionnalités
- **Threads Windows** : Spécifique à Windows (non portable)

**pthread** reste le choix standard pour Linux et la programmation système.

---

## Installation et compilation

### Vérification de pthread

Sur Ubuntu et la plupart des distributions Linux modernes, pthread est déjà installé :

```bash
# Vérifier la présence de la bibliothèque
ls -l /usr/lib/x86_64-linux-gnu/libpthread.so*
```

### Compilation avec pthread

Pour compiler un programme utilisant pthread, vous devez :
1. Inclure le header `<pthread.h>`
2. Lier avec la bibliothèque pthread via l'option `-pthread`

```bash
# Syntaxe de compilation
gcc -Wall -Wextra -pthread mon_programme.c -o mon_programme

# Ou avec l'ancienne syntaxe (déconseillée)
gcc -Wall -Wextra mon_programme.c -o mon_programme -lpthread
```

**⚠️ Important** : L'option `-pthread` doit être utilisée à la fois pour la compilation ET le linking. Elle fait plus que simplement lier la bibliothèque : elle active aussi des définitions de macros nécessaires.

---

## Premier exemple : Hello World multi-threadé

Voici un premier programme simple qui crée un thread secondaire :

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>  // Pour sleep()

// Fonction exécutée par le thread secondaire
void *thread_function(void *arg) {
    (void)arg;
    printf("Hello depuis le thread secondaire !\n");
    printf("ID du thread : %lu\n", pthread_self());
    sleep(1);  // Simule un travail
    printf("Le thread secondaire se termine.\n");
    return NULL;
}

int main(void) {
    pthread_t thread_id;  // Variable pour stocker l'ID du thread
    int result;

    printf("Hello depuis le thread principal (main) !\n");
    printf("ID du thread principal : %lu\n", pthread_self());

    // Création du thread secondaire
    result = pthread_create(&thread_id, NULL, thread_function, NULL);
    if (result != 0) {
        fprintf(stderr, "Erreur lors de la création du thread\n");
        return EXIT_FAILURE;
    }

    printf("Thread secondaire créé avec succès (ID: %lu)\n", thread_id);

    // Attendre la fin du thread secondaire
    result = pthread_join(thread_id, NULL);
    if (result != 0) {
        fprintf(stderr, "Erreur lors de l'attente du thread\n");
        return EXIT_FAILURE;
    }

    printf("Le thread principal se termine.\n");
    return EXIT_SUCCESS;
}
```

### Compilation et exécution

```bash
$ gcc -Wall -Wextra -pthread hello_thread.c -o hello_thread
$ ./hello_thread
Hello depuis le thread principal (main) !  
ID du thread principal : 140234567890  
Thread secondaire créé avec succès (ID: 140234567123)  
Hello depuis le thread secondaire !  
ID du thread : 140234567123  
Le thread secondaire se termine.  
Le thread principal se termine.  
```

### Que se passe-t-il ?

1. **Lignes 1-4** : Inclusion des headers nécessaires
2. **Lignes 7-13** : Définition de la fonction du thread
   - Signature obligatoire : `void *fonction(void *arg)`
   - Retourne `NULL` à la fin
3. **Ligne 16** : Déclaration d'une variable `pthread_t` pour stocker l'ID du thread
4. **Ligne 23** : Création du thread avec `pthread_create()`
5. **Ligne 32** : Attente de la fin du thread avec `pthread_join()`

---

## Anatomie d'un programme pthread

### 1. Le type `pthread_t`

```c
pthread_t thread_id;
```

`pthread_t` est un type **opaque** qui représente l'identifiant d'un thread. Vous ne devez jamais manipuler directement son contenu, seulement le passer aux fonctions pthread.

### 2. La fonction `pthread_create()`

```c
int pthread_create(pthread_t *thread,
                   const pthread_attr_t *attr,
                   void *(*start_routine)(void *),
                   void *arg);
```

**Paramètres** :
- `thread` : Pointeur vers une variable `pthread_t` qui recevra l'ID du thread créé
- `attr` : Attributs du thread (taille de pile, priorité, etc.) - `NULL` pour les valeurs par défaut
- `start_routine` : Pointeur vers la fonction que le thread va exécuter
- `arg` : Argument à passer à la fonction (un pointeur `void*`)

**Retour** :
- `0` en cas de succès
- Code d'erreur (non-zéro) en cas d'échec

**Exemple** :
```c
pthread_t tid;  
int result = pthread_create(&tid, NULL, ma_fonction, NULL);  
if (result != 0) {  
    fprintf(stderr, "Erreur pthread_create : %d\n", result);
}
```

### 3. La signature de la fonction du thread

Toute fonction exécutée par un thread doit avoir cette signature :

```c
void *nom_fonction(void *arg)
```

- **Retour** : `void*` (pointeur générique)
- **Paramètre** : `void*` (pointeur générique)

Cette signature permet une grande flexibilité : vous pouvez passer et retourner n'importe quel type de données via des pointeurs.

**Exemple avec paramètre** :
```c
void *print_number(void *arg) {
    int num = *(int *)arg;  // Cast du void* vers int*
    printf("Nombre reçu : %d\n", num);
    return NULL;
}

int main(void) {
    pthread_t tid;
    int number = 42;
    pthread_create(&tid, NULL, print_number, &number);
    pthread_join(tid, NULL);
    return 0;
}
```

### 4. La fonction `pthread_join()`

```c
int pthread_join(pthread_t thread, void **retval);
```

`pthread_join()` **bloque** le thread appelant jusqu'à ce que le thread spécifié se termine. C'est l'équivalent de `wait()` pour les processus.

**Paramètres** :
- `thread` : ID du thread à attendre
- `retval` : Pointeur pour récupérer la valeur de retour du thread (ou `NULL` si on ne s'y intéresse pas)

**Exemple de récupération de valeur** :
```c
void *compute_sum(void *arg) {
    int *result = malloc(sizeof(int));
    *result = 10 + 20;
    return result;  // Retourne un pointeur
}

int main(void) {
    pthread_t tid;
    void *ret;

    pthread_create(&tid, NULL, compute_sum, NULL);
    pthread_join(tid, &ret);  // Récupère la valeur

    int sum = *(int *)ret;
    printf("Somme calculée : %d\n", sum);
    free(ret);  // Ne pas oublier de libérer

    return 0;
}
```

### 5. La fonction `pthread_self()`

```c
pthread_t pthread_self(void);
```

Retourne l'ID du thread **appelant**. Utile pour identifier quel thread exécute un morceau de code.

```c
void *my_function(void *arg) {
    printf("Mon ID : %lu\n", pthread_self());
    return NULL;
}
```

### 6. La fonction `pthread_exit()`

```c
void pthread_exit(void *retval);
```

Termine le thread appelant et retourne `retval`. Équivalent à `return retval;` dans la fonction du thread.

**Différence importante** :
```c
void *thread_func(void *arg) {
    // Ces deux formes sont équivalentes :
    return NULL;          // Option 1 : return classique
    // ou
    // pthread_exit(NULL);  // Option 2 : terminaison explicite
}
```

**Cas particulier** : Si le thread principal (main) appelle `pthread_exit()`, il se termine mais les autres threads continuent de s'exécuter !

```c
int main(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, long_task, NULL);

    pthread_exit(NULL);  // main se termine mais long_task continue
    // Pas besoin de pthread_join ici
}
```

---

## Concepts clés à retenir

### 1. Les threads partagent la mémoire

```c
int compteur_global = 0;  // Partagé entre tous les threads

void *incrementer(void *arg) {
    for (int i = 0; i < 100000; i++) {
        compteur_global++;  // ⚠️ DANGEREUX sans synchronisation !
    }
    return NULL;
}
```

**Problème** : Si deux threads modifient `compteur_global` en même temps, il y aura des **race conditions** (courses critiques). Nous verrons comment résoudre cela avec les mutex dans les sections suivantes.

### 2. Chaque thread a sa propre pile

```c
void *thread_func(void *arg) {
    int variable_locale = 42;  // Unique à ce thread
    printf("Adresse : %p\n", (void*)&variable_locale);
    return NULL;
}
```

Si vous créez 3 threads exécutant cette fonction, vous obtiendrez 3 adresses différentes car chaque thread a sa propre pile.

### 3. Ordre d'exécution non déterministe

**Vous ne pouvez PAS prédire** dans quel ordre les threads s'exécuteront :

```c
void *thread1(void *arg) { printf("A\n"); return NULL; }  
void *thread2(void *arg) { printf("B\n"); return NULL; }  
void *thread3(void *arg) { printf("C\n"); return NULL; }  

// Possible : ABC, ACB, BAC, BCA, CAB, CBA
// Impossible à garantir sans synchronisation
```

### 4. Toujours gérer les erreurs

Les fonctions pthread retournent `0` en cas de succès, un code d'erreur sinon :

```c
int result = pthread_create(&tid, NULL, func, NULL);  
if (result != 0) {  
    // ERREUR : ne pas utiliser errno avec pthread !
    fprintf(stderr, "pthread_create failed: %d\n", result);
}
```

**⚠️ Important** : Les fonctions pthread **ne modifient PAS errno**. Elles retournent directement le code d'erreur.

---

## Quand utiliser les threads ?

### ✅ Utilisez les threads quand :

1. **Vous avez des tâches parallélisables** : Traiter un grand tableau en le divisant en segments
2. **Vous avez besoin de réactivité** : Interface graphique + calculs lourds en arrière-plan
3. **Vous développez un serveur** : Gérer plusieurs clients simultanément
4. **Vous voulez utiliser tous les cœurs CPU** : Calculs scientifiques, compression, rendu

### ❌ N'utilisez PAS les threads quand :

1. **Le problème est séquentiel** : Chaque étape dépend de la précédente
2. **La communication est complexe** : Trop de synchronisation = perte de performance
3. **Vous débutez en C** : Maîtrisez d'abord les pointeurs, la mémoire, les processus
4. **L'isolation est critique** : Préférez les processus si un crash ne doit pas tout arrêter

---

## Points d'attention pour débutants

### 1. Toujours joindre vos threads

Si vous ne faites pas `pthread_join()`, le thread principal peut se terminer avant les threads secondaires :

```c
// ❌ MAUVAIS : Fuite de ressources
int main(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, task, NULL);
    return 0;  // Termine sans attendre le thread !
}

// ✅ BON
int main(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, task, NULL);
    pthread_join(tid, NULL);  // Attend la fin
    return 0;
}
```

**Exception** : Les threads détachés (nous verrons cela plus tard).

### 2. Ne pas passer de pointeurs vers des variables locales

```c
// ❌ DANGEREUX : data sera détruite quand la fonction retourne
void lancer_thread(void) {
    int data = 42;
    pthread_t tid;
    pthread_create(&tid, NULL, func, &data);
    // Pas de join ici : la fonction retourne,
    // data est détruite, le thread lit un pointeur invalide !
}

// ✅ SOLUTION 1 : Variable statique
void creer_thread(void) {
    static int data = 42;  // Reste en mémoire
    pthread_t tid;
    pthread_create(&tid, NULL, func, &data);
    pthread_join(tid, NULL);
}

// ✅ SOLUTION 2 : Allocation dynamique
void creer_thread(void) {
    int *data = malloc(sizeof(int));
    *data = 42;
    pthread_t tid;
    pthread_create(&tid, NULL, func, data);
    pthread_join(tid, NULL);
    free(data);
}
```

### 3. Déboguer les programmes multi-threadés est difficile

- Les bugs peuvent être **non reproductibles** (dépendent du timing)
- Utilisez des outils comme **Valgrind** avec `--tool=helgrind`
- Utilisez **ThreadSanitizer** : `gcc -fsanitize=thread`
- Ajoutez des `printf()` pour tracer l'exécution (temporairement)

### 4. Commencez simple

Ne créez pas 1000 threads pour votre premier programme ! Commencez par :
1. Un seul thread secondaire
2. Deux threads qui affichent des messages
3. Trois threads avec passage de paramètres simples

---

## Récapitulatif

Dans cette introduction à pthread, vous avez appris :

- ✅ **Ce qu'est un thread** : Unité d'exécution au sein d'un processus
- ✅ **Processus vs Threads** : Différences clés (mémoire, création, isolation)
- ✅ **Pourquoi utiliser pthread** : Performance, réactivité, modélisation naturelle
- ✅ **Les fonctions de base** :
    - `pthread_create()` : Créer un thread
    - `pthread_join()` : Attendre un thread
    - `pthread_self()` : Obtenir son propre ID
    - `pthread_exit()` : Terminer un thread

- ✅ **Les pièges à éviter** : Race conditions, pointeurs invalides, oubli de join

---

## Pour aller plus loin

Dans les sections suivantes, nous verrons :

- **18.2** : Création et terminaison de threads (en détail)
- **18.3** : Passage de paramètres avancé
- **18.4** : Threads détachés vs joinable
- **18.5** : Race conditions et problèmes de concurrence
- **18.6** : Mutex et sections critiques
- **18.7** : Deadlocks et comment les éviter

**Conseil** : Pratiquez les exemples de cette section avant de continuer. Créez des programmes simples avec 2-3 threads et observez leur comportement.

---

📘 **Ressources utiles** :
- `man pthread_create` : Documentation complète
- `man pthreads` : Vue d'ensemble de l'API pthread
- [POSIX Threads Programming](https://hpc-tutorials.llnl.gov/posix/) - Tutorial LLNL

💡 **Prochaine étape** : Section 18.2 - Création et terminaison de threads

⏭️ [Création et terminaison de threads](/18-threads-et-concurrence/02-creation-terminaison.md)
