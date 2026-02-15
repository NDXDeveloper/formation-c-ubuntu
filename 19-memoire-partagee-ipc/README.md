🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 19. Mémoire partagée et IPC avancé

## Introduction au chapitre

Jusqu'à présent, nous avons exploré le langage C dans un contexte où chaque programme s'exécute de manière isolée. Mais dans un système moderne, les applications doivent souvent **communiquer entre elles** : un serveur web doit coordonner plusieurs processus workers, une base de données doit partager des données entre différents clients, un système de cache doit être accessible par plusieurs applications simultanément.

C'est ici qu'intervient l'**IPC** (Inter-Process Communication) : l'ensemble des mécanismes permettant à des processus distincts d'échanger des données et de se synchroniser.

### Pourquoi l'IPC est essentiel ?

#### Le problème de l'isolation

Par défaut, sous Linux, chaque processus possède son **propre espace d'adressage mémoire** :

```
┌─────────────────┐         ┌─────────────────┐
│   Processus A   │         │   Processus B   │
│                 │         │                 │
│  int x = 42;    │         │  int y = 100;   │
│                 │         │                 │
│  Espace mémoire │    X    │  Espace mémoire │
│  isolé          │         │  isolé          │
└─────────────────┘         └─────────────────┘
        │                           │
        └───────────────────────────┘
        Pas de communication directe !
```

**Le processus A ne peut pas accéder aux variables du processus B**, et vice-versa. C'est une protection fondamentale du système d'exploitation pour la stabilité et la sécurité.

Mais comment faire si :
- Un processus producteur génère des données qu'un consommateur doit traiter ?
- Plusieurs processus doivent accéder à une base de données en cache ?
- Un système de logging centralisé doit recevoir des messages de multiples applications ?
- Des workers doivent se coordonner pour traiter des tâches sans se marcher dessus ?

**C'est là que l'IPC devient indispensable.**

---

## Vue d'ensemble de l'IPC sous Linux

Linux offre une riche palette de mécanismes IPC, chacun adapté à des besoins spécifiques. Ces mécanismes se répartissent en plusieurs catégories :

### 1. Communication par messages

**Principe** : Envoyer et recevoir des données structurées discrètes.

- **Pipes** (tubes)
  - Pipes anonymes : communication parent-enfant
  - Named pipes (FIFO) : communication entre processus non liés
  - Simple mais limité (flux unidirectionnel)

- **Message Queues** (files de messages)
  - Communication asynchrone avec typage de messages
  - Ordre FIFO garanti
  - Deux variantes : System V et POSIX

- **Sockets**
  - Communication réseau ou locale (UNIX domain sockets)
  - Flexible et universel
  - TCP/UDP pour réseau, socket local pour IPC

### 2. Mémoire partagée

**Principe** : Plusieurs processus accèdent à la **même région de mémoire physique**.

```
┌─────────────────┐         ┌─────────────────┐
│   Processus A   │         │   Processus B   │
│                 │         │                 │
│   Mapping       │         │   Mapping       │
│   virtuel       │         │   virtuel       │
└────────┬────────┘         └────────┬────────┘
         │                           │
         └────────────┬──────────────┘
                      │
              ┌───────▼────────┐
              │  Shared Memory │
              │  (RAM physique)│
              └────────────────┘
```

**Avantage** : Le mécanisme IPC **le plus rapide** (pas de copie).

**Variantes** :
- **Shared memory POSIX** (`shm_open()`, `mmap()`)
  - API moderne et intuitive
  - Intégration au système de fichiers (`/dev/shm`)

- **Shared memory System V** (`shmget()`, `shmat()`)
  - API plus ancienne mais très répandue
  - Utilise des clés numériques

- **Memory-mapped files** (`mmap()`)
  - Mappe un fichier directement en mémoire
  - Utilisable pour IPC ou accès efficace aux fichiers

### 3. Synchronisation

**Problème** : La mémoire partagée et les accès concurrents nécessitent une **coordination**.

Sans synchronisation :
```c
// Processus A                  // Processus B
int val = counter;              int val = counter;  // Tous deux lisent 10  
val++;                          val++;               // Tous deux incrémentent  
counter = val;                  counter = val;       // Résultat : 11 au lieu de 12 !  
```

**Solutions** :

- **Sémaphores**
  - Compteurs atomiques pour contrôler l'accès aux ressources
  - System V : ensembles de sémaphores, opérations atomiques multiples
  - POSIX : sémaphores individuels, API simple

- **Mutex et variables de condition** (pthread)
  - Exclusion mutuelle entre threads
  - Coordination et attente d'événements

- **Atomics** (C11 `stdatomic.h`)
  - Opérations sans verrous (lock-free)
  - Performance maximale pour synchronisation légère

### 4. Signaux

**Principe** : Notification asynchrone entre processus.

- `SIGTERM` : demande d'arrêt propre
- `SIGUSR1/SIGUSR2` : signaux personnalisables
- `SIGCHLD` : notification de fin d'un processus fils

**Limitation** : Pas adapté au transfert de données (juste des notifications).

---

## Les deux standards IPC : System V vs POSIX

Historiquement, deux familles d'API ont émergé pour l'IPC sous UNIX/Linux :

### System V IPC (années 1980)

**Caractéristiques** :
- Utilise des **clés numériques** générées par `ftok()`
- Les ressources ont des **identifiants opaques** (semid, shmid, msqid)
- **Persiste** après la fin des processus (nécessite suppression explicite)
- Outils spécialisés : `ipcs` (inspecter), `ipcrm` (supprimer)

**Exemple (sémaphore System V)** :
```c
key_t key = ftok("/tmp", 'S');  
int semid = semget(key, 1, IPC_CREAT | 0666);  
// ... opérations ...
semctl(semid, 0, IPC_RMID);  // Supprimer explicitement
```

### POSIX IPC (années 1990-2000)

**Caractéristiques** :
- Utilise des **noms symboliques** (`"/resource_name"`)
- Intégration au **système de fichiers** (`/dev/shm`, `/dev/mqueue`)
- Disparaît automatiquement (selon le type) ou via `unlink()`
- Outils standards : `ls`, `rm`, `cat`

**Exemple (sémaphore POSIX)** :
```c
sem_t *sem = sem_open("/my_sem", O_CREAT, 0666, 1);
// ... opérations ...
sem_unlink("/my_sem");
```

**Tableau comparatif rapide** :

| Aspect | System V | POSIX |
|--------|----------|-------|
| **Nommage** | Clés numériques | Noms symboliques |
| **API** | Complexe | Simple et intuitive |
| **Persistance** | Jusqu'à suppression | Dépend du type |
| **Inspection** | `ipcs` | `ls /dev/shm` |
| **Standard** | Très répandu (legacy) | Moderne, recommandé |

**Ce chapitre couvrira les deux standards**, car :
- System V est omniprésent dans le code existant
- POSIX est recommandé pour les nouveaux projets
- Comprendre les deux est essentiel pour un développeur système

---

## Architecture de ce chapitre

Ce chapitre explore les mécanismes IPC **avancés** pour la communication et le partage de données entre processus. Il est structuré de manière progressive :

### Sections du chapitre

**19.1 Shared Memory (Mémoire partagée POSIX)**
- API moderne avec `shm_open()` et `mmap()`
- Création et utilisation de segments partagés
- Patterns producteur-consommateur
- Intégration avec le système de fichiers

**19.2 Sémaphores System V**
- API complète : `semget()`, `semop()`, `semctl()`
- Ensembles de sémaphores
- Opérations atomiques multiples
- Le flag crucial `SEM_UNDO`
- Comparaison avec POSIX

**19.3 Message Queues System V**
- Communication asynchrone par messages typés
- Filtrage et priorités
- Patterns de communication avancés
- Inspection avec `ipcs`

**19.4 POSIX IPC vs System V IPC**
- Comparaison détaillée des deux standards
- Critères de choix
- Guide de migration
- Recommandations pour nouveaux projets

**19.5 Memory-mapped Files (mmap)**
- Mapper des fichiers en mémoire
- Performance et lazy loading
- MAP_SHARED vs MAP_PRIVATE
- Applications pratiques (bases de données, traitement d'images)

---

## Prérequis pour ce chapitre

Avant d'aborder ce chapitre, vous devriez maîtriser :

✅ **Module 6 - Programmation Système** :
- Processus et `fork()`
- Descripteurs de fichiers
- Appels système de base

✅ **Module 3 - Gestion de la mémoire** :
- Pointeurs et allocation dynamique
- Différence stack vs heap
- Concepts d'adressage

✅ **Module 6 (Section 18) - Threads et concurrence** :
- Problèmes de synchronisation
- Race conditions
- Concepts de mutex (optionnel mais utile)

Si ces concepts ne sont pas clairs, il est recommandé de les réviser avant de continuer.

---

## Contexte : Pourquoi l'IPC avancé ?

### Limitations des IPC basiques

Les mécanismes simples comme les **pipes** ont des limitations :

**Pipes anonymes** :
- ❌ Uniquement parent-enfant
- ❌ Unidirectionnel
- ❌ Flux brut d'octets (pas de structure)
- ❌ Buffer limité

**Named pipes (FIFO)** :
- ✅ Entre processus non liés
- ❌ Unidirectionnel
- ❌ Pas de persistance

### Besoins réels des applications modernes

Les applications complexes nécessitent :

1. **Performance** ⚡
   - Partage de gros volumes de données (bases de données, caches)
   - Accès concurrent à haute fréquence
   - Minimisation des copies

2. **Flexibilité** 🎯
   - Communication bidirectionnelle
   - Multi-producteurs / multi-consommateurs
   - Messages structurés et typés

3. **Synchronisation** 🔒
   - Coordination fine entre processus
   - Gestion des accès concurrents
   - Éviter les race conditions

4. **Persistance** 💾
   - Ressources qui survivent aux processus
   - État partagé maintenu
   - Administration centralisée

**C'est précisément ce que les mécanismes IPC avancés apportent.**

---

## Exemples d'applications réelles

### 1. Base de données : PostgreSQL

PostgreSQL utilise massivement l'IPC avancé :

```
┌────────────────────────────────────────┐
│          PostgreSQL Server             │
├────────────────────────────────────────┤
│  Shared Memory : Shared Buffer Pool    │ ← Cache de données
│  Sémaphores : Lock Manager             │ ← Synchronisation
│  Message Queues : Background Workers   │ ← Communication
└────────────────────────────────────────┘
         ▲              ▲              ▲
         │              │              │
    Client 1       Client 2       Client 3
```

- **Shared memory** : Buffer pool pour cacher les pages de données
- **Sémaphores** : Gestion des verrous sur les tables/lignes
- **Signaux** : Communication entre processus

### 2. Serveur web : Nginx

Architecture multi-processus avec IPC :

```
┌─────────────────┐
│  Master Process │
└────────┬────────┘
         │
    ┌────┴─────┬─────────┬─────────┐
    ▼          ▼         ▼         ▼
 Worker 1   Worker 2  Worker 3  Worker 4
    │          │         │         │
    └──────────┴─────────┴─────────┘
              │
    Shared Memory : Configuration
    Sémaphores : Accept Mutex
```

- **Shared memory** : Configuration partagée, compteurs de stats
- **Sémaphores** : Mutex pour `accept()` sur le socket
- **Signaux** : Contrôle et rechargement graceful

### 3. Système de cache : Redis

Redis en mode cluster utilise :

```
┌───────────┐    ┌───────────┐    ┌───────────┐
│  Redis 1  │◄──►│  Redis 2  │◄──►│  Redis 3  │
└─────┬─────┘    └─────┬─────┘    └─────┬─────┘
      │                │                │
      └────────────────┴────────────────┘
                       │
             Sockets + Shared State
```

- **Memory-mapped files** : Persistence sur disque
- **Sockets** : Communication cluster
- En interne : mmap pour RDB snapshots

### 4. Moteur de jeu : Unity

Partage de données entre le moteur et les scripts :

```
┌─────────────────┐         ┌──────────────┐
│  Moteur (C++)   │◄───────►│ Scripts (C#) │
│  Rendering      │  Shared │  Game Logic  │
│  Physics        │  Memory │              │
└─────────────────┘         └──────────────┘
```

- **Shared memory** : État du jeu, positions des objets
- **Sémaphores** : Synchronisation frame par frame

---

## Patterns de conception IPC courants

### 1. Producteur-Consommateur

**Problème** : Un producteur génère des données, des consommateurs les traitent.

**Solutions** :
- **Message Queues** : File de tâches asynchrone
- **Shared Memory + Sémaphores** : Buffer circulaire partagé

```
┌────────────┐     ┌───────────────┐     ┌─────────────┐
│ Producteur │────►│ Buffer/Queue  │────►│ Consommateur│
└────────────┘     └───────────────┘     └─────────────┘
                           │
                    Synchronisation
                      (sémaphores)
```

### 2. Client-Serveur

**Problème** : Plusieurs clients demandent des services à un serveur central.

**Solutions** :
- **Sockets** : Communication réseau ou locale
- **Shared Memory** : État partagé (cache)
- **Message Queues** : Requêtes asynchrones

```
    Client 1 ─┐
    Client 2 ─┼──► [ Serveur ] ◄──► Shared Memory (Cache)
    Client 3 ─┘
```

### 3. Pool de Workers

**Problème** : Distribuer des tâches entre plusieurs workers.

**Solutions** :
- **Message Queue** : File de tâches
- **Sémaphores** : Contrôle du nombre de workers actifs
- **Shared Memory** : État global partagé

```
┌──────────┐
│  Master  │
└────┬─────┘
     │
┌────┴──────────────────┐
│   Task Queue (MQ)     │
└───┬─────┬─────┬───────┘
    │     │     │
Worker1 Worker2 Worker3
```

### 4. Broadcast

**Problème** : Envoyer un message à tous les processus.

**Solutions** :
- **Shared Memory** : Flag de notification
- **Signaux** : `kill(-pid_group, SIGUSR1)`
- **Message Queue** : Type de message broadcast

---

## Performance : Quel mécanisme choisir ?

### Comparaison des débits

Voici un classement approximatif des mécanismes IPC par **performance** (du plus rapide au plus lent) :

| Rang | Mécanisme | Débit approximatif | Use Case |
|------|-----------|-------------------|----------|
| 🥇 1 | **Shared Memory** | ~10 GB/s | Gros volumes, haute fréquence |
| 🥈 2 | **mmap** | ~8 GB/s | Fichiers, persistence |
| 🥉 3 | **UNIX Domain Sockets** | ~5 GB/s | Flexibilité + performance |
| 4 | **Message Queues** | ~2 GB/s | Messages structurés |
| 5 | **Pipes** | ~1 GB/s | Simple, streaming |
| 6 | **TCP Sockets** | ~1 GB/s | Réseau, distant |

**Note** : Ces chiffres sont indicatifs et dépendent du matériel, de la taille des messages, etc.

### Critères de décision

**Utilisez Shared Memory si** :
- ✅ Performance maximale requise
- ✅ Gros volumes de données
- ✅ Accès aléatoire fréquent
- ❌ Nécessite synchronisation manuelle

**Utilisez Message Queues si** :
- ✅ Communication asynchrone
- ✅ Messages structurés/typés
- ✅ Besoin de buffering
- ❌ Moins performant que shared memory

**Utilisez Sockets si** :
- ✅ Communication réseau
- ✅ Flexibilité maximale
- ✅ Besoin de streaming
- ❌ Plus lent que shared memory locale

**Utilisez mmap si** :
- ✅ Accès fichier haute performance
- ✅ IPC via fichier
- ✅ Persistence automatique

---

## Défis de l'IPC avancé

### 1. Synchronisation

**Problème** : Les accès concurrents sans coordination produisent des résultats imprévisibles.

```c
// Sans synchronisation : DANGER !
shared_data->counter++;  // Race condition si plusieurs processus
```

**Solution** : Sémaphores, mutex, atomics.

### 2. Gestion des erreurs

**Problème** : Un processus peut crasher en tenant un verrou, bloquant tous les autres.

**Solution** : `SEM_UNDO` (System V), timeouts, handlers de signaux.

### 3. Fuites de ressources

**Problème** : Les ressources IPC System V persistent après la fin des processus.

```bash
# Orphelins à nettoyer manuellement
ipcs -a  # Voir les ressources  
ipcrm -s <semid>  # Supprimer  
```

**Solution** : Toujours nettoyer (`IPC_RMID`, `shm_unlink()`), utiliser POSIX si possible.

### 4. Portabilité

**Problème** : Certaines fonctionnalités sont spécifiques à Linux.

**Solution** : Privilégier POSIX pour la portabilité maximale.

### 5. Debugging

**Problème** : Les bugs d'IPC sont difficiles à reproduire et à déboguer.

**Outils** :
- `ipcs` : Inspecter ressources System V
- `ls /dev/shm` : Voir shared memory POSIX
- `strace` : Tracer les appels système
- Valgrind avec Helgrind/DRD : Détecter race conditions

---

## Sécurité et permissions

Les ressources IPC ont des **permissions** comme les fichiers :

```c
// Créer avec permissions 0666 (rw-rw-rw-)
int shmid = shmget(key, size, IPC_CREAT | 0666);

// Ou pour POSIX
sem_t *sem = sem_open("/my_sem", O_CREAT, 0660, 1);  // rw-rw----
```

**Bonnes pratiques** :
- ✅ Utiliser des permissions restrictives (0600, 0660)
- ✅ Valider les entrées
- ✅ Ne pas stocker de données sensibles sans chiffrement
- ✅ Nettoyer les ressources pour éviter les fuites d'information

---

## Objectifs pédagogiques du chapitre

À la fin de ce chapitre, vous serez capable de :

1. ✅ **Comprendre** les différents mécanismes IPC et leurs cas d'usage
2. ✅ **Implémenter** de la mémoire partagée (POSIX et System V)
3. ✅ **Synchroniser** des processus avec des sémaphores
4. ✅ **Communiquer** via des message queues
5. ✅ **Utiliser** mmap pour des accès fichiers performants
6. ✅ **Choisir** le bon mécanisme selon le contexte
7. ✅ **Déboguer** des problèmes de synchronisation
8. ✅ **Comparer** System V et POSIX IPC

---

## Structure de travail recommandée

Pour tirer le meilleur parti de ce chapitre :

### Approche progressive

1. **Lire l'introduction** (cette section) pour comprendre le contexte
2. **Suivre chaque section** dans l'ordre :
   - 19.1 : Shared Memory (POSIX) - Base moderne
   - 19.2 : Sémaphores System V - Synchronisation legacy
   - 19.3 : Message Queues - Communication asynchrone
   - 19.4 : Comparaison POSIX vs System V - Vue d'ensemble
   - 19.5 : mmap - Performance et fichiers

3. **Tester les exemples** de code fournis
4. **Inspecter les ressources** avec les outils (`ipcs`, `ls /dev/shm`)
5. **Combiner les mécanismes** (shared memory + sémaphores)

### Exercices suggérés (après lecture)

Bien que ce chapitre ne contienne pas d'exercices intégrés, voici des projets pour consolider :

- 📝 Implémenter un système de cache partagé avec LRU
- 📝 Créer un pool de workers avec file de tâches
- 📝 Développer un système de logging centralisé
- 📝 Construire un serveur simple avec workers en shared memory

---

## Commandes utiles à connaître

Avant de commencer, familiarisez-vous avec ces commandes :

```bash
# Inspection des ressources System V
ipcs          # Tout  
ipcs -s       # Sémaphores  
ipcs -m       # Shared memory  
ipcs -q       # Message queues  

# Suppression
ipcrm -s <semid>  
ipcrm -m <shmid>  
ipcrm -q <msqid>  

# Ressources POSIX
ls -la /dev/shm/      # Shared memory  
ls -la /dev/mqueue/   # Message queues  

# Informations système
cat /proc/sys/kernel/sem     # Limites sémaphores  
cat /proc/sys/kernel/shmmax  # Taille max shared memory  
cat /proc/sys/kernel/msgmax  # Taille max message  
```

---

## Avertissements importants

⚠️ **Attention** : Les mécanismes IPC avancés sont **puissants mais complexes**. Ils introduisent de nouveaux types de bugs :

1. **Race conditions** : Accès concurrents non synchronisés
2. **Deadlocks** : Verrouillages circulaires
3. **Fuites de ressources** : Ressources non libérées
4. **Corruption de données** : Écritures simultanées

**Recommandations** :
- ✅ Toujours tester avec plusieurs processus
- ✅ Utiliser Valgrind (Helgrind) pour détecter les races
- ✅ Implémenter une gestion d'erreurs robuste
- ✅ Documenter votre protocole de synchronisation
- ✅ Nettoyer systématiquement les ressources

---

## Prêt à commencer ?

Ce chapitre vous donnera les compétences nécessaires pour construire des applications système complexes et performantes. La mémoire partagée et l'IPC avancé sont au cœur de logiciels majeurs comme PostgreSQL, Redis, Nginx et bien d'autres.

**Prochaine étape** : Commencez par la section **19.1 Shared Memory** pour découvrir l'API POSIX moderne et construire votre première application IPC.

Bonne lecture et bon coding ! 🚀

---

## Références pour le chapitre

- Stevens & Rago, *Advanced Programming in the UNIX Environment*, Chapitres 15-16
- Kerrisk, *The Linux Programming Interface*, Chapitres 46-55
- POSIX.1-2001, POSIX.1-2008 standards
- Linux man pages : `man 7 shm_overview`, `man 7 sem_overview`, `man 7 mq_overview`
- Kernel documentation : `Documentation/vm/`, `Documentation/filesystems/`

---

**Section suivante** : 19.1 Shared Memory

⏭️ [Shared memory](/19-memoire-partagee-ipc/01-shared-memory.md)
