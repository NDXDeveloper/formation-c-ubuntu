🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 20.8 Serveur Concurrent

## Introduction

Jusqu'à présent, nous avons créé des **serveurs itératifs** : ils ne peuvent traiter qu'**un seul client à la fois**. Si un client est lent ou reste connecté longtemps, tous les autres clients doivent attendre leur tour.

Pour les applications réelles, ce n'est pas acceptable. Un vrai serveur doit pouvoir gérer **plusieurs clients simultanément**. C'est ce qu'on appelle un **serveur concurrent**.

Dans cette section, nous allons explorer les trois principales approches pour créer des serveurs concurrents en C :
1. **Multi-processus** (fork)
2. **Multi-threads** (pthreads)
3. **I/O Multiplexing** (select, poll, epoll)

**Objectif :** Comprendre les avantages, inconvénients et cas d'usage de chaque approche.

---

## Le Problème du Serveur Itératif

### Rappel : Serveur itératif simple

```c
while (1) {
    client_fd = accept(server_fd, ...);        // Attendre un client
    handle_client(client_fd);                  // Traiter le client
    close(client_fd);                          // Fermer
    // Seulement maintenant, accept() le client suivant
}
```

### Limitations critiques

❌ **Un seul client à la fois** : Les autres clients sont bloqués dans la file d'attente

❌ **Client lent = serveur bloqué** : Si un client prend 10 secondes, tous les autres attendent 10 secondes

❌ **Mauvaise utilisation des ressources** : Le CPU reste inactif pendant les I/O

❌ **Expérience utilisateur dégradée** : Timeouts fréquents, latence élevée

### Exemple du problème

```
Client 1 : Se connecte à 12:00:00, traité pendant 5 secondes
Client 2 : Se connecte à 12:00:01, ATTEND jusqu'à 12:00:05 !
Client 3 : Se connecte à 12:00:02, ATTEND jusqu'à 12:00:10 !
```

**Inacceptable pour un serveur en production !**

---

## Les Solutions : Vue d'Ensemble

| Approche | Principe | Avantages | Inconvénients |
|----------|----------|-----------|---------------|
| **Multi-processus** | Un processus fils par client | Isolation forte, simple | Coût mémoire élevé, communication complexe |
| **Multi-threads** | Un thread par client | Léger, partage mémoire | Synchronisation complexe, risque de bugs |
| **I/O Multiplexing** | Un seul processus/thread | Très efficace, scalable | Complexité du code, architecture différente |

**Il n'y a pas de solution universelle** : le choix dépend de votre application.

---

## Approche 1 : Multi-Processus avec `fork()`

### Concept

Pour chaque client qui se connecte, créer un **processus fils** dédié qui gère ce client.

```
Processus Parent (serveur)
    ├─ accept() client 1 → fork() → Processus Fils 1 (gère client 1)
    ├─ accept() client 2 → fork() → Processus Fils 2 (gère client 2)
    └─ accept() client 3 → fork() → Processus Fils 3 (gère client 3)
```

### La fonction `fork()`

```c
#include <unistd.h>

pid_t fork(void);
```

**Fonctionnement :**
- Crée une **copie** du processus actuel
- **Parent** : `fork()` retourne le PID du fils
- **Fils** : `fork()` retourne 0
- **Erreur** : `fork()` retourne -1

**Après `fork()` :**
- Les deux processus (parent et fils) s'exécutent **en parallèle**
- Le fils hérite des descripteurs de fichiers ouverts (dont les sockets)
- Chaque processus a sa propre mémoire (copie)

### Schéma de fonctionnement

```c
pid_t pid = fork();

if (pid < 0) {
    // Erreur
    perror("fork");
} else if (pid == 0) {
    // Code du FILS
    printf("Je suis le fils, PID = %d\n", getpid());
} else {
    // Code du PARENT
    printf("Je suis le parent, mon fils a le PID = %d\n", pid);
}
```

### Exemple Complet : Serveur Multi-Processus

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Gestionnaire pour nettoyer les processus zombies
void sigchld_handler(int sig) {
    (void)sig;
    // Attendre tous les fils terminés (sans bloquer)
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    printf("[PID %d] Client connecté\n", getpid());

    while ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("[PID %d] Reçu : %s", getpid(), buffer);

        // Écho
        if (send(client_fd, buffer, bytes_received, 0) < 0) {
            perror("send");
            break;
        }
    }

    if (bytes_received == 0) {
        printf("[PID %d] Client déconnecté\n", getpid());
    } else {
        perror("recv");
    }

    close(client_fd);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;

    // Installer le gestionnaire de signal pour SIGCHLD
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;  // Redémarrer accept() si interrompu
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Créer le socket serveur
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // SO_REUSEADDR
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Serveur multi-processus en écoute sur le port %d\n", PORT);

    // Boucle principale
    while (1) {
        client_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        // Afficher l'IP du client
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Nouvelle connexion de %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        // Créer un processus fils pour ce client
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            close(client_fd);
            continue;
        }

        if (pid == 0) {
            // PROCESSUS FILS
            // Le fils n'a pas besoin du socket serveur
            close(server_fd);

            // Gérer le client
            handle_client(client_fd);

            // Terminer le processus fils
            exit(EXIT_SUCCESS);
        } else {
            // PROCESSUS PARENT
            // Le parent n'a pas besoin du socket client (le fils s'en occupe)
            close(client_fd);

            // Le parent continue immédiatement à accept() d'autres clients
        }
    }

    close(server_fd);
    return 0;
}
```

**Compilation et test :**
```bash
gcc -o server_fork server_fork.c -Wall -Wextra
./server_fork
```

**Tester avec plusieurs clients :**
```bash
# Terminal 1
telnet localhost 8080

# Terminal 2
telnet localhost 8080

# Terminal 3
telnet localhost 8080
```

Tous les clients sont servis **simultanément** !

---

### Gestion des Processus Zombies

#### Problème

Quand un processus fils termine, il devient un **zombie** jusqu'à ce que le parent appelle `wait()` ou `waitpid()`.

**Conséquence :** Accumulation de zombies, épuisement de la table des processus.

#### Solution 1 : Signal SIGCHLD

```c
void sigchld_handler(int sig) {
    (void)sig;
    // Nettoyer tous les fils terminés
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// Dans main()
signal(SIGCHLD, sigchld_handler);
// ou mieux :
struct sigaction sa;
sa.sa_handler = sigchld_handler;
sigemptyset(&sa.sa_mask);
sa.sa_flags = SA_RESTART;
sigaction(SIGCHLD, &sa, NULL);
```

#### Solution 2 : Ignorer SIGCHLD (Linux)

```c
signal(SIGCHLD, SIG_IGN);
```

Sur Linux, cela demande au kernel de nettoyer automatiquement les zombies.

---

### Avantages du Multi-Processus

✅ **Isolation forte** : Chaque client dans son propre espace mémoire

✅ **Sécurité** : Un crash d'un processus fils n'affecte pas les autres

✅ **Simplicité** : Pas de synchronisation à gérer

✅ **Robustesse** : Échec d'un client = seulement ce processus meurt

✅ **Debugging facile** : Chaque processus peut être debuggé indépendamment

### Inconvénients du Multi-Processus

❌ **Coût mémoire élevé** : Chaque processus duplique la mémoire

❌ **Coût de création** : `fork()` est relativement lent

❌ **Scalabilité limitée** : Difficile de gérer 10 000+ clients

❌ **Communication complexe** : IPC nécessaire pour communiquer entre processus

❌ **Contexte switching** : Changements de contexte coûteux avec beaucoup de processus

### Quand utiliser Multi-Processus ?

✅ Applications avec peu de clients simultanés (< 100)

✅ Nécessité d'isolation forte (sécurité)

✅ Clients avec des traitements longs et indépendants

✅ Simplification du code (pas de threads)

---

## Approche 2 : Multi-Threads avec `pthreads`

### Concept

Au lieu de créer des processus, créer des **threads** légers qui partagent le même espace mémoire.

```
Processus Serveur
    ├─ Thread Principal (accept des clients)
    ├─ Thread 1 (gère client 1)
    ├─ Thread 2 (gère client 2)
    └─ Thread 3 (gère client 3)
```

### Avantages des Threads vs Processus

| Caractéristique | Processus | Thread |
|-----------------|-----------|--------|
| **Mémoire** | Séparée (copie) | Partagée |
| **Coût création** | Élevé (~1-2 ms) | Faible (~10-100 µs) |
| **Communication** | IPC nécessaire | Variables partagées |
| **Isolation** | Forte | Aucune |
| **Scalabilité** | Limitée | Meilleure |

### API pthread Essentielle

```c
#include <pthread.h>

// Créer un thread
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void*), void *arg);

// Attendre qu'un thread se termine
int pthread_join(pthread_t thread, void **retval);

// Se détacher (pas besoin de join)
int pthread_detach(pthread_t thread);

// Terminer le thread courant
void pthread_exit(void *retval);
```

---

### Exemple Complet : Serveur Multi-Threads

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Structure pour passer des arguments au thread
typedef struct {
    int client_fd;
    struct sockaddr_in client_addr;
} client_info_t;

// Fonction exécutée par chaque thread
void* handle_client_thread(void *arg) {
    client_info_t *info = (client_info_t*)arg;
    int client_fd = info->client_fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // Afficher l'IP du client
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &info->client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    printf("[Thread %lu] Client connecté : %s:%d\n",
           pthread_self(), client_ip, ntohs(info->client_addr.sin_port));

    // Libérer la mémoire allouée pour info
    free(info);

    // Gérer le client (écho)
    while ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("[Thread %lu] Reçu : %s", pthread_self(), buffer);

        if (send(client_fd, buffer, bytes_received, 0) < 0) {
            perror("send");
            break;
        }
    }

    if (bytes_received == 0) {
        printf("[Thread %lu] Client déconnecté\n", pthread_self());
    } else {
        perror("recv");
    }

    close(client_fd);
    return NULL;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    pthread_t thread;

    // Créer le socket serveur
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // SO_REUSEADDR
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Serveur multi-threads en écoute sur le port %d\n", PORT);

    // Boucle principale
    while (1) {
        client_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        // Allouer une structure pour passer les infos au thread
        client_info_t *info = malloc(sizeof(client_info_t));
        if (info == NULL) {
            perror("malloc");
            close(client_fd);
            continue;
        }

        info->client_fd = client_fd;
        info->client_addr = client_addr;

        // Créer un thread pour ce client
        if (pthread_create(&thread, NULL, handle_client_thread, info) != 0) {
            perror("pthread_create");
            free(info);
            close(client_fd);
            continue;
        }

        // Détacher le thread (pas besoin de join)
        pthread_detach(thread);
    }

    close(server_fd);
    return 0;
}
```

**Compilation :**
```bash
gcc -o server_pthread server_pthread.c -Wall -Wextra -pthread
#                                                       ^^^^^^^^ Important !
```

---

### Thread Détaché vs Joinable

#### Thread Joinable (défaut)

```c
pthread_t thread;
pthread_create(&thread, NULL, func, arg);

// Le thread principal doit attendre
pthread_join(thread, NULL);
```

**Problème :** Si on ne fait pas `join()`, fuite de ressources.

#### Thread Détaché (recommandé pour serveurs)

```c
pthread_t thread;
pthread_create(&thread, NULL, func, arg);

// Le thread se nettoie automatiquement à la fin
pthread_detach(thread);
```

**Avantage :** Pas besoin de tracker les threads, nettoyage automatique.

---

### Synchronisation : Le Problème du Partage de Mémoire

#### Exemple de Race Condition

```c
int client_count = 0;  // Variable globale partagée

void* handle_client_thread(void *arg) {
    client_count++;  // ⚠️ RACE CONDITION !
    printf("Clients : %d\n", client_count);

    // ... gérer le client ...

    client_count--;  // ⚠️ RACE CONDITION !
    return NULL;
}
```

**Problème :** Deux threads peuvent modifier `client_count` en même temps.

```
Thread 1 lit client_count = 5
Thread 2 lit client_count = 5    (avant que Thread 1 écrive)
Thread 1 écrit client_count = 6
Thread 2 écrit client_count = 6  (❌ devrait être 7 !)
```

#### Solution : Mutex

```c
#include <pthread.h>

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
int client_count = 0;

void* handle_client_thread(void *arg) {
    // Verrouiller
    pthread_mutex_lock(&count_mutex);
    client_count++;
    int count = client_count;
    pthread_mutex_unlock(&count_mutex);
    // Déverrouiller

    printf("Clients : %d\n", count);

    // ... gérer le client ...

    pthread_mutex_lock(&count_mutex);
    client_count--;
    pthread_mutex_unlock(&count_mutex);

    return NULL;
}
```

**Règle d'or :** Toute variable partagée entre threads doit être protégée par un mutex.

---

### Thread Pool (Pool de Threads)

Au lieu de créer un thread par client, créer un **pool fixe de threads** qui se partagent les clients.

**Avantages :**
- Limiter le nombre de threads (contrôle des ressources)
- Éviter le coût de création/destruction répétée
- Meilleure scalabilité

**Principe :**
```
File de tâches (clients en attente)
    ↓
Thread Pool (5-10 threads workers)
    ↓
Chaque thread prend un client, le traite, puis en prend un autre
```

**Implémentation simplifiée :**
```c
#define THREAD_POOL_SIZE 10

typedef struct {
    int client_fd;
    // Autres infos...
} task_t;

// File de tâches (implémentation omise)
task_queue_t task_queue;
pthread_mutex_t queue_mutex;
pthread_cond_t queue_cond;

void* worker_thread(void *arg) {
    while (1) {
        // Attendre une tâche
        pthread_mutex_lock(&queue_mutex);
        while (queue_empty(&task_queue)) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        }

        task_t task = queue_pop(&task_queue);
        pthread_mutex_unlock(&queue_mutex);

        // Traiter le client
        handle_client(task.client_fd);
    }
    return NULL;
}

int main() {
    // Créer le pool de threads
    pthread_t threads[THREAD_POOL_SIZE];
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_create(&threads[i], NULL, worker_thread, NULL);
    }

    // Boucle accept
    while (1) {
        int client_fd = accept(server_fd, ...);

        // Ajouter à la file
        pthread_mutex_lock(&queue_mutex);
        queue_push(&task_queue, (task_t){.client_fd = client_fd});
        pthread_cond_signal(&queue_cond);  // Réveiller un worker
        pthread_mutex_unlock(&queue_mutex);
    }
}
```

---

### Avantages du Multi-Threading

✅ **Léger** : Faible coût mémoire et CPU

✅ **Rapide** : Création/destruction rapide

✅ **Partage de mémoire** : Communication facile entre threads

✅ **Scalabilité** : Peut gérer des milliers de clients

✅ **Efficacité** : Meilleure utilisation des CPU multi-cœurs

### Inconvénients du Multi-Threading

❌ **Complexité** : Synchronisation, race conditions, deadlocks

❌ **Debugging difficile** : Bugs non-déterministes, difficiles à reproduire

❌ **Pas d'isolation** : Un bug peut corrompre tout le processus

❌ **Thread-safety** : Toutes les fonctions doivent être thread-safe

❌ **Risque de fuite mémoire** : Variables partagées mal gérées

### Quand utiliser Multi-Threading ?

✅ Beaucoup de clients simultanés (100-10 000)

✅ Communication entre clients nécessaire

✅ Serveur performant avec peu de mémoire

✅ CPU multi-cœurs à exploiter

---

## Approche 3 : I/O Multiplexing

### Concept

Au lieu de créer un processus ou thread par client, utiliser **un seul thread** qui surveille **plusieurs sockets** simultanément.

**Principe :** "Dites-moi quand un socket est prêt pour lire/écrire"

**Technologies :**
- `select()` : Standard POSIX, portable
- `poll()` : Amélioration de select
- `epoll()` : Spécifique à Linux, haute performance

### Avantages de l'I/O Multiplexing

✅ **Ultra-léger** : Un seul processus/thread

✅ **Scalabilité extrême** : Peut gérer 100 000+ clients (C10K problem)

✅ **Pas de synchronisation** : Pas de threads = pas de mutex

✅ **Déterministe** : Comportement prévisible

✅ **Efficace** : Pas de contexte switching

### Inconvénients de l'I/O Multiplexing

❌ **Architecture différente** : Event-driven, plus complexe

❌ **Code plus long** : Logique distribuée en callbacks/états

❌ **Un seul CPU** : N'exploite pas les multi-cœurs (sauf avec processus multiples)

❌ **Opérations bloquantes interdites** : Tout doit être non-bloquant

---

## `select()` : I/O Multiplexing de Base

### Principe

`select()` surveille plusieurs descripteurs de fichiers et indique lesquels sont prêts.

```c
#include <sys/select.h>

int select(int nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout);
```

**Paramètres :**
- `nfds` : Valeur maximale des descripteurs + 1
- `readfds` : Ensemble des descripteurs à surveiller en lecture
- `writefds` : Ensemble des descripteurs à surveiller en écriture
- `exceptfds` : Conditions exceptionnelles (rarement utilisé)
- `timeout` : Timeout (NULL = blocage infini)

**Retour :**
- Nombre de descripteurs prêts
- `0` si timeout
- `-1` si erreur

### Macros pour manipuler `fd_set`

```c
FD_ZERO(&set);           // Vider l'ensemble
FD_SET(fd, &set);        // Ajouter fd à l'ensemble
FD_CLR(fd, &set);        // Retirer fd de l'ensemble
FD_ISSET(fd, &set);      // Tester si fd est dans l'ensemble
```

---

### Exemple Complet : Serveur avec `select()`

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 30

int main() {
    int server_fd, client_fds[MAX_CLIENTS];
    fd_set readfds;
    int max_fd, activity, new_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];

    // Initialiser tous les clients à 0 (non utilisés)
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_fds[i] = 0;
    }

    // Créer le socket serveur
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // SO_REUSEADDR
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Serveur select() en écoute sur le port %d\n", PORT);
    printf("Clients max : %d\n", MAX_CLIENTS);

    // Boucle principale
    while (1) {
        // Vider et préparer l'ensemble des descripteurs
        FD_ZERO(&readfds);

        // Ajouter le socket serveur
        FD_SET(server_fd, &readfds);
        max_fd = server_fd;

        // Ajouter tous les sockets clients actifs
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = client_fds[i];

            if (fd > 0) {
                FD_SET(fd, &readfds);
            }

            if (fd > max_fd) {
                max_fd = fd;
            }
        }

        // Attendre une activité sur un des sockets
        activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("select");
            break;
        }

        // Si activité sur le socket serveur = nouvelle connexion
        if (FD_ISSET(server_fd, &readfds)) {
            client_len = sizeof(client_addr);
            new_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

            if (new_socket < 0) {
                perror("accept");
                continue;
            }

            // Afficher infos client
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
            printf("Nouvelle connexion : %s:%d (socket %d)\n",
                   client_ip, ntohs(client_addr.sin_port), new_socket);

            // Ajouter à la liste des clients
            int added = 0;
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_fds[i] == 0) {
                    client_fds[i] = new_socket;
                    printf("Client ajouté à la position %d\n", i);
                    added = 1;
                    break;
                }
            }

            if (!added) {
                printf("Trop de clients, connexion refusée\n");
                close(new_socket);
            }
        }

        // Vérifier tous les clients pour des données entrantes
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = client_fds[i];

            if (fd > 0 && FD_ISSET(fd, &readfds)) {
                // Lire les données
                ssize_t bytes_read = recv(fd, buffer, BUFFER_SIZE - 1, 0);

                if (bytes_read == 0) {
                    // Client déconnecté
                    getpeername(fd, (struct sockaddr*)&client_addr, &client_len);
                    char client_ip[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                    printf("Client déconnecté : %s:%d (socket %d)\n",
                           client_ip, ntohs(client_addr.sin_port), fd);

                    close(fd);
                    client_fds[i] = 0;
                } else if (bytes_read < 0) {
                    perror("recv");
                    close(fd);
                    client_fds[i] = 0;
                } else {
                    // Données reçues
                    buffer[bytes_read] = '\0';
                    printf("Socket %d : %s", fd, buffer);

                    // Écho
                    send(fd, buffer, bytes_read, 0);
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
```

**Compilation :**
```bash
gcc -o server_select server_select.c -Wall -Wextra
./server_select
```

---

### Limitations de `select()`

❌ **Limite de descripteurs** : FD_SETSIZE (généralement 1024)

❌ **Performance** : O(n) pour chaque appel (parcourt tous les descripteurs)

❌ **Modification des sets** : `select()` modifie les `fd_set`, il faut les reconstruire à chaque itération

---

## `poll()` : Amélioration de `select()`

### Avantages sur `select()`

✅ Pas de limite FD_SETSIZE

✅ Ne modifie pas la structure d'entrée

✅ API plus claire

```c
#include <poll.h>

int poll(struct pollfd *fds, nfds_t nfds, int timeout);
```

**Structure `pollfd` :**
```c
struct pollfd {
    int   fd;         // Descripteur de fichier
    short events;     // Événements à surveiller
    short revents;    // Événements qui se sont produits
};
```

**Événements :**
- `POLLIN` : Données disponibles en lecture
- `POLLOUT` : Prêt pour écriture
- `POLLERR` : Erreur
- `POLLHUP` : Déconnexion

### Exemple avec `poll()`

```c
#define MAX_CLIENTS 1000

struct pollfd fds[MAX_CLIENTS + 1];  // +1 pour le serveur
int nfds = 1;  // Nombre actuel de descripteurs

// Initialiser
fds[0].fd = server_fd;
fds[0].events = POLLIN;

for (int i = 1; i <= MAX_CLIENTS; i++) {
    fds[i].fd = -1;  // Non utilisé
}

while (1) {
    int ready = poll(fds, nfds, -1);  // -1 = pas de timeout

    if (ready < 0) {
        perror("poll");
        break;
    }

    // Vérifier le serveur
    if (fds[0].revents & POLLIN) {
        int client_fd = accept(server_fd, ...);

        // Ajouter à la liste
        for (int i = 1; i <= MAX_CLIENTS; i++) {
            if (fds[i].fd == -1) {
                fds[i].fd = client_fd;
                fds[i].events = POLLIN;
                if (i >= nfds) {
                    nfds = i + 1;
                }
                break;
            }
        }
    }

    // Vérifier tous les clients
    for (int i = 1; i < nfds; i++) {
        if (fds[i].fd == -1) continue;

        if (fds[i].revents & POLLIN) {
            // Lire données
            ssize_t n = recv(fds[i].fd, buffer, sizeof(buffer), 0);

            if (n <= 0) {
                // Déconnexion
                close(fds[i].fd);
                fds[i].fd = -1;
            } else {
                // Traiter données
                send(fds[i].fd, buffer, n, 0);
            }
        }
    }
}
```

---

## `epoll()` : Haute Performance (Linux)

### Pourquoi `epoll()` ?

`select()` et `poll()` ont une complexité **O(n)** : ils doivent parcourir tous les descripteurs à chaque appel.

`epoll()` utilise une structure de données optimisée (arbre rouge-noir) avec une complexité **O(1)** pour la majorité des opérations.

**Performances :**
- 100 clients : `select()` ≈ `epoll()`
- 10 000 clients : `epoll()` **100x plus rapide**

### API `epoll()`

```c
#include <sys/epoll.h>

// 1. Créer une instance epoll
int epoll_create1(int flags);

// 2. Ajouter/Modifier/Supprimer un descripteur
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

// 3. Attendre des événements
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
```

**Opérations `epoll_ctl()` :**
- `EPOLL_CTL_ADD` : Ajouter un descripteur
- `EPOLL_CTL_MOD` : Modifier les événements surveillés
- `EPOLL_CTL_DEL` : Supprimer un descripteur

**Événements :**
- `EPOLLIN` : Données disponibles en lecture
- `EPOLLOUT` : Prêt pour écriture
- `EPOLLET` : Mode Edge-Triggered (avancé)

---

### Exemple avec `epoll()`

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define PORT 8080
#define MAX_EVENTS 100
#define BUFFER_SIZE 1024

int main() {
    int server_fd, epoll_fd;
    struct sockaddr_in server_addr;
    struct epoll_event event, events[MAX_EVENTS];

    // Créer socket serveur
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Créer instance epoll
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // Ajouter le socket serveur à epoll
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) < 0) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    printf("Serveur epoll en écoute sur le port %d\n", PORT);

    char buffer[BUFFER_SIZE];

    // Boucle événementielle
    while (1) {
        int n_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        if (n_events < 0) {
            perror("epoll_wait");
            break;
        }

        // Traiter tous les événements
        for (int i = 0; i < n_events; i++) {
            int fd = events[i].data.fd;

            if (fd == server_fd) {
                // Nouvelle connexion
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

                if (client_fd < 0) {
                    perror("accept");
                    continue;
                }

                char client_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                printf("Nouvelle connexion : %s:%d (socket %d)\n",
                       client_ip, ntohs(client_addr.sin_port), client_fd);

                // Ajouter le client à epoll
                event.events = EPOLLIN;
                event.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) < 0) {
                    perror("epoll_ctl ADD client");
                    close(client_fd);
                }
            } else {
                // Données d'un client
                ssize_t bytes_read = recv(fd, buffer, BUFFER_SIZE - 1, 0);

                if (bytes_read <= 0) {
                    // Déconnexion
                    if (bytes_read == 0) {
                        printf("Client déconnecté (socket %d)\n", fd);
                    } else {
                        perror("recv");
                    }

                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                } else {
                    // Écho
                    buffer[bytes_read] = '\0';
                    printf("Socket %d : %s", fd, buffer);
                    send(fd, buffer, bytes_read, 0);
                }
            }
        }
    }

    close(server_fd);
    close(epoll_fd);
    return 0;
}
```

**Compilation :**
```bash
gcc -o server_epoll server_epoll.c -Wall -Wextra
./server_epoll
```

---

## Comparaison des Approches

### Tableau Récapitulatif

| Critère | Fork | Threads | select() | poll() | epoll() |
|---------|------|---------|----------|--------|---------|
| **Scalabilité** | Faible (< 100) | Moyenne (< 1000) | Faible (< 1024) | Moyenne | Excellente (> 10k) |
| **Mémoire** | Élevée | Moyenne | Faible | Faible | Faible |
| **CPU** | Élevé | Moyen | Moyen | Moyen | Faible |
| **Complexité** | Simple | Moyenne | Moyenne | Moyenne | Élevée |
| **Isolation** | Forte | Aucune | Aucune | Aucune | Aucune |
| **Portabilité** | POSIX | POSIX | POSIX | POSIX | Linux |
| **Debugging** | Facile | Difficile | Moyen | Moyen | Moyen |
| **Multi-core** | Oui | Oui | Non | Non | Non* |

*epoll peut être combiné avec plusieurs processus

### Performance selon le nombre de clients

```
Clients    | Fork  | Threads | select | poll  | epoll
-----------|-------|---------|--------|-------|-------
10         | ✅✅✅ | ✅✅✅   | ✅✅✅  | ✅✅✅ | ✅✅✅
100        | ✅✅   | ✅✅✅   | ✅✅    | ✅✅✅ | ✅✅✅
1 000      | ❌     | ✅✅     | ❌      | ✅✅   | ✅✅✅
10 000     | ❌     | ❌       | ❌      | ❌     | ✅✅✅
100 000    | ❌     | ❌       | ❌      | ❌     | ✅✅
```

---

## Choix de l'Architecture

### Arbre de Décision

```
Nombre de clients simultanés ?
│
├─ < 50 clients
│   └─ Utiliser fork() (simplicité)
│
├─ 50-500 clients
│   └─ Threads ou poll()
│
├─ 500-10 000 clients
│   └─ epoll() ou threads avec pool
│
└─ > 10 000 clients
    └─ epoll() + architecture événementielle
```

### Selon le type d'application

| Application | Recommandation |
|-------------|----------------|
| **Serveur web** | epoll() + worker processes |
| **Serveur de chat** | Threads ou epoll() |
| **Serveur de jeu** | epoll() + UDP |
| **Proxy** | epoll() |
| **Serveur de fichiers** | Threads |
| **Daemon système** | fork() pour isolation |

---

## Architecture Hybride : Le Meilleur des Deux Mondes

### Combinaison : Plusieurs processus + epoll()

**Principe :** Utiliser `SO_REUSEPORT` pour créer plusieurs processus, chacun avec sa propre boucle `epoll()`.

**Avantages :**
- Utilisation de tous les cœurs CPU
- Scalabilité extrême
- Isolation entre processus

**Exemple (Nginx, HAProxy) :**
```
Processus 1 (CPU core 1) : epoll() → gère 10 000 clients
Processus 2 (CPU core 2) : epoll() → gère 10 000 clients
Processus 3 (CPU core 3) : epoll() → gère 10 000 clients
Processus 4 (CPU core 4) : epoll() → gère 10 000 clients

Total : 40 000 clients simultanés
```

---

## En Résumé

Pour créer des serveurs capables de gérer plusieurs clients, trois approches principales existent.

**Fork (Multi-Processus) :**
- ✅ Simple, isolation forte
- ❌ Coûteux en mémoire
- 📊 < 100 clients

**Threads (Multi-Threading) :**
- ✅ Léger, partage mémoire
- ❌ Synchronisation complexe
- 📊 100-1000 clients

**epoll (I/O Multiplexing) :**
- ✅ Ultra-performant, scalable
- ❌ Architecture différente
- 📊 1 000-100 000+ clients

**Règles de choix :**

1. **Petit serveur** (< 50 clients) → **fork()** (simplicité)
2. **Serveur moyen** (50-500) → **threads** ou **poll()**
3. **Gros serveur** (500+) → **epoll()** (Linux) ou équivalent
4. **Production critique** → **epoll() + multi-processus**

**Le C10K problem :**
Comment gérer 10 000 connexions simultanées ? Réponse : `epoll()` et architecture événementielle.

Dans la prochaine section, nous verrons comment créer des serveurs non-bloquants et des architectures I/O asynchrones avancées.

---

**→ Prochaine section : 20.9 Non-blocking I/O et epoll**

⏭️ [Non-blocking I/O et epoll](/20-reseau-sockets/09-non-blocking-io-epoll.md)
