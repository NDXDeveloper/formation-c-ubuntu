🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 20.4 Sockets UDP

## Introduction

Nous avons vu les sockets TCP, qui offrent une communication fiable et ordonnée. Maintenant, explorons **UDP** (User Datagram Protocol), un protocole plus léger et rapide, mais sans garantie de livraison.

UDP est le choix idéal pour les applications en temps réel où la vitesse prime sur la fiabilité : streaming vidéo, jeux en ligne, VoIP, DNS, etc.

Dans cette section, nous allons apprendre à utiliser les **sockets UDP** en C et comprendre quand choisir UDP plutôt que TCP.

---

## Rappel : TCP vs UDP

Avant de plonger dans le code, rappelons les différences fondamentales :

| Caractéristique | TCP | UDP |
|-----------------|-----|-----|
| **Type de connexion** | Orienté connexion | Sans connexion |
| **Fiabilité** | Garanti (accusés de réception) | Non garanti (best-effort) |
| **Ordre des données** | Préservé | Peut arriver dans le désordre |
| **Vitesse** | Plus lent (overhead) | Rapide (minimal overhead) |
| **En-tête** | 20+ octets | 8 octets |
| **Contrôle de flux** | Oui | Non |
| **Contrôle de congestion** | Oui | Non |
| **Unité de données** | Flux d'octets continu | Datagrammes indépendants |
| **Use cases** | Web, email, transfert fichiers | Streaming, jeux, VoIP, DNS |

**Analogie :**
- **TCP** : Appel téléphonique (connexion établie, conversation bidirectionnelle fiable)
- **UDP** : Carte postale (envoyé directement, peut se perdre, pas de confirmation)

---

## Caractéristiques d'UDP

### 1. Sans connexion (Connectionless)

Avec UDP, il n'y a **pas de handshake**, pas d'établissement de connexion. Vous envoyez simplement vos données à une adresse IP et un port.

**Conséquence :** Pas de distinction entre "serveur" et "client" au niveau du protocole. Les deux utilisent les mêmes fonctions.

### 2. Messages discrets (Datagrammes)

UDP manipule des **datagrammes** : des messages complets et indépendants.

**Différence avec TCP :**
- **TCP :** Flux continu d'octets. Si vous envoyez "Hello" puis "World", le destinataire peut recevoir "HelloWorld" en un seul `recv()`.
- **UDP :** Chaque message est distinct. Si vous envoyez deux datagrammes "Hello" et "World", ils arrivent séparément (ou pas du tout).

### 3. Pas de garantie de livraison

UDP fait de son **mieux** (best-effort), mais ne garantit rien :
- Les datagrammes peuvent être **perdus**
- Ils peuvent être **dupliqués**
- Ils peuvent arriver **dans le désordre**

⚠️ **Important :** C'est à l'application de gérer ces problèmes si nécessaire.

### 4. Pas de contrôle de flux ni de congestion

UDP envoie les données aussi vite que possible, sans tenir compte de la capacité du réseau ou du destinataire.

**Risque :** Peut saturer le réseau ou le destinataire.

### 5. Léger et rapide

- **En-tête minimal** : 8 octets seulement
- **Pas de retransmission** : Latence plus faible
- **Pas d'état de connexion** : Consomme moins de ressources

---

## Quand utiliser UDP ?

### Cas d'usage idéaux pour UDP

✅ **Applications en temps réel**
- Streaming audio/vidéo (Netflix, YouTube)
- Visioconférence (Zoom, Skype)
- VoIP (appels téléphoniques Internet)

✅ **Jeux en ligne multijoueurs**
- Position des joueurs
- Actions en temps réel (tirs, mouvements)

✅ **Requêtes/Réponses simples**
- DNS (résolution de noms de domaine)
- DHCP (attribution d'adresses IP)
- NTP (synchronisation horaire)

✅ **Broadcasting et multicasting**
- Diffusion de données à plusieurs destinataires simultanément

✅ **Applications tolérantes à la perte**
- Métriques de monitoring (quelques pertes acceptables)
- Logs non critiques

### Quand NE PAS utiliser UDP

❌ **Transfert de fichiers** : La perte de données corromprait le fichier

❌ **Transactions financières** : La fiabilité est critique

❌ **Emails** : Le message doit arriver complet

❌ **Pages web** : Chaque octet est important

❌ **Bases de données** : L'intégrité des données est essentielle

---

## Les Fonctions UDP : Différences avec TCP

### Pas de `connect()`, `listen()`, ou `accept()`

Avec UDP, vous n'établissez pas de connexion. Les fonctions principales sont :

| TCP | UDP |
|-----|-----|
| `socket()` | `socket()` (avec `SOCK_DGRAM`) |
| `bind()` | `bind()` (optionnel pour client) |
| `listen()` | ❌ (n'existe pas en UDP) |
| `accept()` | ❌ (n'existe pas en UDP) |
| `connect()` | ❌ (optionnel, voir plus loin) |
| `send()` / `recv()` | `sendto()` / `recvfrom()` |
| `close()` | `close()` |

### Nouvelles fonctions : `sendto()` et `recvfrom()`

Ces fonctions remplacent `send()` et `recv()`, car il faut spécifier l'adresse du destinataire à chaque envoi.

---

## API UDP : Les Fonctions en Détail

### 1. Créer un socket UDP

```c
int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
```

**Différence avec TCP :** `SOCK_DGRAM` au lieu de `SOCK_STREAM`.

**Exemple complet :**
```c
#include <sys/socket.h>
#include <arpa/inet.h>

int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
if (sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
}
```

---

### 2. `bind()` - Lier le socket (serveur)

Pour un **serveur UDP**, vous devez lier le socket à une adresse et un port, exactement comme en TCP.

```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

**Exemple :**
```c
struct sockaddr_in server_addr;
memset(&server_addr, 0, sizeof(server_addr));

server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr = INADDR_ANY;  // Toutes les interfaces
server_addr.sin_port = htons(8080);

if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("bind");
    close(sockfd);
    exit(EXIT_FAILURE);
}
```

**Différence avec TCP :** Pas besoin d'appeler `listen()` après.

**Client UDP :** Le client peut utiliser `bind()` pour choisir un port spécifique, mais ce n'est **généralement pas nécessaire**. Le système attribue automatiquement un port éphémère lors du premier `sendto()`.

---

### 3. `sendto()` - Envoyer un datagramme

```c
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);
```

**Paramètres :**
- `sockfd` : Descripteur du socket
- `buf` : Pointeur vers les données à envoyer
- `len` : Nombre d'octets à envoyer
- `flags` : Options (généralement `0`)
- `dest_addr` : Adresse du destinataire (IP + port)
- `addrlen` : Taille de la structure d'adresse

**Retour :**
- Nombre d'octets envoyés
- `-1` en cas d'erreur

**Exemple :**
```c
struct sockaddr_in dest_addr;
memset(&dest_addr, 0, sizeof(dest_addr));

dest_addr.sin_family = AF_INET;
dest_addr.sin_port = htons(8080);
inet_pton(AF_INET, "192.168.1.10", &dest_addr.sin_addr);

const char *message = "Hello, UDP!";
ssize_t bytes_sent = sendto(sockfd, message, strlen(message), 0,
                             (struct sockaddr*)&dest_addr, sizeof(dest_addr));

if (bytes_sent < 0) {
    perror("sendto");
}
```

**Points importants :**

1. **Pas de connexion préalable** : Vous spécifiez la destination à chaque envoi

2. **Envoi complet ou rien** : Contrairement à TCP, `sendto()` envoie généralement le datagramme complet ou échoue. Pas d'envois partiels (sauf datagramme trop grand).

3. **Taille maximale** : Limitée par le MTU du réseau (généralement ~1472 octets pour éviter la fragmentation IP avec Ethernet MTU de 1500)

---

### 4. `recvfrom()` - Recevoir un datagramme

```c
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);
```

**Paramètres :**
- `sockfd` : Descripteur du socket
- `buf` : Buffer pour stocker les données reçues
- `len` : Taille maximale du buffer
- `flags` : Options (généralement `0`)
- `src_addr` : Structure qui recevra l'adresse de l'expéditeur (peut être `NULL`)
- `addrlen` : Pointeur vers la taille de la structure (modifié par `recvfrom()`)

**Retour :**
- Nombre d'octets reçus
- `0` : Datagramme vide (rare mais possible)
- `-1` : Erreur

**Exemple :**
```c
char buffer[1024];
struct sockaddr_in client_addr;
socklen_t client_len = sizeof(client_addr);

ssize_t bytes_received = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                                   (struct sockaddr*)&client_addr, &client_len);

if (bytes_received < 0) {
    perror("recvfrom");
} else {
    buffer[bytes_received] = '\0';

    // Afficher l'expéditeur
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    printf("Reçu de %s:%d : %s\n", client_ip, ntohs(client_addr.sin_port), buffer);
}
```

**Points importants :**

1. **Fonction bloquante** : `recvfrom()` attend qu'un datagramme arrive

2. **Un datagramme complet** : Un appel à `recvfrom()` lit **un datagramme entier**, pas juste une partie

3. **Datagramme tronqué** : Si le datagramme est plus grand que le buffer, il est **tronqué** (les octets en trop sont perdus)

4. **Informations sur l'expéditeur** : Vous obtenez automatiquement l'IP et le port de l'expéditeur, pratique pour répondre

---

### 5. `close()` - Fermer le socket

Exactement comme pour TCP :

```c
close(sockfd);
```

---

## Exemple Complet : Serveur Écho UDP

Voici un serveur UDP simple qui renvoie en écho tout ce qu'il reçoit.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // 1. Créer le socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 2. Configurer l'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Toutes les interfaces
    server_addr.sin_port = htons(PORT);

    // 3. Lier le socket à l'adresse
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Serveur UDP en écoute sur le port %d...\n", PORT);

    // 4. Boucle principale : recevoir et répondre
    while (1) {
        client_len = sizeof(client_addr);

        // Recevoir un datagramme
        bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                                  (struct sockaddr*)&client_addr, &client_len);

        if (bytes_received < 0) {
            perror("recvfrom");
            continue;
        }

        buffer[bytes_received] = '\0';

        // Afficher les informations
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Reçu de %s:%d (%zd octets) : %s",
               client_ip, ntohs(client_addr.sin_port), bytes_received, buffer);

        // Renvoyer en écho au client
        ssize_t bytes_sent = sendto(sockfd, buffer, bytes_received, 0,
                                     (struct sockaddr*)&client_addr, client_len);

        if (bytes_sent < 0) {
            perror("sendto");
        } else {
            printf("Écho envoyé (%zd octets)\n", bytes_sent);
        }
    }

    // Ce code n'est jamais atteint (boucle infinie)
    close(sockfd);
    return 0;
}
```

**Compilation :**
```bash
gcc -o udp_server udp_server.c -Wall -Wextra
```

**Exécution :**
```bash
./udp_server
```

**Observations :**

1. **Pas de `listen()` ni `accept()`** : Le serveur reçoit directement les datagrammes

2. **Gère plusieurs clients naturellement** : Chaque `recvfrom()` peut recevoir un datagramme d'un client différent

3. **Sans état** : Le serveur ne maintient pas de connexion active

---

## Exemple Complet : Client UDP

Voici le client correspondant qui envoie des messages au serveur.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t server_len;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // 1. Créer le socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 2. Configurer l'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Client UDP prêt (serveur : 127.0.0.1:%d)\n", PORT);
    printf("Tapez un message (ou 'quit' pour quitter) :\n");

    // 3. Boucle d'interaction
    while (1) {
        printf("> ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        // Vérifier si l'utilisateur veut quitter
        if (strcmp(buffer, "quit\n") == 0) {
            break;
        }

        // Envoyer le message au serveur
        ssize_t bytes_sent = sendto(sockfd, buffer, strlen(buffer), 0,
                                     (struct sockaddr*)&server_addr, sizeof(server_addr));

        if (bytes_sent < 0) {
            perror("sendto");
            continue;
        }

        printf("Envoyé : %zd octets\n", bytes_sent);

        // Attendre la réponse (écho)
        server_len = sizeof(server_addr);
        bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                                  (struct sockaddr*)&server_addr, &server_len);

        if (bytes_received < 0) {
            perror("recvfrom");
            continue;
        }

        buffer[bytes_received] = '\0';
        printf("Écho reçu (%zd octets) : %s", bytes_received, buffer);
    }

    // 4. Fermer le socket
    close(sockfd);
    printf("Déconnecté\n");
    return 0;
}
```

**Compilation :**
```bash
gcc -o udp_client udp_client.c -Wall -Wextra
```

**Exécution :**
```bash
./udp_client
```

**Test :**
1. Lancez le serveur : `./udp_server`
2. Lancez le client : `./udp_client`
3. Tapez des messages dans le client

---

## UDP "Connecté" : Utilisation de `connect()` avec UDP

Bien qu'UDP soit sans connexion, vous pouvez utiliser `connect()` sur un socket UDP pour **associer** le socket à une adresse de destination fixe.

### Avantages de `connect()` avec UDP

✅ **Simplicité** : Vous pouvez utiliser `send()` et `recv()` au lieu de `sendto()` et `recvfrom()`

✅ **Filtrage automatique** : Le socket ne reçoit que les datagrammes provenant de l'adresse connectée

✅ **Erreurs ICMP** : Vous êtes notifié des erreurs (destination unreachable, etc.)

### Exemple d'utilisation

```c
// Créer le socket
int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

// Configurer l'adresse du serveur
struct sockaddr_in server_addr;
memset(&server_addr, 0, sizeof(server_addr));
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(8080);
inet_pton(AF_INET, "192.168.1.10", &server_addr.sin_addr);

// "Connecter" le socket UDP
if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("connect");
    exit(EXIT_FAILURE);
}

// Maintenant on peut utiliser send() et recv() comme avec TCP
send(sockfd, "Hello", 5, 0);
char buffer[1024];
recv(sockfd, buffer, sizeof(buffer), 0);
```

⚠️ **Important :** Ce n'est **pas** une vraie connexion TCP ! Aucun handshake n'a lieu. C'est juste une association locale dans le kernel.

### Quand utiliser `connect()` avec UDP ?

- Client qui communique avec **un seul serveur**
- Applications qui envoient/reçoivent de nombreux datagrammes vers/depuis la même destination
- Quand vous voulez profiter des notifications d'erreurs ICMP

---

## Gestion des Timeouts avec UDP

Comme UDP ne garantit pas la livraison, il est courant de définir un **timeout** pour `recvfrom()`.

### Méthode 1 : `setsockopt()` avec `SO_RCVTIMEO`

```c
struct timeval timeout;
timeout.tv_sec = 5;   // 5 secondes
timeout.tv_usec = 0;

if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
    perror("setsockopt");
}

// Maintenant recvfrom() timeout après 5 secondes
ssize_t bytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
if (bytes < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        printf("Timeout : aucune réponse reçue\n");
    } else {
        perror("recvfrom");
    }
}
```

### Méthode 2 : `select()` ou `poll()`

```c
fd_set readfds;
struct timeval timeout;

FD_ZERO(&readfds);
FD_SET(sockfd, &readfds);

timeout.tv_sec = 5;
timeout.tv_usec = 0;

int ready = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
if (ready < 0) {
    perror("select");
} else if (ready == 0) {
    printf("Timeout : aucune donnée disponible\n");
} else {
    // Des données sont disponibles, on peut faire recvfrom() sans bloquer
    recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
}
```

---

## Exemple : Client UDP avec Retry

Voici un exemple de client UDP qui réessaie en cas de timeout.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define TIMEOUT_SEC 2
#define MAX_RETRIES 3

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int retry_count;

    // Créer le socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Définir un timeout
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    // Configurer l'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    const char *message = "Hello, UDP with retry!";

    // Boucle de retry
    for (retry_count = 0; retry_count < MAX_RETRIES; retry_count++) {
        printf("Tentative %d/%d...\n", retry_count + 1, MAX_RETRIES);

        // Envoyer le message
        if (sendto(sockfd, message, strlen(message), 0,
                   (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("sendto");
            continue;
        }

        // Attendre la réponse
        ssize_t bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, NULL, NULL);

        if (bytes_received < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("Timeout : pas de réponse\n");
                continue;  // Réessayer
            } else {
                perror("recvfrom");
                break;
            }
        }

        // Réponse reçue avec succès
        buffer[bytes_received] = '\0';
        printf("Réponse reçue : %s\n", buffer);
        break;  // Succès, sortir de la boucle
    }

    if (retry_count == MAX_RETRIES) {
        printf("Échec après %d tentatives\n", MAX_RETRIES);
    }

    close(sockfd);
    return 0;
}
```

---

## Broadcasting avec UDP

L'**UDP broadcasting** permet d'envoyer un datagramme à **tous les hôtes** d'un réseau local.

### Activation du broadcast

```c
int broadcast_enable = 1;
if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST,
               &broadcast_enable, sizeof(broadcast_enable)) < 0) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
}
```

### Envoi d'un broadcast

```c
struct sockaddr_in broadcast_addr;
memset(&broadcast_addr, 0, sizeof(broadcast_addr));

broadcast_addr.sin_family = AF_INET;
broadcast_addr.sin_port = htons(8080);
broadcast_addr.sin_addr.s_addr = INADDR_BROADCAST;  // 255.255.255.255

const char *message = "Hello, everyone!";
sendto(sockfd, message, strlen(message), 0,
       (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
```

### Cas d'usage du broadcast

- **Découverte de services** : Trouver des serveurs sur le réseau local
- **Wake-on-LAN** : Réveiller des machines à distance
- **DHCP** : Le client broadcast pour trouver un serveur DHCP

⚠️ **Note :** Le broadcast est limité au réseau local (pas routé sur Internet).

---

## Multicasting avec UDP

Le **multicasting** permet d'envoyer des datagrammes à un **groupe spécifique** d'hôtes.

### Principe

- Les hôtes intéressés s'abonnent à une **adresse multicast** (plage 224.0.0.0 à 239.255.255.255)
- L'émetteur envoie à cette adresse
- Seuls les abonnés reçoivent les données

### Exemple d'émetteur multicast

```c
struct sockaddr_in multicast_addr;
memset(&multicast_addr, 0, sizeof(multicast_addr));

multicast_addr.sin_family = AF_INET;
multicast_addr.sin_port = htons(8080);
inet_pton(AF_INET, "239.0.0.1", &multicast_addr.sin_addr);  // Adresse multicast

const char *message = "Multicast message";
sendto(sockfd, message, strlen(message), 0,
       (struct sockaddr*)&multicast_addr, sizeof(multicast_addr));
```

### Exemple de récepteur multicast

```c
// Joindre le groupe multicast
struct ip_mreq mreq;
inet_pton(AF_INET, "239.0.0.1", &mreq.imr_multiaddr);
mreq.imr_interface.s_addr = INADDR_ANY;

if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
}

// Maintenant, on peut recevoir les datagrammes multicast
recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
```

### Cas d'usage du multicast

- **Streaming vidéo** : Diffuser à plusieurs clients simultanément
- **Jeux en réseau local** : Synchronisation d'état
- **Protocoles de routage** : OSPF, RIP

---

## Taille Maximale des Datagrammes UDP

### Limites théoriques

- **IPv4** : Maximum de 65535 octets (en-tête IP + en-tête UDP + données)
- **En-tête UDP** : 8 octets
- **En-tête IP** : 20 octets (minimum)
- **Données UDP max** : 65535 - 8 - 20 = **65507 octets**

### Limites pratiques

En pratique, il faut tenir compte du **MTU** (Maximum Transmission Unit) du réseau :

- **Ethernet** : MTU = 1500 octets
- **Données UDP max sans fragmentation** : 1500 - 20 (IP) - 8 (UDP) = **1472 octets**

**Recommandation :** Limitez vos datagrammes à ~1400 octets pour éviter la fragmentation IP.

### Pourquoi éviter la fragmentation ?

- **Moins fiable** : Si un fragment est perdu, le datagramme entier est perdu
- **Performance dégradée** : Réassemblage coûteux
- **Firewalls** : Certains bloquent les fragments

### Exemple de vérification de taille

```c
#define MAX_UDP_PAYLOAD 1400

if (data_len > MAX_UDP_PAYLOAD) {
    fprintf(stderr, "Erreur : datagramme trop grand (%zu octets)\n", data_len);
    return -1;
}
```

---

## Différences de Performance : TCP vs UDP

### Test de latence (ping-pong)

**TCP** :
- Handshake : ~1 RTT (Round-Trip Time)
- Envoi message : 1 RTT
- **Total** : ~2 RTT

**UDP** :
- Envoi message : 1 RTT
- **Total** : ~1 RTT

**Gain** : UDP est ~2x plus rapide pour une requête/réponse simple

### Test de débit (streaming)

Pour un flux de données continu :
- **TCP** : Contrôle de congestion peut limiter le débit
- **UDP** : Débit maximal limité seulement par la bande passante

**Mais** : UDP peut saturer le réseau et causer des pertes

### Overhead des en-têtes

| Protocole | Taille en-tête |
|-----------|----------------|
| **UDP** | 8 octets |
| **TCP** | 20-60 octets |

Pour de petits messages, l'économie est significative.

---

## Fiabilité sur UDP : Protocoles de Niveau Supérieur

Si vous avez besoin de fiabilité avec UDP, vous devez l'implémenter au niveau application.

### Techniques courantes

1. **Numéros de séquence**
   - Numéroter chaque datagramme
   - Détecter les pertes et les doublons

2. **Accusés de réception (ACK)**
   - Le destinataire confirme la réception
   - L'émetteur retransmet si pas d'ACK

3. **Timeouts et retransmissions**
   - Réessayer après un délai

4. **Checksum applicatif**
   - Vérifier l'intégrité des données

### Protocoles construits sur UDP

Certains protocoles ajoutent de la fiabilité au-dessus d'UDP :

- **QUIC** (Quick UDP Internet Connections) : Utilisé par HTTP/3
- **DTLS** (Datagram Transport Layer Security) : TLS sur UDP
- **RTP** (Real-time Transport Protocol) : Streaming avec séquençage
- **TFTP** (Trivial File Transfer Protocol) : Transfert de fichiers simple avec ACK

---

## Debugging UDP

### 1. Vérifier que le serveur écoute

```bash
ss -ulpn | grep 8080
# ou
netstat -ulpn | grep 8080
```

**Sortie attendue :**
```
UNCONN  0  0  0.0.0.0:8080  0.0.0.0:*  users:(("udp_server",pid=1234,fd=3))
```

### 2. Capturer les paquets UDP avec `tcpdump`

```bash
sudo tcpdump -i lo -n udp port 8080
```

**Sortie :**
```
17:30:15.123456 IP 127.0.0.1.54321 > 127.0.0.1.8080: UDP, length 11
17:30:15.123789 IP 127.0.0.1.8080 > 127.0.0.1.54321: UDP, length 11
```

### 3. Tester avec `netcat` (nc)

Envoyer un datagramme UDP :
```bash
echo "Hello" | nc -u 127.0.0.1 8080
```

### 4. Vérifier les firewall rules

```bash
sudo iptables -L -n | grep 8080
```

---

## Erreurs Courantes avec UDP

### 1. Oublier `bind()` côté serveur

**Symptôme :** Le serveur ne reçoit jamais de datagrammes

**Solution :** Toujours appeler `bind()` pour un serveur UDP

### 2. Buffer trop petit

**Symptôme :** Datagrammes tronqués

```c
char buffer[10];  // ❌ Trop petit
recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
```

**Solution :** Utiliser un buffer suffisamment grand (au moins 1500 octets)

### 3. Ne pas gérer les pertes

**Symptôme :** L'application attend indéfiniment une réponse qui ne viendra jamais

**Solution :** Implémenter des timeouts et des retries

### 4. Envoyer des datagrammes trop grands

**Symptôme :** Fragmentation IP, pertes de paquets

**Solution :** Limiter la taille à ~1400 octets

### 5. Ne pas vérifier `errno` après timeout

```c
if (recvfrom(...) < 0) {
    // ❌ Traiter toutes les erreurs pareil
    perror("recvfrom");
}

// ✅ Distinguer timeout des vraies erreurs
if (recvfrom(...) < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // Timeout normal
    } else {
        perror("recvfrom");
    }
}
```

---

## Bonnes Pratiques UDP

### 1. Définir des timeouts

Toujours définir un timeout pour `recvfrom()` :
```c
struct timeval tv = {.tv_sec = 5, .tv_usec = 0};
setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
```

### 2. Implémenter des retries

Ne jamais supposer qu'un datagramme arrivera du premier coup.

### 3. Limiter la taille des datagrammes

Restez en dessous de 1400 octets pour éviter la fragmentation.

### 4. Ajouter un checksum applicatif

Pour des données critiques, calculez votre propre checksum.

### 5. Gérer les datagrammes dupliqués

Utilisez des numéros de séquence pour détecter les doublons.

### 6. Logger les erreurs

Gardez une trace des datagrammes perdus, timeouts, etc.

### 7. Tester avec des pertes réseau

Simulez des pertes avec `tc` (traffic control) :
```bash
sudo tc qdisc add dev lo root netem loss 10%
```

---

## Comparaison Finale : Quand Choisir UDP ?

| Critère | Choisir TCP | Choisir UDP |
|---------|-------------|-------------|
| **Fiabilité requise** | ✅ | ❌ |
| **Ordre important** | ✅ | ❌ |
| **Latence critique** | ❌ | ✅ |
| **Temps réel** | ❌ | ✅ |
| **Streaming** | ❌ | ✅ |
| **Petits messages** | ❌ | ✅ |
| **Broadcast/Multicast** | ❌ | ✅ |
| **Requête/Réponse simple** | ❌ | ✅ |
| **Transfert de fichiers** | ✅ | ❌ |
| **API REST** | ✅ | ❌ |
| **Transactions** | ✅ | ❌ |

---

## En résumé

UDP est un protocole léger et rapide, idéal pour les applications en temps réel où la vitesse prime sur la fiabilité.

**Points clés à retenir :**

✅ **Sans connexion** : Pas de handshake, envoi direct

✅ **Datagrammes** : Messages complets et indépendants

✅ **Fonctions principales** : `sendto()` et `recvfrom()`

✅ **Pas de garantie** : Pertes, doublons, désordre possibles

✅ **Rapide** : Latence faible, overhead minimal

✅ **Broadcast/Multicast** : Diffusion à plusieurs destinataires

✅ **Taille limitée** : ~1400 octets recommandés

❌ **Nécessite gestion applicative** : Timeouts, retries, séquençage

**Use cases typiques :**
- Streaming audio/vidéo
- Jeux en ligne
- VoIP
- DNS, DHCP, NTP
- Monitoring, logs

Dans la prochaine section, nous verrons comment gérer les erreurs réseau de manière robuste et comment créer des serveurs concurrents capables de gérer plusieurs clients simultanément.

---

**→ Prochaine section : 20.5 Gestion des Erreurs Réseau**

⏭️ [Gestion des erreurs réseau](/20-reseau-sockets/05-gestion-erreurs-reseau.md)
