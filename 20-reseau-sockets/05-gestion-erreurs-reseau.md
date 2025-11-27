🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 20.5 Gestion des Erreurs Réseau

## Introduction

La programmation réseau est intrinsèquement **fragile** et **imprévisible**. Contrairement aux opérations locales (lecture de fichier, calculs), les opérations réseau peuvent échouer pour d'innombrables raisons : câble débranché, serveur surchargé, routeur défaillant, timeout, etc.

Une application réseau robuste doit **anticiper** et **gérer** ces erreurs de manière élégante. Dans cette section, nous allons explorer les différentes catégories d'erreurs réseau, comment les détecter, et comment y réagir de manière appropriée.

**Règle d'or :** En réseau, **tout peut échouer**. Votre code doit être prêt.

---

## Pourquoi la Gestion des Erreurs est Cruciale

### Conséquences d'une mauvaise gestion

❌ **Plantage de l'application** : Segfault, assertion failed

❌ **Blocage infini** : Le programme attend indéfiniment une réponse qui ne viendra jamais

❌ **Perte de données** : Messages perdus sans notification

❌ **Corruption de données** : État incohérent de l'application

❌ **Mauvaise expérience utilisateur** : Erreurs cryptiques, pas de feedback

### Objectifs d'une bonne gestion

✅ **Robustesse** : L'application continue de fonctionner malgré les erreurs

✅ **Récupération** : Tenter de se remettre automatiquement des erreurs

✅ **Diagnostic** : Logger les erreurs pour faciliter le debugging

✅ **Feedback** : Informer l'utilisateur de manière claire

✅ **Prévention** : Détecter les conditions d'erreur avant qu'elles ne se produisent

---

## Les Catégories d'Erreurs Réseau

### 1. Erreurs de Configuration

Erreurs détectées **immédiatement**, généralement au démarrage.

**Exemples :**
- Socket ne peut pas être créé
- `bind()` échoue (port déjà utilisé)
- Adresse IP invalide
- Permissions insuffisantes

**Caractéristiques :**
- Déterministes (se reproduisent toujours)
- Doivent empêcher le démarrage de l'application
- Faciles à diagnostiquer

**Gestion typique :** Afficher l'erreur et quitter proprement

```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd < 0) {
    perror("socket");
    fprintf(stderr, "Impossible de créer le socket. Vérifiez les permissions.\n");
    exit(EXIT_FAILURE);
}
```

---

### 2. Erreurs de Connexion

Erreurs lors de l'**établissement** d'une connexion.

**Exemples TCP :**
- Le serveur n'écoute pas (ECONNREFUSED)
- Le serveur ne répond pas (ETIMEDOUT)
- Le réseau est inaccessible (ENETUNREACH)
- L'hôte est inaccessible (EHOSTUNREACH)

**Exemples UDP :**
- Moins fréquentes (UDP est sans connexion)
- ICMP "Port Unreachable" sur socket "connecté"

**Caractéristiques :**
- Peuvent être temporaires ou permanentes
- Nécessitent souvent des retries avec backoff
- Doivent être signalées à l'utilisateur

**Gestion typique :** Retry avec délai croissant

```c
#define MAX_RETRIES 3
int retry = 0;

while (retry < MAX_RETRIES) {
    if (connect(sockfd, ...) == 0) {
        break;  // Succès
    }

    if (errno == ECONNREFUSED || errno == ETIMEDOUT) {
        retry++;
        fprintf(stderr, "Tentative %d/%d échouée : %s\n",
                retry, MAX_RETRIES, strerror(errno));

        if (retry < MAX_RETRIES) {
            sleep(2 * retry);  // Backoff exponentiel
        }
    } else {
        perror("connect");
        break;  // Erreur non récupérable
    }
}

if (retry == MAX_RETRIES) {
    fprintf(stderr, "Impossible de se connecter après %d tentatives\n", MAX_RETRIES);
    exit(EXIT_FAILURE);
}
```

---

### 3. Erreurs de Transmission

Erreurs pendant l'**envoi** ou la **réception** de données.

**Exemples :**
- Connexion fermée par le pair (retour de 0)
- Connexion réinitialisée (ECONNRESET)
- Broken pipe (EPIPE)
- Timeout (ETIMEDOUT)
- Interruption par signal (EINTR)

**Caractéristiques :**
- Peuvent survenir à tout moment
- Nécessitent une détection immédiate
- Peuvent nécessiter une reconnexion

**Gestion typique :** Détecter, logger, décider (retry/abort)

```c
ssize_t n = send(sockfd, buffer, len, 0);
if (n < 0) {
    switch (errno) {
        case EPIPE:
        case ECONNRESET:
            fprintf(stderr, "Connexion fermée par le serveur\n");
            // Reconnexion ou arrêt propre
            break;

        case EINTR:
            // Signal reçu, réessayer
            continue;

        case ETIMEDOUT:
            fprintf(stderr, "Timeout lors de l'envoi\n");
            // Décider si on retry ou abandonne
            break;

        default:
            perror("send");
            break;
    }
} else if (n == 0) {
    // Pour send(), 0 ne devrait pas arriver, mais bon à vérifier
    fprintf(stderr, "send() a retourné 0\n");
}
```

---

### 4. Erreurs de Timeout

L'opération prend **trop de temps**.

**Exemples :**
- `recv()` bloque indéfiniment
- Le serveur ne répond pas
- Réseau très lent

**Caractéristiques :**
- Peuvent indiquer un problème réseau ou un serveur surchargé
- Nécessitent une configuration de timeout appropriée
- Comportement souvent acceptable avec retry

**Gestion typique :** Définir des timeouts raisonnables

```c
// Timeout pour recv()
struct timeval tv;
tv.tv_sec = 30;   // 30 secondes
tv.tv_usec = 0;

if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    perror("setsockopt SO_RCVTIMEO");
}

// Maintenant recv() timeout après 30 secondes
ssize_t n = recv(sockfd, buffer, sizeof(buffer), 0);
if (n < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        fprintf(stderr, "Timeout : aucune donnée reçue en 30 secondes\n");
        // Décider quoi faire
    } else {
        perror("recv");
    }
}
```

---

### 5. Erreurs de Ressources

Le système manque de **ressources**.

**Exemples :**
- Trop de descripteurs de fichiers ouverts (EMFILE, ENFILE)
- Mémoire insuffisante (ENOMEM)
- Buffer plein (ENOBUFS)

**Caractéristiques :**
- Souvent temporaires
- Peuvent affecter l'ensemble du système
- Nécessitent une gestion de charge

**Gestion typique :** Limiter les ressources, retry, dégradation gracieuse

```c
int client_fd = accept(server_fd, ...);
if (client_fd < 0) {
    if (errno == EMFILE || errno == ENFILE) {
        fprintf(stderr, "Trop de connexions ouvertes, refus temporaire\n");
        // Possibilité : fermer des connexions inactives
        // Ou : attendre un peu avant le prochain accept()
        sleep(1);
    } else {
        perror("accept");
    }
}
```

---

## La Variable `errno` : Diagnostic des Erreurs

### Fonctionnement de `errno`

Quand une fonction système échoue (retourne -1), elle positionne la variable globale **`errno`** avec un code d'erreur spécifique.

```c
#include <errno.h>

extern int errno;  // Variable globale (thread-local en pratique)
```

**Séquence typique :**

1. Appel système
2. Si erreur (-1), consulter `errno`
3. Interpréter le code d'erreur
4. Réagir en conséquence

⚠️ **Important :** `errno` n'est positionné qu'en cas d'erreur. Ne jamais le consulter si la fonction a réussi.

### Utilisation de `perror()` et `strerror()`

#### `perror()` - Affichage rapide

```c
void perror(const char *s);
```

Affiche `s` suivi de `: ` puis d'un message d'erreur lisible basé sur `errno`.

**Exemple :**
```c
if (connect(sockfd, ...) < 0) {
    perror("connect");
    // Affiche : "connect: Connection refused"
}
```

#### `strerror()` - Message d'erreur personnalisé

```c
#include <string.h>

char *strerror(int errnum);
```

Retourne une chaîne décrivant le code d'erreur.

**Exemple :**
```c
if (connect(sockfd, ...) < 0) {
    fprintf(stderr, "Échec de connexion au serveur : %s\n", strerror(errno));
}
```

#### `strerror_r()` - Version thread-safe

```c
int strerror_r(int errnum, char *buf, size_t buflen);
```

Version thread-safe pour applications multi-threadées.

---

## Codes d'Erreur Réseau Courants

Voici les codes d'erreur que vous rencontrerez le plus fréquemment :

### Erreurs de Connexion

| Code | Description | Cause typique | Réaction |
|------|-------------|---------------|----------|
| **ECONNREFUSED** | Connexion refusée | Serveur n'écoute pas sur ce port | Vérifier port, retry |
| **ETIMEDOUT** | Timeout | Serveur ne répond pas, réseau lent | Retry, augmenter timeout |
| **ENETUNREACH** | Réseau inaccessible | Problème de routage | Vérifier réseau |
| **EHOSTUNREACH** | Hôte inaccessible | Serveur éteint ou firewall | Vérifier connectivité |
| **ECONNRESET** | Connexion réinitialisée | Serveur a crashé ou redémarré | Reconnexion |
| **EPIPE** | Broken pipe | Écriture sur socket fermé | Gérer proprement |

### Erreurs de Configuration

| Code | Description | Cause typique | Réaction |
|------|-------------|---------------|----------|
| **EADDRINUSE** | Adresse déjà utilisée | Port déjà occupé | Changer port ou `SO_REUSEADDR` |
| **EACCES** | Permission refusée | Port <1024 sans root | Utiliser port >1024 ou `sudo` |
| **EADDRNOTAVAIL** | Adresse non disponible | IP invalide pour cette machine | Corriger l'adresse |
| **EINVAL** | Argument invalide | Paramètre incorrect | Vérifier les paramètres |

### Erreurs de Transmission

| Code | Description | Cause typique | Réaction |
|------|-------------|---------------|----------|
| **EINTR** | Interrompu par signal | Signal reçu pendant l'appel | Réessayer l'opération |
| **EAGAIN** / **EWOULDBLOCK** | Opération bloquerait | Timeout ou mode non-bloquant | Réessayer ou timeout |
| **EMSGSIZE** | Message trop grand | Datagramme UDP trop grand | Réduire la taille |

### Erreurs de Ressources

| Code | Description | Cause typique | Réaction |
|------|-------------|---------------|----------|
| **EMFILE** | Trop de fichiers ouverts (processus) | Limite atteinte | Fermer descripteurs inutilisés |
| **ENFILE** | Trop de fichiers ouverts (système) | Limite système atteinte | Attendre, réduire charge |
| **ENOMEM** | Mémoire insuffisante | RAM épuisée | Libérer mémoire, redémarrer |
| **ENOBUFS** | Buffer plein | Buffers réseau saturés | Ralentir envoi, retry |

---

## Détection de Déconnexion

### Pour TCP : Retour de 0

Un retour de **0** pour `recv()` / `read()` signifie que la connexion a été fermée **proprement** par l'autre côté.

```c
ssize_t n = recv(sockfd, buffer, sizeof(buffer), 0);

if (n < 0) {
    perror("recv");
    // Erreur réseau
} else if (n == 0) {
    printf("Connexion fermée par le pair\n");
    // Fermer proprement notre côté
    close(sockfd);
} else {
    // Données reçues normalement
    buffer[n] = '\0';
    printf("Reçu : %s\n", buffer);
}
```

### Différence entre fermeture propre et brutale

**Fermeture propre (retour 0) :**
- Le pair a appelé `close()` ou `shutdown()`
- FIN envoyé
- Données en transit préservées

**Fermeture brutale (ECONNRESET) :**
- Le pair a crashé
- Ou a appelé `close()` avec données non lues
- RST envoyé
- Données en transit perdues

### Détecter une connexion "morte"

Parfois, la connexion est coupée (câble débranché) mais aucun des deux côtés ne le sait encore.

**Solutions :**

#### 1. TCP Keepalive

```c
int keepalive = 1;
if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)) < 0) {
    perror("setsockopt SO_KEEPALIVE");
}

// Optionnel : configurer les paramètres keepalive
int keepidle = 60;   // Inactivité avant premier probe (secondes)
int keepintvl = 10;  // Intervalle entre probes
int keepcnt = 3;     // Nombre de probes avant déclaration morte

setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle));
setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(keepintvl));
setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt));
```

**Fonctionnement :**
- Après 60 secondes d'inactivité, envoie un probe
- Si pas de réponse, réessaie toutes les 10 secondes
- Après 3 échecs, la connexion est déclarée morte

#### 2. Application-level keepalive (ping/pong)

Envoyer périodiquement un message de "heartbeat" :

```c
// Protocole simple : envoyer "PING" toutes les 30 secondes
// Le serveur répond "PONG"
// Si pas de PONG après 3 tentatives, considérer déconnecté
```

---

## Gestion du Signal SIGPIPE

### Le problème

Quand vous écrivez sur un socket TCP dont l'autre extrémité est fermée, le kernel envoie un signal **SIGPIPE** à votre processus.

**Par défaut :** `SIGPIPE` **termine** le processus !

### La solution : Ignorer SIGPIPE

```c
#include <signal.h>

// Ignorer SIGPIPE globalement
signal(SIGPIPE, SIG_IGN);
```

**Pourquoi ignorer ?**
- `send()` / `write()` retournera `-1` avec `errno = EPIPE`
- Vous pouvez gérer l'erreur proprement dans votre code
- Évite un crash inattendu

**Placement :** Au début du `main()`, avant toute opération réseau.

**Exemple complet :**

```c
int main() {
    // Ignorer SIGPIPE pour éviter un crash sur socket fermé
    signal(SIGPIPE, SIG_IGN);

    // Suite du code...

    ssize_t n = send(sockfd, buffer, len, 0);
    if (n < 0) {
        if (errno == EPIPE) {
            fprintf(stderr, "Tentative d'écriture sur socket fermé\n");
            // Gérer proprement (reconnexion, fermeture, etc.)
        } else {
            perror("send");
        }
    }
}
```

---

## Patterns de Gestion d'Erreurs

### Pattern 1 : Vérification Systématique

**Toujours** vérifier les valeurs de retour.

```c
// ❌ Mauvais
socket(AF_INET, SOCK_STREAM, 0);
connect(sockfd, ...);
send(sockfd, buffer, len, 0);

// ✅ Bon
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd < 0) {
    perror("socket");
    return -1;
}

if (connect(sockfd, ...) < 0) {
    perror("connect");
    close(sockfd);
    return -1;
}

ssize_t n = send(sockfd, buffer, len, 0);
if (n < 0) {
    perror("send");
    close(sockfd);
    return -1;
}
```

---

### Pattern 2 : Gestion de EINTR

`EINTR` se produit quand un appel système est interrompu par un signal.

**Solution :** Boucler jusqu'au succès ou vraie erreur.

```c
ssize_t recv_retry(int sockfd, void *buf, size_t len, int flags) {
    ssize_t n;

    do {
        n = recv(sockfd, buf, len, flags);
    } while (n < 0 && errno == EINTR);

    return n;
}
```

**Utilisation :**
```c
ssize_t n = recv_retry(sockfd, buffer, sizeof(buffer), 0);
if (n < 0) {
    perror("recv");
} else if (n == 0) {
    printf("Connexion fermée\n");
} else {
    // Traiter les données
}
```

---

### Pattern 3 : Envoi Complet avec Gestion d'Erreurs

`send()` peut envoyer moins que demandé. Il faut boucler.

```c
ssize_t send_all(int sockfd, const void *buf, size_t len) {
    size_t total_sent = 0;
    const char *ptr = buf;

    while (total_sent < len) {
        ssize_t n = send(sockfd, ptr + total_sent, len - total_sent, 0);

        if (n < 0) {
            if (errno == EINTR) {
                continue;  // Réessayer
            }

            // Erreur réelle
            perror("send");
            return -1;
        }

        total_sent += n;
    }

    return total_sent;
}
```

---

### Pattern 4 : Retry avec Backoff Exponentiel

Pour les opérations qui peuvent échouer temporairement.

```c
#define MAX_RETRIES 5
#define INITIAL_DELAY_MS 100

int connect_with_retry(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int retry = 0;
    int delay_ms = INITIAL_DELAY_MS;

    while (retry < MAX_RETRIES) {
        if (connect(sockfd, addr, addrlen) == 0) {
            return 0;  // Succès
        }

        // Erreur non récupérable
        if (errno != ECONNREFUSED && errno != ETIMEDOUT && errno != ENETUNREACH) {
            perror("connect");
            return -1;
        }

        retry++;
        fprintf(stderr, "Connexion échouée, tentative %d/%d (attente %d ms)\n",
                retry, MAX_RETRIES, delay_ms);

        // Attendre avant retry
        usleep(delay_ms * 1000);

        // Backoff exponentiel : doubler le délai
        delay_ms *= 2;
    }

    fprintf(stderr, "Impossible de se connecter après %d tentatives\n", MAX_RETRIES);
    return -1;
}
```

---

### Pattern 5 : Timeout avec `select()`

Alternative à `SO_RCVTIMEO` pour plus de contrôle.

```c
#include <sys/select.h>

ssize_t recv_with_timeout(int sockfd, void *buf, size_t len, int timeout_sec) {
    fd_set readfds;
    struct timeval tv;

    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;

    int ready = select(sockfd + 1, &readfds, NULL, NULL, &tv);

    if (ready < 0) {
        perror("select");
        return -1;
    } else if (ready == 0) {
        fprintf(stderr, "Timeout après %d secondes\n", timeout_sec);
        errno = ETIMEDOUT;
        return -1;
    }

    // Des données sont disponibles, recv() ne bloquera pas
    return recv(sockfd, buf, len, 0);
}
```

---

### Pattern 6 : Gestion d'État de Connexion

Pour des applications qui doivent maintenir une connexion.

```c
typedef enum {
    STATE_DISCONNECTED,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_ERROR
} connection_state_t;

typedef struct {
    int sockfd;
    connection_state_t state;
    time_t last_activity;
    int retry_count;
} connection_t;

void handle_connection_error(connection_t *conn, int error) {
    fprintf(stderr, "Erreur connexion : %s\n", strerror(error));

    if (conn->sockfd >= 0) {
        close(conn->sockfd);
        conn->sockfd = -1;
    }

    conn->state = STATE_DISCONNECTED;
    conn->retry_count++;

    // Tenter de se reconnecter si pas trop d'échecs
    if (conn->retry_count < 10) {
        fprintf(stderr, "Tentative de reconnexion...\n");
        // Logique de reconnexion
    } else {
        fprintf(stderr, "Trop d'échecs, abandon\n");
        conn->state = STATE_ERROR;
    }
}
```

---

## Logging des Erreurs

Un bon système de logging est essentiel pour diagnostiquer les problèmes en production.

### Niveaux de Log

```c
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_CRITICAL
} log_level_t;

const char* log_level_str[] = {
    "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"
};
```

### Fonction de Log Simple

```c
#include <time.h>
#include <stdarg.h>

void log_message(log_level_t level, const char *format, ...) {
    time_t now = time(NULL);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    fprintf(stderr, "[%s] [%s] ", timestamp, log_level_str[level]);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");
}
```

### Utilisation

```c
if (connect(sockfd, ...) < 0) {
    log_message(LOG_ERROR, "Échec connexion à %s:%d : %s",
                server_ip, server_port, strerror(errno));

    if (errno == ECONNREFUSED) {
        log_message(LOG_WARNING, "Serveur probablement éteint, retry dans 5s");
    }
}
```

### Log vers Fichier

```c
FILE *log_file = NULL;

void init_logging(const char *filename) {
    log_file = fopen(filename, "a");
    if (!log_file) {
        perror("fopen log file");
        log_file = stderr;  // Fallback sur stderr
    }
}

void log_message(log_level_t level, const char *format, ...) {
    if (!log_file) {
        log_file = stderr;
    }

    // Même code qu'avant, mais fprintf() vers log_file
    // ...

    fflush(log_file);  // Forcer l'écriture immédiate
}
```

---

## Stratégies de Récupération

### 1. Reconnexion Automatique

```c
void maintain_connection(connection_t *conn,
                        const char *server_ip,
                        int server_port) {
    while (1) {
        if (conn->state == STATE_DISCONNECTED) {
            log_message(LOG_INFO, "Tentative de connexion...");

            if (reconnect(conn, server_ip, server_port) == 0) {
                log_message(LOG_INFO, "Connexion rétablie");
                conn->retry_count = 0;
            } else {
                int delay = min(60, 2 << conn->retry_count);  // Max 60s
                log_message(LOG_WARNING, "Échec, retry dans %ds", delay);
                sleep(delay);
            }
        } else if (conn->state == STATE_CONNECTED) {
            // Opérations normales
            // ...
        }
    }
}
```

---

### 2. Circuit Breaker

Évite de surcharger un serveur défaillant.

```c
typedef enum {
    CB_CLOSED,   // Normal, requêtes passent
    CB_OPEN,     // Défaillance détectée, requêtes bloquées
    CB_HALF_OPEN // Test si serveur récupéré
} circuit_state_t;

typedef struct {
    circuit_state_t state;
    int failure_count;
    int failure_threshold;  // Ex: 5 échecs consécutifs
    time_t last_failure;
    int timeout;            // Ex: 60 secondes avant retry
} circuit_breaker_t;

int circuit_breaker_allow_request(circuit_breaker_t *cb) {
    time_t now = time(NULL);

    switch (cb->state) {
        case CB_CLOSED:
            return 1;  // OK

        case CB_OPEN:
            if (now - cb->last_failure > cb->timeout) {
                log_message(LOG_INFO, "Circuit breaker : passage en HALF_OPEN");
                cb->state = CB_HALF_OPEN;
                return 1;
            }
            return 0;  // Bloqué

        case CB_HALF_OPEN:
            return 1;  // Tester une requête
    }

    return 0;
}

void circuit_breaker_on_success(circuit_breaker_t *cb) {
    cb->failure_count = 0;
    cb->state = CB_CLOSED;
}

void circuit_breaker_on_failure(circuit_breaker_t *cb) {
    cb->failure_count++;
    cb->last_failure = time(NULL);

    if (cb->failure_count >= cb->failure_threshold) {
        log_message(LOG_WARNING, "Circuit breaker : passage en OPEN");
        cb->state = CB_OPEN;
    }
}
```

---

### 3. Graceful Degradation

Fournir un service dégradé plutôt que de tomber complètement.

**Exemples :**
- Utiliser un cache local si le serveur distant est inaccessible
- Renvoyer des données approximatives
- Désactiver des fonctionnalités non essentielles

```c
data_t* fetch_data(const char *key) {
    data_t *data = NULL;

    // Essayer de récupérer depuis le serveur
    if (server_available) {
        data = fetch_from_server(key);
        if (data) {
            cache_store(key, data);  // Mettre en cache
            return data;
        }
    }

    // Fallback : chercher dans le cache local
    log_message(LOG_WARNING, "Serveur inaccessible, utilisation du cache");
    data = cache_lookup(key);

    if (!data) {
        log_message(LOG_ERROR, "Donnée non disponible : %s", key);
    }

    return data;
}
```

---

## Exemple Complet : Client TCP Robuste

Voici un client TCP avec gestion d'erreurs complète.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024
#define MAX_RETRIES 3
#define RECV_TIMEOUT 10

// Fonction de log simple
void log_error(const char *msg) {
    fprintf(stderr, "[ERROR] %s: %s\n", msg, strerror(errno));
}

// Fonction send avec gestion complète
ssize_t send_all(int sockfd, const void *buf, size_t len) {
    size_t total = 0;
    const char *ptr = buf;

    while (total < len) {
        ssize_t n = send(sockfd, ptr + total, len - total, 0);

        if (n < 0) {
            if (errno == EINTR) continue;
            if (errno == EPIPE || errno == ECONNRESET) {
                fprintf(stderr, "Connexion fermée lors de l'envoi\n");
            } else {
                log_error("send");
            }
            return -1;
        }

        total += n;
    }

    return total;
}

// Fonction recv avec timeout et gestion complète
ssize_t recv_with_error_handling(int sockfd, void *buf, size_t len) {
    ssize_t n;

    do {
        n = recv(sockfd, buf, len, 0);
    } while (n < 0 && errno == EINTR);

    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            fprintf(stderr, "Timeout : pas de réponse du serveur\n");
        } else if (errno == ECONNRESET) {
            fprintf(stderr, "Connexion réinitialisée par le serveur\n");
        } else {
            log_error("recv");
        }
        return -1;
    } else if (n == 0) {
        fprintf(stderr, "Serveur a fermé la connexion\n");
        return 0;
    }

    return n;
}

// Connexion avec retry
int connect_to_server(const char *ip, int port) {
    int sockfd;
    struct sockaddr_in server_addr;
    int retry = 0;

    // Créer le socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        log_error("socket");
        return -1;
    }

    // Configurer timeout pour recv
    struct timeval tv = {.tv_sec = RECV_TIMEOUT, .tv_usec = 0};
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        log_error("setsockopt SO_RCVTIMEO");
    }

    // Préparer adresse serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Adresse IP invalide : %s\n", ip);
        close(sockfd);
        return -1;
    }

    // Tentatives de connexion avec retry
    while (retry < MAX_RETRIES) {
        printf("Tentative de connexion %d/%d à %s:%d...\n",
               retry + 1, MAX_RETRIES, ip, port);

        if (connect(sockfd, (struct sockaddr*)&server_addr,
                    sizeof(server_addr)) == 0) {
            printf("✓ Connecté avec succès\n");
            return sockfd;
        }

        // Analyser l'erreur
        switch (errno) {
            case ECONNREFUSED:
                fprintf(stderr, "✗ Connexion refusée (serveur n'écoute pas?)\n");
                break;
            case ETIMEDOUT:
                fprintf(stderr, "✗ Timeout (serveur ne répond pas?)\n");
                break;
            case ENETUNREACH:
                fprintf(stderr, "✗ Réseau inaccessible\n");
                break;
            case EHOSTUNREACH:
                fprintf(stderr, "✗ Hôte inaccessible\n");
                break;
            default:
                log_error("connect");
                close(sockfd);
                return -1;
        }

        retry++;
        if (retry < MAX_RETRIES) {
            int delay = 2 * retry;
            printf("  Attente de %d secondes avant retry...\n", delay);
            sleep(delay);
        }
    }

    fprintf(stderr, "✗ Impossible de se connecter après %d tentatives\n", MAX_RETRIES);
    close(sockfd);
    return -1;
}

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];

    // Ignorer SIGPIPE
    signal(SIGPIPE, SIG_IGN);

    // Se connecter au serveur
    sockfd = connect_to_server(SERVER_IP, SERVER_PORT);
    if (sockfd < 0) {
        exit(EXIT_FAILURE);
    }

    // Boucle d'interaction
    while (1) {
        printf("\nMessage à envoyer (ou 'quit') : ");

        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        if (strcmp(buffer, "quit\n") == 0) {
            break;
        }

        // Envoyer le message
        if (send_all(sockfd, buffer, strlen(buffer)) < 0) {
            fprintf(stderr, "Erreur lors de l'envoi, fermeture connexion\n");
            break;
        }

        // Recevoir la réponse
        ssize_t n = recv_with_error_handling(sockfd, buffer, BUFFER_SIZE - 1);
        if (n < 0) {
            fprintf(stderr, "Erreur lors de la réception, fermeture connexion\n");
            break;
        } else if (n == 0) {
            fprintf(stderr, "Serveur a fermé la connexion\n");
            break;
        }

        buffer[n] = '\0';
        printf("Réponse : %s", buffer);
    }

    // Nettoyage
    close(sockfd);
    printf("\nDéconnecté proprement\n");

    return 0;
}
```

**Compilation :**
```bash
gcc -o robust_client robust_client.c -Wall -Wextra
```

---

## Tests de Robustesse

### Simuler des Conditions d'Erreur

#### 1. Tester avec serveur éteint

```bash
# Lancer le client sans serveur
./robust_client
# Devrait afficher des erreurs de connexion et retry
```

#### 2. Couper la connexion pendant utilisation

```bash
# Terminal 1 : lancer serveur
./server

# Terminal 2 : lancer client
./robust_client

# Terminal 1 : Ctrl+C pour tuer le serveur
# Observer comment le client détecte la déconnexion
```

#### 3. Simuler latence réseau avec `tc`

```bash
# Ajouter 200ms de latence
sudo tc qdisc add dev lo root netem delay 200ms

# Tester votre application

# Supprimer la latence
sudo tc qdisc del dev lo root
```

#### 4. Simuler perte de paquets

```bash
# 10% de perte de paquets
sudo tc qdisc add dev lo root netem loss 10%

# Tester (surtout pour UDP)

# Supprimer
sudo tc qdisc del dev lo root
```

---

## Checklist de Gestion d'Erreurs

Utilisez cette checklist pour vérifier la robustesse de votre code réseau :

### Initialisation
- [ ] Vérifier retour de `socket()`
- [ ] Vérifier retour de `setsockopt()`
- [ ] Vérifier retour de `bind()`
- [ ] Vérifier retour de `listen()`
- [ ] Gérer `EADDRINUSE` avec `SO_REUSEADDR`

### Connexion
- [ ] Vérifier retour de `connect()` / `accept()`
- [ ] Implémenter retry avec backoff
- [ ] Définir des timeouts appropriés
- [ ] Logger les échecs de connexion

### Transmission
- [ ] Vérifier retour de `send()` / `recv()`
- [ ] Gérer `EINTR` (interruption par signal)
- [ ] Gérer `EPIPE` (broken pipe)
- [ ] Gérer `ECONNRESET` (connexion réinitialisée)
- [ ] Détecter déconnexion (retour de 0)
- [ ] Boucler pour `send()` complet

### Timeouts
- [ ] Définir `SO_RCVTIMEO` / `SO_SNDTIMEO`
- [ ] Ou utiliser `select()` / `poll()`
- [ ] Gérer `EAGAIN` / `EWOULDBLOCK`

### Signaux
- [ ] Ignorer `SIGPIPE` avec `signal(SIGPIPE, SIG_IGN)`

### Ressources
- [ ] Toujours `close()` les sockets
- [ ] Libérer la mémoire allouée
- [ ] Gérer `EMFILE` / `ENFILE`

### Logging
- [ ] Logger les connexions / déconnexions
- [ ] Logger les erreurs avec contexte
- [ ] Logger les métriques (bytes envoyés/reçus)

### Tests
- [ ] Tester avec serveur éteint
- [ ] Tester déconnexion brutale
- [ ] Tester latence élevée
- [ ] Tester perte de paquets (UDP)

---

## En Résumé

La gestion des erreurs réseau est **essentielle** pour créer des applications robustes. Une bonne gestion transforme un programme fragile en un système résilient.

**Points clés à retenir :**

✅ **Toujours vérifier** les valeurs de retour des fonctions système

✅ **Analyser `errno`** pour comprendre la nature de l'erreur

✅ **Ignorer `SIGPIPE`** pour éviter des crashs inattendus

✅ **Implémenter des timeouts** pour éviter les blocages infinis

✅ **Gérer `EINTR`** (interruption par signal) avec retry

✅ **Détecter les déconnexions** (retour de 0 pour `recv()`)

✅ **Logger les erreurs** avec contexte pour faciliter le diagnostic

✅ **Implémenter retry** avec backoff exponentiel pour erreurs temporaires

✅ **Maintenir un état** de connexion pour faciliter la récupération

✅ **Tester les conditions d'erreur** avec des outils comme `tc`

**Règle d'or :** En réseau, tout peut échouer. Votre code doit être prêt à gérer l'inattendu.

---

**→ Prochaine section : 20.6 Options de Sockets**

⏭️ [Options de sockets](/20-reseau-sockets/06-options-sockets.md)
