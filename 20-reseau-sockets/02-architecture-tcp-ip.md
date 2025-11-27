🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 20.2 Architecture TCP/IP

## Introduction

L'architecture **TCP/IP** (Transmission Control Protocol/Internet Protocol) est le fondement technique d'Internet et de la plupart des réseaux modernes. C'est le système qui permet à votre ordinateur de communiquer avec des serveurs à l'autre bout du monde de manière fiable et ordonnée.

Comprendre cette architecture est essentiel pour programmer des applications réseau en C, car les **sockets** que nous allons utiliser sont directement basés sur ce modèle.

---

## Le modèle en couches : Une approche modulaire

L'architecture TCP/IP repose sur un principe fondamental : la **séparation en couches**. Chaque couche a une responsabilité spécifique et communique uniquement avec les couches adjacentes.

### Pourquoi des couches ?

Imaginez que vous voulez envoyer une lettre à l'étranger :

1. Vous **écrivez** le contenu (ce que vous voulez dire)
2. Vous **mettez** la lettre dans une enveloppe avec une adresse
3. Vous **déposez** l'enveloppe dans une boîte aux lettres
4. Le **service postal** achemine la lettre
5. Le **facteur local** livre la lettre au destinataire

Chaque étape est indépendante. Le facteur n'a pas besoin de connaître le contenu de la lettre, et vous n'avez pas besoin de savoir comment le service postal achemine le courrier. C'est exactement le principe des couches réseau.

### Les 4 couches du modèle TCP/IP

```
┌─────────────────────────────────┐
│   Couche 4 : Application        │  HTTP, FTP, SSH, DNS
├─────────────────────────────────┤
│   Couche 3 : Transport          │  TCP, UDP
├─────────────────────────────────┤
│   Couche 2 : Internet           │  IP (IPv4, IPv6), ICMP
├─────────────────────────────────┤
│   Couche 1 : Accès réseau       │  Ethernet, WiFi, Câble
└─────────────────────────────────┘
```

**Principe clé :** Chaque couche **encapsule** les données de la couche supérieure.

---

## Couche 1 : Accès réseau (Link Layer)

### Rôle
La couche d'**accès réseau** gère la transmission physique des bits sur le média (câble, WiFi, fibre optique).

### Responsabilités
- Transmission des **trames** (frames) sur le réseau local
- Adressage matériel avec les **adresses MAC**
- Détection d'erreurs de transmission
- Contrôle d'accès au média (qui peut parler et quand ?)

### Protocoles typiques
- **Ethernet** : Réseaux locaux câblés
- **WiFi (802.11)** : Réseaux sans fil
- **PPP** : Connexions point-à-point (modems)

### Adresse MAC
Chaque carte réseau possède une adresse MAC unique (6 octets) :
```
Exemple : 00:1A:2B:3C:4D:5E
```

**Analogie :** C'est comme le numéro de châssis d'une voiture : unique et gravé dans le matériel.

### Ce que le programmeur doit savoir
En tant que développeur C travaillant avec des sockets, vous **n'interagissez presque jamais directement** avec cette couche. Le système d'exploitation s'en charge automatiquement.

---

## Couche 2 : Internet (Network Layer)

### Rôle
La couche **Internet** gère l'**acheminement des paquets** à travers plusieurs réseaux interconnectés. C'est elle qui permet la communication entre des machines sur des réseaux différents.

### Protocole principal : IP (Internet Protocol)

**IP** est le protocole central d'Internet. Il définit :
- Comment **adresser** les machines (adresses IP)
- Comment **fragmenter** les données en paquets
- Comment **router** (acheminer) les paquets d'un réseau à un autre

### Deux versions d'IP

#### IPv4 (Internet Protocol version 4)
- Format : 4 octets (32 bits)
- Notation décimale pointée : `192.168.1.10`
- Nombre d'adresses : ~4,3 milliards
- **Problème :** Épuisement des adresses disponibles

**Structure d'une adresse IPv4 :**
```
192.168.1.10
│   │   │ │
│   │   │ └─ Hôte
│   │   └─── Sous-réseau
│   └─────── Réseau
└─────────── Classe
```

#### IPv6 (Internet Protocol version 6)
- Format : 16 octets (128 bits)
- Notation hexadécimale : `2001:0db8:85a3::8a2e:0370:7334`
- Nombre d'adresses : 340 undécillions (nombre astronomique)
- **Avantage :** Résout le problème d'épuisement des adresses

### Adresses IP spéciales

| Adresse | Signification |
|---------|---------------|
| `127.0.0.1` | Localhost (boucle locale, la machine elle-même) |
| `0.0.0.0` | Toutes les interfaces réseau |
| `255.255.255.255` | Broadcast (diffusion à tous) |
| `192.168.x.x` | Réseaux privés (non routables sur Internet) |
| `10.x.x.x` | Réseaux privés (non routables sur Internet) |

### Routage

Le **routage** est le processus de détermination du chemin qu'un paquet doit suivre pour atteindre sa destination.

**Exemple simplifié :**
```
Votre PC (192.168.1.10)
    ↓
Routeur maison (192.168.1.1)
    ↓
Routeur FAI
    ↓
Routeur Internet
    ↓
Routeur FAI destinataire
    ↓
Serveur destination (93.184.216.34)
```

Chaque routeur examine l'adresse IP de destination et décide où envoyer le paquet ensuite.

### Protocole ICMP

**ICMP** (Internet Control Message Protocol) est utilisé pour les messages de contrôle et d'erreur.

**Exemple d'utilisation :** La commande `ping` utilise ICMP pour tester la connectivité.

```bash
$ ping google.com
PING google.com (142.250.185.46): 56 data bytes
64 bytes from 142.250.185.46: icmp_seq=0 ttl=115 time=12.3 ms
```

### Caractéristiques d'IP

✅ **Best-effort** : IP fait de son mieux, mais ne garantit pas la livraison

✅ **Sans connexion** : Chaque paquet est indépendant

✅ **Pas de fiabilité** : Les paquets peuvent être perdus, dupliqués ou arriver dans le désordre

❌ **Pas de contrôle de flux** : IP ne gère pas la congestion

⚠️ Ces limitations sont **intentionnelles**. C'est le rôle de la couche supérieure (Transport) d'ajouter ces fonctionnalités si nécessaire.

---

## Couche 3 : Transport (Transport Layer)

### Rôle
La couche **Transport** gère la communication **de bout en bout** entre deux applications. Elle ajoute des fonctionnalités essentielles comme la fiabilité et le multiplexage.

### Notion de Port

Les **ports** permettent de distinguer plusieurs applications sur la même machine.

**Analogie :** Si l'adresse IP est l'adresse d'un immeuble, le port est le numéro d'appartement.

```
Machine : 192.168.1.10
    ├─ Port 80  : Serveur web (HTTP)
    ├─ Port 22  : Serveur SSH
    ├─ Port 3306: MySQL
    └─ Port 8080: Application custom
```

- **Plage des ports :** 0 à 65535 (16 bits)
- **Ports privilégiés :** 0-1023 (nécessitent root sous Linux)

### Les deux protocoles principaux : TCP et UDP

---

## TCP (Transmission Control Protocol)

### Caractéristiques de TCP

TCP est un protocole **orienté connexion**, **fiable** et **ordonné**.

#### 1. Orienté connexion (Connection-oriented)

Avant d'échanger des données, une **connexion** doit être établie via le **three-way handshake** :

```
Client                          Serveur
  |                               |
  |------- SYN (seq=x) ---------->|
  |                               |
  |<---- SYN-ACK (seq=y, ack=x+1)-|
  |                               |
  |------- ACK (ack=y+1) -------->|
  |                               |
  |    Connexion établie          |
```

**Étapes :**
1. **SYN** : Le client demande une connexion
2. **SYN-ACK** : Le serveur accepte et demande confirmation
3. **ACK** : Le client confirme

**Analogie :** C'est comme une conversation téléphonique. Vous appelez, l'autre décroche, et vous vous assurez mutuellement que vous vous entendez avant de parler.

#### 2. Fiable (Reliable)

TCP **garantit** que :
- Tous les octets envoyés arrivent à destination
- Aucun octet n'est dupliqué
- L'ordre d'envoi est préservé

**Mécanisme :**
- Chaque segment TCP a un **numéro de séquence**
- Le destinataire envoie des **accusés de réception (ACK)**
- Si un ACK n'arrive pas, l'émetteur **retransmet** automatiquement

```
Client                          Serveur
  |                               |
  |------- Données (seq=1000) --->|
  |                               |
  |<------ ACK (ack=1500) --------|  ✅ Reçu
  |                               |
  |------- Données (seq=1500) --->|
  |           ❌ Perdu            |
  |                               |
  |  (timeout)                    |
  |------- Données (seq=1500) --->|  🔄 Retransmission
  |                               |
  |<------ ACK (ack=2000) --------|  ✅ Reçu
```

#### 3. Ordonné (Ordered)

Les données arrivent **dans l'ordre** d'envoi, même si les paquets empruntent des chemins différents.

#### 4. Contrôle de flux

TCP ajuste la vitesse d'envoi pour ne pas **submerger** le destinataire.

**Fenêtre de réception :** Le destinataire annonce combien d'octets il peut encore recevoir.

#### 5. Contrôle de congestion

TCP détecte et s'adapte à la **congestion réseau** pour éviter de surcharger le réseau.

**Algorithmes :**
- Slow start
- Congestion avoidance
- Fast retransmit
- Fast recovery

### Format d'un segment TCP

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|          Source Port          |       Destination Port        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                        Sequence Number                        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Acknowledgment Number                      |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Data |           |U|A|P|R|S|F|                               |
| Offset| Reserved  |R|C|S|S|Y|I|            Window             |
|       |           |G|K|H|T|N|N|                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|           Checksum            |         Urgent Pointer        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Options (si présentes)                     |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                             Data                              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Champs importants :**
- **Source/Destination Port :** Pour le multiplexage
- **Sequence Number :** Position des données dans le flux
- **Acknowledgment Number :** Prochain octet attendu
- **Flags :** SYN, ACK, FIN, RST, etc.
- **Window :** Fenêtre de réception
- **Checksum :** Détection d'erreurs

### Fermeture de connexion TCP

La fermeture utilise le **four-way handshake** :

```
Client                          Serveur
  |                               |
  |------- FIN (seq=x) ---------->|
  |                               |
  |<------ ACK (ack=x+1) ---------|
  |                               |
  |<------ FIN (seq=y) -----------|
  |                               |
  |------- ACK (ack=y+1) -------->|
  |                               |
  |    Connexion fermée           |
```

### Quand utiliser TCP ?

✅ **Applications nécessitant la fiabilité :**
- Navigation web (HTTP/HTTPS)
- Transfert de fichiers (FTP, SFTP)
- Email (SMTP, IMAP, POP3)
- Bases de données
- SSH

✅ **Quand l'ordre des données est critique**

✅ **Quand on ne peut tolérer la perte de données**

❌ **Applications en temps réel sensibles à la latence** (mieux avec UDP)

---

## UDP (User Datagram Protocol)

### Caractéristiques d'UDP

UDP est un protocole **sans connexion**, **non fiable** et **non ordonné**.

#### 1. Sans connexion (Connectionless)

Pas de handshake, pas d'établissement de connexion. On envoie directement les données.

**Analogie :** C'est comme envoyer une carte postale. Vous écrivez l'adresse et vous la postez, sans vérifier si elle arrivera.

#### 2. Non fiable (Unreliable)

UDP **ne garantit rien** :
- Les paquets peuvent être **perdus**
- Les paquets peuvent être **dupliqués**
- Les paquets peuvent arriver **dans le désordre**

⚠️ **Important :** "Non fiable" ne signifie pas "mauvais" ! Cela signifie simplement que le protocole ne gère pas ces aspects. C'est à l'application de le faire si nécessaire.

#### 3. Léger et rapide

- **Pas d'accusés de réception** → Moins d'overhead
- **Pas de retransmission** → Latence plus faible
- **En-tête minimal** (8 octets vs 20+ pour TCP)

### Format d'un datagramme UDP

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|          Source Port          |       Destination Port        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|            Length             |           Checksum            |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                             Data                              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Beaucoup plus simple que TCP !**

### Quand utiliser UDP ?

✅ **Applications en temps réel :**
- Streaming vidéo/audio (YouTube, Twitch)
- VoIP (Skype, Discord)
- Jeux en ligne (FPS, MMORPG)
- Visioconférence (Zoom)

✅ **Requêtes simples :** DNS (résolution de noms)

✅ **Broadcasting et multicasting**

✅ **Quand la latence est plus importante que la fiabilité**

❌ **Transfert de fichiers critiques**

❌ **Transactions financières**

### Pourquoi le streaming utilise UDP ?

Imaginez que vous regardez un match de football en direct :
- **Avec TCP :** Si un paquet vidéo est perdu, TCP le retransmet. Mais pendant ce temps, les paquets suivants attendent. Résultat : l'image se fige pendant 2 secondes.
- **Avec UDP :** Si un paquet est perdu, tant pis, on continue. Vous aurez peut-être un petit artefact visuel, mais le flux continue en temps réel.

**Préférence :** Un petit artefact > un freeze de 2 secondes.

---

## Comparaison TCP vs UDP

| Critère | TCP | UDP |
|---------|-----|-----|
| **Connexion** | Orienté connexion (3-way handshake) | Sans connexion |
| **Fiabilité** | Garantie de livraison | Pas de garantie |
| **Ordre** | Données ordonnées | Peut arriver dans le désordre |
| **Vitesse** | Plus lent (overhead) | Rapide (minimal overhead) |
| **Contrôle de flux** | Oui | Non |
| **Contrôle congestion** | Oui | Non |
| **En-tête** | 20+ octets | 8 octets |
| **Use cases** | Web, email, fichiers, BD | Streaming, jeux, VoIP, DNS |
| **Complexité** | Plus complexe à implémenter | Simple |

**Règle simple :**
- Besoin de **fiabilité** ? → **TCP**
- Besoin de **vitesse/temps réel** ? → **UDP**

---

## Couche 4 : Application (Application Layer)

### Rôle
La couche **Application** est celle avec laquelle les programmes interagissent directement. Elle fournit des services réseau aux applications.

### Protocoles d'application populaires

#### HTTP/HTTPS (Web)
- **Port :** 80 (HTTP), 443 (HTTPS)
- **Transport :** TCP
- **Usage :** Navigation web, APIs REST

**Exemple de requête HTTP :**
```http
GET /index.html HTTP/1.1
Host: www.example.com
User-Agent: Mozilla/5.0
```

#### FTP (File Transfer Protocol)
- **Ports :** 20 (données), 21 (contrôle)
- **Transport :** TCP
- **Usage :** Transfert de fichiers

#### SSH (Secure Shell)
- **Port :** 22
- **Transport :** TCP
- **Usage :** Connexion sécurisée, tunneling

#### DNS (Domain Name System)
- **Port :** 53
- **Transport :** **UDP** (principalement), TCP (transferts de zone)
- **Usage :** Résolution de noms de domaine

**Exemple :**
```
www.google.com → 142.250.185.46
```

#### SMTP/IMAP/POP3 (Email)
- **Ports :** 25 (SMTP), 143 (IMAP), 110 (POP3)
- **Transport :** TCP
- **Usage :** Envoi et réception d'emails

#### DHCP (Dynamic Host Configuration Protocol)
- **Ports :** 67 (serveur), 68 (client)
- **Transport :** UDP
- **Usage :** Attribution automatique d'adresses IP

### Sockets et couche Application

Lorsque vous programmez avec des **sockets** en C, vous travaillez principalement au niveau de la **couche Transport** (TCP/UDP). Vous créez ensuite votre propre **protocole d'application** au-dessus.

---

## Encapsulation : Comment tout s'emboîte

L'**encapsulation** est le processus d'ajout d'en-têtes par chaque couche.

### Envoi de données (du haut vers le bas)

```
Application:   [Données HTTP]
                   ↓
Transport:     [En-tête TCP] [Données HTTP]
                   ↓
Internet:      [En-tête IP] [En-tête TCP] [Données HTTP]
                   ↓
Accès réseau:  [En-tête Ethernet] [En-tête IP] [En-tête TCP] [Données HTTP] [FCS]
                   ↓
           Transmission physique sur le câble
```

### Réception de données (du bas vers le haut)

```
           Réception depuis le câble
                   ↓
Accès réseau:  Vérifie FCS, retire en-tête Ethernet
                   ↓
Internet:      Vérifie checksum IP, retire en-tête IP
                   ↓
Transport:     Vérifie checksum TCP, réordonne, retire en-tête TCP
                   ↓
Application:   [Données HTTP] prêtes à être utilisées
```

**Chaque couche ne voit que son en-tête et les données qu'elle encapsule.**

### Exemple concret : Requête web

Vous tapez `www.google.com` dans votre navigateur :

1. **Application :** Votre navigateur crée une requête HTTP GET
2. **Transport :** TCP ajoute son en-tête (port source/dest, seq, ack)
3. **Internet :** IP ajoute son en-tête (IP source/dest)
4. **Accès réseau :** Ethernet ajoute son en-tête (MAC source/dest)
5. **Transmission :** Les bits sont envoyés sur le câble/WiFi

Chez Google, le processus inverse se produit, couche par couche, jusqu'à ce que le serveur web reçoive la requête HTTP originale.

---

## MTU et Fragmentation

### MTU (Maximum Transmission Unit)

Le **MTU** est la taille maximale d'un paquet qui peut être transmis sur un réseau.

- **Ethernet :** MTU = 1500 octets
- **Internet :** MTU minimum requis = 576 octets (IPv4), 1280 octets (IPv6)

### Fragmentation

Si un paquet IP est trop grand pour le MTU, il est **fragmenté** en plusieurs petits paquets.

**Problème :** La fragmentation peut dégrader les performances.

**Solution moderne :** **Path MTU Discovery** (PMTUD) détermine le MTU optimal pour éviter la fragmentation.

---

## Sockets : L'interface de programmation

### Qu'est-ce qu'un socket ?

Un **socket** est une **interface de programmation** (API) qui permet aux applications d'utiliser TCP ou UDP.

**Analogie :** Un socket est comme une prise téléphonique. Vous y branchez votre téléphone (application), et le réseau se charge du reste.

### Types de sockets

#### Stream Sockets (SOCK_STREAM)
- Utilisent **TCP**
- Connexion orientée
- Flux d'octets continu et fiable
- **Usage typique :** Serveurs web, bases de données

#### Datagram Sockets (SOCK_DGRAM)
- Utilisent **UDP**
- Sans connexion
- Messages indépendants (datagrammes)
- **Usage typique :** DNS, streaming, jeux

### L'API socket en C

Nous verrons dans les prochaines sections comment utiliser concrètement ces fonctions :

```c
int socket(int domain, int type, int protocol);
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int listen(int sockfd, int backlog);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

---

## Outils de diagnostic réseau

Quelques commandes Linux utiles pour comprendre TCP/IP :

### ping
Teste la connectivité (utilise ICMP) :
```bash
$ ping google.com
```

### traceroute
Affiche le chemin emprunté par les paquets :
```bash
$ traceroute google.com
```

### netstat / ss
Affiche les connexions réseau actives :
```bash
$ ss -tuln    # TCP et UDP, listening, numérique
```

### tcpdump / Wireshark
Capture et analyse les paquets réseau :
```bash
$ sudo tcpdump -i eth0 port 80
```

### nslookup / dig
Résolution DNS :
```bash
$ dig google.com
```

---

## En résumé

L'architecture TCP/IP est le fondement de toutes les communications réseau modernes. Comprendre ce modèle est essentiel pour programmer des applications réseau robustes.

**Points clés à retenir :**

✅ **4 couches :** Accès réseau, Internet, Transport, Application

✅ **IP** : Adressage et routage des paquets

✅ **TCP** : Fiable, ordonné, orienté connexion (web, email, fichiers)

✅ **UDP** : Rapide, léger, sans garantie (streaming, jeux, DNS)

✅ **Encapsulation** : Chaque couche ajoute son en-tête

✅ **Sockets** : Interface de programmation pour TCP/UDP

✅ **Ports** : Permettent le multiplexage d'applications

Dans les sections suivantes, nous allons mettre en pratique ces concepts en créant nos premiers programmes réseau avec des **sockets TCP**.

---

**→ Prochaine section : 20.3 Sockets TCP**

⏭️ [Sockets TCP](/20-reseau-sockets/03-sockets-tcp.md)
