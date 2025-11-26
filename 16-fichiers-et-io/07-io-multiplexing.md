🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 16.7 I/O multiplexing

## Introduction

L'**I/O multiplexing** (multiplexage d'entrées/sorties) est une technique qui permet à un programme de **surveiller plusieurs descripteurs de fichiers simultanément** et de réagir quand l'un d'eux est prêt pour une opération (lecture ou écriture).

> **💡 Analogie :** Imaginez que vous êtes un serveur dans un restaurant avec plusieurs tables :
> - **Approche naïve** : Vous restez debout devant la table 1 en attendant que le client commande, puis vous passez à la table 2, etc. (très inefficace !)
> - **I/O Multiplexing** : Vous surveillez toutes les tables en même temps et vous vous dirigez vers celle où un client lève la main (efficace !).

## Le problème : Gérer plusieurs sources de données

### Scénario typique : Serveur réseau

Un serveur doit souvent :
- Accepter de nouvelles connexions
- Lire les données de plusieurs clients connectés
- Écrire les réponses à ces clients
- Gérer l'entrée clavier (stdin)

**Sans multiplexing :**
```c
// ❌ PROBLÈME : Cette approche bloque !
while (1) {
    // Bloque jusqu'à une nouvelle connexion
    int client_fd = accept(server_fd, ...);

    // Bloque jusqu'à recevoir des données de ce client
    read(client_fd, buffer, size);

    // Pendant ce temps, impossible d'accepter d'autres clients !
}
```

**Problèmes :**
- Si `accept()` bloque, on ne peut pas lire des clients existants
- Si `read()` bloque sur un client, on ne peut pas gérer les autres
- Un client lent bloque tout le serveur

### Solutions possibles

Il existe plusieurs approches pour résoudre ce problème :

| Approche | Description | Avantages | Inconvénients |
|----------|-------------|-----------|---------------|
| **Busy-waiting** | Boucle qui teste chaque fd | Simple | CPU à 100%, inefficace |
| **Multithreading** | Un thread par client | Chaque client indépendant | Overhead, complexité |
| **Non-blocking I/O** | Opérations non-bloquantes | Pas de blocage | Consomme du CPU |
| **I/O Multiplexing** | Surveiller plusieurs fd | Efficace, un seul thread | API plus complexe |

**L'I/O multiplexing** est souvent la meilleure solution pour gérer de nombreuses connexions avec un seul processus.

## Le concept du multiplexing

### Principe de base

```
┌─────────────────────────────────────────────┐
│         Programme (un seul thread)          │
└──────────────────┬──────────────────────────┘
                   │
                   ▼
         ┌─────────────────┐
         │  select/poll/   │
         │     epoll       │ ← Surveille plusieurs fd
         └─────────┬───────┘
                   │
       ┌───────────┼───────────┐
       │           │           │
       ▼           ▼           ▼
    ┌─────┐    ┌─────┐    ┌─────┐
    │ fd 3│    │ fd 4│    │ fd 5│
    └─────┘    └─────┘    └─────┘
       │           │           │
       ▼           ▼           ▼
   Client 1    Client 2    Client 3
```

**Le multiplexer dit :** "Attendez ici. Je vous réveillerai quand au moins un des descripteurs sera prêt."

### Fonctionnement général

```c
// Pseudo-code
while (1) {
    // 1. Dire au noyau quels descripteurs surveiller
    watch(fd1, fd2, fd3, ...);

    // 2. Bloquer jusqu'à ce qu'au moins un soit prêt
    ready_fds = wait_for_activity();

    // 3. Traiter les descripteurs prêts
    for each fd in ready_fds {
        if (fd == server_socket) {
            // Nouvelle connexion
            accept(...);
        } else {
            // Données disponibles
            read(fd, ...);
        }
    }
}
```

## Les trois mécanismes principaux

Linux propose trois APIs pour l'I/O multiplexing :

| Mécanisme | Année | Portabilité | Performance | Limite fd |
|-----------|-------|-------------|-------------|-----------|
| **select()** | 1983 | ✅ POSIX (portable) | ⚠️ O(n) | 1024 max |
| **poll()** | 1986 | ✅ POSIX (portable) | ⚠️ O(n) | Pas de limite |
| **epoll()** | 2002 | ❌ Linux seulement | ✅ O(1) | Pas de limite |

**Recommandation :**
- Pour la **portabilité** : `poll()`
- Pour la **performance** (serveur haute charge) : `epoll()`
- **Évitez `select()`** sauf pour compatibilité ancienne

## select() : Le mécanisme historique

### Présentation

`select()` permet de surveiller plusieurs descripteurs de fichiers pour voir s'ils sont prêts pour la lecture, l'écriture, ou s'ils ont une condition exceptionnelle.

```c
#include <sys/select.h>

int select(int nfds,
           fd_set *readfds,
           fd_set *writefds,
           fd_set *exceptfds,
           struct timeval *timeout);
```

**Paramètres :**
- `nfds` : Le plus grand numéro de fd + 1
- `readfds` : Ensemble de fd à surveiller pour la lecture
- `writefds` : Ensemble de fd à surveiller pour l'écriture
- `exceptfds` : Ensemble de fd à surveiller pour conditions exceptionnelles (rarement utilisé)
- `timeout` : Durée maximale d'attente (NULL = infini)

**Retour :**
- Nombre de descripteurs prêts (> 0)
- 0 si timeout
- -1 en cas d'erreur

### La structure fd_set

`fd_set` est un ensemble de descripteurs de fichiers. On le manipule avec des macros :

```c
FD_ZERO(&set);      // Vider l'ensemble
FD_SET(fd, &set);   // Ajouter fd à l'ensemble
FD_CLR(fd, &set);   // Retirer fd de l'ensemble
FD_ISSET(fd, &set); // Tester si fd est dans l'ensemble
```

### Exemple basique avec select()

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>

int main(void) {
    fd_set readfds;
    struct timeval tv;

    while (1) {
        // 1. Préparer l'ensemble des descripteurs
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);  // Surveiller stdin (0)

        // 2. Configurer le timeout (5 secondes)
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        printf("En attente d'une entrée (timeout 5s)...\n");

        // 3. Appeler select()
        int ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);

        if (ret == -1) {
            perror("select");
            return 1;
        } else if (ret == 0) {
            printf("Timeout ! Rien reçu.\n");
        } else {
            // 4. Vérifier quel descripteur est prêt
            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                char buffer[256];
                ssize_t n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
                buffer[n] = '\0';
                printf("Vous avez tapé : %s", buffer);
            }
        }
    }

    return 0;
}
```

**Comportement :**
```
En attente d'une entrée (timeout 5s)...
[utilisateur tape "hello"]
Vous avez tapé : hello
En attente d'une entrée (timeout 5s)...
[attente de 5 secondes]
Timeout ! Rien reçu.
```

### Exemple : Surveiller plusieurs descripteurs

```c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

int main(void) {
    // Ouvrir deux fichiers en lecture non-bloquante
    int fd1 = open("file1.txt", O_RDONLY | O_NONBLOCK);
    int fd2 = open("file2.txt", O_RDONLY | O_NONBLOCK);

    if (fd1 == -1 || fd2 == -1) {
        perror("open");
        return 1;
    }

    fd_set readfds;
    int max_fd = (fd1 > fd2) ? fd1 : fd2;

    while (1) {
        // Préparer l'ensemble
        FD_ZERO(&readfds);
        FD_SET(fd1, &readfds);
        FD_SET(fd2, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        if (STDIN_FILENO > max_fd) max_fd = STDIN_FILENO;

        printf("En attente de données...\n");

        // Surveiller (pas de timeout)
        int ret = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (ret == -1) {
            perror("select");
            break;
        }

        // Vérifier chaque descripteur
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            char buffer[256];
            ssize_t n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
            if (n > 0) {
                buffer[n] = '\0';
                printf("stdin: %s", buffer);
            }
        }

        if (FD_ISSET(fd1, &readfds)) {
            char buffer[256];
            ssize_t n = read(fd1, buffer, sizeof(buffer) - 1);
            if (n > 0) {
                buffer[n] = '\0';
                printf("file1: %s\n", buffer);
            } else if (n == 0) {
                printf("file1: EOF\n");
                FD_CLR(fd1, &readfds);  // Retirer de la surveillance
            }
        }

        if (FD_ISSET(fd2, &readfds)) {
            char buffer[256];
            ssize_t n = read(fd2, buffer, sizeof(buffer) - 1);
            if (n > 0) {
                buffer[n] = '\0';
                printf("file2: %s\n", buffer);
            } else if (n == 0) {
                printf("file2: EOF\n");
                FD_CLR(fd2, &readfds);
            }
        }
    }

    close(fd1);
    close(fd2);

    return 0;
}
```

### Limites de select()

1. **Limite de 1024 descripteurs** (définie par `FD_SETSIZE`)
2. **Inefficace avec beaucoup de descripteurs** (parcourt tous les fd à chaque appel)
3. **fd_set est modifié** par select() (doit être réinitialisé à chaque fois)
4. **Copie des ensembles** entre espace utilisateur et noyau

**⚠️ Pour ces raisons, `select()` est dépassé pour les serveurs modernes.**

## poll() : L'amélioration de select()

### Présentation

`poll()` est une amélioration de `select()` qui résout certains problèmes :

```c
#include <poll.h>

int poll(struct pollfd *fds, nfds_t nfds, int timeout);
```

**Paramètres :**
- `fds` : Tableau de structures `pollfd`
- `nfds` : Nombre d'éléments dans le tableau
- `timeout` : Timeout en millisecondes (-1 = infini, 0 = non-bloquant)

**Retour :**
- Nombre de descripteurs prêts (> 0)
- 0 si timeout
- -1 en cas d'erreur

### La structure pollfd

```c
struct pollfd {
    int   fd;       // Descripteur de fichier
    short events;   // Événements à surveiller
    short revents;  // Événements qui se sont produits (rempli par poll)
};
```

**Événements courants :**

| Constante | Signification |
|-----------|---------------|
| `POLLIN` | Données disponibles pour lecture |
| `POLLOUT` | Prêt pour écriture |
| `POLLERR` | Erreur sur le descripteur |
| `POLLHUP` | Déconnexion (hangup) |
| `POLLNVAL` | Descripteur invalide |

### Exemple basique avec poll()

```c
#include <stdio.h>
#include <unistd.h>
#include <poll.h>

int main(void) {
    struct pollfd fds[1];

    // Surveiller stdin
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;  // Intéressé par la lecture

    while (1) {
        printf("En attente d'une entrée (timeout 5s)...\n");

        // Timeout de 5000 ms = 5 secondes
        int ret = poll(fds, 1, 5000);

        if (ret == -1) {
            perror("poll");
            return 1;
        } else if (ret == 0) {
            printf("Timeout ! Rien reçu.\n");
        } else {
            // Vérifier quel événement s'est produit
            if (fds[0].revents & POLLIN) {
                char buffer[256];
                ssize_t n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
                buffer[n] = '\0';
                printf("Vous avez tapé : %s", buffer);
            }
        }
    }

    return 0;
}
```

### Exemple : Serveur TCP simple avec poll()

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 10
#define PORT 8080

int main(void) {
    // Créer le socket serveur
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    // Option pour réutiliser l'adresse
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        return 1;
    }

    // Listen
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        return 1;
    }

    printf("Serveur en écoute sur le port %d\n", PORT);

    // Préparer le tableau pour poll()
    struct pollfd fds[MAX_CLIENTS + 1];

    // Initialiser tous les fd à -1 (non utilisé)
    for (int i = 0; i < MAX_CLIENTS + 1; i++) {
        fds[i].fd = -1;
    }

    // Le premier slot est pour le socket serveur
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    int nfds = 1;  // Nombre de slots utilisés

    // Boucle principale
    while (1) {
        printf("En attente d'événements...\n");

        int ret = poll(fds, nfds, -1);  // Timeout infini

        if (ret == -1) {
            perror("poll");
            break;
        }

        // Parcourir tous les descripteurs
        for (int i = 0; i < nfds; i++) {
            if (fds[i].revents == 0) {
                continue;  // Rien sur ce descripteur
            }

            if (fds[i].fd == server_fd) {
                // Nouvelle connexion
                int client_fd = accept(server_fd, NULL, NULL);
                if (client_fd == -1) {
                    perror("accept");
                    continue;
                }

                printf("Nouvelle connexion : fd %d\n", client_fd);

                // Ajouter au tableau
                if (nfds < MAX_CLIENTS + 1) {
                    fds[nfds].fd = client_fd;
                    fds[nfds].events = POLLIN;
                    nfds++;
                } else {
                    printf("Trop de clients, connexion refusée\n");
                    close(client_fd);
                }
            } else {
                // Données d'un client
                char buffer[1024];
                ssize_t n = read(fds[i].fd, buffer, sizeof(buffer) - 1);

                if (n <= 0) {
                    // Client déconnecté
                    printf("Client fd %d déconnecté\n", fds[i].fd);
                    close(fds[i].fd);

                    // Compacter le tableau
                    fds[i] = fds[nfds - 1];
                    fds[nfds - 1].fd = -1;
                    nfds--;
                    i--;  // Revérifier ce slot
                } else {
                    // Écho : renvoyer les données
                    buffer[n] = '\0';
                    printf("Reçu de fd %d : %s", fds[i].fd, buffer);
                    write(fds[i].fd, buffer, n);
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
```

**Test du serveur :**
```bash
# Terminal 1 : Lancer le serveur
$ ./server

# Terminal 2 : Se connecter avec telnet
$ telnet localhost 8080
hello
hello
test
test
```

### Avantages de poll() sur select()

- ✅ Pas de limite de 1024 descripteurs
- ✅ Plus clair : un tableau plutôt que des bitmasks
- ✅ `events` et `revents` séparés (pas besoin de réinitialiser)
- ✅ Plus portable que `epoll()`

⚠️ Toujours O(n) : le noyau parcourt tous les descripteurs

## epoll() : Le mécanisme moderne (Linux)

### Présentation

`epoll()` est l'API d'I/O multiplexing la plus performante sous Linux. Elle est utilisée par les serveurs web haute performance (Nginx, Node.js, etc.).

**Avantages :**
- ✅ **O(1)** : Performance constante même avec des milliers de descripteurs
- ✅ **Edge-triggered** : Notifie seulement des changements
- ✅ Pas de copie des descripteurs à chaque appel

**Inconvénient :**
- ❌ **Linux seulement** (pas portable)

### Les trois fonctions epoll

```c
#include <sys/epoll.h>

// 1. Créer une instance epoll
int epoll_create1(int flags);

// 2. Ajouter/modifier/supprimer des descripteurs
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

// 3. Attendre des événements
int epoll_wait(int epfd, struct epoll_event *events,
               int maxevents, int timeout);
```

### La structure epoll_event

```c
struct epoll_event {
    uint32_t events;   // Événements (EPOLLIN, EPOLLOUT, etc.)
    epoll_data_t data; // Données utilisateur (souvent le fd)
};

typedef union epoll_data {
    void *ptr;
    int fd;
    uint32_t u32;
    uint64_t u64;
} epoll_data_t;
```

### Workflow avec epoll()

```c
// 1. Créer l'instance epoll
int epfd = epoll_create1(0);

// 2. Ajouter des descripteurs
struct epoll_event ev;
ev.events = EPOLLIN;
ev.data.fd = fd;
epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

// 3. Boucle d'événements
struct epoll_event events[MAX_EVENTS];
while (1) {
    int n = epoll_wait(epfd, events, MAX_EVENTS, -1);

    for (int i = 0; i < n; i++) {
        int fd = events[i].data.fd;
        // Traiter fd
    }
}
```

### Exemple : Serveur TCP avec epoll()

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define MAX_EVENTS 64
#define PORT 8080

// Rendre un socket non-bloquant
int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(void) {
    // Créer le socket serveur
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    set_nonblocking(server_fd);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        return 1;
    }

    if (listen(server_fd, SOMAXCONN) == -1) {
        perror("listen");
        return 1;
    }

    printf("Serveur en écoute sur le port %d\n", PORT);

    // Créer l'instance epoll
    int epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create1");
        return 1;
    }

    // Ajouter le socket serveur à epoll
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        perror("epoll_ctl");
        return 1;
    }

    // Tableau pour recevoir les événements
    struct epoll_event events[MAX_EVENTS];

    // Boucle principale
    while (1) {
        int n = epoll_wait(epfd, events, MAX_EVENTS, -1);

        if (n == -1) {
            perror("epoll_wait");
            break;
        }

        // Traiter chaque événement
        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;

            if (fd == server_fd) {
                // Nouvelle connexion
                while (1) {
                    int client_fd = accept(server_fd, NULL, NULL);
                    if (client_fd == -1) {
                        break;  // Plus de connexions en attente
                    }

                    set_nonblocking(client_fd);

                    printf("Nouvelle connexion : fd %d\n", client_fd);

                    // Ajouter le client à epoll
                    ev.events = EPOLLIN | EPOLLET;  // Edge-triggered
                    ev.data.fd = client_fd;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
                }
            } else {
                // Données d'un client
                char buffer[1024];
                ssize_t n = read(fd, buffer, sizeof(buffer) - 1);

                if (n <= 0) {
                    // Client déconnecté
                    printf("Client fd %d déconnecté\n", fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                } else {
                    // Écho
                    buffer[n] = '\0';
                    printf("Reçu de fd %d : %s", fd, buffer);
                    write(fd, buffer, n);
                }
            }
        }
    }

    close(epfd);
    close(server_fd);

    return 0;
}
```

### Edge-triggered vs Level-triggered

**Level-triggered (par défaut) :**
- `epoll_wait()` notifie tant que la condition est vraie
- Plus simple, mais peut générer beaucoup d'événements

**Edge-triggered (EPOLLET) :**
- `epoll_wait()` notifie seulement lors du changement d'état
- Plus efficace, mais nécessite des sockets non-bloquants
- Il faut lire/écrire jusqu'à `EAGAIN`

```c
// Edge-triggered : Lire jusqu'à épuisement
while (1) {
    ssize_t n = read(fd, buffer, size);
    if (n == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            break;  // Plus rien à lire
        }
        perror("read");
        break;
    }
    if (n == 0) {
        // EOF
        break;
    }
    // Traiter les données
}
```

## Comparaison des trois mécanismes

### Tableau comparatif

| Caractéristique | select() | poll() | epoll() |
|----------------|----------|--------|---------|
| **Portabilité** | ✅ Excellente (POSIX) | ✅ Bonne (POSIX) | ❌ Linux seulement |
| **Limite fd** | ⚠️ 1024 (FD_SETSIZE) | ✅ Aucune | ✅ Aucune |
| **Performance** | ⚠️ O(n) | ⚠️ O(n) | ✅ O(1) |
| **Réinitialisation** | ⚠️ Oui (fd_set modifié) | ✅ Non | ✅ Non |
| **API** | ⚠️ Complexe (bitmask) | ✅ Simple (tableau) | ⚠️ Moyenne (3 fonctions) |
| **Edge-triggered** | ❌ Non | ❌ Non | ✅ Oui (EPOLLET) |
| **Serveurs modernes** | ❌ Obsolète | ⚠️ Acceptable | ✅ Recommandé |

### Benchmarks (10 000 connexions)

```
select() : ~500 ms par itération   (très lent)
poll()   : ~300 ms par itération   (lent)
epoll()  : ~0.1 ms par itération   (rapide !)
```

**Conclusion :** Pour un serveur haute performance sous Linux, utilisez `epoll()`.

### Quand utiliser chaque mécanisme ?

| Cas d'usage | Recommandation |
|-------------|----------------|
| **Application portable** (Linux + BSD + macOS) | `poll()` |
| **Serveur haute performance Linux** | `epoll()` |
| **Peu de descripteurs** (< 10) | N'importe lequel |
| **Code legacy existant** | Gardez `select()` |
| **Apprentissage** | Commencez par `poll()` |

## Exemple avancé : Serveur de chat

Voici un serveur de chat simple qui utilise `poll()` pour gérer plusieurs clients :

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 100
#define PORT 8080
#define BUFFER_SIZE 1024

int main(void) {
    // Créer le socket serveur
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 5);

    printf("Serveur de chat démarré sur le port %d\n", PORT);

    // Tableau pour poll()
    struct pollfd fds[MAX_CLIENTS + 1];
    for (int i = 0; i < MAX_CLIENTS + 1; i++) {
        fds[i].fd = -1;
    }

    fds[0].fd = server_fd;
    fds[0].events = POLLIN;
    int nfds = 1;

    // Tableau pour stocker les noms des clients
    char client_names[MAX_CLIENTS + 1][32];

    while (1) {
        int ret = poll(fds, nfds, -1);
        if (ret == -1) break;

        for (int i = 0; i < nfds; i++) {
            if (fds[i].revents == 0) continue;

            if (fds[i].fd == server_fd) {
                // Nouvelle connexion
                int client_fd = accept(server_fd, NULL, NULL);
                printf("Nouvelle connexion : fd %d\n", client_fd);

                if (nfds < MAX_CLIENTS + 1) {
                    fds[nfds].fd = client_fd;
                    fds[nfds].events = POLLIN;

                    // Demander le nom
                    write(client_fd, "Votre nom : ", 12);

                    snprintf(client_names[nfds], sizeof(client_names[nfds]),
                            "Client%d", client_fd);

                    nfds++;
                } else {
                    close(client_fd);
                }
            } else {
                // Message d'un client
                char buffer[BUFFER_SIZE];
                ssize_t n = read(fds[i].fd, buffer, sizeof(buffer) - 1);

                if (n <= 0) {
                    // Déconnexion
                    printf("%s s'est déconnecté\n", client_names[i]);

                    // Notifier les autres
                    char msg[BUFFER_SIZE];
                    snprintf(msg, sizeof(msg), "%s a quitté le chat\n",
                            client_names[i]);

                    for (int j = 1; j < nfds; j++) {
                        if (j != i && fds[j].fd != -1) {
                            write(fds[j].fd, msg, strlen(msg));
                        }
                    }

                    close(fds[i].fd);
                    fds[i] = fds[nfds - 1];
                    strcpy(client_names[i], client_names[nfds - 1]);
                    fds[nfds - 1].fd = -1;
                    nfds--;
                    i--;
                } else {
                    buffer[n] = '\0';

                    // Si c'est le premier message, c'est le nom
                    if (strncmp(client_names[i], "Client", 6) == 0) {
                        // Retirer le \n
                        buffer[strcspn(buffer, "\n")] = 0;
                        strncpy(client_names[i], buffer,
                               sizeof(client_names[i]) - 1);

                        char welcome[BUFFER_SIZE];
                        snprintf(welcome, sizeof(welcome),
                                "Bienvenue %s ! Tapez vos messages.\n",
                                client_names[i]);
                        write(fds[i].fd, welcome, strlen(welcome));

                        // Notifier les autres
                        snprintf(welcome, sizeof(welcome),
                                "%s a rejoint le chat\n", client_names[i]);
                        for (int j = 1; j < nfds; j++) {
                            if (j != i && fds[j].fd != -1) {
                                write(fds[j].fd, welcome, strlen(welcome));
                            }
                        }
                    } else {
                        // Message de chat
                        printf("%s: %s", client_names[i], buffer);

                        // Broadcaster à tous les autres clients
                        char msg[BUFFER_SIZE];
                        snprintf(msg, sizeof(msg), "%s: %s",
                                client_names[i], buffer);

                        for (int j = 1; j < nfds; j++) {
                            if (j != i && fds[j].fd != -1) {
                                write(fds[j].fd, msg, strlen(msg));
                            }
                        }
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
```

**Test :**
```bash
# Terminal 1 : Serveur
$ ./chat_server
Serveur de chat démarré sur le port 8080

# Terminal 2 : Client 1
$ telnet localhost 8080
Votre nom : Alice
Bienvenue Alice ! Tapez vos messages.
Bob a rejoint le chat
Bob: Salut !
Alice: Salut Bob !

# Terminal 3 : Client 2
$ telnet localhost 8080
Votre nom : Bob
Bienvenue Bob ! Tapez vos messages.
Alice: Salut Bob !
Bob: Salut !
```

## Bonnes pratiques

### 1. Utiliser des sockets non-bloquants

Pour `epoll()` en mode edge-triggered et pour éviter le blocage :

```c
int flags = fcntl(fd, F_GETFL, 0);
fcntl(fd, F_SETFL, flags | O_NONBLOCK);
```

### 2. Gérer EAGAIN et EWOULDBLOCK

```c
ssize_t n = read(fd, buffer, size);
if (n == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // Normal avec non-blocking I/O
        return;
    }
    perror("read");
}
```

### 3. Toujours vérifier les valeurs de retour

```c
// ✅ BON
int ret = poll(fds, nfds, timeout);
if (ret == -1) {
    perror("poll");
    return 1;
}

// ❌ MAUVAIS
poll(fds, nfds, timeout);
// Continue sans vérifier !
```

### 4. Nettoyer les descripteurs fermés

```c
if (n <= 0) {
    close(fds[i].fd);
    // Retirer de la surveillance
    fds[i] = fds[nfds - 1];
    nfds--;
    i--;  // Revérifier ce slot
}
```

### 5. Gérer les signaux

```c
// poll() et epoll_wait() retournent -1 avec errno=EINTR si signal
if (ret == -1 && errno == EINTR) {
    continue;  // Réessayer
}
```

## Pièges courants

### 1. Oublier de réinitialiser fd_set avec select()

```c
// ❌ ERREUR
fd_set readfds;
FD_ZERO(&readfds);
FD_SET(fd, &readfds);

while (1) {
    select(..., &readfds, ...);  // readfds est modifié !
    // Au deuxième tour, readfds est vide !
}

// ✅ CORRECT
while (1) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    select(..., &readfds, ...);
}
```

### 2. Mauvais calcul de nfds pour select()

```c
// ❌ ERREUR
int max_fd = 10;
select(max_fd, ...);  // Devrait être max_fd + 1 !

// ✅ CORRECT
select(max_fd + 1, ...);
```

### 3. Ne pas gérer EOF proprement

```c
ssize_t n = read(fd, buffer, size);
if (n == 0) {
    // EOF : le client s'est déconnecté
    close(fd);
    // Retirer de la surveillance !
}
```

### 4. Mélanger blocking et non-blocking

```c
// Si fd est non-bloquant, gérer EAGAIN
ssize_t n = read(fd, buffer, size);
if (n == -1 && errno != EAGAIN) {
    perror("read");
}
```

### 5. Oublier EPOLLHUP et EPOLLERR avec epoll()

```c
if (events[i].events & (EPOLLERR | EPOLLHUP)) {
    // Erreur ou déconnexion
    close(events[i].data.fd);
    continue;
}
```

## Cas d'usage réels

### 1. Serveur web (Nginx, Apache)

```c
// Nginx utilise epoll() pour gérer des milliers de connexions
while (1) {
    n = epoll_wait(epfd, events, MAX_EVENTS, -1);

    for (int i = 0; i < n; i++) {
        if (nouvelle_connexion) {
            accept_connection();
        } else if (requete_http) {
            parse_http_request();
            send_http_response();
        }
    }
}
```

### 2. Base de données (Redis, PostgreSQL)

```c
// Gérer plusieurs connexions clients
while (1) {
    poll(client_fds, num_clients, -1);

    for each ready_fd {
        read_query();
        execute_query();
        send_response();
    }
}
```

### 3. Proxy / Load balancer

```c
// Transférer les données entre clients et backends
while (1) {
    epoll_wait(epfd, events, MAX_EVENTS, -1);

    for each event {
        if (from_client) {
            read_from_client();
            write_to_backend();
        } else if (from_backend) {
            read_from_backend();
            write_to_client();
        }
    }
}
```

## Points clés à retenir

✅ **I/O multiplexing** = Surveiller plusieurs descripteurs simultanément

✅ **select()** = Ancien, limité à 1024 fd, O(n), portable

✅ **poll()** = Mieux que select(), pas de limite, O(n), portable

✅ **epoll()** = Le meilleur pour Linux, O(1), edge-triggered

✅ **Edge-triggered** nécessite des sockets non-bloquants

✅ **Toujours vérifier** les valeurs de retour et gérer les erreurs

✅ **Nettoyer** les descripteurs fermés de la surveillance

✅ **Cas d'usage** : Serveurs réseau, bases de données, proxies

✅ **Performance** : epoll() >> poll() >> select()

✅ **Portabilité** : poll() est le meilleur compromis

---

## Prochaines étapes

Maintenant que vous maîtrisez l'I/O multiplexing, explorez :
- **Section 16.8** : I/O asynchrone (AIO)
- **chapitre 20** : Programmation réseau avec sockets
- **chapitre 18** : Threads et concurrence
- **Module 6** : Programmation système complète

L'I/O multiplexing est essentiel pour créer des serveurs performants qui gèrent de nombreuses connexions simultanées. C'est la base de tous les serveurs web et systèmes distribués modernes !

⏭️ [I/O asynchrone (AIO)](/16-fichiers-et-io/08-io-asynchrone.md)
