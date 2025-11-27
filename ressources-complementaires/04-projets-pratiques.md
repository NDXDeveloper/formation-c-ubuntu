🔝 Retour au [Sommaire](/SOMMAIRE.md)

# Projets Pratiques Recommandés

## Introduction

La meilleure façon d'apprendre le C est de **coder des projets concrets**. Cette section vous propose une liste de projets progressifs, classés par niveau de difficulté, qui vous permettront d'appliquer immédiatement ce que vous apprenez.

> **💡 Principe clé** : Chaque projet doit être terminé avant de passer au suivant. Un projet "presque fini" n'enseigne rien.

---

## 🎯 Comment Aborder un Projet ?

### Méthodologie en 6 Étapes

**1. Comprenez l'Objectif**
- Lisez la description du projet
- Identifiez ce que le programme doit faire
- Listez les fonctionnalités principales

**2. Décomposez en Sous-Problèmes**
- Découpez le projet en petites tâches
- Commencez par la partie la plus simple
- Ajoutez les fonctionnalités une par une

**3. Créez la Structure de Base**
- Fichiers sources (.c) et headers (.h)
- Fonction `main()` squelette
- Makefile ou CMakeLists.txt

**4. Implémentez Progressivement**
- Une fonctionnalité à la fois
- Compilez et testez après chaque ajout
- Ne passez pas à la suivante tant que ça ne marche pas

**5. Déboguez et Testez**
- GDB pour les bugs
- Valgrind pour les fuites mémoire
- Tests unitaires pour les fonctions importantes

**6. Finalisez**
- Documentation (README.md)
- Commentaires dans le code
- Gestion des erreurs
- Validation avec `-Wall -Werror`

---

## 🌱 Projets Niveau Débutant

### Modules Couverts
- Module 1 : Écosystème C/Linux
- Module 2 : Fondamentaux du Langage
- Module 3 : Gestion de la Mémoire (bases)

---

### Projet 1 : Calculatrice en Ligne de Commande

**Durée estimée** : 2-4 heures

**Description**
Créez une calculatrice qui demande deux nombres et une opération (+, -, *, /) à l'utilisateur, puis affiche le résultat.

**Exemple d'utilisation** :
```
$ ./calculatrice
Entrez le premier nombre : 42
Entrez l'opérateur (+, -, *, /) : *
Entrez le second nombre : 3
Résultat : 42 * 3 = 126
```

**Concepts mis en œuvre** :
- Entrées/sorties avec `printf` et `scanf`
- Variables et types de données (int, float)
- Opérateurs arithmétiques
- Structures de contrôle (if/else ou switch)
- Gestion des erreurs (division par zéro)

**Points d'attention** :
- ✅ Validez les entrées utilisateur
- ✅ Gérez la division par zéro
- ✅ Utilisez des float pour les divisions décimales
- ✅ Ajoutez un message d'erreur pour les opérateurs invalides

**Extensions possibles** :
- Calculs en chaîne (continuer après un résultat)
- Opérations supplémentaires (modulo %, puissance)
- Mode interactif (boucle jusqu'à "quit")
- Historique des calculs

---

### Projet 2 : Convertisseur de Températures

**Durée estimée** : 2-3 heures

**Description**
Créez un programme qui convertit des températures entre Celsius, Fahrenheit et Kelvin.

**Exemple d'utilisation** :
```
$ ./temp_converter
=== Convertisseur de Températures ===
1. Celsius → Fahrenheit
2. Fahrenheit → Celsius
3. Celsius → Kelvin
4. Kelvin → Celsius
Votre choix : 1
Température en Celsius : 25
25.0°C = 77.0°F
```

**Concepts mis en œuvre** :
- Fonctions (une fonction par conversion)
- Menu interactif avec boucle
- Types flottants (float/double)
- Formules mathématiques

**Formules** :
- Celsius → Fahrenheit : `F = C * 9/5 + 32`
- Fahrenheit → Celsius : `C = (F - 32) * 5/9`
- Celsius → Kelvin : `K = C + 273.15`
- Kelvin → Celsius : `C = K - 273.15`

**Points d'attention** :
- ✅ Utilisez des fonctions pour chaque conversion
- ✅ Validez que Kelvin ne soit pas négatif
- ✅ Affichez les résultats avec 2 décimales (`%.2f`)
- ✅ Permettez plusieurs conversions (boucle)

**Extensions possibles** :
- Conversion par arguments en ligne de commande
- Support de Rankine et Réaumur
- Lecture depuis un fichier de températures

---

### Projet 3 : Jeu du Nombre Mystère

**Durée estimée** : 3-4 heures

**Description**
Le programme tire un nombre aléatoire entre 1 et 100. L'utilisateur doit le deviner en un minimum de tentatives. Le programme indique "trop grand" ou "trop petit".

**Exemple d'utilisation** :
```
$ ./devine_nombre
J'ai choisi un nombre entre 1 et 100.
Tentative 1 : 50
Trop grand !
Tentative 2 : 25
Trop petit !
Tentative 3 : 37
Bravo ! Vous avez trouvé en 3 tentatives.
```

**Concepts mis en œuvre** :
- Nombres aléatoires (`rand()`, `srand()`)
- Boucles (while)
- Conditions (if/else)
- Compteur de tentatives

**Points d'attention** :
- ✅ Initialisez le générateur aléatoire avec `srand(time(NULL))`
- ✅ Incluez `<stdlib.h>` pour rand() et `<time.h>` pour time()
- ✅ Validez les entrées (entre 1 et 100)
- ✅ Félicitez l'utilisateur selon le nombre de tentatives

**Extensions possibles** :
- Niveaux de difficulté (facile : 1-50, difficile : 1-1000)
- Mode deux joueurs
- Meilleur score enregistré
- Nombre de tentatives limité

---

### Projet 4 : Gestionnaire de Mots de Passe Simple

**Durée estimée** : 5-7 heures

**Description**
Un programme qui stocke des paires nom/mot de passe dans un fichier texte et permet d'ajouter, rechercher et lister les entrées.

**Exemple d'utilisation** :
```
$ ./passmanager
=== Gestionnaire de Mots de Passe ===
1. Ajouter un compte
2. Rechercher un mot de passe
3. Lister tous les comptes
4. Quitter
Votre choix : 1
Nom du site : GitHub
Mot de passe : ************
Enregistré !
```

**Concepts mis en œuvre** :
- Lecture/écriture de fichiers (`fopen`, `fprintf`, `fscanf`, `fclose`)
- Structures (`struct` pour compte)
- Allocation dynamique (liste de comptes)
- Menu interactif
- Manipulation de chaînes

**Structure de données suggérée** :
```c
typedef struct {
    char site[50];
    char mot_de_passe[50];
} Compte;
```

**Points d'attention** :
- ✅ Sauvegardez dans un fichier texte simple (pas de chiffrement pour débuter)
- ✅ Gérez les erreurs d'ouverture de fichier
- ✅ Fermez toujours les fichiers (`fclose()`)
- ✅ Validez les entrées (longueur max)

**Extensions possibles** :
- Chiffrement basique (ROT13, XOR)
- Génération de mots de passe aléatoires
- Suppression d'un compte
- Modification d'un mot de passe

⚠️ **Note de sécurité** : Ce projet est pédagogique. Ne l'utilisez JAMAIS pour de vrais mots de passe sans chiffrement robuste !

---

### Projet 5 : Tri de Tableaux

**Durée estimée** : 4-6 heures

**Description**
Implémentez plusieurs algorithmes de tri (bubble sort, selection sort, insertion sort) et comparez leurs performances.

**Exemple d'utilisation** :
```
$ ./tri
Tableau initial : 64 34 25 12 22 11 90
Tri à bulles : 11 12 22 25 34 64 90
Temps : 0.000123 secondes

Tri par sélection : 11 12 22 25 34 64 90
Temps : 0.000098 secondes
```

**Concepts mis en œuvre** :
- Tableaux et pointeurs
- Algorithmes de tri
- Fonctions
- Mesure du temps (`clock()`)
- Génération de données de test

**Algorithmes à implémenter** :
1. **Bubble Sort** (le plus simple)
2. **Selection Sort**
3. **Insertion Sort**
4. **Quick Sort** (bonus, récursif)

**Points d'attention** :
- ✅ Testez avec différentes tailles de tableaux
- ✅ Vérifiez que le tri fonctionne (tableau croissant)
- ✅ Mesurez le temps avec `clock_t`
- ✅ Utilisez des fonctions séparées pour chaque algorithme

**Extensions possibles** :
- Tri décroissant
- Lecture des nombres depuis un fichier
- Visualisation ASCII du tri en cours
- Compteur de comparaisons et d'échanges

---

## 🌿 Projets Niveau Intermédiaire

### Modules Couverts
- Module 3 : Gestion de la Mémoire (complet)
- Module 4 : Structures de Données et Modularité
- Module 5 : Outillage DevOps (CMake, debugging)

---

### Projet 6 : Gestionnaire de Contacts

**Durée estimée** : 10-15 heures

**Description**
Application complète de gestion de contacts avec sauvegarde sur disque, recherche et suppression.

**Fonctionnalités** :
- Ajouter un contact (nom, téléphone, email, adresse)
- Afficher tous les contacts
- Rechercher par nom
- Supprimer un contact
- Modifier un contact
- Sauvegarder/charger depuis un fichier

**Concepts mis en œuvre** :
- Structures complexes
- Allocation dynamique (`malloc`, `realloc`, `free`)
- Liste chaînée ou tableau dynamique
- Fichiers (binaire ou texte)
- Modularité (plusieurs fichiers .c/.h)

**Structure suggérée** :
```c
typedef struct Contact {
    char nom[50];
    char telephone[20];
    char email[50];
    char adresse[100];
    struct Contact *suivant;  // Pour liste chaînée
} Contact;

typedef struct {
    Contact *premier;
    int taille;
} ListeContacts;
```

**Architecture de fichiers** :
```
contacts/
├── src/
│   ├── main.c
│   ├── contact.c
│   ├── liste.c
│   └── fichier.c
├── include/
│   ├── contact.h
│   ├── liste.h
│   └── fichier.h
├── CMakeLists.txt
└── README.md
```

**Points d'attention** :
- ✅ Gérez la mémoire proprement (Valgrind clean)
- ✅ Utilisez CMake pour la compilation
- ✅ Validez les entrées (email format, etc.)
- ✅ Ajoutez des tests unitaires

**Extensions possibles** :
- Export CSV
- Import depuis vCard
- Recherche avancée (par email, par numéro)
- Tri alphabétique
- Interface ncurses (TUI)

---

### Projet 7 : Éditeur de Texte Simple

**Durée estimée** : 15-20 heures

**Description**
Un éditeur de texte en ligne de commande avec fonctionnalités de base (ouvrir, éditer, sauvegarder).

**Fonctionnalités** :
- Ouvrir un fichier
- Afficher le contenu ligne par ligne
- Insérer du texte à une ligne donnée
- Supprimer une ligne
- Rechercher un mot
- Sauvegarder les modifications

**Concepts mis en œuvre** :
- Manipulation avancée de fichiers
- Buffer de texte (liste chaînée de lignes)
- Gestion mémoire dynamique
- Parsing de commandes
- État du programme (modifié/non modifié)

**Commandes suggérées** :
```
:o fichier.txt    # Ouvrir
:l               # Lister les lignes
:i 5 texte       # Insérer à la ligne 5
:d 3             # Supprimer ligne 3
:s mot           # Rechercher
:w               # Sauvegarder
:q               # Quitter
```

**Points d'attention** :
- ✅ Utilisez une liste chaînée pour stocker les lignes
- ✅ Alertez si le fichier est modifié avant de quitter
- ✅ Gérez les fichiers inexistants
- ✅ Numérotez les lignes à l'affichage

**Extensions possibles** :
- Undo/Redo
- Copier/coller de lignes
- Rechercher et remplacer
- Coloration syntaxique basique
- Mode interactif complet

---

### Projet 8 : Shell Minimaliste

**Durée estimée** : 20-25 heures

**Description**
Un shell basique qui exécute des commandes externes, gère les pipes et les redirections.

**Fonctionnalités** :
- Exécution de commandes (`ls`, `cat`, etc.)
- Gestion des arguments
- Pipes (`ls | grep txt`)
- Redirections (`cat file.txt > output.txt`)
- Commandes internes (cd, exit)
- Historique des commandes

**Concepts mis en œuvre** :
- `fork()`, `exec()`, `wait()`
- Pipes (`pipe()`)
- Redirections (`dup2()`)
- Parsing de ligne de commande
- Variables d'environnement
- Gestion des signaux (SIGINT)

**Architecture** :
1. Boucle principale (afficher prompt, lire commande)
2. Parser (découper la ligne en tokens)
3. Exécuteur (fork + exec)
4. Gestion des pipes et redirections

**Points d'attention** :
- ✅ Gérez le CTRL+C sans quitter le shell
- ✅ Récupérez les processus fils (pas de zombies)
- ✅ Gérez les erreurs d'exécution
- ✅ Libérez toute la mémoire

**Extensions possibles** :
- Variables shell ($PATH, $HOME)
- Background jobs (&)
- Completion (TAB)
- Scripts shell (.sh)
- Alias de commandes

---

### Projet 9 : Serveur TCP Echo

**Durée estimée** : 12-18 heures

**Description**
Un serveur qui écoute sur un port réseau, accepte des connexions clients, et renvoie en écho tout ce qu'il reçoit.

**Fonctionnalités** :
- Écoute sur un port (configurable)
- Accepte plusieurs clients (un à la fois, puis concurrent)
- Echo de chaque message reçu
- Logs des connexions
- Arrêt propre (SIGINT)

**Concepts mis en œuvre** :
- Sockets (`socket()`, `bind()`, `listen()`, `accept()`)
- Communication réseau (`send()`, `recv()`)
- Threads (`pthread`) pour multi-clients
- Mutex pour logs thread-safe
- Gestion des signaux

**Architecture** :
```
Serveur (main thread)
├── Socket écoute
├── Boucle accept()
└── Thread par client
    ├── recv()
    ├── traitement
    └── send()
```

**Points d'attention** :
- ✅ Fermez les sockets proprement
- ✅ Gérez les déconnexions clients
- ✅ Évitez les race conditions (mutex)
- ✅ Testez avec `telnet` ou `nc` (netcat)

**Test** :
```bash
# Terminal 1 : lancer le serveur
$ ./echo_server 8080
Serveur en écoute sur le port 8080...

# Terminal 2 : connecter un client
$ telnet localhost 8080
Hello!
Hello!
```

**Extensions possibles** :
- Protocole personnalisé (commandes)
- Limite de clients simultanés
- Timeout de connexion
- Logs dans un fichier
- Support IPv6

---

### Projet 10 : Analyseur de Logs

**Durée estimée** : 12-15 heures

**Description**
Programme qui lit des fichiers de logs (Apache, syslog, etc.) et génère des statistiques.

**Fonctionnalités** :
- Lecture de gros fichiers (streaming)
- Parsing de lignes de logs
- Statistiques : nombre d'erreurs, IPs les plus fréquentes, etc.
- Export des résultats (texte, JSON)
- Filtrage par date/niveau

**Exemple de log Apache** :
```
192.168.1.1 - - [10/Oct/2024:13:55:36 +0200] "GET /index.html HTTP/1.1" 200 2326
192.168.1.2 - - [10/Oct/2024:13:55:40 +0200] "GET /style.css HTTP/1.1" 404 512
```

**Concepts mis en œuvre** :
- Lecture efficace de fichiers
- Expressions régulières (optionnel, ou parsing manuel)
- Tables de hachage (comptage des IPs)
- Structures pour statistiques
- Formatage de sortie

**Statistiques à calculer** :
- Nombre total de requêtes
- Requêtes par code HTTP (200, 404, 500, etc.)
- Top 10 des IPs
- Top 10 des URLs
- Bande passante totale

**Points d'attention** :
- ✅ Gérez les gros fichiers (plusieurs GB)
- ✅ Ne chargez pas tout en mémoire
- ✅ Optimisez les structures de données
- ✅ Validez le format de chaque ligne

**Extensions possibles** :
- Support de plusieurs formats (syslog, nginx)
- Graphiques ASCII (histogrammes)
- Détection d'anomalies (pics de trafic)
- Mode temps réel (tail -f)

---

## 🌳 Projets Niveau Avancé

### Modules Couverts
- Module 6 : Programmation Système Linux
- Module 7 : Techniques Avancées et Optimisation
- Module 8 : CI/CD

---

### Projet 11 : Allocateur Mémoire Personnalisé

**Durée estimée** : 25-35 heures

**Description**
Implémentez votre propre `malloc()`, `free()`, `realloc()` en gérant un tas (heap) personnalisé.

**Fonctionnalités** :
- Allocation de blocs (`my_malloc()`)
- Libération (`my_free()`)
- Réallocation (`my_realloc()`)
- Fusion de blocs libres (coalescing)
- Stratégies d'allocation (first-fit, best-fit)

**Concepts mis en œuvre** :
- Gestion bas niveau de la mémoire
- Structures de contrôle (metadata)
- Algorithmes d'allocation
- Alignement mémoire
- Listes chaînées de blocs

**Architecture** :
```
Bloc mémoire :
[Header: taille, libre/utilisé] [Data: espace utilisateur]
```

**Points d'attention** :
- ✅ Alignez les blocs (8 ou 16 bytes)
- ✅ Gérez la fragmentation
- ✅ Détectez le double-free
- ✅ Mesurez les performances vs malloc()

**Extensions possibles** :
- Pool allocator
- Garbage collector simple
- Protection contre la corruption
- Statistiques d'utilisation

**Difficulté** : ⭐⭐⭐⭐⭐ (très avancé)

---

### Projet 12 : Mini Serveur HTTP

**Durée estimée** : 30-40 heures

**Description**
Serveur web minimaliste qui sert des fichiers statiques (HTML, CSS, images) via HTTP/1.1.

**Fonctionnalités** :
- Parsing de requêtes HTTP
- Servir des fichiers statiques
- Gestion des codes de statut (200, 404, 500)
- Headers HTTP (Content-Type, Content-Length)
- Keep-alive
- Logging des requêtes

**Exemple de requête** :
```
GET /index.html HTTP/1.1
Host: localhost:8080
```

**Exemple de réponse** :
```
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 1234

<html>...</html>
```

**Concepts mis en œuvre** :
- Sockets avancées
- Parsing HTTP
- Gestion MIME types
- I/O multiplexing (`select()` ou `epoll()`)
- Threads ou event-driven architecture
- Chemins de fichiers (sécurité)

**Architecture** :
```
Main Loop
├── Accept connexions
├── Parse requête HTTP
├── Résoudre chemin fichier
├── Lire fichier
├── Construire réponse HTTP
└── Envoyer réponse
```

**Points d'attention** :
- ✅ Validez les chemins (pas d'accès à /../etc/passwd)
- ✅ Gérez les erreurs réseau
- ✅ Supportez les gros fichiers
- ✅ Performance avec epoll (Linux)

**Test** :
```bash
$ ./http_server 8080
# Puis ouvrez http://localhost:8080 dans un navigateur
```

**Extensions possibles** :
- CGI basique (exécution de scripts)
- Virtual hosts
- Compression gzip
- HTTPS (OpenSSL)
- Reverse proxy

**Difficulté** : ⭐⭐⭐⭐ (avancé)

---

### Projet 13 : Moniteur Système

**Durée estimée** : 20-30 heures

**Description**
Agent de monitoring qui collecte des métriques système (CPU, RAM, disque, réseau) et les expose au format Prometheus.

**Fonctionnalités** :
- Collecte CPU usage (`/proc/stat`)
- Collecte RAM usage (`/proc/meminfo`)
- Collecte disque (`statvfs()`)
- Collecte réseau (`/proc/net/dev`)
- Export Prometheus (endpoint HTTP)
- Daemonisation (background)

**Concepts mis en œuvre** :
- Parsing de /proc
- Calcul de métriques
- Serveur HTTP simple
- Daemonisation (`fork()`, `setsid()`)
- Fichier PID
- Signaux (SIGHUP pour reload config)

**Format Prometheus** :
```
# HELP cpu_usage_percent CPU usage percentage
# TYPE cpu_usage_percent gauge
cpu_usage_percent 42.5

# HELP memory_used_bytes Memory used in bytes
# TYPE memory_used_bytes gauge
memory_used_bytes 4294967296
```

**Points d'attention** :
- ✅ Lisez /proc efficacement
- ✅ Calculez les deltas pour CPU
- ✅ Gérez le reload de config
- ✅ Logs dans syslog

**Test** :
```bash
$ ./sysmon --daemon
$ curl http://localhost:9100/metrics
```

**Extensions possibles** :
- Plus de métriques (température, load average)
- Alerting (seuils)
- Envoi vers InfluxDB/Prometheus
- Interface web (graphiques)

**Difficulté** : ⭐⭐⭐⭐ (avancé)

---

### Projet 14 : Programme eBPF

**Durée estimée** : 25-35 heures

**Description**
Programme eBPF qui trace les appels système `open()` et compte les ouvertures de fichiers par processus.

**Fonctionnalités** :
- Programme eBPF (kernel space)
- Programme utilisateur (user space)
- BPF map pour partager données
- Affichage en temps réel
- Filtrage par PID ou nom de fichier

**Concepts mis en œuvre** :
- eBPF programs
- BPF maps (hash map)
- libbpf
- Tracepoints ou kprobes
- Communication kernel ↔ userspace

**Architecture** :
```
Kernel Space (eBPF)
├── Hook sur sys_open
├── Récupérer PID, nom fichier
└── Incrémenter compteur dans BPF map

User Space
├── Charger programme eBPF
├── Créer BPF maps
└── Lire maps périodiquement
```

**Points d'attention** :
- ✅ Installez libbpf-dev
- ✅ Utilisez bpftool pour debug
- ✅ Gérez les permissions (root)
- ✅ Testez avec BCC d'abord (plus simple)

**Test** :
```bash
$ sudo ./trace_open
Tracing open() calls... Hit Ctrl-C to end.
PID    COMM             COUNT
1234   firefox          42
5678   code             18
...
```

**Extensions possibles** :
- Tracer d'autres syscalls
- Latence des opérations
- Filtrage avancé
- Export des données

**Difficulté** : ⭐⭐⭐⭐⭐ (très avancé, nécessite connaissances kernel)

---

### Projet 15 : Système de Fichiers FUSE

**Durée estimée** : 35-45 heures

**Description**
Implémentez un système de fichiers simple en espace utilisateur avec FUSE (par exemple, un filesystem en lecture seule qui lit des données depuis un fichier JSON).

**Fonctionnalités** :
- Monter le filesystem
- Lister les fichiers (readdir)
- Lire le contenu (read)
- Obtenir les attributs (getattr)
- Démonter proprement

**Concepts mis en œuvre** :
- FUSE API
- Opérations filesystem (read, readdir, getattr, etc.)
- Inodes virtuels
- Gestion des paths
- État du filesystem

**Points d'attention** :
- ✅ Installez libfuse-dev
- ✅ Commencez par un exemple simple (hello)
- ✅ Gérez les erreurs (-ENOENT, -EACCES)
- ✅ Testez avec `ls`, `cat`

**Test** :
```bash
$ ./myfs mountpoint/
$ ls mountpoint/
file1.txt  file2.txt
$ cat mountpoint/file1.txt
Hello from FUSE!
$ fusermount -u mountpoint/
```

**Extensions possibles** :
- Écriture de fichiers
- Création/suppression
- Permissions
- Chiffrement à la volée

**Difficulté** : ⭐⭐⭐⭐⭐ (très avancé)

---

## 📊 Tableau Récapitulatif

| Projet | Niveau | Durée | Concepts Clés |
|--------|--------|-------|---------------|
| Calculatrice | Débutant | 2-4h | I/O, opérateurs, conditions |
| Convertisseur Temp | Débutant | 2-3h | Fonctions, menu |
| Nombre Mystère | Débutant | 3-4h | Boucles, rand() |
| Gestionnaire Mots de Passe | Débutant | 5-7h | Fichiers, structures |
| Tri de Tableaux | Débutant | 4-6h | Algorithmes, tableaux |
| Gestionnaire Contacts | Intermédiaire | 10-15h | Allocation dynamique, modularité |
| Éditeur Texte | Intermédiaire | 15-20h | Listes chaînées, fichiers |
| Shell Minimaliste | Intermédiaire | 20-25h | fork/exec, pipes |
| Serveur Echo | Intermédiaire | 12-18h | Sockets, threads |
| Analyseur Logs | Intermédiaire | 12-15h | Parsing, structures données |
| Allocateur Mémoire | Avancé | 25-35h | Gestion bas niveau |
| Serveur HTTP | Avancé | 30-40h | Réseau, HTTP, epoll |
| Moniteur Système | Avancé | 20-30h | /proc, daemon, metrics |
| Programme eBPF | Avancé | 25-35h | eBPF, kernel tracing |
| Filesystem FUSE | Avancé | 35-45h | FUSE API, filesystem ops |

---

## 🎯 Progression Recommandée

### Parcours Débutant (4-8 semaines)
1. Calculatrice
2. Convertisseur de Températures
3. Nombre Mystère
4. Tri de Tableaux
5. Gestionnaire de Mots de Passe

**Objectif** : Maîtriser les fondamentaux et la gestion basique de la mémoire.

---

### Parcours Intermédiaire (8-12 semaines)
1. Gestionnaire de Contacts
2. Analyseur de Logs
3. Serveur Echo (mono-thread puis multi-thread)
4. Shell Minimaliste OU Éditeur de Texte

**Objectif** : Maîtriser allocation dynamique, modularité, et programmation système de base.

---

### Parcours Avancé (12+ semaines)
1. Serveur HTTP
2. Moniteur Système
3. Allocateur Mémoire OU Programme eBPF
4. (Optionnel) Filesystem FUSE

**Objectif** : Expertise système, performance, et technologies avancées.

---

## 💡 Conseils Généraux

### Pour Chaque Projet

**Avant de commencer** :
- ✅ Lisez la description complète
- ✅ Identifiez les modules du cours nécessaires
- ✅ Faites un plan papier (architecture, fonctions principales)

**Pendant le développement** :
- ✅ Commit Git réguliers (chaque fonctionnalité)
- ✅ Testez après chaque ajout
- ✅ Valgrind après chaque allocation/free
- ✅ Documentation progressive

**À la fin** :
- ✅ README.md complet (description, compilation, utilisation)
- ✅ Compilation sans warnings (`-Wall -Werror`)
- ✅ Valgrind clean (0 leaks)
- ✅ Tests de base effectués

---

### Quand Vous Êtes Bloqué

1. **Relisez la section du cours** concernée
2. **Consultez la man page** de la fonction
3. **Cherchez sur Google/Stack Overflow** (termes en anglais)
4. **Simplifiez le problème** (version minimale qui marche)
5. **Demandez de l'aide** (communautés C)

**Ne restez pas bloqué plus de 2 heures** sur un problème. Demandez de l'aide !

---

### Partage et Feedback

✅ **Publiez sur GitHub** :
- Portfolio visible par les recruteurs
- Feedback de la communauté
- Historique de votre progression

✅ **Partagez votre code** :
- r/C_Programming : "Show-off Saturday"
- Discord C Programming
- Demandez des code reviews

---

## 🌟 Motivation

> "Le seul moyen d'apprendre le C est de coder en C."
> — Dennis Ritchie (créateur du langage C)

Chaque projet terminé est une **victoire**. Chaque bug résolu vous rend **meilleur**. Chaque ligne de code est un **pas vers la maîtrise**.

**N'abandonnez pas si le premier projet prend du temps.** Avec la pratique, vous serez de plus en plus rapide et efficace.

---

## 🚀 Conclusion

Ces projets vous permettent de progresser de manière **structurée et concrète**. Ils couvrent tous les aspects du C, du plus basique au plus avancé :

- ✅ Syntaxe et fondamentaux
- ✅ Gestion de la mémoire
- ✅ Structures de données
- ✅ Programmation système
- ✅ Réseau et concurrence
- ✅ Performance et optimisation
- ✅ Technologies modernes (eBPF)

**Commencez par le projet 1, et construisez votre expertise progressivement !**

**Bonne programmation ! 🎉**

⏭️ Retour au [Sommaire](/SOMMAIRE.md)
