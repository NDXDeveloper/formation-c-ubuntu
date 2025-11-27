🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 20.1 Modèle Client/Serveur

## Introduction

Le modèle **client/serveur** est l'architecture fondamentale qui régit la majorité des communications sur Internet et les réseaux locaux. Que vous naviguiez sur le web, envoyiez un email, ou regardiez une vidéo en streaming, vous utilisez ce modèle sans même y penser.

Avant de plonger dans le code des sockets en C, il est essentiel de bien comprendre ce modèle conceptuel, car il détermine comment nos programmes vont interagir sur le réseau.

---

## Qu'est-ce que le modèle Client/Serveur ?

Le modèle client/serveur est un paradigme de communication réseau où deux entités distinctes jouent des rôles complémentaires :

### Le Serveur
Le **serveur** est un programme qui :
- **Écoute** en permanence sur un port réseau spécifique
- **Attend** passivement les demandes de connexion
- **Répond** aux requêtes des clients
- **Fournit** des services ou des ressources (fichiers, données, calculs, etc.)
- Peut gérer **plusieurs clients simultanément**

**Analogie :** Imaginez un serveur comme un restaurant. Il est toujours ouvert (écoute), attend que des clients arrivent, prend leurs commandes (requêtes), et leur sert ce qu'ils demandent (réponses).

### Le Client
Le **client** est un programme qui :
- **Initie** la connexion vers le serveur
- **Envoie** des requêtes ou demandes
- **Reçoit** les réponses du serveur
- **Utilise** les services fournis par le serveur
- Se connecte généralement de manière temporaire

**Analogie :** Le client est comme vous lorsque vous entrez dans un restaurant. Vous initiez l'interaction, passez commande, recevez votre repas, puis partez.

---

## Caractéristiques fondamentales

### 1. Asymétrie des rôles

Le serveur et le client ont des responsabilités différentes :

| Aspect | Serveur | Client |
|--------|---------|--------|
| **Initialisation** | Démarre en premier et reste actif | Démarre quand l'utilisateur en a besoin |
| **Connexion** | Attend passivement les connexions | Initie activement la connexion |
| **Adresse** | Adresse fixe et connue | Adresse dynamique (souvent) |
| **Disponibilité** | Disponible 24/7 (idéalement) | Disponible temporairement |
| **Multiplexage** | Gère plusieurs clients en parallèle | Se connecte à un ou plusieurs serveurs |

### 2. Communication initiée par le client

C'est **toujours le client** qui initie la communication. Le serveur ne peut pas "appeler" un client de manière spontanée (sauf architectures spécifiques comme WebSockets ou Server Push).

```
Client ──[Demande de connexion]──> Serveur
Client <─[Acceptation]─────────── Serveur
Client ──[Requête]────────────────> Serveur
Client <─[Réponse]────────────── Serveur
```

### 3. Indépendance technologique

Le client et le serveur peuvent être :
- Écrits dans des **langages différents** (client en Python, serveur en C)
- Exécutés sur des **systèmes d'exploitation différents** (client Windows, serveur Linux)
- Situés sur des **machines différentes** (client à Paris, serveur à Tokyo)

Ce qui compte, c'est qu'ils parlent le même **protocole** (ensemble de règles de communication).

---

## Exemples concrets du modèle Client/Serveur

### Navigation Web
- **Client :** Votre navigateur (Chrome, Firefox, etc.)
- **Serveur :** Le serveur web qui héberge le site (Apache, Nginx)
- **Protocole :** HTTP/HTTPS
- **Communication :**
  1. Vous tapez `www.example.com` dans votre navigateur
  2. Le navigateur (client) envoie une requête HTTP GET au serveur
  3. Le serveur répond avec la page HTML
  4. Votre navigateur affiche la page

### Email
- **Client :** Votre logiciel de messagerie (Thunderbird, Outlook)
- **Serveur :** Serveur SMTP (envoi) et IMAP/POP3 (réception)
- **Protocole :** SMTP, IMAP, ou POP3
- **Communication :**
  1. Vous écrivez un email et cliquez sur "Envoyer"
  2. Votre client se connecte au serveur SMTP
  3. Le serveur SMTP transmet l'email au serveur du destinataire
  4. Le client du destinataire récupère l'email via IMAP/POP3

### Jeux en ligne
- **Client :** Le jeu installé sur votre ordinateur/console
- **Serveur :** Serveur de jeu centralisé
- **Protocole :** Protocole propriétaire (souvent basé sur UDP ou TCP)
- **Communication :**
  1. Le client envoie vos actions (déplacements, tirs, etc.)
  2. Le serveur traite les actions de tous les joueurs
  3. Le serveur renvoie l'état mis à jour du monde de jeu
  4. Votre client affiche les changements

### Base de données
- **Client :** Application métier, script Python, etc.
- **Serveur :** Serveur de base de données (PostgreSQL, MySQL)
- **Protocole :** Protocole spécifique à la BD (PostgreSQL Protocol, MySQL Protocol)
- **Communication :**
  1. Le client envoie une requête SQL
  2. Le serveur exécute la requête
  3. Le serveur renvoie les résultats
  4. Le client traite les données

---

## Flux typique d'une communication Client/Serveur

Voici le cycle de vie complet d'une communication client/serveur :

### Côté Serveur

1. **Création du socket**
   - Le serveur crée un point de communication réseau

2. **Liaison (Bind)**
   - Le serveur s'attache à une adresse IP et un port spécifiques
   - Exemple : `192.168.1.10:8080`

3. **Écoute (Listen)**
   - Le serveur se met en mode "écoute"
   - Il attend activement les connexions entrantes

4. **Acceptation (Accept)**
   - Lorsqu'un client se connecte, le serveur accepte la connexion
   - Un nouveau socket est créé pour cette communication spécifique

5. **Communication**
   - Le serveur lit les données envoyées par le client
   - Il traite la requête
   - Il envoie une réponse

6. **Fermeture**
   - La connexion avec ce client est fermée
   - Le serveur retourne à l'étape "Accept" pour d'autres clients

### Côté Client

1. **Création du socket**
   - Le client crée son point de communication réseau

2. **Connexion (Connect)**
   - Le client initie une connexion vers l'adresse du serveur
   - Il spécifie l'IP et le port du serveur

3. **Communication**
   - Le client envoie sa requête
   - Il attend et lit la réponse du serveur

4. **Fermeture**
   - Le client ferme la connexion

### Diagramme séquentiel

```
SERVEUR                           CLIENT
   |                                 |
   | 1. socket()                     |
   | 2. bind()                       |
   | 3. listen()                     |
   | 4. accept()                     |
   |     (attente...)                |
   |                                 | 1. socket()
   |                                 | 2. connect() ------>
   |<--------- Connexion établie -------->|
   |                                 |
   | 5. read() <----------- 3. write() (requête)
   | (traitement)                    |
   | 6. write() (réponse) ---------> 4. read()
   |                                 |
   | 7. close()              5. close()
   |                                 |
```

---

## Adressage : IP et Ports

Pour qu'un client puisse se connecter à un serveur, il a besoin de deux informations cruciales :

### 1. L'adresse IP
L'**adresse IP** identifie de manière unique une machine sur le réseau.

- **IPv4 :** Format `192.168.1.10` (4 octets, environ 4 milliards d'adresses)
- **IPv6 :** Format `2001:0db8:85a3::8a2e:0370:7334` (16 octets, nombre astronomique d'adresses)

**Adresses spéciales :**
- `127.0.0.1` (localhost) : Désigne la machine locale (boucle locale)
- `0.0.0.0` : Toutes les interfaces réseau de la machine

### 2. Le port
Le **port** est un numéro qui identifie une application ou un service spécifique sur la machine.

- **Plage :** 0 à 65535 (16 bits)
- **Ports privilégiés :** 0-1023 (nécessitent les droits root sous Linux)
- **Ports enregistrés :** 1024-49151 (enregistrés auprès de l'IANA)
- **Ports dynamiques :** 49152-65535 (utilisés temporairement par les clients)

**Ports standards bien connus :**
- Port 80 : HTTP (web non sécurisé)
- Port 443 : HTTPS (web sécurisé)
- Port 22 : SSH (connexion sécurisée)
- Port 25 : SMTP (envoi d'emails)
- Port 21 : FTP (transfert de fichiers)
- Port 3306 : MySQL
- Port 5432 : PostgreSQL

**Analogie :** Si l'adresse IP est l'adresse d'un immeuble, le port est le numéro d'appartement dans cet immeuble.

---

## Types de serveurs : Concurrent vs Itératif

### Serveur Itératif
Un serveur **itératif** traite les clients **un par un**, séquentiellement.

**Fonctionnement :**
1. Accepte une connexion
2. Traite complètement la requête
3. Ferme la connexion
4. Accepte le client suivant

**Avantages :**
- Simple à implémenter
- Pas de problèmes de concurrence

**Inconvénients :**
- Si un client est lent, tous les autres attendent
- Mauvaise utilisation des ressources
- Non adapté aux applications réelles

**Cas d'usage :**
- Serveurs de test/développement
- Services très simples et rapides
- Environnements contrôlés avec peu de clients

### Serveur Concurrent
Un serveur **concurrent** peut gérer **plusieurs clients simultanément**.

**Stratégies d'implémentation :**

1. **Multi-processus (fork)**
   - Un processus fils par client
   - Isolation complète
   - Coût mémoire élevé

2. **Multi-threads (pthreads)**
   - Un thread par client
   - Partage de mémoire
   - Plus léger que les processus

3. **I/O multiplexing (select, poll, epoll)**
   - Un seul processus/thread
   - Gère plusieurs connexions via des événements
   - Très efficace pour beaucoup de connexions

4. **I/O asynchrone (event-driven)**
   - Architecture basée sur les callbacks
   - Non bloquant
   - Haute performance (Node.js, Nginx)

**Avantages :**
- Plusieurs clients servis en parallèle
- Meilleure utilisation des ressources
- Essentiel pour applications réelles

**Inconvénients :**
- Plus complexe à développer
- Gestion de la concurrence (synchronisation)
- Risque de bugs (race conditions, deadlocks)

---

## Protocoles de communication

Un **protocole** est un ensemble de règles qui définit comment le client et le serveur communiquent.

### Éléments d'un protocole

1. **Format des messages**
   - Structure des données échangées
   - Encodage (texte, binaire, JSON, XML, etc.)

2. **Séquence des messages**
   - Qui parle en premier ?
   - Ordre des requêtes/réponses

3. **Gestion des erreurs**
   - Codes d'erreur
   - Mécanismes de retry

4. **Terminaison**
   - Comment se termine la communication ?

### Exemple : Protocole HTTP simplifié

**Requête du client :**
```
GET /index.html HTTP/1.1
Host: www.example.com
User-Agent: Mozilla/5.0

```

**Réponse du serveur :**
```
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 1234

<html>
  <body>Hello World</body>
</html>
```

### Exemple : Protocole personnalisé simple

Imaginons un protocole d'écho basique :

**Format :**
- Le client envoie une ligne de texte terminée par `\n`
- Le serveur renvoie la même ligne en MAJUSCULES
- Le client envoie "QUIT\n" pour terminer

**Échange :**
```
Client → Serveur: "hello\n"
Serveur → Client: "HELLO\n"
Client → Serveur: "world\n"
Serveur → Client: "WORLD\n"
Client → Serveur: "QUIT\n"
[Connexion fermée]
```

---

## Avantages du modèle Client/Serveur

### 1. Centralisation
- Les ressources sont centralisées sur le serveur
- Facilite la maintenance et les mises à jour
- Un seul point à sécuriser et à sauvegarder

### 2. Contrôle et sécurité
- Le serveur contrôle l'accès aux ressources
- Authentification centralisée
- Politique de sécurité unifiée

### 3. Scalabilité
- Possibilité d'ajouter plus de serveurs (load balancing)
- Mise à l'échelle indépendante du client et du serveur

### 4. Accessibilité
- Les clients peuvent être légers (thin clients)
- Accès depuis n'importe quel appareil
- Indépendance de plateforme

### 5. Cohérence des données
- Une seule source de vérité (le serveur)
- Pas de problèmes de synchronisation entre clients

---

## Inconvénients et défis

### 1. Point de défaillance unique
- Si le serveur tombe, le service est indisponible
- **Solution :** Redondance, haute disponibilité

### 2. Goulot d'étranglement
- Le serveur peut devenir saturé
- **Solution :** Load balancing, mise à l'échelle horizontale

### 3. Latence réseau
- La communication réseau introduit des délais
- **Solution :** Caching, CDN, optimisation protocole

### 4. Dépendance réseau
- Nécessite une connexion réseau fonctionnelle
- **Solution :** Mode hors ligne, synchronisation différée

### 5. Coûts
- Infrastructure serveur coûteuse
- Maintenance, bande passante
- **Solution :** Cloud, serveurs mutualisés

---

## Variantes et évolutions du modèle

### Peer-to-Peer (P2P)
Les machines sont à la fois clients et serveurs.
- **Exemples :** BitTorrent, Bitcoin
- **Avantages :** Pas de serveur central, décentralisé
- **Inconvénients :** Complexité, sécurité

### Architecture 3-tiers
Séparation en trois couches :
1. **Présentation** (client web)
2. **Logique métier** (serveur d'application)
3. **Données** (serveur de base de données)

### Microservices
Architecture où plusieurs serveurs spécialisés communiquent entre eux.
- Chaque service est un serveur pour certains, un client pour d'autres
- **Exemples :** API REST, architecture Kubernetes

### Architecture serverless
Le "serveur" est géré automatiquement par le cloud provider.
- **Exemples :** AWS Lambda, Google Cloud Functions
- Le développeur ne voit que des fonctions

---

## En résumé

Le modèle client/serveur est la colonne vertébrale d'Internet et des applications réseau modernes. Comprendre ses principes est essentiel avant de coder des applications réseau en C.

**Points clés à retenir :**

✅ Le **serveur** écoute passivement, le **client** initie la connexion

✅ L'adressage se fait via **IP + Port**

✅ Un **protocole** définit les règles de communication

✅ Les serveurs **concurrents** sont essentiels en production

✅ Ce modèle est omniprésent : web, email, jeux, bases de données, etc.

Dans les sections suivantes, nous allons voir comment implémenter concrètement ce modèle en C avec l'API des **sockets**, en commençant par l'architecture TCP/IP.

---

**→ Prochaine section : 20.2 Architecture TCP/IP**

⏭️ [Architecture TCP/IP](/20-reseau-sockets/02-architecture-tcp-ip.md)
