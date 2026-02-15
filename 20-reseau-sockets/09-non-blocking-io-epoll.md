🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 20.9 Non-blocking I/O et epoll

## Introduction

Dans la section précédente, nous avons vu comment utiliser `epoll()` pour gérer plusieurs clients simultanément. Mais nous avons utilisé des sockets en mode **bloquant**, ce qui n'exploite pas pleinement la puissance d'`epoll()`.

Dans cette section, nous allons découvrir le **I/O non-bloquant** : un mode où les opérations réseau retournent immédiatement au lieu d'attendre. C'est la base des serveurs haute performance comme Nginx, Redis, et Node.js.

**Objectif :** Comprendre le I/O non-bloquant et créer un serveur événementiel performant avec `epoll()`.

---

## I/O Bloquant vs Non-Bloquant

### Mode Bloquant (Défaut)

En mode **bloquant**, les opérations réseau **attendent** jusqu'à ce qu'elles puissent se compléter.

```c
// Mode bloquant (défaut)
char buffer[1024];  
ssize_t n = recv(sockfd, buffer, sizeof(buffer), 0);  
// ⏳ Le programme s'arrête ici jusqu'à ce que des données arrivent
```

**Comportement :**
- `recv()` attend qu'il y ait des données à lire
- `send()` attend qu'il y ait de la place dans le buffer d'envoi
- `accept()` attend qu'un client se connecte
- `connect()` attend que la connexion soit établie

**Problème :** Si vous gérez plusieurs clients, vous ne pouvez pas attendre sur un socket sans ignorer les autres.

---

### Mode Non-Bloquant

En mode **non-bloquant**, les opérations retournent **immédiatement**, même si elles ne peuvent pas se compléter.

```c
// Mode non-bloquant
char buffer[1024];  
ssize_t n = recv(sockfd, buffer, sizeof(buffer), 0);  
// ⚡ Retourne immédiatement

if (n < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // Pas de données disponibles maintenant, réessayer plus tard
    } else {
        // Vraie erreur
        perror("recv");
    }
}
```

**Comportement :**
- Si l'opération ne peut pas se compléter → retourne `-1` avec `errno = EAGAIN`
- Si l'opération réussit partiellement → retourne le nombre d'octets traités
- Si l'opération réussit complètement → retourne le résultat normal

---

## Rendre un Socket Non-Bloquant

### Méthode 1 : `fcntl()` (Recommandée)

```c
#include <fcntl.h>

int set_nonblocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        return -1;
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        return -1;
    }

    return 0;
}
```

**Utilisation :**
```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0);  
set_nonblocking(sockfd);  

// Maintenant recv(), send(), etc. sont non-bloquants
```

---

### Méthode 2 : Flag à la création (Linux)

```c
// Créer directement un socket non-bloquant
int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
```

**Avantage :** Atomique, pas de race condition.

---

### Méthode 3 : `ioctl()` (Ancienne méthode)

```c
#include <sys/ioctl.h>

int nonblock = 1;  
ioctl(sockfd, FIONBIO, &nonblock);  
```

**Note :** Moins portable que `fcntl()`.

---

## Gérer EAGAIN et EWOULDBLOCK

### Le Concept

Quand une opération ne peut pas se compléter immédiatement, elle retourne `-1` avec :
- `errno = EAGAIN` (Try again)
- `errno = EWOULDBLOCK` (Would block)

Sur Linux, `EAGAIN == EWOULDBLOCK`, mais sur d'autres systèmes ils peuvent être différents.

**Bonne pratique :** Toujours tester les deux.

```c
if (errno == EAGAIN || errno == EWOULDBLOCK) {
    // C'est normal, l'opération n'est pas prête
    // Réessayer plus tard
}
```

---

### Exemple : Lecture Non-Bloquante

```c
ssize_t read_nonblocking(int sockfd, char *buffer, size_t len) {
    ssize_t n = recv(sockfd, buffer, len, 0);

    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Pas de données disponibles maintenant
            return 0;  // Signaler "pas de données"
        } else {
            // Vraie erreur
            perror("recv");
            return -1;
        }
    } else if (n == 0) {
        // Connexion fermée
        return -1;
    }

    // n > 0 : données lues avec succès
    return n;
}
```

---

### Exemple : Écriture Non-Bloquante

L'écriture non-bloquante est plus complexe car `send()` peut envoyer **moins** que demandé.

```c
ssize_t write_nonblocking(int sockfd, const char *buffer, size_t len) {
    ssize_t n = send(sockfd, buffer, len, 0);

    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Le buffer d'envoi est plein
            return 0;  // Rien envoyé, réessayer plus tard
        } else {
            perror("send");
            return -1;
        }
    }

    // n >= 0 : octets envoyés (peut être < len)
    return n;
}
```

**Important :** Si `send()` retourne `n < len`, il faut **sauvegarder** les octets non envoyés et les envoyer plus tard.

---

## epoll : Edge-Triggered vs Level-Triggered

### Deux Modes de Notification

`epoll()` peut notifier les événements de deux manières :

1. **Level-Triggered (LT)** : Mode par défaut
2. **Edge-Triggered (ET)** : Mode avancé

---

### Level-Triggered (LT) - Mode par Défaut

**Principe :** Notification tant que la condition est vraie.

```
Données disponibles → epoll_wait() notifie
    ↓
Vous ne lisez pas tout
    ↓
epoll_wait() notifie à nouveau
    ↓
Vous ne lisez toujours pas
    ↓
epoll_wait() notifie encore...
```

**Comportement :**
- `epoll_wait()` notifie **à chaque fois** qu'il y a des données non lues
- Similaire à `select()` et `poll()`
- Plus tolérant aux erreurs de programmation

**Exemple :**
```c
struct epoll_event event;  
event.events = EPOLLIN;  // Level-Triggered par défaut  
event.data.fd = sockfd;  
epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);  
```

**Quand utiliser LT :**
- ✅ Vous débutez avec `epoll()`
- ✅ Simplicité du code
- ✅ Sockets bloquants ou non-bloquants

---

### Edge-Triggered (ET) - Mode Avancé

**Principe :** Notification uniquement sur les **changements** d'état.

```
Données arrivent → epoll_wait() notifie UNE FOIS
    ↓
Vous devez TOUT lire maintenant
    ↓
Si vous ne lisez pas tout, epoll_wait() ne notifie plus !
```

**Comportement :**
- Notification seulement quand l'état **change** (0 → données, pas de données → données)
- **Obligation** de tout lire/écrire jusqu'à `EAGAIN`
- Nécessite des sockets **non-bloquants**
- Plus efficace (moins d'appels système)

**Exemple :**
```c
struct epoll_event event;  
event.events = EPOLLIN | EPOLLET;  // Edge-Triggered  
event.data.fd = sockfd;  
epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);  
```

**Quand utiliser ET :**
- ✅ Performance maximale
- ✅ Vous maîtrisez le I/O non-bloquant
- ✅ Serveur haute charge (1000+ clients)

---

### Comparaison LT vs ET

| Aspect | Level-Triggered | Edge-Triggered |
|--------|-----------------|----------------|
| **Notification** | Répétée tant que données disponibles | Une seule fois par changement |
| **Lecture** | Partielle OK | **TOUT lire** jusqu'à EAGAIN |
| **Socket** | Bloquant ou non-bloquant | **Obligatoirement** non-bloquant |
| **Performance** | Bonne | Excellente |
| **Complexité** | Simple | Complexe |
| **Risque** | Faible | Famine si mal codé |

---

### Exemple de Famine en ET

```c
// ❌ MAUVAIS CODE en Edge-Triggered
while (1) {
    int n = epoll_wait(epfd, events, MAX_EVENTS, -1);

    for (int i = 0; i < n; i++) {
        int fd = events[i].data.fd;

        char buffer[100];
        // ❌ Ne lit que 100 octets !
        recv(fd, buffer, 100, 0);

        // S'il y a 10 000 octets, les 9 900 restants ne seront JAMAIS lus
        // Car epoll_wait() ne notifiera plus ce fd
    }
}
```

**Résultat :** Le client attend indéfiniment, le serveur ne répond plus → **deadlock**.

---

## Architecture Event-Driven avec epoll ET

### Gestion d'État par Connexion

En mode Edge-Triggered, chaque connexion doit avoir un **état** :

```c
typedef enum {
    STATE_READING,   // En train de lire
    STATE_WRITING,   // En train d'écrire
    STATE_CLOSING    // À fermer
} connection_state_t;

typedef struct {
    int fd;
    connection_state_t state;

    // Buffers de lecture
    char read_buffer[4096];
    size_t read_pos;

    // Buffers d'écriture (données à envoyer)
    char write_buffer[4096];
    size_t write_pos;
    size_t write_len;
} connection_t;
```

---

### Pattern de Lecture Complète

En Edge-Triggered, il faut tout lire jusqu'à `EAGAIN`.

```c
void handle_read_et(connection_t *conn) {
    while (1) {
        ssize_t n = recv(conn->fd,
                        conn->read_buffer + conn->read_pos,
                        sizeof(conn->read_buffer) - conn->read_pos,
                        0);

        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Tout a été lu, c'est normal
                break;
            } else {
                // Erreur réelle
                perror("recv");
                conn->state = STATE_CLOSING;
                return;
            }
        } else if (n == 0) {
            // Connexion fermée
            conn->state = STATE_CLOSING;
            return;
        }

        // n > 0 : données reçues
        conn->read_pos += n;

        // Vérifier si le buffer est plein
        if (conn->read_pos >= sizeof(conn->read_buffer)) {
            fprintf(stderr, "Buffer plein !\n");
            break;
        }
    }

    // Traiter les données lues
    process_data(conn);
}
```

**Points clés :**
1. Boucle `while(1)` pour tout lire
2. Sortir sur `EAGAIN` (c'est normal)
3. Gérer le buffer plein

---

### Pattern d'Écriture Complète

L'écriture est plus complexe car `send()` peut ne pas tout envoyer.

```c
void handle_write_et(connection_t *conn) {
    while (conn->write_pos < conn->write_len) {
        ssize_t n = send(conn->fd,
                        conn->write_buffer + conn->write_pos,
                        conn->write_len - conn->write_pos,
                        0);

        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Buffer d'envoi plein, réessayer plus tard
                // Activer la notification EPOLLOUT
                struct epoll_event event;
                event.events = EPOLLIN | EPOLLOUT | EPOLLET;
                event.data.ptr = conn;
                epoll_ctl(epfd, EPOLL_CTL_MOD, conn->fd, &event);
                return;
            } else {
                perror("send");
                conn->state = STATE_CLOSING;
                return;
            }
        }

        // n >= 0 : octets envoyés
        conn->write_pos += n;
    }

    // Tout envoyé !
    conn->write_pos = 0;
    conn->write_len = 0;

    // Désactiver EPOLLOUT pour éviter les notifications inutiles
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.ptr = conn;
    epoll_ctl(epfd, EPOLL_CTL_MOD, conn->fd, &event);
}
```

**Points clés :**
1. Boucle tant qu'il reste des données à envoyer
2. Sur `EAGAIN`, activer `EPOLLOUT` pour être notifié quand on peut réenvoyer
3. Quand tout est envoyé, désactiver `EPOLLOUT`

---

## Exemple Complet : Serveur Écho avec epoll ET

Voici un serveur écho complet en mode Edge-Triggered.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define PORT 8080
#define MAX_EVENTS 64
#define BUFFER_SIZE 4096

typedef enum {
    STATE_READING,
    STATE_WRITING,
    STATE_CLOSING
} state_t;

typedef struct {
    int fd;
    state_t state;
    char read_buf[BUFFER_SIZE];
    size_t read_pos;
    char write_buf[BUFFER_SIZE];
    size_t write_pos;
    size_t write_len;
} connection_t;

// Rendre un socket non-bloquant
int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// Gérer la lecture (Edge-Triggered)
void handle_read(int epfd, connection_t *conn) {
    while (1) {
        ssize_t n = recv(conn->fd,
                        conn->read_buf + conn->read_pos,
                        BUFFER_SIZE - conn->read_pos - 1,
                        0);

        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;  // Tout lu
            } else {
                perror("recv");
                conn->state = STATE_CLOSING;
                return;
            }
        } else if (n == 0) {
            printf("[FD %d] Client déconnecté\n", conn->fd);
            conn->state = STATE_CLOSING;
            return;
        }

        conn->read_pos += n;

        if (conn->read_pos >= BUFFER_SIZE - 1) {
            fprintf(stderr, "[FD %d] Buffer plein\n", conn->fd);
            break;
        }
    }

    // Préparer l'écho : copier read_buf → write_buf
    if (conn->read_pos > 0) {
        memcpy(conn->write_buf, conn->read_buf, conn->read_pos);
        conn->write_len = conn->read_pos;
        conn->write_pos = 0;
        conn->read_pos = 0;

        // Passer en mode écriture
        conn->state = STATE_WRITING;

        // Activer EPOLLOUT
        struct epoll_event event;
        event.events = EPOLLIN | EPOLLOUT | EPOLLET;
        event.data.ptr = conn;
        epoll_ctl(epfd, EPOLL_CTL_MOD, conn->fd, &event);
    }
}

// Gérer l'écriture (Edge-Triggered)
void handle_write(int epfd, connection_t *conn) {
    while (conn->write_pos < conn->write_len) {
        ssize_t n = send(conn->fd,
                        conn->write_buf + conn->write_pos,
                        conn->write_len - conn->write_pos,
                        0);

        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Buffer plein, on réessaiera plus tard
                return;
            } else {
                perror("send");
                conn->state = STATE_CLOSING;
                return;
            }
        }

        conn->write_pos += n;
    }

    // Tout envoyé, repasser en mode lecture
    conn->write_pos = 0;
    conn->write_len = 0;
    conn->state = STATE_READING;

    // Désactiver EPOLLOUT
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.ptr = conn;
    epoll_ctl(epfd, EPOLL_CTL_MOD, conn->fd, &event);
}

int main() {
    int server_fd, epfd;
    struct sockaddr_in addr;
    struct epoll_event event, events[MAX_EVENTS];

    // Créer socket serveur
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Rendre non-bloquant
    if (set_nonblocking(server_fd) < 0) {
        perror("set_nonblocking");
        exit(EXIT_FAILURE);
    }

    // Bind
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Créer epoll
    epfd = epoll_create1(0);
    if (epfd < 0) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // Ajouter serveur en Edge-Triggered
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = server_fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &event) < 0) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    printf("Serveur epoll ET sur port %d\n", PORT);

    // Boucle événementielle
    while (1) {
        int n = epoll_wait(epfd, events, MAX_EVENTS, -1);

        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == server_fd) {
                // Nouvelles connexions (Edge-Triggered)
                while (1) {
                    struct sockaddr_in client_addr;
                    socklen_t len = sizeof(client_addr);
                    int client_fd = accept(server_fd,
                                          (struct sockaddr*)&client_addr,
                                          &len);

                    if (client_fd < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            break;  // Plus de connexions en attente
                        } else {
                            perror("accept");
                            break;
                        }
                    }

                    char ip[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
                    printf("[FD %d] Connexion de %s:%d\n",
                           client_fd, ip, ntohs(client_addr.sin_port));

                    // Rendre non-bloquant
                    set_nonblocking(client_fd);

                    // Créer la connexion
                    connection_t *conn = malloc(sizeof(connection_t));
                    memset(conn, 0, sizeof(connection_t));
                    conn->fd = client_fd;
                    conn->state = STATE_READING;

                    // Ajouter à epoll en Edge-Triggered
                    event.events = EPOLLIN | EPOLLET;
                    event.data.ptr = conn;
                    if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &event) < 0) {
                        perror("epoll_ctl");
                        free(conn);
                        close(client_fd);
                    }
                }
            } else {
                // Événement client
                connection_t *conn = events[i].data.ptr;

                if (events[i].events & EPOLLIN) {
                    handle_read(epfd, conn);
                }

                if (events[i].events & EPOLLOUT) {
                    if (conn->state == STATE_WRITING) {
                        handle_write(epfd, conn);
                    }
                }

                // Fermer si nécessaire
                if (conn->state == STATE_CLOSING) {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, conn->fd, NULL);
                    close(conn->fd);
                    free(conn);
                }
            }
        }
    }

    close(server_fd);
    close(epfd);
    return 0;
}
```

**Compilation :**
```bash
gcc -o server_epoll_et server_epoll_et.c -Wall -Wextra
./server_epoll_et
```

**Test :**
```bash
# Terminal 1
./server_epoll_et

# Terminal 2
telnet localhost 8080  
Hello  
# Le serveur renvoie : Hello
```

---

## Patterns Avancés

### 1. Buffers Dynamiques

Pour gérer de grandes données, utilisez des buffers dynamiques redimensionnables.

```c
typedef struct {
    char *data;
    size_t capacity;
    size_t length;
} dynamic_buffer_t;

void buffer_append(dynamic_buffer_t *buf, const char *data, size_t len) {
    if (buf->length + len > buf->capacity) {
        // Redimensionner
        size_t new_capacity = (buf->capacity + len) * 2;
        char *new_data = realloc(buf->data, new_capacity);
        if (!new_data) {
            fprintf(stderr, "Erreur realloc\n");
            return;
        }
        buf->data = new_data;
        buf->capacity = new_capacity;
    }

    memcpy(buf->data + buf->length, data, len);
    buf->length += len;
}
```

---

### 2. Zero-Copy avec `sendfile()`

Pour envoyer des fichiers, utilisez `sendfile()` pour éviter de copier en mémoire.

```c
#include <sys/sendfile.h>

// Envoyer un fichier directement
int file_fd = open("file.txt", O_RDONLY);  
off_t offset = 0;  
size_t file_size = 1024;  

ssize_t sent = sendfile(socket_fd, file_fd, &offset, file_size);
```

**Avantages :**
- Pas de copie kernel → userspace → kernel
- Performance optimale
- Moins d'utilisation CPU

---

### 3. EPOLLONESHOT

Pour éviter les race conditions dans les architectures multi-threadées.

```c
event.events = EPOLLIN | EPOLLONESHOT;  
event.data.ptr = conn;  
epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);  

// Après traitement, ré-armer manuellement
event.events = EPOLLIN | EPOLLONESHOT;  
epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);  
```

**Effet :** Le descripteur est automatiquement désactivé après une notification.

---

### 4. EPOLLEXCLUSIVE (Linux 4.5+)

Pour répartir les connexions entre plusieurs threads/processus.

```c
event.events = EPOLLIN | EPOLLEXCLUSIVE;
```

**Effet :** Un seul thread/processus est réveillé par événement (évite le "thundering herd").

---

## Debugging et Profiling

### 1. Vérifier les Sockets Non-Bloquants

```bash
# Voir les flags d'un socket
ls -l /proc/<PID>/fd/<FD>

# Avec lsof
lsof -p <PID>
```

---

### 2. Surveiller les Événements epoll

```c
// Logger les événements
if (events[i].events & EPOLLIN)  printf("EPOLLIN\n");  
if (events[i].events & EPOLLOUT) printf("EPOLLOUT\n");  
if (events[i].events & EPOLLERR) printf("EPOLLERR\n");  
if (events[i].events & EPOLLHUP) printf("EPOLLHUP\n");  
```

---

### 3. Statistiques de Performance

```c
struct stats {
    uint64_t total_connections;
    uint64_t active_connections;
    uint64_t bytes_read;
    uint64_t bytes_written;
    time_t start_time;
};

void print_stats(struct stats *s) {
    time_t uptime = time(NULL) - s->start_time;
    printf("Uptime: %ld s\n", uptime);
    printf("Connexions totales: %lu\n", s->total_connections);
    printf("Connexions actives: %lu\n", s->active_connections);
    printf("Débit entrant: %.2f MB/s\n",
           s->bytes_read / (double)uptime / 1024 / 1024);
    printf("Débit sortant: %.2f MB/s\n",
           s->bytes_written / (double)uptime / 1024 / 1024);
}
```

---

## Erreurs Courantes et Solutions

### 1. Oublier de Tout Lire en ET

**Symptôme :** Le serveur se bloque, ne répond plus à certains clients.

**Cause :** Pas de boucle `while` pour lire jusqu'à `EAGAIN`.

**Solution :**
```c
// ✅ Bon
while (1) {
    ssize_t n = recv(...);
    if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        break;  // Tout lu
    }
    // Traiter...
}
```

---

### 2. Ne Pas Désactiver EPOLLOUT

**Symptôme :** `epoll_wait()` retourne continuellement, CPU à 100%.

**Cause :** `EPOLLOUT` est toujours actif, même quand il n'y a rien à écrire.

**Solution :** Activer `EPOLLOUT` seulement quand nécessaire, le désactiver après.

```c
// Activer quand on a des données à envoyer
event.events = EPOLLIN | EPOLLOUT | EPOLLET;  
epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);  

// Désactiver quand tout est envoyé
event.events = EPOLLIN | EPOLLET;  
epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);  
```

---

### 3. Fuites Mémoire avec les Connexions

**Symptôme :** Mémoire croissante sans limite.

**Cause :** Oublier de `free()` les structures de connexion.

**Solution :** Toujours libérer lors de la fermeture.

```c
if (conn->state == STATE_CLOSING) {
    epoll_ctl(epfd, EPOLL_CTL_DEL, conn->fd, NULL);
    close(conn->fd);
    free(conn);  // ✅ Important !
}
```

---

### 4. Socket Bloquant en Mode ET

**Symptôme :** Le serveur se bloque complètement.

**Cause :** Socket resté en mode bloquant avec epoll ET.

**Solution :** Toujours `set_nonblocking()` avant d'ajouter à epoll ET.

```c
int client_fd = accept(...);  
set_nonblocking(client_fd);  // ✅ Obligatoire en ET  
epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &event);  
```

---

## Performances : Level-Triggered vs Edge-Triggered

### Benchmarks Typiques

**Configuration :** 10 000 connexions simultanées, 1000 requêtes/s

| Métrique | Level-Triggered | Edge-Triggered |
|----------|-----------------|----------------|
| **CPU** | 35% | 22% |
| **Appels système** | ~20 000/s | ~12 000/s |
| **Latence (p99)** | 5 ms | 3 ms |
| **Débit max** | 50k req/s | 80k req/s |

**Gain ET :** ~30-40% de performance en haute charge.

---

## Architecture Recommandée : Multi-Processus + epoll

Pour exploiter tous les cœurs CPU :

```
Processus 1 (Core 1) : epoll ET → 25k connexions  
Processus 2 (Core 2) : epoll ET → 25k connexions  
Processus 3 (Core 3) : epoll ET → 25k connexions  
Processus 4 (Core 4) : epoll ET → 25k connexions  

Total : 100k connexions, 4 cœurs utilisés
```

**Implémentation :**

```c
int main() {
    int num_workers = sysconf(_SC_NPROCESSORS_ONLN);  // Nombre de CPU

    for (int i = 0; i < num_workers; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            // Processus fils
            worker_main();  // Boucle epoll
            exit(0);
        }
    }

    // Processus parent surveille les fils
    while (1) {
        wait(NULL);
    }
}
```

**Avec `SO_REUSEPORT` :**

```c
int opt = 1;  
setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));  
```

Le kernel distribue automatiquement les connexions entre les processus.

---

## Comparaison Finale : Bloquant vs Non-Bloquant

| Aspect | Bloquant | Non-Bloquant |
|--------|----------|--------------|
| **Simplicité** | ✅ Simple | ❌ Complexe |
| **Performance** | Moyenne | ✅ Excellente |
| **Scalabilité** | Limitée | ✅ > 100k clients |
| **CPU** | Plus élevé | ✅ Optimisé |
| **Latence** | Moyenne | ✅ Faible |
| **Debugging** | Facile | Difficile |
| **Code** | Court | Long |

---

## En Résumé

Le **I/O non-bloquant** avec **epoll Edge-Triggered** est la base des serveurs haute performance modernes.

**Points clés à retenir :**

✅ **Non-bloquant** : Les opérations retournent immédiatement

✅ **EAGAIN** est normal : Signifie "réessayez plus tard"

✅ **Edge-Triggered** : Notification seulement sur changements d'état

✅ **Tout lire** : En ET, boucler jusqu'à `EAGAIN`

✅ **Gérer EPOLLOUT** : Activer/désactiver selon les besoins

✅ **État par connexion** : Tracker lecture/écriture/fermeture

✅ **Buffers** : Gérer les lectures/écritures partielles

**Workflow typique :**

1. Créer socket et le rendre **non-bloquant**
2. Ajouter à `epoll()` avec `EPOLLET`
3. Sur `EPOLLIN` : Boucler `recv()` jusqu'à `EAGAIN`
4. Traiter les données
5. Sur `EPOLLOUT` : Boucler `send()` jusqu'à `EAGAIN` ou tout envoyé
6. Désactiver `EPOLLOUT` quand plus rien à envoyer

**Quand utiliser :**
- Serveurs haute performance (> 1000 clients)
- Applications sensibles à la latence
- Besoins de scalabilité extrême

**Exemples de serveurs utilisant epoll ET :**
- Nginx
- Redis
- Memcached
- HAProxy

Dans la prochaine section, nous verrons comment créer un mini-serveur HTTP complet utilisant toutes ces techniques.

---

**→ Prochaine section : 20.10 Création d'un Mini-Serveur HTTP**

⏭️ [Création d'un mini-serveur HTTP](/20-reseau-sockets/10-mini-serveur-http.md)
