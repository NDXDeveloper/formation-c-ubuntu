🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 20.3 Sockets TCP

## Introduction

Maintenant que nous comprenons le modèle client/serveur et l'architecture TCP/IP, il est temps de passer à la pratique ! Les **sockets TCP** sont l'outil principal pour créer des applications réseau en C sous Linux.

Un **socket** est une abstraction qui représente un point de communication réseau. C'est l'équivalent d'un descripteur de fichier, mais pour le réseau au lieu du système de fichiers.

Dans cette section, nous allons explorer en détail l'API des sockets POSIX et apprendre à créer des serveurs et clients TCP robustes.

---

## Qu'est-ce qu'un Socket ?

### Définition

Un **socket** est une interface de programmation (API) qui permet à deux programmes de communiquer via le réseau. En C sous Linux, un socket est représenté par un **descripteur de fichier** (un simple entier).

### Analogie

Pensez à un socket comme à une **prise téléphonique** :
- Vous créez la prise (création du socket)
- Vous y branchez votre téléphone (liaison à une adresse)
- Vous attendez qu'on vous appelle ou vous appelez quelqu'un (connexion)
- Vous parlez et écoutez (envoi/réception de données)
- Vous raccrochez (fermeture du socket)

### Types de sockets

En TCP/IP, il existe deux types principaux :

| Type | Constante | Protocole | Caractéristiques |
|------|-----------|-----------|------------------|
| **Stream** | `SOCK_STREAM` | TCP | Connexion, fiable, ordonné, flux d'octets |
| **Datagram** | `SOCK_DGRAM` | UDP | Sans connexion, non fiable, messages |

Dans cette section, nous nous concentrons sur les **sockets TCP** (`SOCK_STREAM`).

---

## Les Structures de Données Essentielles

Avant de coder, il faut comprendre les structures de données utilisées par l'API socket.

### 1. `struct sockaddr` - Structure générique

```c
struct sockaddr {
    sa_family_t sa_family;    // Famille d'adresses (AF_INET, AF_INET6, etc.)
    char        sa_data[14];  // Adresse (format dépend de sa_family)
};
```

C'est la structure **générique** utilisée par l'API socket. En pratique, on ne l'utilise presque jamais directement.

### 2. `struct sockaddr_in` - Pour IPv4

```c
struct sockaddr_in {
    sa_family_t    sin_family;  // AF_INET pour IPv4
    in_port_t      sin_port;    // Port (en network byte order)
    struct in_addr sin_addr;    // Adresse IPv4
    char           sin_zero[8]; // Padding (doit être à zéro)
};

struct in_addr {
    uint32_t s_addr;  // Adresse IPv4 (en network byte order)
};
```

**C'est cette structure qu'on utilise en pratique pour TCP/IPv4.**

### 3. `struct sockaddr_in6` - Pour IPv6

```c
struct sockaddr_in6 {
    sa_family_t     sin6_family;   // AF_INET6
    in_port_t       sin6_port;     // Port
    uint32_t        sin6_flowinfo; // IPv6 flow information
    struct in6_addr sin6_addr;     // Adresse IPv6
    uint32_t        sin6_scope_id; // Scope ID
};
```

Nous nous concentrerons sur IPv4 pour simplifier.

### Pourquoi cette complexité ?

L'API socket a été conçue pour être **générique** et supporter plusieurs protocoles (IPv4, IPv6, Unix sockets, etc.). C'est pourquoi il y a cette hiérarchie de structures.

**En pratique :**
- Vous utilisez `struct sockaddr_in` pour remplir les informations
- Vous castez en `struct sockaddr*` lors des appels système

```c
struct sockaddr_in addr;
// ... remplir addr ...
bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
//                    ↑ cast nécessaire
```

---

## Ordre des Octets : Network Byte Order

### Le problème de l'endianness

Les processeurs stockent les nombres multi-octets différemment :
- **Big-endian** : L'octet de poids fort en premier (ex: SPARC, PowerPC, Motorola 68000)
- **Little-endian** : L'octet de poids faible en premier (ex: x86, x86_64)

### Network Byte Order

Le réseau utilise **toujours** le format **big-endian** (appelé "network byte order").

### Fonctions de conversion

Pour garantir la portabilité, on utilise ces fonctions :

```c
#include <arpa/inet.h>

// Host to Network (conversions vers network byte order)
uint32_t htonl(uint32_t hostlong);    // 32 bits (pour IP)  
uint16_t htons(uint16_t hostshort);   // 16 bits (pour ports)  

// Network to Host (conversions depuis network byte order)
uint32_t ntohl(uint32_t netlong);     // 32 bits  
uint16_t ntohs(uint16_t netshort);    // 16 bits  
```

**Mnémonique :**
- **h** = host (machine locale)
- **n** = network (réseau)
- **s** = short (16 bits)
- **l** = long (32 bits)

**Exemple :**
```c
uint16_t port = 8080;  
addr.sin_port = htons(port);  // Conversion obligatoire !  
```

⚠️ **Règle d'or :** Toujours convertir les ports et adresses IP avant de les mettre dans les structures socket.

---

## Fonctions de Conversion d'Adresses IP

### Conversion String → Binaire

```c
#include <arpa/inet.h>

// Méthode moderne (supporte IPv4 et IPv6)
int inet_pton(int af, const char *src, void *dst);
```

**Paramètres :**
- `af` : `AF_INET` (IPv4) ou `AF_INET6` (IPv6)
- `src` : Adresse IP en chaîne (ex: "192.168.1.10")
- `dst` : Pointeur vers `struct in_addr` (pour IPv4)

**Retour :**
- `1` : Succès
- `0` : Adresse invalide
- `-1` : Erreur (errno positionné)

**Exemple :**
```c
struct sockaddr_in addr;  
addr.sin_family = AF_INET;  
addr.sin_port = htons(8080);  

if (inet_pton(AF_INET, "192.168.1.10", &addr.sin_addr) <= 0) {
    perror("inet_pton");
    exit(EXIT_FAILURE);
}
```

### Conversion Binaire → String

```c
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
```

**Paramètres :**
- `af` : `AF_INET` ou `AF_INET6`
- `src` : Pointeur vers `struct in_addr`
- `dst` : Buffer pour stocker la chaîne
- `size` : Taille du buffer (utiliser `INET_ADDRSTRLEN` pour IPv4)

**Exemple :**
```c
char ip_str[INET_ADDRSTRLEN];  
inet_ntop(AF_INET, &addr.sin_addr, ip_str, INET_ADDRSTRLEN);  
printf("Adresse IP : %s\n", ip_str);  
```

### Constantes utiles

```c
#define INET_ADDRSTRLEN   16   // "255.255.255.255\0"
#define INET6_ADDRSTRLEN  46   // Taille max pour IPv6
```

### Adresses IP spéciales

```c
// Écouter sur toutes les interfaces
addr.sin_addr.s_addr = INADDR_ANY;  // 0.0.0.0

// Localhost (boucle locale) — htonl() nécessaire car 0x7f000001 ≠ 0
addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // 127.0.0.1

// Broadcast
addr.sin_addr.s_addr = INADDR_BROADCAST;  // 255.255.255.255
```

---

## L'API Socket TCP : Vue d'Ensemble

Voici les étapes typiques pour un serveur et un client TCP :

### Serveur TCP

```
1. socket()     Créer le socket
2. bind()       Lier à une adresse IP et un port
3. listen()     Passer en mode écoute
4. accept()     Accepter les connexions (bloquant)
5. recv()       Recevoir des données
   send()       Envoyer des données
6. close()      Fermer la connexion
```

### Client TCP

```
1. socket()     Créer le socket
2. connect()    Se connecter au serveur
3. send()       Envoyer des données
   recv()       Recevoir des données
4. close()      Fermer la connexion
```

**Différence clé :** Le serveur utilise `bind()`, `listen()`, et `accept()`, tandis que le client utilise `connect()` directement.

---

## Les Fonctions en Détail

### 1. `socket()` - Créer un socket

```c
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```

**Paramètres :**
- `domain` : Famille d'adresses
  - `AF_INET` : IPv4
  - `AF_INET6` : IPv6
  - `AF_UNIX` : Unix domain sockets (IPC local)
- `type` : Type de socket
  - `SOCK_STREAM` : TCP (flux d'octets fiable)
  - `SOCK_DGRAM` : UDP (datagrammes)
- `protocol` : Protocole spécifique (généralement `0` pour le choix par défaut)

**Retour :**
- Descripteur de fichier (entier ≥ 0) en cas de succès
- `-1` en cas d'erreur (errno positionné)

**Exemple :**
```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0);  
if (sockfd < 0) {  
    perror("socket");
    exit(EXIT_FAILURE);
}
```

**Analogie :** Créer le téléphone (l'objet physique), mais il n'est pas encore branché.

---

### 2. `bind()` - Lier le socket à une adresse

```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

**Paramètres :**
- `sockfd` : Descripteur du socket (retourné par `socket()`)
- `addr` : Pointeur vers la structure d'adresse (castée en `struct sockaddr*`)
- `addrlen` : Taille de la structure (`sizeof(struct sockaddr_in)`)

**Retour :**
- `0` en cas de succès
- `-1` en cas d'erreur

**Exemple :**
```c
struct sockaddr_in server_addr;  
memset(&server_addr, 0, sizeof(server_addr));  

server_addr.sin_family = AF_INET;  
server_addr.sin_addr.s_addr = INADDR_ANY;  // Toutes les interfaces  
server_addr.sin_port = htons(8080);        // Port 8080  

if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("bind");
    close(sockfd);
    exit(EXIT_FAILURE);
}
```

**Rôle :**
- Associe le socket à une **adresse IP** et un **port** spécifiques
- Nécessaire **uniquement pour le serveur**
- Le client n'a généralement pas besoin de `bind()` (le système choisit un port éphémère)

**Erreurs courantes :**
- **EADDRINUSE** : Le port est déjà utilisé par un autre processus
- **EACCES** : Tentative d'utiliser un port privilégié (<1024) sans être root

**Analogie :** Brancher le téléphone sur une ligne avec un numéro spécifique.

---

### 3. `listen()` - Passer en mode écoute

```c
int listen(int sockfd, int backlog);
```

**Paramètres :**
- `sockfd` : Descripteur du socket
- `backlog` : Nombre maximal de connexions en attente dans la file

**Retour :**
- `0` en cas de succès
- `-1` en cas d'erreur

**Exemple :**
```c
if (listen(sockfd, 10) < 0) {
    perror("listen");
    close(sockfd);
    exit(EXIT_FAILURE);
}
printf("Serveur en écoute sur le port 8080...\n");
```

**Rôle :**
- Marque le socket comme **passif** (serveur)
- Le socket est maintenant prêt à accepter des connexions
- Le paramètre `backlog` définit la taille de la file d'attente des connexions non encore acceptées

**Le paramètre backlog :**
- Si `backlog = 10`, le système peut mettre en file jusqu'à 10 connexions en attente
- Si la file est pleine, les nouvelles connexions sont refusées (le client reçoit ECONNREFUSED)
- Valeur typique : entre 5 et 128

**Analogie :** Activer le répondeur du téléphone. Les appels peuvent arriver et patienter jusqu'à ce que vous décrochiez.

---

### 4. `accept()` - Accepter une connexion

```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

**Paramètres :**
- `sockfd` : Socket en écoute (celui sur lequel vous avez appelé `listen()`)
- `addr` : Pointeur vers une structure qui recevra l'adresse du client (peut être `NULL`)
- `addrlen` : Pointeur vers la taille de la structure (modifié par `accept()`)

**Retour :**
- **Nouveau descripteur de socket** pour cette connexion spécifique
- `-1` en cas d'erreur

**Exemple :**
```c
struct sockaddr_in client_addr;  
socklen_t client_len = sizeof(client_addr);  

int client_fd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);  
if (client_fd < 0) {  
    perror("accept");
    // Selon l'erreur, on peut continuer ou non
}

// Afficher l'IP du client
char client_ip[INET_ADDRSTRLEN];  
inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);  
printf("Client connecté : %s:%d\n", client_ip, ntohs(client_addr.sin_port));  
```

**Points importants :**

1. **Fonction bloquante :** `accept()` attend qu'un client se connecte. Le programme s'arrête à cette ligne tant qu'aucun client n'arrive.

2. **Deux sockets distincts :**
   - Le socket original (`sockfd`) reste en **écoute** pour d'autres clients
   - Le nouveau socket (`client_fd`) est dédié à **cette connexion** spécifique

3. **Informations sur le client :** La structure `client_addr` contient l'IP et le port du client

**Analogie :** Décrocher le téléphone quand il sonne. Vous avez maintenant une ligne de communication active avec l'appelant.

---

### 5. `connect()` - Se connecter à un serveur (client)

```c
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

**Paramètres :**
- `sockfd` : Descripteur du socket créé par `socket()`
- `addr` : Adresse du serveur (IP + port)
- `addrlen` : Taille de la structure

**Retour :**
- `0` en cas de succès
- `-1` en cas d'erreur

**Exemple :**
```c
struct sockaddr_in server_addr;  
memset(&server_addr, 0, sizeof(server_addr));  

server_addr.sin_family = AF_INET;  
server_addr.sin_port = htons(8080);  

if (inet_pton(AF_INET, "192.168.1.10", &server_addr.sin_addr) <= 0) {
    perror("inet_pton");
    exit(EXIT_FAILURE);
}

if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("connect");
    close(sockfd);
    exit(EXIT_FAILURE);
}

printf("Connecté au serveur !\n");
```

**Rôle :**
- Initie une connexion TCP vers le serveur (three-way handshake)
- **Fonction bloquante** : Attend que la connexion soit établie
- Une fois `connect()` réussie, le socket est prêt pour envoyer/recevoir des données

**Erreurs courantes :**
- **ECONNREFUSED** : Le serveur n'écoute pas sur ce port
- **ETIMEDOUT** : Le serveur ne répond pas (firewall, réseau down)
- **ENETUNREACH** : Le réseau est inaccessible

**Analogie :** Composer le numéro de téléphone et attendre que quelqu'un décroche.

---

### 6. `send()` / `write()` - Envoyer des données

```c
ssize_t send(int sockfd, const void *buf, size_t len, int flags);  
ssize_t write(int sockfd, const void *buf, size_t count);  
```

**Paramètres :**
- `sockfd` : Descripteur du socket connecté
- `buf` : Pointeur vers les données à envoyer
- `len` / `count` : Nombre d'octets à envoyer
- `flags` : Options (généralement `0`)

**Retour :**
- Nombre d'octets **réellement envoyés** (peut être inférieur à `len`)
- `-1` en cas d'erreur

**Exemple avec `send()` :**
```c
const char *message = "Hello, Client!";  
ssize_t bytes_sent = send(client_fd, message, strlen(message), 0);  
if (bytes_sent < 0) {  
    perror("send");
}
```

**Exemple avec `write()` :**
```c
const char *message = "Hello, Client!";  
ssize_t bytes_sent = write(client_fd, message, strlen(message));  
if (bytes_sent < 0) {  
    perror("write");
}
```

**Différences `send()` vs `write()` :**
- `write()` : Fonction POSIX générique (fichiers, pipes, sockets)
- `send()` : Spécifique aux sockets, supporte des flags supplémentaires
- En pratique, pour TCP, ils sont souvent interchangeables

**Points importants :**

1. **Envoi partiel :** `send()` peut envoyer moins d'octets que demandé. Il faut boucler pour tout envoyer :

```c
ssize_t send_all(int sockfd, const void *buf, size_t len) {
    size_t total_sent = 0;
    const char *ptr = buf;

    while (total_sent < len) {
        ssize_t sent = send(sockfd, ptr + total_sent, len - total_sent, 0);
        if (sent < 0) {
            return -1;  // Erreur
        }
        total_sent += sent;
    }
    return total_sent;
}
```

2. **Fonction bloquante :** Si le buffer d'envoi TCP est plein, `send()` attend qu'il y ait de la place.

---

### 7. `recv()` / `read()` - Recevoir des données

```c
ssize_t recv(int sockfd, void *buf, size_t len, int flags);  
ssize_t read(int sockfd, void *buf, size_t count);  
```

**Paramètres :**
- `sockfd` : Descripteur du socket connecté
- `buf` : Buffer pour stocker les données reçues
- `len` / `count` : Taille maximale du buffer
- `flags` : Options (généralement `0`)

**Retour :**
- Nombre d'octets reçus
- `0` : La connexion a été fermée par l'autre côté
- `-1` : Erreur

**Exemple avec `recv()` :**
```c
char buffer[1024];  
ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);  

if (bytes_received < 0) {
    perror("recv");
} else if (bytes_received == 0) {
    printf("Client déconnecté\n");
} else {
    buffer[bytes_received] = '\0';  // Terminer la chaîne
    printf("Reçu : %s\n", buffer);
}
```

**Points importants :**

1. **Fonction bloquante :** `recv()` attend jusqu'à ce que des données arrivent.

2. **Retour de 0 = connexion fermée :** C'est la manière propre de détecter une déconnexion.

3. **Ne reçoit pas forcément tout d'un coup :** Si le client envoie 1000 octets, `recv()` peut les recevoir en plusieurs appels (par exemple 512, puis 488).

4. **Pas de garantie de frontières de messages :** TCP est un flux d'octets continu. Si vous envoyez "Hello" puis "World", le destinataire peut recevoir "HelloWorld" en un seul `recv()`.

5. **Toujours terminer les chaînes :** Si vous traitez les données comme une chaîne de caractères, ajoutez `\0` à la fin.

---

### 8. `close()` - Fermer le socket

```c
#include <unistd.h>

int close(int fd);
```

**Paramètre :**
- `fd` : Descripteur du socket à fermer

**Retour :**
- `0` en cas de succès
- `-1` en cas d'erreur

**Exemple :**
```c
close(client_fd);  
close(sockfd);  
```

**Rôle :**
- Libère les ressources associées au socket
- En TCP, initie la fermeture de connexion (envoi d'un paquet FIN)
- **Toujours fermer les sockets** pour éviter les fuites de descripteurs de fichiers

**Analogie :** Raccrocher le téléphone.

---

## Exemple Complet : Serveur Écho TCP

Voici un serveur TCP simple qui renvoie en écho tout ce qu'il reçoit.

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
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // 1. Créer le socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Option pour réutiliser l'adresse immédiatement (évite "Address already in use")
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 2. Configurer l'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Écouter sur toutes les interfaces
    server_addr.sin_port = htons(PORT);

    // 3. Lier le socket à l'adresse
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 4. Passer en mode écoute
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Serveur en écoute sur le port %d...\n", PORT);

    // Boucle principale : accepter et traiter les clients
    while (1) {
        client_len = sizeof(client_addr);

        // 5. Accepter une connexion
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;  // Continuer à accepter d'autres clients
        }

        // Afficher les informations du client
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Client connecté : %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        // 6. Communiquer avec le client (écho)
        while ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
            buffer[bytes_received] = '\0';
            printf("Reçu : %s", buffer);

            // Renvoyer en écho
            if (send(client_fd, buffer, bytes_received, 0) < 0) {
                perror("send");
                break;
            }
        }

        if (bytes_received == 0) {
            printf("Client déconnecté\n");
        } else if (bytes_received < 0) {
            perror("recv");
        }

        // 7. Fermer la connexion avec ce client
        close(client_fd);
    }

    // Ce code n'est jamais atteint (boucle infinie), mais bonne pratique
    close(server_fd);
    return 0;
}
```

**Compilation :**
```bash
gcc -o server server.c -Wall -Wextra
```

**Exécution :**
```bash
./server
```

---

## Exemple Complet : Client Écho TCP

Voici le client correspondant qui se connecte au serveur et envoie des messages.

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
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // 1. Créer le socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 2. Configurer l'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convertir l'adresse IP du serveur
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 3. Se connecter au serveur
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connecté au serveur\n");
    printf("Tapez un message (ou 'quit' pour quitter) :\n");

    // 4. Boucle d'interaction
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
        if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
            perror("send");
            break;
        }

        // Recevoir la réponse (écho)
        bytes_received = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received < 0) {
            perror("recv");
            break;
        } else if (bytes_received == 0) {
            printf("Serveur déconnecté\n");
            break;
        }

        buffer[bytes_received] = '\0';
        printf("Écho : %s", buffer);
    }

    // 5. Fermer le socket
    close(sockfd);
    printf("Déconnecté\n");
    return 0;
}
```

**Compilation :**
```bash
gcc -o client client.c -Wall -Wextra
```

**Exécution :**
```bash
./client
```

**Test :**
1. Lancez le serveur dans un terminal : `./server`
2. Lancez le client dans un autre terminal : `./client`
3. Tapez des messages dans le client, ils seront renvoyés en écho

---

## Options de Socket Avancées : `setsockopt()`

### Le problème "Address already in use"

Lorsque vous arrêtez un serveur TCP, le système garde le socket dans un état **TIME_WAIT** pendant environ 60 secondes. Si vous essayez de relancer le serveur immédiatement, vous obtenez l'erreur :

```
bind: Address already in use
```

### La solution : `SO_REUSEADDR`

```c
int setsockopt(int sockfd, int level, int optname,
               const void *optval, socklen_t optlen);
```

**Exemple d'utilisation :**
```c
int opt = 1;  
if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {  
    perror("setsockopt");
    exit(EXIT_FAILURE);
}
```

**Ce que fait `SO_REUSEADDR` :**
- Permet de réutiliser une adresse locale qui est en état TIME_WAIT
- **Doit être appelé avant `bind()`**
- Essentiel pour le développement (redémarrages fréquents)

### Autres options utiles

```c
// Définir un timeout pour recv() (éviter de bloquer indéfiniment)
struct timeval timeout;  
timeout.tv_sec = 5;   // 5 secondes  
timeout.tv_usec = 0;  
setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));  

// Définir la taille du buffer d'envoi
int sndbuf = 65536;  
setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf));  

// Activer TCP keepalive (détection de connexions mortes)
int keepalive = 1;  
setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));  
```

---

## Gestion des Erreurs : `errno` et `perror()`

### La variable `errno`

Quand une fonction système échoue (retourne -1), elle positionne la variable globale **errno** avec un code d'erreur.

```c
#include <errno.h>
```

### La fonction `perror()`

```c
void perror(const char *s);
```

Affiche un message d'erreur lisible basé sur `errno`.

**Exemple :**
```c
if (connect(sockfd, ...) < 0) {
    perror("connect");  // Affiche : "connect: Connection refused"
    exit(EXIT_FAILURE);
}
```

### Codes d'erreur courants

| Code | Description |
|------|-------------|
| `ECONNREFUSED` | Connexion refusée (serveur n'écoute pas) |
| `ETIMEDOUT` | Timeout (serveur ne répond pas) |
| `EADDRINUSE` | Adresse déjà utilisée (port occupé) |
| `EACCES` | Permission refusée (port <1024 sans root) |
| `EINTR` | Appel système interrompu par un signal |
| `EAGAIN` / `EWOULDBLOCK` | Opération bloquerait (mode non-bloquant) |
| `EPIPE` | Broken pipe (écriture sur socket fermé) |

### Gestion robuste

```c
ssize_t bytes_sent = send(sockfd, buffer, len, 0);  
if (bytes_sent < 0) {  
    if (errno == EINTR) {
        // Réessayer en cas d'interruption par signal
        bytes_sent = send(sockfd, buffer, len, 0);
    } else {
        perror("send");
        // Gérer l'erreur
    }
}
```

---

## Limitations du Serveur Itératif

Le serveur d'exemple ci-dessus est **itératif** : il ne peut gérer qu'**un seul client à la fois**.

**Problème :**
- Tant que le premier client est connecté, les autres clients sont bloqués dans la file d'attente
- Si un client est lent, tous les autres attendent

**Solutions (vues dans les sections suivantes) :**
1. **Fork** : Créer un processus fils par client
2. **Threads** : Créer un thread par client
3. **I/O multiplexing** : `select()`, `poll()`, `epoll()`
4. **I/O asynchrone** : Architecture event-driven

---

## Conseils et Bonnes Pratiques

### 1. Toujours vérifier les retours

```c
// ❌ Mauvais
socket(AF_INET, SOCK_STREAM, 0);

// ✅ Bon
int sockfd = socket(AF_INET, SOCK_STREAM, 0);  
if (sockfd < 0) {  
    perror("socket");
    exit(EXIT_FAILURE);
}
```

### 2. Initialiser les structures à zéro

```c
struct sockaddr_in addr;  
memset(&addr, 0, sizeof(addr));  // ✅ Important !  
```

### 3. Utiliser `SO_REUSEADDR` en développement

```c
int opt = 1;  
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));  
```

### 4. Toujours fermer les sockets

```c
close(client_fd);  
close(server_fd);  
```

### 5. Gérer les envois partiels

Utiliser une fonction helper comme `send_all()` présentée plus haut.

### 6. Terminer les chaînes de caractères

```c
buffer[bytes_received] = '\0';
```

### 7. Ne jamais faire confiance aux données réseau

- Toujours valider les données reçues
- Vérifier les longueurs
- Protéger contre les buffer overflows

### 8. Gérer proprement les signaux

En production, installer un gestionnaire pour `SIGPIPE` (envoi sur socket fermé).

```c
signal(SIGPIPE, SIG_IGN);  // Ignorer SIGPIPE
```

---

## Debugging avec `netstat` et `ss`

### Afficher les sockets en écoute

```bash
# Méthode moderne (ss)
ss -tuln

# Ancienne méthode (netstat)
netstat -tuln
```

**Options :**
- `-t` : TCP
- `-u` : UDP
- `-l` : Listening (en écoute)
- `-n` : Numérique (ne pas résoudre les noms)

**Exemple de sortie :**
```
State   Recv-Q  Send-Q  Local Address:Port  Peer Address:Port  
LISTEN  0       5       0.0.0.0:8080        0.0.0.0:*  
```

### Afficher les connexions actives

```bash
ss -tn
```

---

## En résumé

Les sockets TCP en C permettent de créer des applications réseau robustes et performantes. Voici les points clés :

**Côté Serveur :**
1. `socket()` : Créer le socket
2. `bind()` : Lier à une adresse/port
3. `listen()` : Passer en écoute
4. `accept()` : Accepter les clients (bloquant)
5. `recv()`/`send()` : Communiquer
6. `close()` : Fermer

**Côté Client :**
1. `socket()` : Créer le socket
2. `connect()` : Se connecter au serveur
3. `send()`/`recv()` : Communiquer
4. `close()` : Fermer

**Points importants :**
- ✅ Toujours vérifier les valeurs de retour
- ✅ Utiliser `htons()` / `htonl()` pour les conversions
- ✅ Gérer `errno` pour les erreurs
- ✅ Utiliser `SO_REUSEADDR` en développement
- ✅ Fermer tous les sockets
- ✅ Le serveur d'exemple est itératif (un client à la fois)

Dans les prochaines sections, nous verrons comment créer des serveurs **concurrents** capables de gérer plusieurs clients simultanément, ainsi que l'utilisation d'UDP.

---

**→ Prochaine section : 20.4 Sockets UDP**

⏭️ [Sockets UDP](/20-reseau-sockets/04-sockets-udp.md)
