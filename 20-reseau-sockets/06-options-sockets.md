🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 20.6 Options de Sockets

## Introduction

Les sockets possèdent de nombreux **paramètres configurables** qui permettent d'ajuster leur comportement selon les besoins de votre application. Ces options contrôlent des aspects tels que les timeouts, la taille des buffers, le comportement de fermeture, et bien plus encore.

Dans cette section, nous allons explorer les options de sockets les plus importantes et apprendre à les utiliser avec les fonctions `setsockopt()` et `getsockopt()`.

**Pourquoi les options de sockets sont importantes :**
- Optimiser les performances
- Gérer les timeouts
- Résoudre des problèmes courants (port déjà utilisé)
- Adapter le comportement réseau à vos besoins spécifiques

---

## Les Fonctions `setsockopt()` et `getsockopt()`

### `setsockopt()` - Définir une option

```c
#include <sys/socket.h>

int setsockopt(int sockfd, int level, int optname,
               const void *optval, socklen_t optlen);
```

**Paramètres :**
- `sockfd` : Descripteur du socket
- `level` : Niveau du protocole
  - `SOL_SOCKET` : Options au niveau socket (génériques)
  - `IPPROTO_TCP` : Options spécifiques à TCP
  - `IPPROTO_IP` : Options spécifiques à IP
  - `IPPROTO_IPV6` : Options spécifiques à IPv6
- `optname` : Nom de l'option (ex: `SO_REUSEADDR`)
- `optval` : Pointeur vers la valeur de l'option
- `optlen` : Taille de la valeur

**Retour :**
- `0` en cas de succès
- `-1` en cas d'erreur (errno positionné)

**Exemple de base :**
```c
int opt = 1;  
if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {  
    perror("setsockopt");
    exit(EXIT_FAILURE);
}
```

---

### `getsockopt()` - Lire une option

```c
int getsockopt(int sockfd, int level, int optname,
               void *optval, socklen_t *optlen);
```

**Paramètres :**
- Identiques à `setsockopt()`, sauf :
- `optval` : Pointeur vers un buffer pour recevoir la valeur
- `optlen` : Pointeur vers la taille du buffer (modifié par la fonction)

**Retour :**
- `0` en cas de succès
- `-1` en cas d'erreur

**Exemple :**
```c
int opt;  
socklen_t optlen = sizeof(opt);  

if (getsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, &optlen) < 0) {
    perror("getsockopt");
} else {
    printf("SO_REUSEADDR est %s\n", opt ? "activé" : "désactivé");
}
```

---

## Options au Niveau Socket (SOL_SOCKET)

Ces options s'appliquent à tous les types de sockets.

### 1. `SO_REUSEADDR` - Réutiliser une adresse locale

#### Problème résolu

Après avoir fermé un serveur TCP, si vous essayez de le relancer immédiatement, vous obtenez :

```
bind: Address already in use
```

**Cause :** Le socket reste dans l'état TIME_WAIT pendant ~60 secondes pour s'assurer que tous les paquets en transit sont bien traités.

#### Solution

```c
int opt = 1;  
if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {  
    perror("setsockopt SO_REUSEADDR");
}
```

**Effet :**
- Permet de réutiliser immédiatement l'adresse locale
- **Essentiel en développement** pour redémarrages fréquents
- **Recommandé en production** pour éviter les temps d'arrêt

**Quand l'utiliser :**
- ✅ Toujours pour les serveurs TCP
- ✅ En développement pour itérations rapides

**Quand ne pas l'utiliser :**
❌ Si vous voulez garantir qu'un seul serveur tourne à la fois

**Placement dans le code :**
```c
int server_fd = socket(AF_INET, SOCK_STREAM, 0);

// ⚠️ AVANT bind()
int opt = 1;  
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));  

bind(server_fd, ...);  
listen(server_fd, ...);  
```

---

### 2. `SO_REUSEPORT` - Réutiliser le port (Linux 3.9+)

#### Cas d'usage

Permet à **plusieurs processus** d'écouter sur le **même port** simultanément.

```c
int opt = 1;  
setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));  
```

**Utilité :**
- Load balancing automatique au niveau kernel
- Plusieurs instances d'un serveur sur le même port
- Le kernel distribue les connexions entrantes entre les processus

**Exemple de déploiement :**
```bash
# Lancer 4 instances du même serveur
./server &
./server &
./server &
./server &

# Le kernel répartit les connexions entre les 4
```

**Avantages :**
- ✅ Meilleure utilisation des CPU multi-cœurs
- ✅ Load balancing automatique
- ✅ Pas besoin de reverse proxy externe

**Différence avec `SO_REUSEADDR` :**

| Option | Usage |
|--------|-------|
| `SO_REUSEADDR` | Redémarrer rapidement un serveur |
| `SO_REUSEPORT` | Plusieurs serveurs simultanés sur même port |

---

### 3. `SO_KEEPALIVE` - Garder la connexion vivante

#### Problème résolu

Détecter une connexion TCP "morte" (câble débranché, machine éteinte sans fermeture propre).

```c
int keepalive = 1;  
if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)) < 0) {  
    perror("setsockopt SO_KEEPALIVE");
}
```

**Fonctionnement :**
1. Après un certain temps d'inactivité, envoie un "probe" (paquet de vérification)
2. Si pas de réponse, réessaie périodiquement
3. Après un certain nombre d'échecs, la connexion est déclarée morte

**Configuration fine (Linux) :**
```c
// Temps d'inactivité avant le premier probe (secondes)
int keepidle = 60;  
setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle));  

// Intervalle entre les probes (secondes)
int keepintvl = 10;  
setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(keepintvl));  

// Nombre de probes avant déclaration morte
int keepcnt = 3;  
setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt));  
```

**Calcul du timeout total :**
```
Timeout = keepidle + (keepintvl * keepcnt)
        = 60 + (10 * 3)
        = 90 secondes
```

**Quand l'utiliser :**
- ✅ Connexions longue durée (serveurs de chat, websockets)
- ✅ Détecter des clients qui ont crashé
- ✅ Libérer des ressources automatiquement

**Inconvénients :**
- ❌ Overhead réseau (paquets keepalive)
- ❌ Peut détecter faussement des connexions mortes sur réseaux instables

---

### 4. `SO_RCVBUF` et `SO_SNDBUF` - Taille des buffers

#### Contrôler la taille des buffers réseau

```c
// Augmenter le buffer de réception à 256 KB
int rcvbuf = 256 * 1024;  
if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf)) < 0) {  
    perror("setsockopt SO_RCVBUF");
}

// Augmenter le buffer d'envoi à 256 KB
int sndbuf = 256 * 1024;  
if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf)) < 0) {  
    perror("setsockopt SO_SNDBUF");
}
```

**Taille par défaut :** Généralement ~200 KB (varie selon le système)

**Pourquoi modifier :**
- **Augmenter** : Pour connexions haute bande passante ou haute latence
- **Diminuer** : Pour économiser la mémoire avec beaucoup de connexions

**Formule de base :**
```
Buffer optimal ≈ Bande passante × RTT (Round-Trip Time)
```

**Exemple :**
- Bande passante : 100 Mbps = 12.5 MB/s
- RTT : 50 ms = 0.05 s
- Buffer optimal : 12.5 × 0.05 = **625 KB**

**Vérifier la valeur réelle :**
```c
int rcvbuf;  
socklen_t optlen = sizeof(rcvbuf);  
getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, &optlen);  
printf("Buffer de réception : %d octets\n", rcvbuf);  
```

⚠️ **Note :** Le kernel peut doubler la valeur demandée pour ses métadonnées.

---

### 5. `SO_RCVTIMEO` et `SO_SNDTIMEO` - Timeouts

#### Définir un timeout pour recv() et send()

```c
struct timeval timeout;  
timeout.tv_sec = 5;   // 5 secondes  
timeout.tv_usec = 0;  // 0 microsecondes  

// Timeout pour recv()
if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
    perror("setsockopt SO_RCVTIMEO");
}

// Timeout pour send()
if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
    perror("setsockopt SO_SNDTIMEO");
}
```

**Comportement :**
- Si l'opération ne se termine pas dans le délai, elle retourne `-1`
- `errno` est positionné à `EAGAIN` ou `EWOULDBLOCK`

**Exemple d'utilisation :**
```c
// Définir timeout de 10 secondes
struct timeval tv = {.tv_sec = 10, .tv_usec = 0};  
setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));  

// recv() timeout après 10 secondes
ssize_t n = recv(sockfd, buffer, sizeof(buffer), 0);  
if (n < 0) {  
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        fprintf(stderr, "Timeout : aucune donnée reçue\n");
    } else {
        perror("recv");
    }
}
```

**Quand l'utiliser :**
- ✅ Client qui attend une réponse du serveur
- ✅ Éviter les blocages infinis
- ✅ Applications interactives nécessitant de la réactivité

**Valeur par défaut :** 0 (pas de timeout, blocage infini)

---

### 6. `SO_LINGER` - Contrôler le comportement de fermeture

#### Problème

Par défaut, `close()` retourne immédiatement, même si des données sont encore en attente d'envoi.

#### Solution

```c
struct linger ling;  
ling.l_onoff = 1;   // Activer linger  
ling.l_linger = 10; // Attendre max 10 secondes  

if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling)) < 0) {
    perror("setsockopt SO_LINGER");
}
```

**Trois modes de fonctionnement :**

#### Mode 1 : Linger désactivé (défaut)

```c
struct linger ling = {.l_onoff = 0};
```

- `close()` retourne immédiatement
- Les données en attente sont envoyées en arrière-plan
- Fermeture gracieuse (envoi de FIN)

#### Mode 2 : Linger avec timeout

```c
struct linger ling = {.l_onoff = 1, .l_linger = 10};
```

- `close()` bloque jusqu'à :
  - Toutes les données sont envoyées et acquittées
  - OU le timeout expire (10 secondes ici)
- Fermeture gracieuse si succès

#### Mode 3 : Linger avec timeout de 0 (RST)

```c
struct linger ling = {.l_onoff = 1, .l_linger = 0};
```

- `close()` abandonne toutes les données en attente
- Envoie un RST (reset) au lieu de FIN
- **Fermeture brutale** : l'autre côté reçoit `ECONNRESET`

**Cas d'usage du mode RST :**
```c
// Utile pour forcer la fermeture immédiate sans TIME_WAIT
struct linger ling = {.l_onoff = 1, .l_linger = 0};  
setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));  
close(sockfd);  // Fermeture brutale, pas de TIME_WAIT  
```

⚠️ **Attention :** Mode RST doit être utilisé avec précaution. Le destinataire peut perdre des données.

---

### 7. `SO_BROADCAST` - Autoriser le broadcast (UDP)

#### Activer l'envoi de broadcast

Par défaut, envoyer à une adresse broadcast (255.255.255.255) est refusé.

```c
int broadcast = 1;  
if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {  
    perror("setsockopt SO_BROADCAST");
}

// Maintenant on peut envoyer en broadcast
struct sockaddr_in broadcast_addr;  
broadcast_addr.sin_family = AF_INET;  
broadcast_addr.sin_port = htons(8080);  
broadcast_addr.sin_addr.s_addr = INADDR_BROADCAST;  // 255.255.255.255  

const char *msg = "Hello everyone!";  
sendto(sockfd, msg, strlen(msg), 0,  
       (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
```

**Cas d'usage :**
- Découverte de services sur le réseau local
- Wake-on-LAN
- Diffusion d'informations à tous les hôtes du réseau

---

### 8. `SO_ERROR` - Récupérer les erreurs asynchrones

#### Utilité

Récupérer les erreurs qui se sont produites sur le socket, particulièrement utile en mode non-bloquant.

```c
int error = 0;  
socklen_t len = sizeof(error);  

if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
    perror("getsockopt SO_ERROR");
} else if (error != 0) {
    fprintf(stderr, "Erreur socket : %s\n", strerror(error));
}
```

**Cas d'usage typique :**
- Vérifier si `connect()` non-bloquant a réussi
- Diagnostiquer des erreurs après `select()`/`poll()`

**Exemple avec connect() non-bloquant :**
```c
// Rendre le socket non-bloquant
int flags = fcntl(sockfd, F_GETFL, 0);  
fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);  

// Tenter connexion (retournera -1 avec errno=EINPROGRESS)
if (connect(sockfd, ...) < 0 && errno != EINPROGRESS) {
    perror("connect");
}

// Attendre que la connexion soit établie
fd_set writefds;  
FD_ZERO(&writefds);  
FD_SET(sockfd, &writefds);  

struct timeval timeout = {.tv_sec = 5, .tv_usec = 0};  
if (select(sockfd + 1, NULL, &writefds, NULL, &timeout) > 0) {  
    // Vérifier si connexion réussie
    int error;
    socklen_t len = sizeof(error);
    getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);

    if (error == 0) {
        printf("Connexion établie\n");
    } else {
        fprintf(stderr, "Connexion échouée : %s\n", strerror(error));
    }
}
```

---

## Options Spécifiques à TCP (IPPROTO_TCP)

### 1. `TCP_NODELAY` - Désactiver l'algorithme de Nagle

#### L'algorithme de Nagle (activé par défaut)

L'algorithme de Nagle **regroupe** les petits paquets pour réduire l'overhead réseau.

**Fonctionnement :**
- Si vous envoyez 10 octets, puis 20 octets, puis 15 octets rapidement
- TCP peut attendre un peu et envoyer un seul paquet de 45 octets
- **Avantage :** Moins de paquets réseau
- **Inconvénient :** Latence accrue

#### Désactiver Nagle

```c
int flag = 1;  
if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag)) < 0) {  
    perror("setsockopt TCP_NODELAY");
}
```

**Effet :** Chaque `send()` génère un paquet TCP immédiat.

**Quand désactiver Nagle (TCP_NODELAY = 1) :**
- ✅ Applications interactives (SSH, jeux en ligne)
- ✅ Protocoles requête/réponse avec petits messages
- ✅ Quand la latence est plus importante que la bande passante

**Quand garder Nagle activé (TCP_NODELAY = 0, défaut) :**
- ✅ Transfert de gros fichiers
- ✅ Streaming de données continues
- ✅ Quand la bande passante est plus importante que la latence

**Exemple : Serveur de jeu**
```c
int client_fd = accept(server_fd, ...);

// Désactiver Nagle pour réactivité maximale
int nodelay = 1;  
setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));  
```

---

### 2. `TCP_CORK` - Regrouper les données (Linux)

#### Opposé de TCP_NODELAY

```c
int cork = 1;  
setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, &cork, sizeof(cork));  
```

**Effet :** Accumule les données jusqu'à ce que :
- Le buffer soit plein
- Ou `TCP_CORK` soit désactivé

**Cas d'usage :** Envoyer plusieurs petits morceaux qui forment logiquement un tout.

**Exemple : Serveur HTTP**
```c
// Activer cork
int cork = 1;  
setsockopt(client_fd, IPPROTO_TCP, TCP_CORK, &cork, sizeof(cork));  

// Envoyer les en-têtes HTTP
send(client_fd, "HTTP/1.1 200 OK\r\n", 17, 0);  
send(client_fd, "Content-Type: text/html\r\n", 25, 0);  
send(client_fd, "Content-Length: 1234\r\n\r\n", 24, 0);  

// Envoyer le corps
send(client_fd, body, body_len, 0);

// Désactiver cork pour forcer l'envoi
cork = 0;  
setsockopt(client_fd, IPPROTO_TCP, TCP_CORK, &cork, sizeof(cork));  
```

**Résultat :** Tout est envoyé dans 1 ou 2 paquets au lieu de 4.

⚠️ **Note :** `TCP_NODELAY` et `TCP_CORK` sont mutuellement exclusifs.

---

### 3. `TCP_QUICKACK` - Contrôler les ACKs immédiats (Linux)

#### Contrôler le delayed ACK

Par défaut, TCP attend un peu avant d'envoyer un ACK, au cas où il y aurait des données à renvoyer dans le même paquet.

```c
int quickack = 1;  
setsockopt(sockfd, IPPROTO_TCP, TCP_QUICKACK, &quickack, sizeof(quickack));  
```

**Effet :** Force l'envoi immédiat des ACKs.

**Quand utiliser :**
- Mesures de latence précises
- Protocoles sensibles au timing

⚠️ **Important :** Cette option se **réinitialise** après chaque `recv()`. Il faut la réactiver si nécessaire.

---

### 4. `TCP_MAXSEG` - Taille maximale des segments

#### Contrôler le MSS (Maximum Segment Size)

```c
int mss = 1400;  
if (setsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &mss, sizeof(mss)) < 0) {  
    perror("setsockopt TCP_MAXSEG");
}
```

**Utilité :** Limiter la taille des segments TCP pour éviter la fragmentation IP.

**Valeur typique :**
- Ethernet : MSS = 1460 (MTU 1500 - 20 IP - 20 TCP)
- Tunnel VPN : MSS réduit à cause de l'overhead

⚠️ **Note :** Généralement géré automatiquement par le kernel (Path MTU Discovery).

---

## Options Spécifiques à IP (IPPROTO_IP)

### 1. `IP_TTL` - Time To Live

#### Définir le TTL des paquets

Le TTL est décrémenté à chaque routeur. Quand il atteint 0, le paquet est détruit.

```c
int ttl = 64;  
if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {  
    perror("setsockopt IP_TTL");
}
```

**Valeur par défaut :** 64 (Linux)

**Cas d'usage :**
- Limiter la portée géographique des paquets
- Traceroute utilise des TTL croissants

---

### 2. `IP_TOS` - Type of Service

#### Définir la priorité des paquets

```c
int tos = IPTOS_LOWDELAY;  // Priorité : faible latence  
if (setsockopt(sockfd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos)) < 0) {  
    perror("setsockopt IP_TOS");
}
```

**Valeurs possibles :**
- `IPTOS_LOWDELAY` : Faible latence (VoIP, jeux)
- `IPTOS_THROUGHPUT` : Haut débit (transfert fichiers)
- `IPTOS_RELIABILITY` : Fiabilité (email)
- `IPTOS_LOWCOST` : Bas coût (backup)

⚠️ **Note :** Les routeurs peuvent ignorer ou modifier le TOS.

---

### 3. `IP_ADD_MEMBERSHIP` - Rejoindre un groupe multicast

#### S'abonner à un groupe multicast (UDP)

```c
struct ip_mreq mreq;

// Adresse multicast à rejoindre
inet_pton(AF_INET, "239.0.0.1", &mreq.imr_multiaddr);

// Interface réseau (ou INADDR_ANY)
mreq.imr_interface.s_addr = INADDR_ANY;

if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
    perror("setsockopt IP_ADD_MEMBERSHIP");
}

// Maintenant on reçoit les datagrammes multicast
```

**Quitter le groupe :**
```c
setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
```

---

## Tableau Récapitulatif des Options Courantes

| Option | Niveau | Type | Usage typique |
|--------|--------|------|---------------|
| `SO_REUSEADDR` | `SOL_SOCKET` | int | Redémarrer serveur immédiatement |
| `SO_REUSEPORT` | `SOL_SOCKET` | int | Plusieurs serveurs même port |
| `SO_KEEPALIVE` | `SOL_SOCKET` | int | Détecter connexions mortes |
| `SO_RCVBUF` | `SOL_SOCKET` | int | Taille buffer réception |
| `SO_SNDBUF` | `SOL_SOCKET` | int | Taille buffer envoi |
| `SO_RCVTIMEO` | `SOL_SOCKET` | struct timeval | Timeout recv() |
| `SO_SNDTIMEO` | `SOL_SOCKET` | struct timeval | Timeout send() |
| `SO_LINGER` | `SOL_SOCKET` | struct linger | Comportement fermeture |
| `SO_BROADCAST` | `SOL_SOCKET` | int | Autoriser broadcast UDP |
| `SO_ERROR` | `SOL_SOCKET` | int | Lire erreurs asynchrones |
| `TCP_NODELAY` | `IPPROTO_TCP` | int | Désactiver Nagle |
| `TCP_CORK` | `IPPROTO_TCP` | int | Regrouper données |
| `TCP_KEEPIDLE` | `IPPROTO_TCP` | int | Délai avant keepalive |
| `TCP_KEEPINTVL` | `IPPROTO_TCP` | int | Intervalle keepalive |
| `TCP_KEEPCNT` | `IPPROTO_TCP` | int | Nombre tentatives keepalive |
| `IP_TTL` | `IPPROTO_IP` | int | Time To Live |
| `IP_TOS` | `IPPROTO_IP` | int | Type of Service |
| `IP_ADD_MEMBERSHIP` | `IPPROTO_IP` | struct ip_mreq | Rejoindre multicast |

---

## Exemple Complet : Serveur Optimisé

Voici un serveur TCP avec options optimisées.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#define PORT 8080

int create_optimized_server() {
    int server_fd;
    struct sockaddr_in addr;

    // 1. Créer socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return -1;
    }

    // 2. SO_REUSEADDR : Redémarrage rapide
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                   &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt SO_REUSEADDR");
    }

    // 3. SO_REUSEPORT : Permettre plusieurs instances (Linux 3.9+)
    #ifdef SO_REUSEPORT
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT,
                   &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt SO_REUSEPORT");
    }
    #endif

    // 4. SO_KEEPALIVE : Détecter connexions mortes
    int keepalive = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_KEEPALIVE,
                   &keepalive, sizeof(keepalive)) < 0) {
        perror("setsockopt SO_KEEPALIVE");
    }

    // 5. TCP_KEEPIDLE : Attendre 60s avant premier probe
    int keepidle = 60;
    if (setsockopt(server_fd, IPPROTO_TCP, TCP_KEEPIDLE,
                   &keepidle, sizeof(keepidle)) < 0) {
        perror("setsockopt TCP_KEEPIDLE");
    }

    // 6. TCP_KEEPINTVL : 10s entre probes
    int keepintvl = 10;
    if (setsockopt(server_fd, IPPROTO_TCP, TCP_KEEPINTVL,
                   &keepintvl, sizeof(keepintvl)) < 0) {
        perror("setsockopt TCP_KEEPINTVL");
    }

    // 7. TCP_KEEPCNT : 3 probes avant déclaration morte
    int keepcnt = 3;
    if (setsockopt(server_fd, IPPROTO_TCP, TCP_KEEPCNT,
                   &keepcnt, sizeof(keepcnt)) < 0) {
        perror("setsockopt TCP_KEEPCNT");
    }

    // 8. SO_RCVBUF : Buffer réception 256 KB
    int rcvbuf = 256 * 1024;
    if (setsockopt(server_fd, SOL_SOCKET, SO_RCVBUF,
                   &rcvbuf, sizeof(rcvbuf)) < 0) {
        perror("setsockopt SO_RCVBUF");
    }

    // 9. SO_SNDBUF : Buffer envoi 256 KB
    int sndbuf = 256 * 1024;
    if (setsockopt(server_fd, SOL_SOCKET, SO_SNDBUF,
                   &sndbuf, sizeof(sndbuf)) < 0) {
        perror("setsockopt SO_SNDBUF");
    }

    // 10. Bind
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        return -1;
    }

    // 11. Listen
    if (listen(server_fd, 128) < 0) {
        perror("listen");
        close(server_fd);
        return -1;
    }

    printf("Serveur optimisé en écoute sur le port %d\n", PORT);
    return server_fd;
}

void handle_client(int client_fd) {
    // Désactiver Nagle pour latence minimale
    int nodelay = 1;
    setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));

    // Définir timeout de réception
    struct timeval tv = {.tv_sec = 30, .tv_usec = 0};
    setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // Communication avec le client...
    char buffer[1024];
    ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);
    // ...

    close(client_fd);
}

int main() {
    int server_fd = create_optimized_server();
    if (server_fd < 0) {
        exit(EXIT_FAILURE);
    }

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        handle_client(client_fd);
    }

    close(server_fd);
    return 0;
}
```

---

## Diagnostic : Afficher les Options Actuelles

Fonction utilitaire pour afficher les options d'un socket :

```c
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

void print_socket_options(int sockfd) {
    int value;
    socklen_t len = sizeof(value);

    printf("=== Options du socket %d ===\n", sockfd);

    // SO_REUSEADDR
    if (getsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &value, &len) == 0) {
        printf("SO_REUSEADDR: %s\n", value ? "ON" : "OFF");
    }

    // SO_KEEPALIVE
    if (getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &value, &len) == 0) {
        printf("SO_KEEPALIVE: %s\n", value ? "ON" : "OFF");
    }

    // SO_RCVBUF
    if (getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &value, &len) == 0) {
        printf("SO_RCVBUF: %d octets\n", value);
    }

    // SO_SNDBUF
    if (getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &value, &len) == 0) {
        printf("SO_SNDBUF: %d octets\n", value);
    }

    // TCP_NODELAY
    if (getsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &value, &len) == 0) {
        printf("TCP_NODELAY: %s\n", value ? "ON" : "OFF");
    }

    // TCP_KEEPIDLE
    if (getsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &value, &len) == 0) {
        printf("TCP_KEEPIDLE: %d secondes\n", value);
    }

    // TCP_KEEPINTVL
    if (getsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &value, &len) == 0) {
        printf("TCP_KEEPINTVL: %d secondes\n", value);
    }

    // TCP_KEEPCNT
    if (getsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &value, &len) == 0) {
        printf("TCP_KEEPCNT: %d\n", value);
    }

    printf("===========================\n");
}
```

**Utilisation :**
```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0);  
print_socket_options(sockfd);  
```

---

## Bonnes Pratiques

### 1. Toujours vérifier les retours

```c
// ❌ Mauvais
setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

// ✅ Bon
if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt SO_REUSEADDR");
    // Décider si c'est fatal ou non
}
```

---

### 2. Définir les options AVANT bind() / connect()

Certaines options doivent être définies avant d'utiliser le socket :

```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0);

// ✅ Définir SO_REUSEADDR AVANT bind()
int opt = 1;  
setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));  

bind(sockfd, ...);  // Maintenant bind() peut réutiliser l'adresse
```

---

### 3. Documenter les choix d'options

```c
// Désactiver Nagle pour réduire la latence dans notre protocole
// requête/réponse interactif. Les messages sont petits (<100 octets)
// et nécessitent une réponse immédiate.
int nodelay = 1;  
setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));  
```

---

### 4. Tester les limites système

```bash
# Vérifier les limites de buffers
sysctl net.core.rmem_max  
sysctl net.core.wmem_max  

# Augmenter si nécessaire (root requis)
sudo sysctl -w net.core.rmem_max=16777216  
sudo sysctl -w net.core.wmem_max=16777216  
```

---

### 5. Adapter aux besoins spécifiques

| Type d'application | Options recommandées |
|--------------------|----------------------|
| **Serveur web** | `SO_REUSEADDR`, `TCP_CORK`, buffers élevés |
| **Jeu en ligne** | `TCP_NODELAY`, timeouts courts |
| **Chat/Websocket** | `SO_KEEPALIVE`, `TCP_NODELAY` |
| **Transfert fichiers** | Buffers très élevés, Nagle activé |
| **API REST** | `SO_REUSEADDR`, timeouts configurables |

---

## Pièges Courants

### 1. SO_REUSEADDR sans fermeture propre

Si vous utilisez `SO_REUSEADDR` et que votre programme crash sans `close()`, le port peut rester bloqué.

**Solution :** Toujours fermer proprement avec un gestionnaire de signal.

---

### 2. TCP_NODELAY et TCP_CORK ensemble

Ces options sont **incompatibles**. L'une désactive Nagle, l'autre le renforce.

**Solution :** Choisir l'une ou l'autre selon le besoin.

---

### 3. Augmenter les buffers sans besoin

Des buffers trop grands consomment de la mémoire inutilement.

**Solution :** Mesurer les performances avant d'augmenter.

---

### 4. Oublier les limites système

Demander un buffer de 10 MB alors que le système limite à 4 MB ne sert à rien.

**Solution :** Vérifier `sysctl net.core.rmem_max` et `net.core.wmem_max`.

---

## En Résumé

Les options de sockets permettent d'affiner le comportement réseau de votre application. Bien les comprendre et les utiliser améliore significativement la robustesse et les performances.

**Options essentielles à connaître :**

✅ **`SO_REUSEADDR`** : Toujours pour les serveurs (redémarrage rapide)

✅ **`SO_KEEPALIVE`** : Détecter les connexions mortes (longue durée)

✅ **`SO_RCVTIMEO` / `SO_SNDTIMEO`** : Éviter les blocages infinis

✅ **`TCP_NODELAY`** : Applications interactives (désactiver Nagle)

✅ **`SO_RCVBUF` / `SO_SNDBUF`** : Optimiser pour haute bande passante

✅ **`SO_LINGER`** : Contrôler le comportement de fermeture

**Règles d'or :**
1. Toujours vérifier les retours de `setsockopt()`
2. Définir les options avant `bind()` / `connect()`
3. Documenter pourquoi vous utilisez une option
4. Tester les performances avant et après
5. Adapter les options au type d'application

Dans la prochaine section, nous verrons comment résoudre les noms de domaine et travailler avec DNS en C.

---

**→ Prochaine section : 20.7 Résolution de Noms**

⏭️ [Résolution de noms](/20-reseau-sockets/07-resolution-noms.md)
