🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 20.7 Résolution de Noms

## Introduction

Jusqu'à présent, nous avons utilisé des adresses IP directement dans notre code : `192.168.1.10`, `127.0.0.1`, etc. Mais en pratique, les humains préfèrent utiliser des **noms de domaine** comme `www.google.com` ou `github.com`.

La **résolution de noms** (ou résolution DNS) est le processus qui convertit un nom de domaine lisible par l'homme en une adresse IP utilisable par les ordinateurs.

**Analogie :** C'est comme chercher le numéro de téléphone d'un ami dans un annuaire. Vous connaissez son nom (domaine), l'annuaire (DNS) vous donne son numéro (IP).

Dans cette section, nous allons apprendre à résoudre des noms de domaine en C, en utilisant les fonctions modernes et portables.

---

## Pourquoi Utiliser des Noms au Lieu d'Adresses IP ?

### Avantages des noms de domaine

✅ **Mémorisation** : `google.com` est plus facile à retenir que `142.250.185.46`

✅ **Flexibilité** : L'IP peut changer sans que les utilisateurs aient besoin de le savoir

✅ **Redondance** : Un nom peut pointer vers plusieurs IP (load balancing)

✅ **Services virtuels** : Plusieurs noms peuvent partager la même IP (hébergement mutualisé)

✅ **Portabilité** : Le même code fonctionne en IPv4 et IPv6

### Exemple concret

```c
// ❌ Code fragile : IP en dur
inet_pton(AF_INET, "93.184.216.34", &addr.sin_addr);

// ✅ Code robuste : résolution du nom
// Si example.com change d'IP, le code continue de fonctionner
```

---

## Le Système DNS (Domain Name System)

### Qu'est-ce que DNS ?

Le **DNS** est un système distribué hiérarchique qui associe des noms de domaine à des adresses IP.

**Architecture hiérarchique :**

```
                      . (root)
                       |
        +--------------+-------------+
        |              |             |
       com            org           net
        |              |             |
    +---+---+      +---+---+
    |       |      |       |
  google  amazon  github  wikipedia
    |
    www
```

### Processus de résolution (simplifié)

Quand vous tapez `www.google.com` :

1. **Cache local** : Vérifier si l'adresse est déjà en cache
2. **Fichier /etc/hosts** : Vérifier les mappings locaux
3. **Serveur DNS récursif** : Contacter votre serveur DNS (FAI)
4. **Serveurs DNS racine** : Si nécessaire, remonter la hiérarchie
5. **Serveurs .com** : Demander qui gère google.com
6. **Serveurs Google** : Obtenir l'IP de www.google.com
7. **Réponse** : Retourner l'adresse IP au client

**Durée typique :** 20-100 ms (sans cache), <1 ms (avec cache)

### Types d'enregistrements DNS

| Type | Description | Exemple |
|------|-------------|---------|
| **A** | IPv4 address | `google.com → 142.250.185.46` |
| **AAAA** | IPv6 address | `google.com → 2a00:1450:4007:80e::200e` |
| **CNAME** | Alias | `www.example.com → example.com` |
| **MX** | Mail server | `example.com → mail.example.com` |
| **NS** | Name server | `google.com → ns1.google.com` |
| **PTR** | Reverse (IP → nom) | `142.250.185.46 → google.com` |

---

## Fonction Obsolète : `gethostbyname()` ❌

### Pourquoi elle est obsolète

Cette fonction était la méthode traditionnelle, mais elle a des limitations :

- ❌ **IPv4 uniquement** : Ne supporte pas IPv6
- ❌ **Non thread-safe** : Utilise un buffer statique global
- ❌ **API complexe** : Structures compliquées
- ❌ **Déconseillée** : Marquée comme obsolète dans POSIX

```c
// ❌ NE PAS UTILISER (obsolète)
struct hostent *gethostbyname(const char *name);
```

**Note :** Si vous voyez cette fonction dans du vieux code, remplacez-la par `getaddrinfo()`.

---

## Fonction Moderne : `getaddrinfo()` ✅

### L'API recommandée

`getaddrinfo()` est la méthode **moderne**, **thread-safe**, et **portable** pour résoudre des noms.

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int getaddrinfo(const char *node,           // Nom d'hôte ou IP
                const char *service,        // Port ou nom de service
                const struct addrinfo *hints, // Critères de recherche
                struct addrinfo **res);     // Résultats (liste chaînée)
```

**Paramètres :**
- `node` : Nom de domaine (ex: "google.com") ou adresse IP
- `service` : Port (ex: "80") ou nom de service (ex: "http")
- `hints` : Structure pour spécifier ce que vous cherchez (IPv4/IPv6, TCP/UDP, etc.)
- `res` : Pointeur vers une liste chaînée de résultats (alloué par la fonction)

**Retour :**
- `0` en cas de succès
- Code d'erreur non-nul en cas d'échec (utiliser `gai_strerror()`)

---

## Structure `addrinfo`

### Définition

```c
struct addrinfo {
    int              ai_flags;     // Options (AI_PASSIVE, etc.)
    int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
    int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
    int              ai_protocol;  // IPPROTO_TCP, IPPROTO_UDP, 0
    socklen_t        ai_addrlen;   // Taille de ai_addr
    struct sockaddr *ai_addr;      // Adresse socket (cast selon ai_family)
    char            *ai_canonname; // Nom canonique (si AI_CANONNAME)
    struct addrinfo *ai_next;      // Élément suivant (liste chaînée)
};
```

### Champs importants

- **`ai_family`** : Famille d'adresses
  - `AF_INET` : IPv4
  - `AF_INET6` : IPv6
  - `AF_UNSPEC` : IPv4 ou IPv6 (n'importe)

- **`ai_socktype`** : Type de socket
  - `SOCK_STREAM` : TCP
  - `SOCK_DGRAM` : UDP

- **`ai_addr`** : Pointeur vers la structure d'adresse réelle
  - Cast en `struct sockaddr_in*` pour IPv4
  - Cast en `struct sockaddr_in6*` pour IPv6

- **`ai_next`** : Pointeur vers le résultat suivant (liste chaînée)

**Important :** Un nom peut avoir plusieurs adresses (IPv4 et IPv6, ou plusieurs serveurs).

---

## Utilisation de `getaddrinfo()` : Étape par Étape

### Étape 1 : Préparer les hints

```c
struct addrinfo hints;  
memset(&hints, 0, sizeof(hints));  

hints.ai_family = AF_UNSPEC;      // IPv4 ou IPv6  
hints.ai_socktype = SOCK_STREAM;  // TCP  
hints.ai_flags = 0;               // Pas de flags spéciaux  
hints.ai_protocol = 0;            // N'importe quel protocole  
```

**Options pour `ai_family` :**
- `AF_UNSPEC` : Accepter IPv4 et IPv6
- `AF_INET` : IPv4 uniquement
- `AF_INET6` : IPv6 uniquement

**Options pour `ai_flags` :**
- `AI_PASSIVE` : Pour serveur (écoute sur toutes interfaces)
- `AI_CANONNAME` : Demander le nom canonique
- `AI_NUMERICHOST` : `node` est déjà une adresse IP (pas de résolution)
- `AI_ADDRCONFIG` : Retourner uniquement les familles configurées sur le système

---

### Étape 2 : Appeler `getaddrinfo()`

```c
struct addrinfo *result;  
int status;  

status = getaddrinfo("www.google.com", "80", &hints, &result);  
if (status != 0) {  
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    exit(EXIT_FAILURE);
}
```

**Note :** `gai_strerror()` convertit le code d'erreur en message lisible.

---

### Étape 3 : Parcourir les résultats

`getaddrinfo()` peut retourner plusieurs adresses. Il faut les essayer jusqu'à ce qu'une fonctionne.

```c
struct addrinfo *rp;  
int sockfd = -1;  

// Parcourir la liste des résultats
for (rp = result; rp != NULL; rp = rp->ai_next) {
    // Créer le socket avec les paramètres de ce résultat
    sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (sockfd == -1) {
        continue;  // Essayer le suivant
    }

    // Tenter de se connecter
    if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == 0) {
        break;  // Succès !
    }

    // Échec, fermer et essayer le suivant
    close(sockfd);
    sockfd = -1;
}

if (sockfd == -1) {
    fprintf(stderr, "Impossible de se connecter\n");
    freeaddrinfo(result);
    exit(EXIT_FAILURE);
}

printf("Connecté avec succès !\n");
```

---

### Étape 4 : Libérer la mémoire

**Crucial :** `getaddrinfo()` alloue de la mémoire dynamiquement. Il faut la libérer avec `freeaddrinfo()`.

```c
freeaddrinfo(result);
```

---

## Exemple Complet : Client TCP avec Résolution de Nom

```c
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define HOSTNAME "www.example.com"
#define PORT "80"

int main() {
    struct addrinfo hints, *result, *rp;
    int sockfd, status;

    // 1. Préparer les hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;      // IPv4 ou IPv6
    hints.ai_socktype = SOCK_STREAM;  // TCP

    // 2. Résoudre le nom
    printf("Résolution de %s...\n", HOSTNAME);
    status = getaddrinfo(HOSTNAME, PORT, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    // 3. Essayer de se connecter à chaque adresse
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        // Afficher l'adresse tentée
        char host[NI_MAXHOST];
        getnameinfo(rp->ai_addr, rp->ai_addrlen,
                    host, sizeof(host), NULL, 0, NI_NUMERICHOST);
        printf("Tentative de connexion à %s...\n", host);

        // Créer le socket
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd == -1) {
            perror("socket");
            continue;
        }

        // Tenter la connexion
        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            printf("✓ Connecté avec succès !\n");
            break;  // Succès
        }

        // Échec
        perror("connect");
        close(sockfd);
        sockfd = -1;
    }

    // 4. Vérifier si connexion réussie
    if (sockfd == -1) {
        fprintf(stderr, "✗ Impossible de se connecter à %s\n", HOSTNAME);
        freeaddrinfo(result);
        exit(EXIT_FAILURE);
    }

    // 5. Libérer la mémoire
    freeaddrinfo(result);

    // 6. Utiliser la connexion
    const char *request = "GET / HTTP/1.0\r\nHost: www.example.com\r\n\r\n";
    send(sockfd, request, strlen(request), 0);

    char buffer[4096];
    ssize_t bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("\nRéponse du serveur :\n%s\n", buffer);
    }

    // 7. Fermer
    close(sockfd);
    return 0;
}
```

**Compilation et exécution :**
```bash
gcc -o client_dns client_dns.c -Wall -Wextra
./client_dns
```

**Sortie attendue :**
```
Résolution de www.example.com...  
Tentative de connexion à 93.184.216.34...  
✓ Connecté avec succès !

Réponse du serveur :  
HTTP/1.1 200 OK  
...
```

---

## Exemple : Serveur avec `getaddrinfo()` et `AI_PASSIVE`

Pour un serveur, utilisez le flag `AI_PASSIVE` et passez `NULL` comme `node`.

```c
#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define PORT "8080"

int main() {
    struct addrinfo hints, *result, *rp;
    int server_fd, status;

    // 1. Préparer les hints pour un serveur
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;      // IPv4 ou IPv6
    hints.ai_socktype = SOCK_STREAM;  // TCP
    hints.ai_flags = AI_PASSIVE;      // Pour bind() (écoute)

    // 2. Résoudre (node=NULL pour INADDR_ANY)
    status = getaddrinfo(NULL, PORT, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    // 3. Essayer de créer et bind le socket
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        // Créer socket
        server_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (server_fd == -1) {
            continue;
        }

        // Réutiliser l'adresse
        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        // Bind
        if (bind(server_fd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;  // Succès
        }

        close(server_fd);
        server_fd = -1;
    }

    freeaddrinfo(result);

    if (server_fd == -1) {
        fprintf(stderr, "Impossible de bind\n");
        exit(EXIT_FAILURE);
    }

    // 4. Listen
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Serveur en écoute sur le port %s\n", PORT);

    // 5. Accept des clients...
    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        printf("Client connecté\n");
        // Traiter le client...
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
```

**Avantages de cette approche :**
- Fonctionne automatiquement en IPv4 et IPv6
- Pas besoin de gérer manuellement les structures `sockaddr_in` / `sockaddr_in6`
- Code plus portable

---

## Résolution Inverse : `getnameinfo()`

### Convertir IP → Nom

`getnameinfo()` fait l'inverse de `getaddrinfo()` : elle convertit une adresse IP en nom d'hôte.

```c
#include <sys/socket.h>
#include <netdb.h>

int getnameinfo(const struct sockaddr *addr, socklen_t addrlen,
                char *host, socklen_t hostlen,
                char *serv, socklen_t servlen,
                int flags);
```

**Paramètres :**
- `addr` : Structure d'adresse à convertir
- `addrlen` : Taille de la structure
- `host` : Buffer pour stocker le nom d'hôte
- `hostlen` : Taille du buffer host
- `serv` : Buffer pour stocker le nom du service/port
- `servlen` : Taille du buffer serv
- `flags` : Options de comportement

**Retour :**
- `0` en cas de succès
- Code d'erreur non-nul en cas d'échec

---

### Flags de `getnameinfo()`

| Flag | Description |
|------|-------------|
| `NI_NUMERICHOST` | Retourner l'IP en format numérique (pas de résolution) |
| `NI_NUMERICSERV` | Retourner le port en format numérique |
| `NI_NAMEREQD` | Erreur si le nom ne peut pas être résolu |
| `NI_DGRAM` | Le service est basé sur UDP (pas TCP) |

---

### Exemple : Afficher IP et port d'un client

```c
void print_client_info(struct sockaddr_storage *client_addr, socklen_t addr_len) {
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    int status = getnameinfo((struct sockaddr*)client_addr, addr_len,
                             host, sizeof(host),
                             service, sizeof(service),
                             NI_NUMERICHOST | NI_NUMERICSERV);

    if (status == 0) {
        printf("Client connecté depuis %s:%s\n", host, service);
    } else {
        fprintf(stderr, "getnameinfo: %s\n", gai_strerror(status));
    }
}

// Utilisation dans accept()
struct sockaddr_storage client_addr;  
socklen_t addr_len = sizeof(client_addr);  

int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);  
if (client_fd >= 0) {  
    print_client_info(&client_addr, addr_len);
}
```

---

### Exemple : Résolution inverse complète (IP → Nom)

```c
void reverse_lookup(const char *ip_address) {
    struct addrinfo hints, *result;
    char hostname[NI_MAXHOST];

    // 1. Convertir l'IP en structure addrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_NUMERICHOST;  // ip_address est déjà numérique

    if (getaddrinfo(ip_address, NULL, &hints, &result) != 0) {
        fprintf(stderr, "Adresse IP invalide\n");
        return;
    }

    // 2. Résolution inverse
    int status = getnameinfo(result->ai_addr, result->ai_addrlen,
                             hostname, sizeof(hostname),
                             NULL, 0, 0);

    if (status == 0) {
        printf("%s → %s\n", ip_address, hostname);
    } else {
        fprintf(stderr, "Impossible de résoudre : %s\n", gai_strerror(status));
    }

    freeaddrinfo(result);
}

// Utilisation
reverse_lookup("8.8.8.8");  // → dns.google  
reverse_lookup("1.1.1.1");  // → one.one.one.one  
```

---

## Gestion des Erreurs DNS

### Codes d'erreur `getaddrinfo()`

| Code | Signification |
|------|---------------|
| `EAI_AGAIN` | Erreur temporaire, réessayer plus tard |
| `EAI_BADFLAGS` | Flags invalides dans hints |
| `EAI_FAIL` | Erreur permanente du serveur DNS |
| `EAI_FAMILY` | Famille d'adresses non supportée |
| `EAI_MEMORY` | Mémoire insuffisante |
| `EAI_NONAME` | Nom ou service inconnu |
| `EAI_SERVICE` | Service non supporté pour ce type de socket |
| `EAI_SOCKTYPE` | Type de socket non supporté |
| `EAI_SYSTEM` | Erreur système (consulter errno) |

### Fonction `gai_strerror()`

```c
const char *gai_strerror(int errcode);
```

Convertit un code d'erreur en message lisible.

**Exemple :**
```c
int status = getaddrinfo("invalid.domain.xyz", "80", &hints, &result);  
if (status != 0) {  
    fprintf(stderr, "Erreur DNS : %s\n", gai_strerror(status));

    switch (status) {
        case EAI_NONAME:
            fprintf(stderr, "Le domaine n'existe pas\n");
            break;
        case EAI_AGAIN:
            fprintf(stderr, "Problème temporaire, réessayez\n");
            break;
        case EAI_FAIL:
            fprintf(stderr, "Échec permanent de résolution\n");
            break;
    }
}
```

---

## Configuration Système DNS

### 1. Fichier `/etc/hosts`

Contient des mappings locaux **statiques** qui ont priorité sur DNS.

**Format :**
```
# /etc/hosts
127.0.0.1       localhost
::1             localhost
192.168.1.100   monserveur.local
192.168.1.50    dev.monserveur.local
```

**Utilité :**
- Tests en local sans serveur DNS
- Blocage de domaines (adblock)
- Accès à des machines locales

**Ordre de résolution :** `getaddrinfo()` vérifie d'abord `/etc/hosts` avant de contacter DNS.

---

### 2. Fichier `/etc/resolv.conf`

Configure les serveurs DNS à utiliser.

**Format :**
```
# /etc/resolv.conf
nameserver 8.8.8.8        # Google DNS  
nameserver 1.1.1.1        # Cloudflare DNS  
search localdomain        # Suffixe par défaut  
```

**Options courantes :**
- `nameserver` : Adresse IP du serveur DNS (max 3)
- `search` : Domaines à ajouter automatiquement
- `timeout` : Délai avant timeout (secondes)
- `attempts` : Nombre de tentatives

---

### 3. Ordre de résolution : `/etc/nsswitch.conf`

Définit l'ordre de consultation des sources.

**Exemple :**
```
# /etc/nsswitch.conf
hosts: files dns
```

**Signification :**
1. `files` : Consulter `/etc/hosts`
2. `dns` : Consulter les serveurs DNS

**Autres ordres possibles :**
```
hosts: files dns myhostname  # Ajouter le hostname local  
hosts: files mdns4_minimal [NOTFOUND=return] dns  # Support mDNS (Avahi)  
```

---

## Cache DNS

### Cache système

Les systèmes Linux n'ont généralement **pas** de cache DNS système par défaut (contrairement à Windows/macOS).

**Solutions de cache :**
- **systemd-resolved** : Cache DNS moderne (Ubuntu 18.04+)
- **dnsmasq** : Cache léger
- **nscd** : Name Service Cache Daemon (ancien)

### Vérifier le cache

```bash
# Avec systemd-resolved
resolvectl statistics

# Avec dnsmasq
sudo killall -USR1 dnsmasq && sudo journalctl -u dnsmasq | tail
```

### Vider le cache

```bash
# systemd-resolved
sudo systemd-resolve --flush-caches

# dnsmasq
sudo killall -HUP dnsmasq

# nscd
sudo nscd -i hosts
```

---

## Résolution de Noms de Services

### Fichier `/etc/services`

Associe des noms de services à des numéros de port.

**Extraits :**
```
# /etc/services
http            80/tcp          www  
https           443/tcp  
ssh             22/tcp  
ftp             21/tcp  
smtp            25/tcp          mail  
```

### Utiliser des noms de services avec `getaddrinfo()`

Au lieu de passer "80", vous pouvez passer "http" :

```c
struct addrinfo hints, *result;  
memset(&hints, 0, sizeof(hints));  
hints.ai_family = AF_UNSPEC;  
hints.ai_socktype = SOCK_STREAM;  

// Utiliser le nom du service au lieu du numéro
getaddrinfo("www.example.com", "http", &hints, &result);
// Équivalent à "80" pour TCP
```

**Avantages :**
- Code plus lisible
- Portabilité (si les ports standards changent)

---

## Outils de Diagnostic DNS

### 1. `host` - Résolution simple

```bash
host www.google.com
# www.google.com has address 142.250.185.46
# www.google.com has IPv6 address 2a00:1450:4007:80e::200e
```

### 2. `dig` - Résolution détaillée

```bash
dig www.google.com

# Voir uniquement l'IP
dig +short www.google.com

# Résolution inverse
dig -x 8.8.8.8
```

### 3. `nslookup` - Outil interactif

```bash
nslookup www.google.com

# Spécifier un serveur DNS
nslookup www.google.com 8.8.8.8
```

### 4. `getent` - Tester la résolution système

```bash
# Utilise la même méthode que getaddrinfo()
getent hosts www.google.com

# Inclut /etc/hosts
getent hosts localhost
```

---

## Bonnes Pratiques

### 1. Toujours utiliser `getaddrinfo()` (pas `gethostbyname()`)

```c
// ✅ Bon : moderne, IPv6-ready, thread-safe
getaddrinfo("google.com", "80", &hints, &result);

// ❌ Mauvais : obsolète
gethostbyname("google.com");
```

---

### 2. Essayer toutes les adresses retournées

```c
// ✅ Bon : parcourir tous les résultats
for (rp = result; rp != NULL; rp = rp->ai_next) {
    sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (sockfd == -1) continue;

    if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == 0) {
        break;  // Succès
    }

    close(sockfd);
}

// ❌ Mauvais : utiliser seulement la première
sockfd = socket(result->ai_family, ...);  
connect(sockfd, result->ai_addr, ...);  
```

---

### 3. Toujours libérer avec `freeaddrinfo()`

```c
struct addrinfo *result;  
getaddrinfo(..., &result);  

// ... utiliser result ...

freeaddrinfo(result);  // ✅ Obligatoire !
```

---

### 4. Gérer les erreurs de résolution

```c
int status = getaddrinfo(hostname, port, &hints, &result);  
if (status != 0) {  
    // ✅ Afficher un message utile
    fprintf(stderr, "Impossible de résoudre %s : %s\n",
            hostname, gai_strerror(status));

    // Suggestion à l'utilisateur
    if (status == EAI_NONAME) {
        fprintf(stderr, "Vérifiez l'orthographe du nom de domaine\n");
    }
    return -1;
}
```

---

### 5. Utiliser `AF_UNSPEC` pour IPv4/IPv6

```c
// ✅ Bon : supporte IPv4 et IPv6
hints.ai_family = AF_UNSPEC;

// ❌ Mauvais : force IPv4 uniquement
hints.ai_family = AF_INET;
```

---

### 6. Timeout pour résolution DNS

Par défaut, `getaddrinfo()` peut bloquer longtemps (30+ secondes).

**Solution avec thread ou processus fils :**

```c
#include <pthread.h>

struct dns_query {
    const char *hostname;
    const char *port;
    struct addrinfo hints;
    struct addrinfo **result;
    int status;
};

void* dns_resolver_thread(void *arg) {
    struct dns_query *query = arg;
    query->status = getaddrinfo(query->hostname, query->port,
                                &query->hints, query->result);
    return NULL;
}

int getaddrinfo_with_timeout(const char *hostname, const char *port,
                              const struct addrinfo *hints,
                              struct addrinfo **res,
                              int timeout_sec) {
    struct dns_query query = {
        .hostname = hostname,
        .port = port,
        .hints = *hints,
        .result = res
    };

    pthread_t thread;
    pthread_create(&thread, NULL, dns_resolver_thread, &query);

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_sec;

    int ret = pthread_timedjoin_np(thread, NULL, &ts);
    if (ret == ETIMEDOUT) {
        pthread_cancel(thread);
        pthread_join(thread, NULL);
        return EAI_AGAIN;  // Timeout
    }

    return query.status;
}
```

---

### 7. Mettre en cache les résultats

Si vous résolvez souvent le même nom, gardez les résultats en cache :

```c
struct dns_cache_entry {
    char hostname[256];
    struct addrinfo *result;
    time_t timestamp;
};

// Cache simple avec TTL de 5 minutes
#define CACHE_TTL 300

struct dns_cache_entry cache[10];  
int cache_size = 0;  

struct addrinfo* cached_getaddrinfo(const char *hostname,
                                     const char *port,
                                     const struct addrinfo *hints) {
    time_t now = time(NULL);

    // Chercher dans le cache
    for (int i = 0; i < cache_size; i++) {
        if (strcmp(cache[i].hostname, hostname) == 0) {
            if (now - cache[i].timestamp < CACHE_TTL) {
                return cache[i].result;  // Cache valide
            } else {
                // Expirer l'entrée
                freeaddrinfo(cache[i].result);
                cache_size--;
                // ... réorganiser le cache ...
                break;
            }
        }
    }

    // Résolution normale
    struct addrinfo *result;
    if (getaddrinfo(hostname, port, hints, &result) == 0) {
        // Ajouter au cache
        if (cache_size < 10) {
            strncpy(cache[cache_size].hostname, hostname, 255);
            cache[cache_size].result = result;
            cache[cache_size].timestamp = now;
            cache_size++;
        }
        return result;
    }

    return NULL;
}
```

---

## Exemple Avancé : Client HTTP Simple

Voici un exemple complet de client HTTP utilisant la résolution DNS.

```c
#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int http_get(const char *hostname, const char *path) {
    struct addrinfo hints, *result, *rp;
    int sockfd, status;
    char request[2048], response[4096];

    // 1. Résolution DNS
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    printf("Résolution de %s...\n", hostname);
    status = getaddrinfo(hostname, "80", &hints, &result);
    if (status != 0) {
        fprintf(stderr, "Erreur DNS : %s\n", gai_strerror(status));
        return -1;
    }

    // 2. Connexion
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd == -1) continue;

        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;  // Succès
        }

        close(sockfd);
        sockfd = -1;
    }

    freeaddrinfo(result);

    if (sockfd == -1) {
        fprintf(stderr, "Impossible de se connecter\n");
        return -1;
    }

    printf("Connecté à %s\n", hostname);

    // 3. Envoyer requête HTTP
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n"
             "\r\n",
             path, hostname);

    if (send(sockfd, request, strlen(request), 0) < 0) {
        perror("send");
        close(sockfd);
        return -1;
    }

    // 4. Recevoir réponse
    printf("\n=== Réponse ===\n");
    ssize_t bytes;
    while ((bytes = recv(sockfd, response, sizeof(response) - 1, 0)) > 0) {
        response[bytes] = '\0';
        printf("%s", response);
    }
    printf("\n=============\n");

    // 5. Fermer
    close(sockfd);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <hostname> <path>\n", argv[0]);
        fprintf(stderr, "Exemple: %s www.example.com /index.html\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    http_get(argv[1], argv[2]);
    return 0;
}
```

**Utilisation :**
```bash
gcc -o http_client http_client.c
./http_client www.example.com /
```

---

## En Résumé

La résolution de noms est essentielle pour créer des applications réseau robustes et maintenables. `getaddrinfo()` est l'API moderne et recommandée.

**Points clés à retenir :**

✅ **Utiliser `getaddrinfo()`** (pas `gethostbyname()`)

✅ **Essayer toutes les adresses** retournées (boucle sur la liste)

✅ **Toujours appeler `freeaddrinfo()`** pour libérer la mémoire

✅ **Utiliser `AF_UNSPEC`** pour supporter IPv4 et IPv6

✅ **Gérer les erreurs** avec `gai_strerror()`

✅ **Utiliser `AI_PASSIVE`** pour les serveurs

✅ **`getnameinfo()`** pour résolution inverse (IP → nom)

✅ **Comprendre la configuration** (/etc/hosts, /etc/resolv.conf)

**Avantages de `getaddrinfo()` :**
- Support IPv4 et IPv6 transparent
- Thread-safe
- API moderne et standard
- Gère automatiquement les détails des structures

**Workflow typique :**
```c
// 1. Préparer hints
struct addrinfo hints = {
    .ai_family = AF_UNSPEC,
    .ai_socktype = SOCK_STREAM
};

// 2. Résoudre
struct addrinfo *result;  
getaddrinfo("google.com", "80", &hints, &result);  

// 3. Boucler et essayer
for (rp = result; rp; rp = rp->ai_next) {
    sockfd = socket(rp->ai_family, ...);
    if (connect(sockfd, rp->ai_addr, ...) == 0) break;
    close(sockfd);
}

// 4. Libérer
freeaddrinfo(result);
```

Dans la prochaine section, nous verrons comment créer des serveurs capables de gérer plusieurs clients simultanément.

---

**→ Prochaine section : 20.8 Serveur Concurrent**

⏭️ [Serveur concurrent](/20-reseau-sockets/08-serveur-concurrent.md)
