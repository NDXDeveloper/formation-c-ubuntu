🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 34.4 Serveur Web Minimaliste

## Introduction

Dans cette étude de cas DevOps, nous allons construire un **serveur web HTTP complet** en C. Ce projet synthétise de nombreux concepts que nous avons étudiés tout au long de cette formation : programmation système, gestion de la mémoire, sockets réseau, architecture logicielle, et bonnes pratiques de développement.

Un serveur web est un programme qui écoute sur un port réseau, accepte des connexions de clients (navigateurs web), reçoit des requêtes HTTP, et renvoie des réponses (pages HTML, images, CSS, etc.). C'est un composant fondamental de l'infrastructure web moderne.

---

## Qu'est-ce qu'un Serveur Web ?

### Définition

Un **serveur web** (ou serveur HTTP) est un logiciel qui :

1. **Écoute** sur un port TCP (généralement 80 pour HTTP, 443 pour HTTPS)
2. **Accepte** les connexions de clients distants
3. **Reçoit** des requêtes HTTP (GET, POST, HEAD, etc.)
4. **Traite** ces requêtes (lire des fichiers, exécuter du code, etc.)
5. **Envoie** des réponses HTTP avec le contenu demandé
6. **Gère** plusieurs clients simultanément

### Exemples de serveurs web célèbres

| Serveur | Langage | Part de marché | Caractéristiques |
|---------|---------|----------------|------------------|
| **Apache** | C | ~30% | Modulaire, .htaccess, stable |
| **nginx** | C | ~35% | Event-driven, très performant |
| **lighttpd** | C | ~1% | Léger, FastCGI |
| **IIS** | C++ | ~5% | Windows, intégration .NET |
| **Caddy** | Go | <1% | HTTPS automatique, moderne |

**Notre serveur :** Nous créerons un serveur minimaliste mais fonctionnel, inspiré de nginx et lighttpd, capable de servir des fichiers statiques à des milliers de clients simultanés.

---

## Pourquoi Créer un Serveur Web en C ?

### Avantages du C pour un Serveur Web

✅ **Performance exceptionnelle**
- Pas de garbage collector (latence prévisible)
- Accès direct aux appels système
- Optimisations au niveau du compilateur

✅ **Consommation mémoire minimale**
- nginx : ~10 Mo par worker
- Apache : ~5-50 Mo par processus
- Notre serveur : ~2-5 Mo

✅ **Scalabilité**
- Contrôle total sur l'architecture
- Event loop efficace avec epoll/kqueue
- Peut gérer 10 000+ connexions simultanées

✅ **Compréhension profonde**
- Apprentissage des mécanismes réseau
- Maîtrise du protocole HTTP
- Patterns d'architecture événementielle

✅ **Portabilité**
- Compile sur Linux, BSD, macOS
- Binaire statique déployable partout
- Pas de dépendances runtime

### Cas d'usage réels

**Où un serveur C est-il pertinent ?**

- 🎯 **Systèmes embarqués** : Routeurs, IoT, dispositifs à ressources limitées
- 🎯 **CDN / Edge computing** : Servir du contenu à très haute performance
- 🎯 **Microservices légers** : Endpoints HTTP ultra-rapides
- 🎯 **Apprentissage** : Comprendre les fondamentaux du web
- 🎯 **Infrastructure critique** : Où la fiabilité et la prédictibilité sont essentielles

---

## Vue d'Ensemble : Ce que Nous Allons Construire

### Fonctionnalités de notre serveur

Notre serveur web minimaliste supportera :

📄 **Serving de fichiers statiques**
- HTML, CSS, JavaScript
- Images (PNG, JPG, GIF, SVG)
- Documents (PDF, TXT)
- Fonts, vidéos, archives

🌐 **Protocole HTTP/1.1**
- Méthodes GET et HEAD
- Headers standards
- Codes de statut (200, 404, 403, 500, etc.)
- Types MIME automatiques

⚡ **Architecture event-driven**
- Modèle asynchrone non-bloquant
- Multiplexage I/O avec epoll (Linux)
- Gestion de milliers de connexions simultanées
- Consommation CPU optimale

🔒 **Sécurité de base**
- Protection contre path traversal
- Validation des requêtes
- Limites de taille
- Timeouts

📊 **Features modernes**
- Logging des requêtes (format Apache)
- Détection automatique des types MIME
- Support des répertoires avec index.html
- Gestion propre des erreurs

### Ce que nous NE ferons PAS

Pour garder le projet pédagogique et maintenable :

- ❌ HTTPS/TLS (mais vous pourrez l'ajouter avec OpenSSL)
- ❌ HTTP/2 ou HTTP/3 (protocoles binaires complexes)
- ❌ CGI/FastCGI (exécution de scripts dynamiques)
- ❌ Virtual hosts (plusieurs sites sur un serveur)
- ❌ Compression gzip à la volée (optionnel, ajout possible)
- ❌ WebSockets (protocole différent)
- ❌ Authentification (Basic Auth serait possible)

**Objectif :** Un serveur **simple, rapide, et éducatif**, capable de servir un site web statique en production.

---

## Architecture Globale du Serveur

### Diagramme de l'architecture

```
┌────────────────────────────────────────────────────────────────┐
│                    SERVEUR WEB MINIMALISTE                     │
│                                                                │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  1. BOUCLE ÉVÉNEMENTIELLE (Event Loop)                  │   │
│  │     • epoll_wait() attend les événements réseau         │   │
│  │     • Gestion asynchrone des I/O                        │   │
│  │     • Scalabilité : 1 thread → 10k+ connexions          │   │
│  └──────────────────┬──────────────────────────────────────┘   │
│                     │                                          │
│                     ▼                                          │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  2. PARSER HTTP                                         │   │
│  │     • Analyse de la request line (GET /index.html)      │   │
│  │     • Extraction des headers (Host, User-Agent)         │   │
│  │     • Validation du protocole                           │   │
│  └──────────────────┬──────────────────────────────────────┘   │
│                     │                                          │
│                     ▼                                          │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  3. GESTIONNAIRE DE FICHIERS STATIQUES                  │   │
│  │     • Conversion URI → Chemin disque                    │   │
│  │     • Vérification sécurité (path traversal)            │   │
│  │     • Lecture fichier (sendfile zéro-copie)             │   │
│  │     • Détermination type MIME                           │   │
│  │     • Génération réponse HTTP                           │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                │
└────────────────────────────────────────────────────────────────┘
                            │
                            │ HTTP/TCP
                            ▼
                 ┌──────────────────────┐
                 │   Clients (Web)      │
                 │   • Firefox          │
                 │   • Chrome           │
                 │   • curl             │
                 │   • wget             │
                 └──────────────────────┘
```

### Flux d'une requête complète

```
1. CLIENT envoie:
   ┌─────────────────────────────────────┐
   │ GET /images/logo.png HTTP/1.1       │
   │ Host: localhost:8080                │
   │ User-Agent: Mozilla/5.0             │
   │                                     │
   └─────────────────────────────────────┘

2. SERVEUR (Boucle événementielle):
   epoll_wait() détecte → Nouvelle donnée sur socket

3. PARSER HTTP:
   ├─> Méthode: GET
   ├─> URI: /images/logo.png
   ├─> Version: HTTP/1.1
   └─> Headers: {Host: localhost:8080, ...}

4. GESTIONNAIRE DE FICHIERS:
   ├─> URI → /var/www/html/images/logo.png
   ├─> Vérifier sécurité (pas de ../..)
   ├─> Vérifier existence et permissions
   ├─> Type MIME: image/png
   ├─> Lire le fichier (8 KB)
   └─> Construire réponse HTTP

5. SERVEUR envoie:
   ┌─────────────────────────────────────┐
   │ HTTP/1.1 200 OK                     │
   │ Content-Type: image/png             │
   │ Content-Length: 8192                │
   │                                     │
   │ [binary data: PNG image]            │
   └─────────────────────────────────────┘

6. CLIENT affiche l'image dans le navigateur
```

---

## Les Trois Piliers du Serveur

Notre implémentation se divise en **trois composants principaux**, correspondant aux trois sous-sections :

### 1. Architecture Event-Driven (Section 34.4.1)

**Problème :** Comment gérer 1000 clients simultanés sans créer 1000 threads ?

**Solution :** Architecture événementielle avec epoll

**Concepts clés :**
- Multiplexage I/O (select, poll, epoll)
- Sockets non-bloquants
- Boucle d'événements (event loop)
- Edge-triggered vs Level-triggered
- Scalabilité O(1) avec epoll

**Ce que vous apprendrez :**
- Créer un serveur TCP avec `socket()`, `bind()`, `listen()`, `accept()`
- Utiliser `epoll_create()`, `epoll_ctl()`, `epoll_wait()`
- Gérer les événements réseau de manière asynchrone
- Comprendre pourquoi nginx est si performant

**Fichiers :** `event_loop.c`, `event_loop.h`

### 2. HTTP Parsing (Section 34.4.2)

**Problème :** Comment interpréter les requêtes HTTP textuelles ?

**Solution :** Parser robuste avec validation

**Concepts clés :**
- Structure d'une requête HTTP (request line, headers, body)
- Parsing de chaînes en C (`sscanf`, `strchr`, parsing manuel)
- Validation et sécurité (injection, taille max)
- Génération de réponses HTTP
- Codes de statut (200, 404, 403, 500)

**Ce que vous apprendrez :**
- Lire et parser le protocole HTTP/1.1
- Extraire méthode, URI, version, headers
- Générer des réponses HTTP valides
- Gérer les erreurs proprement

**Fichiers :** `http_parser.c`, `http_parser.h`

### 3. Static File Serving (Section 34.4.3)

**Problème :** Comment servir des fichiers depuis le disque de manière sécurisée et efficace ?

**Solution :** Lecture optimisée avec sendfile() et validation

**Concepts clés :**
- Conversion URI → chemin système
- Sécurité : path traversal, permissions
- Appels système : `stat()`, `access()`, `open()`, `sendfile()`
- Types MIME et Content-Type
- Cache HTTP (Last-Modified, ETag)

**Ce que vous apprendrez :**
- Mapper les URLs vers le système de fichiers
- Prévenir les attaques path traversal
- Utiliser `sendfile()` pour zéro-copie
- Détecter automatiquement les types MIME
- Implémenter le cache HTTP

**Fichiers :** `file_handler.c`, `file_handler.h`

---

## Technologies et Concepts Utilisés

### Programmation Système Linux

**Sockets POSIX**
- `socket()` : Création d'un socket TCP/IP
- `bind()` : Association à un port
- `listen()` : Mise en mode écoute
- `accept()` : Acceptation de connexions
- `read()` / `write()` : Communication

**Multiplexage I/O**
- `select()` : Surveiller plusieurs descripteurs (ancien)
- `poll()` : Amélioration de select() (portable)
- `epoll()` : API haute performance Linux

**Fichiers**
- `open()`, `read()`, `close()` : Manipulation de fichiers
- `stat()`, `fstat()` : Informations sur les fichiers
- `access()` : Vérification des permissions
- `sendfile()` : Transfert zéro-copie (Linux)
- `realpath()` : Résolution de chemins

### Protocole HTTP

**HTTP/1.1 (RFC 7230-7235)**
- Format textuel des requêtes/réponses
- Méthodes : GET, HEAD, POST, PUT, DELETE
- Headers : Host, User-Agent, Content-Type, Content-Length
- Codes de statut : 2xx (succès), 4xx (client), 5xx (serveur)
- Types MIME : text/html, image/png, application/json

**Caractéristiques HTTP/1.1**
- Connexions persistantes (keep-alive)
- Chunked transfer encoding
- Range requests (téléchargement partiel)
- Cache (Last-Modified, ETag, If-Modified-Since)

### Patterns d'Architecture

**Event-Driven Architecture**
- Réaction aux événements (connexions, données)
- Modèle asynchrone non-bloquant
- Utilisé par : nginx, Node.js, Redis, HAProxy

**Reactor Pattern**
- Boucle événementielle centrale
- Handlers pour chaque type d'événement
- Démultiplexage des I/O

**State Machine**
- Gestion du cycle de vie des connexions
- États : READING_REQUEST, PROCESSING, SENDING_RESPONSE
- Transitions d'états événementielles

---

## Structure du Projet

### Organisation des fichiers

```
webserver/
├── src/
│   ├── main.c                  # Point d'entrée
│   ├── event_loop.c            # Boucle époll
│   ├── event_loop.h
│   ├── http_parser.c           # Parsing HTTP
│   ├── http_parser.h
│   ├── file_handler.c          # Serving fichiers
│   ├── file_handler.h
│   ├── mime_types.c            # Détection MIME
│   ├── mime_types.h
│   ├── logger.c                # Logging
│   └── logger.h
├── include/
│   └── common.h                # Définitions communes
├── www/                        # Document root
│   ├── index.html
│   ├── css/
│   │   └── style.css
│   ├── js/
│   │   └── script.js
│   └── images/
│       └── logo.png
├── config/
│   └── server.conf             # Configuration (optionnel)
├── tests/
│   ├── test_parser.c
│   └── test_file_handler.c
├── scripts/
│   ├── benchmark.sh
│   └── run_tests.sh
├── Makefile
├── CMakeLists.txt              # Build avec CMake
├── README.md
└── .gitignore
```

### Dépendances

**Aucune dépendance externe !**

Notre serveur utilise uniquement :
- La libc standard (glibc sur Linux)
- Les appels système POSIX
- L'API epoll de Linux

**Avantages :**
- Compilation triviale sur tout Linux
- Binaire statique possible
- Empreinte disque minimale (~100 Ko)
- Pas de gestion de dépendances

---

## Prérequis Techniques

### Connaissances nécessaires

Pour suivre ce chapitre, vous devez maîtriser :

✅ **C Fondamental** (Modules 1-4)
- Pointeurs et gestion mémoire
- Structures de données
- Chaînes de caractères en C
- Allocation dynamique

✅ **Programmation Système** (Module 6)
- Descripteurs de fichiers
- Appels système de base
- Sockets TCP/IP
- Processus et signaux

✅ **Outillage** (Module 5)
- Compilation avec gcc/make
- Debugging avec GDB
- Valgrind pour fuites mémoire
- strace pour tracer les appels système

### Environnement de développement

**Système d'exploitation :**
- Linux (Ubuntu 20.04+ recommandé)
- Debian, Fedora, Arch aussi OK
- Note : epoll est spécifique à Linux (pour BSD/macOS, utiliser kqueue)

**Outils requis :**
```bash
# Compiler et debugger
sudo apt install build-essential gdb valgrind strace

# Outils réseau
sudo apt install net-tools curl wget

# Test de charge
sudo apt install apache2-utils  # pour ab (Apache Bench)

# Optionnel : wrk pour benchmarks avancés
git clone https://github.com/wg/wrk.git
cd wrk && make
```

**Vérification :**
```bash
# Vérifier GCC
gcc --version
# GCC 9.0+ recommandé

# Vérifier support epoll
grep epoll /usr/include/sys/epoll.h
# Doit afficher les définitions epoll
```

---

## Objectifs Pédagogiques

### Compétences techniques

À la fin de ce chapitre, vous serez capable de :

- 🎯 **Créer un serveur TCP/IP** à partir de zéro
- 🎯 **Implémenter une architecture event-driven** avec epoll
- 🎯 **Parser le protocole HTTP/1.1** correctement
- 🎯 **Servir des fichiers statiques** de manière sécurisée
- 🎯 **Gérer des milliers de connexions** simultanées
- 🎯 **Optimiser avec sendfile()** (zéro-copie)
- 🎯 **Sécuriser contre les attaques** courantes (path traversal)
- 🎯 **Logger et monitorer** un serveur en production
- 🎯 **Benchmarker et profiler** les performances

### Compétences DevOps

- 🎯 **Comprendre l'infrastructure web** moderne
- 🎯 **Analyser les performances** réseau
- 🎯 **Comparer les architectures** (threads vs event-loop)
- 🎯 **Déployer un service** Linux
- 🎯 **Monitorer en production** (logs, métriques)
- 🎯 **Debugger des problèmes** réseau et concurrence

---

## Métriques de Performance Attendues

### Sur une machine moderne (4 cores, 16 GB RAM)

**Connexions simultanées :**
- 10 000+ connexions : ✅ Facile
- 50 000+ connexions : ✅ Possible avec tuning kernel
- 100 000+ connexions : ✅ Possible (C10K problem résolu)

**Débit :**
- Fichiers petits (<10 KB) : 10 000-50 000 requêtes/seconde
- Fichiers moyens (100 KB) : 1 000-5 000 requêtes/seconde
- Fichiers gros (1 MB) : Limité par le débit réseau (1 Gbps ≈ 125 MB/s)

**Latence :**
- Temps de réponse moyen : 0.5-2 ms (localhost)
- P99 (99e percentile) : <5 ms

**Consommation ressources :**
- Mémoire : 2-5 Mo (base) + ~1 Ko par connexion active
- CPU : <1% pour trafic léger, 100% d'un core pour saturation

**Comparaison avec nginx :**
```
Métrique           Notre serveur    nginx
──────────────────────────────────────────
Req/sec (static)   15k-30k          30k-50k
Latence (P50)      1-2 ms           0.5-1 ms
Mémoire (base)     2-5 Mo           10-20 Mo
```

**Note :** nginx est plus optimisé, mais notre serveur est dans le même ordre de grandeur !

---

## Méthodologie de Développement

### Approche itérative

Nous construirons le serveur **étape par étape** :

**Phase 1 : Serveur TCP basique**
```c
// Accepter des connexions, envoyer "Hello World"
int main() {
    int server_fd = create_socket(8080);
    while (1) {
        int client = accept(server_fd, ...);
        write(client, "Hello\n", 6);
        close(client);
    }
}
```

**Phase 2 : Ajout de epoll**
```c
// Gérer plusieurs clients avec event loop
int epoll_fd = epoll_create1(0);
while (1) {
    int n = epoll_wait(epoll_fd, events, MAX, -1);
    for (int i = 0; i < n; i++) {
        handle_event(events[i]);
    }
}
```

**Phase 3 : Parser HTTP**
```c
// Lire et parser les requêtes HTTP
http_request_t req;
parse_http_request(buffer, &req);
printf("GET %s HTTP/1.1\n", req.uri);
```

**Phase 4 : Servir des fichiers**
```c
// Lire et envoyer des fichiers
char path[PATH_MAX];
uri_to_path(req.uri, DOCUMENT_ROOT, path);
sendfile(client_fd, file_fd, NULL, file_size);
```

**Phase 5 : Intégration et polish**
- Gestion des erreurs robuste
- Logging complet
- Sécurité renforcée
- Tests de charge

### Tests continus

À chaque étape :

- ✅ **Compilation sans warnings** (`-Wall -Wextra -Werror`)
- ✅ **Tests manuels** avec curl et navigateur
- ✅ **Valgrind clean** (0 leaks)
- ✅ **Tests de charge** (ab, wrk)
- ✅ **Code review** (lisibilité, maintenabilité)

---

## Cas d'Usage Réels

### Scénario 1 : Site Web Statique

**Contexte :** Blog personnel avec Hugo/Jekyll générant du HTML statique.

**Solution :**
```bash
# Générer le site
hugo build -d /var/www/html

# Lancer le serveur
./webserver --port 8080 --root /var/www/html

# Résultat : Site accessible à http://example.com
```

**Avantages :**
- Pas de PHP/Node.js requis
- Très rapide (statique)
- Consommation mémoire minimale
- Facile à mettre en cache (CDN)

### Scénario 2 : Serveur de Fichiers Local

**Contexte :** Partager des fichiers sur un réseau local.

**Solution :**
```bash
# Servir le répertoire Documents
./webserver --port 8080 --root ~/Documents

# Accéder depuis un autre PC
firefox http://192.168.1.100:8080/
```

### Scénario 3 : Serveur de Développement

**Contexte :** Développer un site web frontend (React, Vue).

**Solution :**
```bash
# Build du projet React
npm run build  # → génère build/

# Servir avec notre serveur
./webserver --port 3000 --root build/

# Hot reload : relancer après chaque build
```

### Scénario 4 : Embedded Device

**Contexte :** Interface web sur un Raspberry Pi ou routeur.

**Solution :**
- Cross-compiler pour ARM
- Binaire statique de ~100 Ko
- Interface de configuration via HTML
- Faible consommation mémoire (2-3 Mo)

---

## Évolutions Possibles

Une fois le MVP fonctionnel, vous pourrez ajouter :

### Fonctionnalités Réseau

- 🔧 **HTTPS/TLS** : Chiffrement avec OpenSSL/mbedTLS
- 🔧 **HTTP/2** : Multiplexage, server push
- 🔧 **WebSockets** : Communication bidirectionnelle
- 🔧 **IPv6** : Support du protocole IPv6
- 🔧 **Keep-Alive** : Connexions persistantes

### Fonctionnalités Serveur

- 🔧 **Compression gzip** : Compression à la volée avec zlib
- 🔧 **Range Requests** : Téléchargement partiel (vidéos)
- 🔧 **Directory Listing** : Génération automatique d'index
- 🔧 **Virtual Hosts** : Plusieurs sites sur un serveur
- 🔧 **URL Rewriting** : Réécriture d'URLs (mod_rewrite)
- 🔧 **CGI/FastCGI** : Exécution de scripts dynamiques

### DevOps et Monitoring

- 🔧 **Prometheus metrics** : Exposition de métriques
- 🔧 **Health check endpoint** : `/health` pour orchestrateurs
- 🔧 **Access logs** : Format Apache/nginx
- 🔧 **Rate limiting** : Limitation de débit par IP
- 🔧 **Admin API** : Endpoints de gestion (stats, config)

### Performance

- 🔧 **Multi-threading** : Workers par cœur CPU
- 🔧 **io_uring** : API I/O Linux ultra-performante
- 🔧 **Cache mémoire** : Cache de fichiers fréquents
- 🔧 **Sendfile pour tout** : Optimisation zéro-copie

---

## Plan des Sous-Sections

Les trois prochaines sections détaillent l'implémentation complète :

### **34.4.1 Architecture Event-Driven** ⚡

**Objectif :** Créer une boucle événementielle capable de gérer des milliers de connexions.

**Contenu :**
- Problème : gérer plusieurs clients simultanés
- Comparaison : threads vs event-loop
- I/O blocking vs non-blocking
- Mécanismes : select(), poll(), epoll()
- Implémentation complète avec epoll
- Edge-triggered vs Level-triggered
- Patterns et bonnes pratiques

**Livrable :** Serveur TCP asynchrone avec epoll

---

### **34.4.2 HTTP Parsing** 📄

**Objectif :** Comprendre et parser le protocole HTTP/1.1.

**Contenu :**
- Structure d'une requête HTTP
- Parsing de la request line
- Parsing des headers
- Génération de réponses HTTP
- Codes de statut et erreurs
- Types MIME
- Sécurité et validation

**Livrable :** Parser HTTP robuste

---

### **34.4.3 Static File Serving** 📁

**Objectif :** Servir des fichiers depuis le disque de manière sécurisée et performante.

**Contenu :**
- Conversion URI → chemin système
- Sécurité : path traversal
- Lecture efficace avec sendfile()
- Gestion des répertoires et index.html
- Types MIME automatiques
- Cache HTTP (304 Not Modified)
- Gestion des erreurs (404, 403, 500)

**Livrable :** Serveur web complet fonctionnel

---

## Ressources Complémentaires

### Documentation

**Protocole HTTP :**
- RFC 7230 : HTTP/1.1 - Message Syntax and Routing
- RFC 7231 : HTTP/1.1 - Semantics and Content
- RFC 7232 : HTTP/1.1 - Conditional Requests
- RFC 7233 : HTTP/1.1 - Range Requests

**API Linux :**
- `man 7 epoll` : Documentation epoll
- `man 2 sendfile` : Transfert zéro-copie
- `man 7 socket` : Programmation sockets
- `man 2 accept` : Accepter des connexions

### Serveurs Open Source à Étudier

**Pour Apprendre :**
- **thttpd** : Ultra-simple, ~3000 lignes C
- **lighttpd** : Léger, bien structuré
- **Mongoose** : Embedded web server en un seul fichier

**Pour la Production :**
- **nginx** : Architecture event-driven de référence
- **Apache** : Modulaire, documentation exhaustive

### Livres

- Stevens & Rago, "Advanced Programming in the UNIX Environment"
  - Chapitre 16 : Network IPC (Sockets)
- Kerrisk, "The Linux Programming Interface"
  - Chapitres 60-63 : Sockets
- Tanenbaum, "Computer Networks"
  - HTTP et couche application

### Articles et Blogs

- **"The C10K Problem"** by Dan Kegel : Problème historique
- **"How nginx works"** : Architecture nginx expliquée
- **"Writing a Web Server in C"** : Nombreux tutoriels en ligne

---

## Vue d'Ensemble du Code Final

Pour vous donner une idée de la complexité, voici la taille approximative :

```
webserver/
├── event_loop.c        (~250 lignes)    # Boucle epoll
├── http_parser.c       (~350 lignes)    # Parsing HTTP
├── file_handler.c      (~300 lignes)    # Serving fichiers
├── mime_types.c        (~150 lignes)    # Table MIME
├── logger.c            (~100 lignes)    # Logging
├── main.c              (~150 lignes)    # Point d'entrée
└── Total: ~1300 lignes de code C
```

**Temps de développement :** 3-5 jours pour un développeur C intermédiaire
**Binaire compilé :** ~80-150 Ko (statique), ~40-60 Ko (dynamique)
**Consommation RAM :** ~2-5 Mo en fonctionnement
**Performance :** 10 000-30 000 requêtes/seconde (fichiers <10 KB)

**C'est un projet réaliste, déployable en production pour des cas d'usage simples, et extraordinairement pédagogique.**

---

## Conclusion de l'Introduction

Nous allons maintenant construire ce serveur web **composant par composant**, en commençant par l'architecture événementielle qui est la fondation de la performance.

**Points clés à retenir :**

- ✨ Un serveur web en C est **simple dans son principe** (sockets + HTTP + fichiers)
- ✨ La **performance** vient de l'architecture event-driven avec epoll
- ✨ La **sécurité** nécessite une validation rigoureuse (path traversal)
- ✨ C'est un projet **complet et réaliste** que vous pouvez utiliser réellement

**Prêt ?** Passons à la section **34.4.1 : Architecture Event-Driven** pour poser les fondations de notre serveur haute performance !

---

## Aperçu de l'Utilisation Finale

À la fin de ce chapitre, vous aurez un serveur utilisable ainsi :

```bash
# Compilation
make

# Lancement simple
./webserver

# Lancement avec options
./webserver --port 8080 --root /var/www/html --workers 4

# Test
curl http://localhost:8080/
curl http://localhost:8080/style.css
curl http://localhost:8080/images/logo.png

# Benchmark
ab -n 10000 -c 100 http://localhost:8080/index.html
wrk -t4 -c400 -d30s http://localhost:8080/

# Résultat attendu:
# Requests per second: 15000-30000 [#/sec]
# Latency (avg): 1-2 ms
# No memory leaks (Valgrind clean)
```

**Votre serveur sera :**
- ✅ Fonctionnel (navigateurs web)
- ✅ Performant (10k+ req/s)
- ✅ Sécurisé (validation stricte)
- ✅ Maintenable (code clair)
- ✅ Production-ready (pour sites statiques)

---

**Prochaine section :** 34.4.1 Architecture Event-Driven

⏭️ [Architecture event-driven](/34-etudes-cas-devops/04.1-architecture-event-driven.md)
