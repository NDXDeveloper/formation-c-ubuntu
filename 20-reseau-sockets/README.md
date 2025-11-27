🔝 Retour au [Sommaire](/SOMMAIRE.md)

# Chapitre 20 : Réseau (Sockets)

## Introduction

Bienvenue dans le chapitre sur la **programmation réseau** en C ! Ce chapitre va vous apprendre à créer des applications qui communiquent à travers un réseau, qu'il soit local (votre ordinateur ou votre réseau domestique) ou global (Internet).

La programmation réseau est au cœur d'une multitude d'applications modernes :
- **Navigateurs web** qui affichent des pages Internet
- **Applications de messagerie** (WhatsApp, Telegram, Signal)
- **Jeux en ligne** multijoueurs
- **Services de streaming** (Netflix, YouTube, Spotify)
- **Cloud computing** et services web (API REST)
- **Systèmes distribués** et bases de données

Dans ce chapitre, vous allez apprendre à utiliser les **sockets** : l'interface de programmation qui permet aux programmes de communiquer via le réseau, que ce soit sur le même ordinateur ou à l'autre bout du monde.

---

## Qu'est-ce qu'un Socket ?

### Analogie Simple

Imaginez que vous voulez envoyer une lettre à un ami :

1. **Vous écrivez la lettre** (vos données)
2. **Vous la mettez dans une enveloppe** (le paquet réseau)
3. **Vous inscrivez l'adresse** (adresse IP + port)
4. **Vous la postez** (envoi via le réseau)
5. **Votre ami la reçoit dans sa boîte aux lettres** (socket récepteur)

Un **socket** est comme une **boîte aux lettres numérique** : un point de connexion qui permet d'envoyer et de recevoir des données.

### Définition Technique

Un socket est une **abstraction** fournie par le système d'exploitation qui permet à deux programmes de communiquer, que ce soit :
- Sur la même machine (communication locale)
- Sur des machines différentes connectées par un réseau
- À travers Internet

**Analogie du téléphone :**
- Un socket est comme un téléphone
- L'adresse IP est comme le numéro de téléphone
- Le port est comme l'extension (poste)
- Vous pouvez appeler (client) ou attendre des appels (serveur)

---

## Pourquoi Apprendre la Programmation Réseau ?

### 1. Omniprésence

Presque toutes les applications modernes utilisent le réseau :
- Applications web (frontend ↔ backend)
- Applications mobiles (app ↔ serveur)
- IoT (objets connectés)
- Systèmes embarqués
- Services cloud

**Chiffres clés :**
- Plus de 5 milliards d'utilisateurs Internet dans le monde
- Des milliards de requêtes HTTP chaque seconde
- Des téraoctets de données transitent en permanence

### 2. Opportunités de Carrière

Maîtriser la programmation réseau ouvre de nombreuses portes :
- **Développeur backend** (serveurs, API, microservices)
- **Ingénieur DevOps** (infrastructure, déploiement)
- **Spécialiste sécurité** (pare-feu, détection d'intrusions)
- **Développeur systèmes** (Linux, Unix, serveurs)
- **Ingénieur réseau** (protocoles, routage)

### 3. Compréhension Fondamentale

Même si vous utilisez des frameworks haut niveau (Node.js, Django, Spring), comprendre les bases vous permet de :
- **Débugger efficacement** les problèmes réseau
- **Optimiser les performances** (latence, débit)
- **Concevoir de meilleures architectures**
- **Comprendre la sécurité** (attaques, vulnérabilités)

---

## Ce que Vous Allez Apprendre

### Vue d'Ensemble du chapitre

Ce chapitre couvre progressivement tous les aspects de la programmation réseau en C :

#### **Fondamentaux (Sections 20.1-20.4)**
- Le modèle client/serveur
- L'architecture TCP/IP et les protocoles
- Les sockets TCP (connexion fiable)
- Les sockets UDP (communication rapide)

#### **Gestion et Fiabilité (Sections 20.5-20.7)**
- Gestion robuste des erreurs réseau
- Options avancées des sockets
- Résolution de noms de domaine (DNS)

#### **Performance et Scalabilité (Sections 20.8-20.9)**
- Serveurs concurrents (multi-processus, multi-threads)
- I/O non-bloquant et epoll
- Architectures haute performance

#### **Projet Final (Section 20.10)**
- Création d'un mini-serveur HTTP
- Application pratique de toutes les connaissances

### Compétences Acquises

À la fin de ce chapitre, vous serez capable de :

✅ **Créer des applications client/serveur** en C

✅ **Implémenter des protocoles** TCP et UDP

✅ **Gérer plusieurs clients simultanément** (concurrence)

✅ **Débugger des problèmes réseau** avec des outils professionnels

✅ **Optimiser les performances** pour des milliers de connexions

✅ **Comprendre le fonctionnement interne** du web et d'Internet

✅ **Créer un serveur HTTP** fonctionnel

---

## Concepts Clés à Maîtriser

### 1. Adresses IP

Une **adresse IP** identifie de manière unique un ordinateur sur un réseau.

**IPv4 (32 bits) :**
```
192.168.1.10
```
Format : 4 nombres de 0 à 255 séparés par des points

**IPv6 (128 bits) :**
```
2001:0db8:85a3:0000:0000:8a2e:0370:7334
```
Format : 8 groupes de 4 chiffres hexadécimaux

**Adresses spéciales :**
- `127.0.0.1` (localhost) : Votre propre machine
- `0.0.0.0` : Toutes les interfaces réseau
- `255.255.255.255` : Broadcast (diffusion)

### 2. Ports

Un **port** est un numéro (0-65535) qui identifie une application spécifique sur une machine.

**Analogie de l'immeuble :**
- L'adresse IP = L'adresse de l'immeuble
- Le port = Le numéro d'appartement

**Ports standards :**
- Port 80 : HTTP (web)
- Port 443 : HTTPS (web sécurisé)
- Port 22 : SSH (connexion sécurisée)
- Port 21 : FTP (transfert de fichiers)
- Port 25 : SMTP (email)
- Port 3306 : MySQL (base de données)

**Catégories de ports :**
- **0-1023** : Ports privilégiés (nécessitent les droits root)
- **1024-49151** : Ports enregistrés (applications connues)
- **49152-65535** : Ports dynamiques/privés (usage temporaire)

### 3. Protocoles

Un **protocole** est un ensemble de règles qui définissent comment les données sont formatées et échangées.

**Les deux protocoles principaux :**

#### TCP (Transmission Control Protocol)
- **Fiable** : Les données arrivent intactes et dans l'ordre
- **Orienté connexion** : Établissement d'une connexion avant l'échange
- **Contrôle de flux** : Adaptation à la vitesse du destinataire
- **Exemple** : Navigation web, email, transfert de fichiers

**Analogie :** Conversation téléphonique (connexion établie, dialogue bidirectionnel)

#### UDP (User Datagram Protocol)
- **Non fiable** : Les données peuvent être perdues ou arriver dans le désordre
- **Sans connexion** : Pas d'établissement préalable
- **Rapide** : Moins d'overhead, latence minimale
- **Exemple** : Streaming vidéo, jeux en ligne, VoIP

**Analogie :** Envoyer des cartes postales (pas de garantie de réception, pas d'ordre garanti)

### 4. Modèle OSI et TCP/IP

Le réseau fonctionne en **couches** qui s'empilent :

```
┌─────────────────────────┐
│   Application (HTTP)    │ ← Votre programme
├─────────────────────────┤
│   Transport (TCP/UDP)   │ ← Sockets
├─────────────────────────┤
│   Réseau (IP)           │ ← Routage
├─────────────────────────┤
│   Liaison (Ethernet)    │ ← Matériel
└─────────────────────────┘
```

**Chaque couche :**
- A un rôle spécifique
- Communique avec les couches adjacentes
- Est indépendante des autres (abstraction)

**Votre code travaille au niveau Application et Transport** (sockets).

---

## Prérequis

Avant de commencer ce chapitre, vous devriez être à l'aise avec :

### Connaissances en C

✅ Pointeurs et gestion de la mémoire (`malloc`, `free`)

✅ Structures de données (`struct`, tableaux)

✅ Manipulation de chaînes de caractères

✅ Gestion des fichiers (`open`, `read`, `write`, `close`)

✅ Processus et signaux (pour les serveurs concurrents)

### Connaissances Système

✅ Système de fichiers Linux/Unix

✅ Commandes de base du terminal

✅ Compilation avec `gcc`

✅ Utilisation du debugger `gdb`

### Concepts Réseau (Optionnel)

Une connaissance de base du réseau est utile mais pas obligatoire. Nous expliquerons tous les concepts au fur et à mesure.

---

## Environnement de Développement

### Système d'Exploitation

Les exemples de ce chapitre sont conçus pour **Linux/Unix**. Si vous êtes sur :

- **Linux** : Parfait ! Tout fonctionnera nativement
- **macOS** : Compatible, les sockets POSIX fonctionnent de la même manière
- **Windows** : Utilisez WSL (Windows Subsystem for Linux) ou une VM Linux

### Outils Nécessaires

```bash
# Compilateur C
gcc --version

# Outils réseau
ping
netstat (ou ss)
tcpdump
nc (netcat)

# Debugger
gdb
```

### Installation des Outils (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install build-essential
sudo apt install net-tools
sudo apt install tcpdump
sudo apt install netcat
sudo apt install gdb
```

---

## Structure du chapitre

Le chapitre est organisé en 10 sections progressives :

### **Partie 1 : Fondamentaux**

**20.1 Modèle Client/Serveur**
- Architecture client/serveur
- Rôles et responsabilités
- Flux de communication

**20.2 Architecture TCP/IP**
- Couches du modèle TCP/IP
- Protocoles TCP et UDP
- Encapsulation des données

**20.3 Sockets TCP**
- API des sockets
- Création de clients et serveurs TCP
- Connexions fiables

**20.4 Sockets UDP**
- Communication sans connexion
- Datagrammes
- Use cases d'UDP

### **Partie 2 : Robustesse**

**20.5 Gestion des Erreurs Réseau**
- Codes d'erreur réseau
- Stratégies de récupération
- Timeouts et reconnexions

**20.6 Options de Sockets**
- `setsockopt` et `getsockopt`
- Configuration avancée
- Optimisation des performances

**20.7 Résolution de Noms**
- DNS et `getaddrinfo()`
- Résolution d'adresses
- Support IPv4/IPv6

### **Partie 3 : Performance**

**20.8 Serveur Concurrent**
- Multi-processus (fork)
- Multi-threads (pthreads)
- I/O multiplexing (select, poll, epoll)

**20.9 Non-blocking I/O et epoll**
- Sockets non-bloquants
- Architecture événementielle
- Haute performance

### **Partie 4 : Projet**

**20.10 Création d'un Mini-Serveur HTTP**
- Protocole HTTP
- Parsing de requêtes
- Serveur web complet

---

## Pédagogie et Approche

### Apprentissage Progressif

Chaque section s'appuie sur les précédentes :
1. **Concepts théoriques** expliqués simplement
2. **Exemples de code** commentés et fonctionnels
3. **Exercices pratiques** pour consolider
4. **Conseils d'experts** et bonnes pratiques

### Code Complet et Testable

Tous les exemples sont :
- **Complets** : Pas de code tronqué
- **Compilables** : Fonctionnent tel quel
- **Commentés** : Explications ligne par ligne
- **Testables** : Instructions pour exécuter

### Approche Pratique

Vous allez **écrire du code** dès le début :
- Clients et serveurs simples
- Programmes réseau utilitaires
- Un serveur web fonctionnel

### Debugging et Outils

Vous apprendrez à utiliser les outils professionnels :
- `netstat` / `ss` : Surveiller les connexions
- `tcpdump` / `wireshark` : Capturer le trafic réseau
- `telnet` / `nc` : Tester les serveurs
- `strace` : Tracer les appels système

---

## Analogies et Métaphores

Tout au long du chapitre, nous utiliserons des analogies pour clarifier les concepts :

**Le réseau comme un service postal :**
- Adresse IP = Adresse postale
- Port = Numéro d'appartement
- Socket = Boîte aux lettres
- Paquet = Enveloppe/Colis
- TCP = Courrier recommandé
- UDP = Carte postale

**Le serveur comme un restaurant :**
- Serveur = Restaurant qui attend des clients
- Socket d'écoute = Porte d'entrée
- `accept()` = Accueillir un client
- `recv()` = Prendre la commande
- `send()` = Servir le plat
- `close()` = Client qui part

**Le protocole comme une conversation :**
- Règles de politesse
- Format des messages
- Qui parle quand
- Comment terminer la conversation

---

## Conseils pour Réussir

### 1. Pratiquez Régulièrement

La programmation réseau s'apprend en **codant** :
- Tapez chaque exemple vous-même
- Expérimentez avec les paramètres
- Créez vos propres variations

### 2. Testez en Conditions Réelles

Ne vous contentez pas de `localhost` :
- Testez entre plusieurs machines
- Simulez des pannes réseau
- Mesurez les performances

### 3. Lisez les Erreurs

Les messages d'erreur réseau sont **précieux** :
- `Connection refused` : Serveur pas démarré
- `Address already in use` : Port déjà utilisé
- `Connection reset` : Connexion fermée brutalement

Apprenez à les interpréter !

### 4. Utilisez les Outils

Maîtrisez les outils de diagnostic :
```bash
# Voir toutes les connexions
ss -tunap

# Capturer le trafic
sudo tcpdump -i lo -X port 8080

# Tester un serveur
telnet localhost 8080
```

### 5. Consultez la Documentation

Les pages de manuel sont vos amies :
```bash
man socket
man tcp
man ip
man 2 socket  # Documentation de l'appel système socket()
man 7 ip      # Documentation du protocole IP
```

### 6. Soyez Patient avec les Bugs

Les bugs réseau peuvent être **subtils** :
- Race conditions
- Deadlocks
- Problèmes de temporisation
- Erreurs intermittentes

C'est normal ! Prenez le temps de débugger méthodiquement.

---

## Ressources Complémentaires

### Livres de Référence

- **"Unix Network Programming"** par W. Richard Stevens (La bible !)
- **"TCP/IP Illustrated"** par W. Richard Stevens
- **"The Linux Programming Interface"** par Michael Kerrisk
- **"Computer Networks"** par Andrew Tanenbaum

### Documentation en Ligne

- **Beej's Guide to Network Programming** (gratuit, excellent pour débuter)
- **Pages man Linux** (documentation système)
- **RFC des protocoles** (spécifications officielles)

### Outils et Tutoriels

- **Wireshark** : Analyseur de paquets réseau (GUI)
- **netcat** : Couteau suisse du réseau
- **socat** : Outil avancé de manipulation de sockets

---

## Motivation

### La Magie du Réseau

Il y a quelque chose de **magique** dans la programmation réseau :

Vous tapez quelques lignes de code sur votre ordinateur, et soudain vous pouvez :
- Communiquer avec une machine à l'autre bout du monde
- Créer un serveur accessible par des millions de personnes
- Construire des systèmes distribués complexes
- Faire fonctionner le Web, les jeux, les apps mobiles...

**C'est le pouvoir de connecter** le monde numérique.

### L'Importance Historique

Les sockets ont été inventés dans les années 1980 à Berkeley (d'où **Berkeley Sockets**). Cette API simple mais puissante est à la base :
- D'Internet tel que nous le connaissons
- De toutes les applications en réseau modernes
- Des infrastructures cloud et microservices

**Apprendre les sockets, c'est toucher à l'histoire de l'informatique** et comprendre les fondations du monde connecté.

### Le Défi et la Récompense

La programmation réseau est **challengeante** :
- Beaucoup de concepts nouveaux
- Beaucoup de détails techniques
- Beaucoup de cas particuliers

Mais c'est aussi **extrêmement gratifiant** :
- Voir votre premier serveur répondre à un client
- Observer le trafic réseau que vous avez créé
- Construire des applications distribuées qui fonctionnent

**Chaque programme réseau qui fonctionne est une petite victoire !**

---

## Conclusion

Vous êtes maintenant prêt à plonger dans le monde fascinant de la programmation réseau en C !

Ce chapitre vous donnera les compétences pour :
- Comprendre comment fonctionnent vraiment les applications réseau
- Créer vos propres serveurs et clients
- Débugger et optimiser des communications réseau
- Construire des systèmes distribués performants

**La programmation réseau est un superbe voyage** à travers les couches d'abstraction, du matériel jusqu'aux protocoles applicatifs. C'est une compétence technique précieuse et un domaine intellectuellement stimulant.

Prenez votre temps, expérimentez beaucoup, et surtout : **amusez-vous** !

Bienvenue dans le monde des sockets et de la programmation réseau. 🌐

---

**→ Prochaine section : 20.1 Modèle Client/Serveur**

⏭️ [Modèle Client/Serveur](/20-reseau-sockets/01-modele-client-serveur.md)
