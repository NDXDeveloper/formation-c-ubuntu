🔝 Retour au [Sommaire](/SOMMAIRE.md)

# Parcours Recommandé : SysAdmin → Ingénieur Système

## 🎯 Profil Cible

Vous êtes administrateur système Linux avec une solide expérience en gestion de serveurs, scripts bash, et résolution de problèmes système. Vous maîtrisez la ligne de commande, les permissions, les services systemd, et vous avez peut-être déjà analysé des logs ou des core dumps. Maintenant, vous souhaitez comprendre **comment fonctionnent vraiment les choses** et développer vos propres outils système en C.

**Prérequis :**
- Expérience significative en administration Linux (2+ ans)
- Maîtrise de bash, sed, awk, grep
- Connaissance approfondie du système de fichiers Unix
- Compréhension des processus, permissions, réseau de base
- Expérience avec systemd, cron, logs système

**Niveau de départ :** Expert Linux, débutant en programmation C

---

## 🚀 Objectifs de la Transition

À l'issue de ce parcours, vous serez capable de :

1. **Comprendre le fonctionnement interne** des commandes Linux que vous utilisez quotidiennement (ls, ps, top, netstat...)
2. **Écrire vos propres outils système** en C pour des besoins spécifiques
3. **Déboguer des applications** qui plantent en production (core dumps, gdb)
4. **Analyser et optimiser** les performances système au niveau le plus bas
5. **Contribuer à des projets système** open-source (systemd, util-linux, procps...)
6. **Développer des daemons** et services système robustes

**Différence avec votre rôle actuel :**
- **Avant :** Vous utilisez les outils système et scripts pour gérer l'infrastructure
- **Après :** Vous créez les outils système et comprenez leur fonctionnement interne

---

## 📚 Parcours Modulaire Adapté

### Phase 1 : Du Script Bash au Programme C (3-4 semaines)

#### 🟢 Module 1 : L'Écosystème C sous Linux *(Priorité : ÉLEVÉE)*

**Pourquoi ce module en premier ?**

En tant que SysAdmin, vous connaissez Linux comme votre poche. Ce module vous montrera pourquoi le C est le langage de Linux et comment il se lie à tout ce que vous faites au quotidien.

**Sections essentielles :**
- 1.1 Pourquoi le C en 2025 ?
- 1.2 La relation symbiotique entre C et UNIX/Linux ⭐
- 1.3 Normes du langage (focus sur C11)

**Ce que vous découvrirez :**
- Pourquoi `ls`, `grep`, `systemd`, le kernel Linux sont tous écrits en C
- Comment vos scripts bash interagissent avec les programmes C du système
- La différence entre un script interprété et un programme compilé

**Analogie pour SysAdmin :** Si bash est comme gérer des serveurs via SSH et des scripts, le C est comme construire le serveur lui-même et son système d'exploitation.

---

#### 🟢 Module 2 : Fondamentaux du Langage *(Priorité : ÉLEVÉE)*

**Transition en douceur depuis bash**

Vous êtes habitué à écrire des scripts bash. Le C est plus strict mais infiniment plus puissant et rapide.

**Chapitre 4 : Types de données et représentation mémoire**
- 4.1 Types primitifs et leur taille
- 4.2 Signed vs Unsigned (crucial pour comprendre les PID, UID, GID)
- 4.5 Endianness et portabilité

**Pourquoi c'est important :** Vous avez déjà vu des valeurs négatives bizarres dans les logs ou des problèmes avec des UIDs mal gérés ? C'est lié aux types signés/non signés.

**Chapitre 5 : Opérateurs et structures de contrôle**
- 5.3 Opérateurs bit-à-bit ⭐ (vous les utilisez déjà avec `chmod` !)
- 5.5 Boucles et contrôle de flux
- 5.7 Formatage automatique avec clang-format

**Exemple concret :** Comprendre comment `chmod 755` fonctionne réellement :
```
7 = 111 en binaire = rwx
5 = 101 en binaire = r-x
5 = 101 en binaire = r-x
```
Vous manipulez des bits sans le savoir à chaque fois que vous utilisez chmod !

**Chapitre 6 : Les Fonctions**
- 6.1 Déclaration, définition et prototypes
- 6.2 La Pile : Comprendre les Stack Frames ⭐
- 6.5 La fonction main et ses paramètres (argc, argv)

**Connexion avec votre expérience :** Quand vous faites `./mon_script arg1 arg2` en bash, vous passez des paramètres. En C, c'est pareil mais vous avez un contrôle total sur leur traitement.

---

### Phase 2 : Configuration d'Environnement (1 semaine)

#### 🔵 Module 1 - Section 2 : Configuration de l'environnement Ubuntu

**2.1 Installation de la toolchain**
- Installation de GCC, make, gdb
- Commandes que vous connaissez déjà : `apt install build-essential`

**2.2 Les versions de GCC et leur gestion**
- Comment gérer plusieurs versions de GCC (utile si vous maintenez des systèmes legacy)

**2.5 DevContainers : Environnement reproductible moderne** ⭐
- 2.5.1 Qu'est-ce qu'un DevContainer ?
- 2.5.3 Configuration GCC/GDB/CMake/Valgrind

**Pourquoi DevContainers pour un SysAdmin ?**
Imaginez pouvoir distribuer un environnement de développement complet avec toute la toolchain à votre équipe, sans que personne n'ait à installer quoi que ce soit. C'est l'équivalent de Docker pour le développement.

---

#### 🟡 Module 5 - Chapitre 13 : Build Systems *(Priorité : MOYENNE)*

**13.2 GNU Make : Fondamentaux** ⭐

Commencez par Make car c'est plus proche de ce que vous connaissez avec bash.

- 13.2.1 Syntaxe des Makefile
- 13.2.2 Règles, cibles et dépendances
- 13.2.3 Variables et macros

**Connexion avec votre expérience :** Un Makefile, c'est comme un script bash amélioré qui sait quels fichiers ont changé et ne recompile que le nécessaire.

**13.1 CMake : Le standard moderne**

Une fois Make maîtrisé, passez à CMake. C'est plus complexe mais plus puissant et multi-plateforme.

---

### Phase 3 : La Mémoire - Comprendre ce qui se passe réellement (4-5 semaines)

#### 🟠 Module 3 : La Gestion de la Mémoire *(Priorité : CRITIQUE)* 🔥

**Pourquoi c'est crucial pour un SysAdmin → Ingénieur Système ?**

Vous avez probablement déjà vu des messages comme :
- "Segmentation fault (core dumped)"
- "Out of memory: Kill process 1234"
- Des applications qui consomment de plus en plus de mémoire

Ce module vous explique **exactement** ce qui se passe et comment le déboguer.

**Chapitre 7 : Les Pointeurs - Démystification** ⭐

Les pointeurs sont le concept le plus important (et le plus redouté) du C. Mais en tant que SysAdmin, vous avez déjà manipulé des concepts similaires.

- 7.1 Concept d'adresse mémoire
- 7.2 Déclaration, déréférencement et adresse
- 7.3 L'arithmétique des pointeurs
- 7.5 Pointeurs NULL et dangling pointers

**Analogie SysAdmin :**
- Un pointeur, c'est comme un lien symbolique : il pointe vers un emplacement
- Déréférencer un pointeur, c'est comme suivre le lien symbolique
- Un pointeur NULL, c'est comme un lien symbolique cassé
- Un "dangling pointer", c'est comme un lien symbolique vers un fichier supprimé

**Chapitre 8 : Tableaux et Chaînes de caractères**
- 8.1 Relation tableaux et pointeurs
- 8.3 Les chaînes en C (très différent de bash !)
- 8.5 Dangers des chaînes : Buffer Overflows ⭐

**Ce que vous apprendrez :** Pourquoi certaines applications sont vulnérables aux buffer overflows et comment les prévenir.

**Chapitre 9 : Allocation Dynamique** 🔥
- 9.1 Stack vs Heap : Diagramme et différences ⭐
- 9.2 Durée de vie et performance
- 9.3 Les fonctions malloc, calloc, realloc et free
- 9.5 Fuites mémoire : causes et prévention
- 9.6 Double free et corruption de tas

**Impact sur votre travail :**
Quand un processus consomme toute la RAM et que l'OOM killer intervient, c'est souvent dû à une fuite mémoire. Maintenant vous saurez l'identifier et la corriger.

---

### Phase 4 : Programmation Système - Le Cœur du Métier (6-8 semaines)

#### 🔴 Module 6 : Programmation Système Linux *(Priorité : CRITIQUE)* 🔥🔥🔥

**C'est LE module central pour votre transition vers ingénieur système.**

**Chapitre 16 : Gestion des Fichiers et Entrées/Sorties** ⭐

**Sections essentielles :**
- 16.1 Descripteurs de fichiers ⭐
- 16.2 Appels système (open, read, write, close, stat)
- 16.3 Différence appels système vs bibliothèque
- 16.4 Permissions et modes
- 16.5 I/O bufferisé vs non bufferisé
- 16.6 dup, dup2 et redirection

**Ce que vous apprendrez :**

Vous utilisez déjà ces concepts en bash :
```bash
# Redirection
ls > fichier.txt 2>&1

# Pipes
cat fichier.txt | grep "error"
```

Maintenant vous saurez exactement comment cela fonctionne au niveau C et pourquoi `stdin` est le descripteur 0, `stdout` le 1, et `stderr` le 2.

**Projet concret :** Réécrivez une version simplifiée de `cat`, `cp`, ou `wc` pour comprendre comment ces commandes fonctionnent réellement.

**Sections avancées :**
- 16.7 I/O multiplexing (select, poll, epoll)
- 16.8 I/O asynchrone (AIO)

**Utilité :** Comprendre comment des serveurs comme nginx gèrent des milliers de connexions simultanées.

---

**Chapitre 17 : Processus et Signaux** ⭐⭐⭐

**C'est le chapitre le plus important pour un SysAdmin.**

Vous manipulez des processus tous les jours : `ps`, `kill`, `systemctl`, `nice`, `jobs`... Ce chapitre vous explique ce qui se passe en coulisses.

**Sections essentielles :**
- 17.1 Création de processus (fork, exec) 🔥
- 17.2 Les PIDs et hiérarchie
- 17.3 Processus orphelins et zombies ⭐
- 17.4 Variables d'environnement
- 17.5 Gestion des signaux 🔥
- 17.6 Signaux courants (SIGTERM, SIGKILL, SIGHUP, SIGUSR1...)
- 17.8 Pipes anonymes
- 17.9 Named pipes (FIFO)

**Exemples concrets que vous comprendrez enfin :**

1. **Pourquoi `kill -9` est différent de `kill -15` ?**
   - SIGKILL (9) : tue immédiatement, non interceptable
   - SIGTERM (15) : demande gentiment au processus de se terminer

2. **Qu'est-ce qu'un processus zombie ?**
   - Un processus terminé dont le parent n'a pas récupéré le code de retour
   - Vous les avez vus dans `ps` avec un état `Z`

3. **Comment fonctionne le shell ?**
   - Le shell fait un `fork()` pour créer un nouveau processus
   - Puis un `exec()` pour remplacer ce processus par la commande à exécuter

4. **Pourquoi `systemctl reload nginx` ne coupe pas les connexions ?**
   - Le processus reçoit un signal (SIGHUP généralement) qui lui dit de recharger sa config sans s'arrêter

**Projet pratique :** Créez votre propre daemon qui :
- Se détache du terminal (daemonisation)
- Réagit à SIGHUP pour recharger sa configuration
- Écrit des logs dans syslog
- Se termine proprement sur SIGTERM

---

**Chapitre 18 : Threads et Concurrence (POSIX)** ⭐

**Sections pour un SysAdmin :**
- 18.1 Introduction à pthread
- 18.2 Création et terminaison de threads
- 18.5 Problèmes de concurrence : Race conditions
- 18.6 Mutex et sections critiques
- 18.7 Deadlocks

**Pourquoi c'est important :**
Comprendre comment les applications multi-threadées fonctionnent vous aide à :
- Diagnostiquer des deadlocks en production
- Comprendre pourquoi une application utilise 400% de CPU (4 cœurs à 100%)
- Analyser les core dumps de programmes multi-threadés

**Sections avancées (optionnelles pour commencer) :**
- 18.10 Concurrence lock-free avec les Atomics (C11)
- 18.11 Read-write locks
- 18.12 Thread-local storage

---

**Chapitre 20 : Réseau (Sockets)** ⭐

**Sections essentielles :**
- 20.1 Modèle Client/Serveur
- 20.2 Architecture TCP/IP (vous connaissez déjà, mais au niveau C)
- 20.3 Sockets TCP
- 20.4 Sockets UDP
- 20.6 Options de sockets (SO_REUSEADDR...)
- 20.8 Serveur concurrent

**Ce que vous apprendrez :**

Comment créer des outils réseau système comme :
- Un client de monitoring qui se connecte à un serveur
- Un serveur de logs centralisé
- Un outil de diagnostic réseau personnalisé

**Projet concret :** Créez un mini-serveur qui écoute sur un port et répond aux connexions. C'est la base pour comprendre comment fonctionnent Apache, nginx, ou sshd.

---

**Chapitre 19 : Mémoire partagée et IPC avancé** ⭐

**Pourquoi c'est utile :**
- 19.1 Shared memory (comment les processus partagent des données)
- 19.5 Memory-mapped files (mmap) - utilisé par de nombreuses bases de données

**Utilité pratique :** Comprendre comment des outils comme `ipcs` et `ipcrm` fonctionnent, et pourquoi vous devez parfois nettoyer des segments de mémoire partagée orphelins.

---

### Phase 5 : Débogage Professionnel (3-4 semaines)

#### 🟡 Module 5 - Chapitre 15 : Débogage et Analyse de Qualité *(Priorité : CRITIQUE)* 🔥

**En tant que SysAdmin, vous avez déjà dû analyser des crashes. Maintenant vous allez apprendre à le faire comme un pro.**

**15.2 Utilisation de GDB** ⭐⭐⭐
- 15.2.1 Breakpoints, watchpoints, catchpoints
- 15.2.2 Stepping (step, next, continue)
- 15.2.3 Backtrace et analyse de la pile 🔥
- 15.2.4 Inspection des variables
- 15.2.5 GDB TUI mode

**Cas d'usage quotidien :**
Un service plante en production. Vous avez un core dump. GDB vous permet de :
1. Charger le core dump : `gdb /usr/bin/monservice core.12345`
2. Voir la stack trace : `bt`
3. Inspecter les variables au moment du crash : `print ma_variable`
4. Comprendre exactement ce qui s'est passé

**15.3 GDB avancé**
- 15.3.1 Core dumps 🔥 (configuration avec `ulimit`, analyse)
- 15.3.2 Debugging distant (déboguer un service sur un serveur sans l'arrêter)
- 15.3.4 rr (Time Travel Debugging) - rejouez un crash à l'infini !

**15.4 Détection de fuites avec Valgrind** ⭐
- 15.4.1 Memcheck
- 15.4.2 Interprétation des rapports

**Exemple concret :**
Un daemon consomme de plus en plus de mémoire. Vous lancez :
```bash
valgrind --leak-check=full ./mon_daemon
```
Et Valgrind vous dit exactement où la fuite se produit.

**15.1 Sanitizers : Première ligne de défense**
- 15.1.1 AddressSanitizer (ASan)
- 15.1.4 LeakSanitizer (LSan)

**Avantage sur Valgrind :** Les sanitizers sont plus rapides et s'intègrent à la compilation. Parfaits pour détecter les bugs pendant le développement.

---

### Phase 6 : Observabilité Moderne et Outils Avancés (3-4 semaines)

#### 🔴 Module 6 - Chapitre 21 : Introduction à eBPF *(Priorité : HAUTE)* ⭐🔥

**eBPF est LA technologie qui va révolutionner votre métier de SysAdmin/Ingénieur Système.**

**Qu'est-ce qu'eBPF ?**

C'est une technologie qui vous permet d'exécuter du code dans le kernel Linux de manière sécurisée, sans avoir à compiler un module kernel. C'est révolutionnaire pour l'observabilité et le monitoring.

**Sections essentielles :**
- 21.1 Qu'est-ce qu'eBPF ?
  - 21.1.2 Pourquoi eBPF révolutionne Linux ⭐
- 21.2 Architecture eBPF
  - 21.2.1 BPF programs
  - 21.2.2 BPF maps
- 21.3 Use cases DevOps et observabilité 🔥
  - 21.3.1 Tracing et monitoring
  - 21.3.2 Networking
  - 21.3.3 Sécurité
  - 21.3.4 Performance analysis
- 21.5 Premier programme eBPF
  - 21.5.1 Hello World : tracer les appels système

**Exemples de ce que vous pourrez faire avec eBPF :**

1. **Tracer tous les appels à `open()` sur le système** en temps réel
2. **Identifier quel processus génère le plus d'I/O disque** avec une précision microseconde
3. **Détecter des comportements suspects** (ex: un processus qui essaie de lire /etc/shadow)
4. **Analyser les performances réseau** paquet par paquet
5. **Monitorer les latences des appels système** sans overhead

**Outils eBPF que vous utiliserez :**
- `bpftrace` : langage de haut niveau pour écrire des scripts eBPF rapidement
- `bcc` (BPF Compiler Collection) : collection d'outils prêts à l'emploi
- `libbpf` : bibliothèque C pour écrire vos propres outils

**Projet pratique :** Créez un outil qui trace tous les fichiers ouverts par un processus spécifique en temps réel. C'est plus puissant que `strace` et avec moins d'overhead !

---

### Phase 7 : Structures de Données et Organisation (2-3 semaines)

#### 🟢 Module 4 : Structures de Données et Modularité *(Priorité : MOYENNE)*

**Chapitre 10 : Types définis par l'utilisateur**
- 10.1 struct : Création d'objets complexes ⭐
- 10.2 Alignement mémoire et padding
- 10.5 enum : Code lisible et gestion d'états

**Pourquoi c'est utile :**
Créer des structures pour représenter des concepts système :
- Une structure pour un utilisateur (`struct user` avec UID, GID, nom...)
- Une structure pour un processus
- Une structure pour une connexion réseau

**Chapitre 12 : Organisation du code et Compilation modulaire**
- 12.1 Fichiers d'en-tête et gardes d'inclusion
- 12.3 Compilation séparée
- 12.4 Organisation d'un projet

**Transition de script bash vers projet C :**
En bash, vous avez un script monolithique. En C, vous apprenez à organiser le code en modules réutilisables.

---

### Phase 8 : Sécurité et Performance (2-3 semaines)

#### 🟡 Module 7 : Techniques Avancées et Optimisation

**Chapitre 26 : Sécurité et Code défensif** ⭐
- 26.1 Secure C Coding Standards (CERT C)
- 26.2 Validation des entrées
- 26.3 Prévention des buffer overflows
- 26.6 Principe du moindre privilège
- 26.8 Compilation avec hardening flags

**Pourquoi c'est crucial :**
En tant que SysAdmin, vous êtes responsable de la sécurité. Savoir comment les vulnérabilités système se produisent vous rend bien meilleur dans votre métier.

**Exemple :** Comprendre pourquoi `strcpy()` est dangereuse et utiliser `strncpy()` ou `strlcpy()` à la place.

**Chapitre 27 : Optimisation et Performance**
- 27.1 Flags d'optimisation GCC (-O2, -O3)
- 27.3 Profiling (gprof, perf) ⭐
- 27.4 Cache awareness
- 27.10 Benchmarking rigoureux

**Utilité pratique :**
- Identifier les bottlenecks d'une application système
- Optimiser un outil que vous avez développé
- Comprendre pourquoi une application consomme 100% CPU

---

## 📊 Chronologie Recommandée

### Parcours Standard (20-24 semaines)

**Semaines 1-4 : Fondations du C**
- Module 1 : Écosystème C
- Module 2 : Fondamentaux du langage (chapitres 4, 5, 6)
- Configuration de l'environnement
- **Mini-projet :** Réécrivez un script bash simple en C

**Semaines 5-8 : Maîtriser la Mémoire**
- Module 3 complet (Pointeurs, Tableaux, Allocation dynamique)
- Chapitre 15.2 : GDB (commencez à déboguer vos programmes)
- **Projet :** Implémentez une liste chaînée simple

**Semaines 9-14 : Programmation Système (CŒUR)**
- Module 6 - Chapitre 16 : Fichiers et I/O
- Module 6 - Chapitre 17 : Processus et Signaux 🔥
- Module 6 - Chapitre 20 : Sockets (bases)
- **Projet majeur :** Créez un daemon de monitoring système

**Semaines 15-18 : Débogage et Observabilité**
- Module 5 - Chapitre 15 complet (GDB, Valgrind, Sanitizers)
- Module 6 - Chapitre 21 : eBPF ⭐
- **Projet :** Outil de tracing eBPF personnalisé

**Semaines 19-22 : Approfondissement**
- Module 6 - Chapitre 18 : Threads (bases)
- Module 4 - Chapitre 10 : Structures
- Module 7 - Chapitre 26 : Sécurité
- Module 7 - Chapitre 27 : Performance et profiling

**Semaines 23-24 : Consolidation**
- Module 9 : Projets de synthèse
- Contribution à un projet open-source
- **Projet final :** Voir section ci-dessous

---

### Parcours Intensif (14-16 semaines)

Pour ceux qui peuvent y consacrer plus de temps (20h+/semaine) :

**Semaines 1-3 :** Fondations (Modules 1 et 2)  
**Semaines 4-6 :** Mémoire (Module 3)  
**Semaines 7-11 :** Programmation Système (Module 6 - chapitres 16, 17, 20, 21)  
**Semaines 12-14 :** Débogage (Module 5 - chapitre 15)  
**Semaines 15-16 :** Projet final et consolidation  

---

## 🎓 Compétences Acquises

### Avant cette formation

- ✓ Vous administrez des serveurs Linux
- ✓ Vous écrivez des scripts bash
- ✓ Vous utilisez des outils comme `strace`, `lsof`, `netstat`
- ✓ Vous lisez les logs et diagnostiquez des problèmes
- ✓ Vous gérez les services avec systemd

**Mais :**
- ❌ Vous ne comprenez pas vraiment comment ces outils fonctionnent
- ❌ Vous êtes limité par ce que bash peut faire
- ❌ Vous ne pouvez pas déboguer efficacement les applications qui plantent
- ❌ Vous dépendez des outils existants

---

### Après cette formation

✅ **Compréhension profonde du système**
- Vous savez exactement comment Linux gère les processus, la mémoire, les fichiers
- Vous comprenez ce que font les appels système sous le capot
- Vous maîtrisez les concepts de fork, exec, signaux, descripteurs de fichiers

✅ **Développement d'outils système**
- Vous créez vos propres outils en C pour des besoins spécifiques
- Vous pouvez automatiser des tâches complexes au niveau système
- Vous développez des daemons et services robustes

✅ **Débogage avancé**
- Vous analysez des core dumps avec GDB
- Vous identifiez les fuites mémoire avec Valgrind
- Vous tracez les appels système avec eBPF
- Vous comprenez les stack traces et pouvez les interpréter

✅ **Observabilité moderne**
- Vous utilisez eBPF pour des diagnostics en temps réel
- Vous créez des outils de monitoring personnalisés
- Vous analysez les performances au niveau le plus bas

✅ **Contribution open-source**
- Vous pouvez lire et comprendre le code de projets système (systemd, util-linux...)
- Vous contribuez à des projets C open-source
- Vous corrigez des bugs dans les outils système que vous utilisez

---

## 💼 Débouchés Professionnels

Cette formation vous positionne pour évoluer vers :

**Postes accessibles :**
- **Ingénieur Système Senior**
- **Site Reliability Engineer (SRE)** avec focus système
- **Performance Engineer**
- **Linux Kernel Developer** (avec approfondissement)
- **Développeur d'outils système**
- **Consultant en performance et optimisation système**
- **Ingénieur en observabilité** (eBPF)

**Avantage compétitif :**
La plupart des SysAdmin ne programment pas en C. En maîtrisant ce langage, vous vous démarquez considérablement et accédez à des postes d'ingénierie système avec des salaires bien supérieurs.

**Évolution salariale typique :**
- SysAdmin : 35-50k€
- Ingénieur Système : 45-70k€
- SRE avec compétences système avancées : 55-85k€
- Performance Engineer / eBPF specialist : 60-90k€+

---

## 🛠️ Modules à Prioriser vs Reporter

### ⚡ Priorité CRITIQUE (à maîtriser absolument)

- ✅ Module 3 : Gestion de la mémoire (pointeurs, allocation)
- ✅ Module 6 - Chapitres 16, 17 : Fichiers, processus, signaux
- ✅ Module 5 - Chapitre 15 : GDB et débogage
- ✅ Module 6 - Chapitre 21 : eBPF

### 🟡 Priorité MOYENNE (utile mais pas immédiate)

- Module 4 : Structures de données (surtout chapitre 10)
- Module 5 - Chapitre 13 : Build systems (Make surtout)
- Module 6 - Chapitre 18 : Threads (bases)
- Module 7 - Chapitre 26 : Sécurité

### 🔵 Priorité BASSE (à voir plus tard)

- Module 4 - Chapitre 11 : Structures de données avancées
- Module 6 - Chapitre 19 : IPC avancé (sauf mmap)
- Module 7 - Chapitres 22-25 : Techniques très avancées
- Module 8 : CI/CD (sauf si vous voulez aussi devenir DevOps)

---

## 🚀 Projet de Validation de Compétences

### **Projet Final : Daemon de Monitoring Système Avancé**

**Objectif :** Créer un daemon complet qui démontre toutes les compétences acquises.

**Fonctionnalités :**

1. **Gestion des processus (Module 6 - Ch. 17)**
   - Le daemon se détache correctement du terminal
   - Crée un fichier PID dans `/var/run/`
   - Réagit proprement aux signaux :
     - SIGTERM : arrêt propre
     - SIGHUP : rechargement de la configuration
     - SIGUSR1 : rotation des logs

2. **Collecte de métriques système (Module 6 - Ch. 16)**
   - Lit `/proc/stat` pour CPU
   - Lit `/proc/meminfo` pour la mémoire
   - Lit `/proc/net/dev` pour le réseau
   - Utilise `getrusage()` pour les ressources

3. **Traçage eBPF optionnel (Module 6 - Ch. 21)**
   - Trace les appels système les plus fréquents
   - Identifie les processus consommant le plus d'I/O

4. **Communication (Module 6 - Ch. 20)**
   - Expose les métriques via un socket Unix
   - Ou via HTTP simple (bonus)

5. **Logging (Module 6 - Ch. 16)**
   - Écrit dans syslog
   - Supporte différents niveaux de log
   - Rotation automatique

6. **Robustesse**
   - Gestion d'erreurs complète
   - Pas de fuites mémoire (vérifié avec Valgrind)
   - Pas de race conditions (vérifié avec ThreadSanitizer si multi-threadé)

**Compétences validées :**
- ✅ Fork, daemonisation, PID files
- ✅ Gestion des signaux
- ✅ Lecture du système de fichiers /proc
- ✅ Parsing de fichiers
- ✅ Sockets (Unix ou TCP)
- ✅ Logging système
- ✅ Gestion de la mémoire
- ✅ Débogage avec GDB et Valgrind
- ✅ Optionnel : eBPF

**Durée estimée :** 3-4 semaines

---

## 📚 Ressources Complémentaires pour SysAdmin

### Livres Essentiels

1. **"The Linux Programming Interface"** par Michael Kerrisk
   - LA référence pour la programmation système Linux
   - Écrit par le mainteneur des man pages Linux

2. **"Advanced Programming in the UNIX Environment"** (APUE) par Stevens
   - Le classique de la programmation système Unix/Linux

3. **"Linux System Programming"** par Robert Love
   - Plus accessible, parfait pour débuter

### Projets Open-Source à Étudier

Une fois les bases acquises, étudiez le code de ces projets :

1. **util-linux** : Collection d'outils système (`mount`, `fdisk`, `lsblk`...)
   - Code relativement simple à comprendre
   - Nombreux exemples d'appels système

2. **procps** : Outils de monitoring (`ps`, `top`, `free`...)
   - Montre comment lire `/proc`

3. **systemd** : Le système d'init moderne
   - Plus complexe mais fascinant
   - Utilise de nombreuses APIs système avancées

4. **strace** : Outil de tracing des appels système
   - Montre comment tracer d'autres processus

---

## 🎯 Conseils pour Réussir en tant que SysAdmin

### 1. **Partez de votre expérience**

Vous avez un avantage énorme : vous connaissez déjà Linux. Reliez chaque concept C à quelque chose que vous connaissez :
- Les pipes en C → les pipes shell (`|`)
- Les signaux en C → `kill` et `killall`
- Les permissions en C → `chmod` et `chown`
- Les processus en C → `ps` et `top`

### 2. **Réécrivez vos outils préférés**

Le meilleur moyen d'apprendre :
- Semaine 1-2 : Réécrivez `cat` et `wc`
- Semaine 3-4 : Réécrivez `grep` (version simple)
- Semaine 5-6 : Réécrivez `ls`
- Semaine 8-10 : Créez votre propre `top` simplifié

### 3. **Déboguez en production**

Dès que vous êtes à l'aise avec GDB :
- Configurez les core dumps sur vos serveurs de test (`ulimit -c unlimited`)
- Quand une application plante, analysez le core dump
- Installez les packages de debug symbols (`-dbgsym` sur Ubuntu)

### 4. **Explorez /proc et /sys**

Ces pseudo-systèmes de fichiers sont une mine d'informations :
```bash
# Listez tous les descripteurs de fichiers d'un processus
ls -l /proc/1234/fd/

# Voyez les mappings mémoire
cat /proc/1234/maps

# Voyez les limites
cat /proc/1234/limits
```

Puis écrivez des programmes C qui lisent ces informations.

### 5. **Utilisez eBPF dès que possible**

eBPF est l'avenir du monitoring système. Dès la semaine 12-14, commencez à jouer avec :
```bash
# Installez bpftrace
apt install bpftrace

# Tracez tous les open()
sudo bpftrace -e 'tracepoint:syscalls:sys_enter_open { printf("%s %s\n", comm, str(args->filename)); }'
```

### 6. **Contribuez à l'open-source**

Une fois à l'aise (semaine 16-20) :
- Trouvez un bug dans un outil système que vous utilisez
- Proposez un patch
- C'est le meilleur moyen de progresser et de se faire remarquer

---

## ✅ Checklist de Progression

Cochez au fur et à mesure de votre progression :

### Semaines 1-4 : Fondations
- [ ] J'ai écrit mon premier "Hello World" en C
- [ ] Je comprends la différence entre pile et tas
- [ ] Je sais compiler avec GCC et les options de base
- [ ] J'ai réécrit `cat` en C

### Semaines 5-8 : Mémoire
- [ ] Je maîtrise les pointeurs et leur arithmétique
- [ ] Je sais utiliser malloc/free correctement
- [ ] Je détecte les fuites mémoire avec Valgrind
- [ ] J'ai implémenté une liste chaînée

### Semaines 9-14 : Système
- [ ] Je comprends les descripteurs de fichiers
- [ ] Je sais faire un fork() et exec()
- [ ] Je gère les signaux (SIGTERM, SIGHUP...)
- [ ] J'ai créé un daemon simple
- [ ] Je sais créer un socket TCP

### Semaines 15-18 : Débogage avancé
- [ ] Je maîtrise GDB (breakpoints, backtrace, inspect)
- [ ] Je sais analyser un core dump
- [ ] J'ai écrit mon premier programme eBPF
- [ ] J'utilise bpftrace régulièrement

### Semaines 19-24 : Expertise
- [ ] Je comprends les threads et le multithreading
- [ ] Je profile avec perf
- [ ] J'ai contribué à un projet open-source
- [ ] J'ai terminé mon projet final

---

## 🎉 Conclusion

Cette formation transforme un SysAdmin expérimenté en ingénieur système capable de :
- **Comprendre** le fonctionnement interne de Linux
- **Créer** ses propres outils système
- **Déboguer** les applications au niveau le plus bas
- **Optimiser** les performances système
- **Contribuer** à l'écosystème open-source Linux

**Votre avantage :** Contrairement aux développeurs qui apprennent le C sans connaître Linux, vous partez avec une connaissance profonde du système. Vous apprenez juste à programmer ce que vous administrez déjà.

**Durée réaliste :** 20-24 semaines à raison de 10-15h/semaine. C'est un investissement, mais il transformera complètement votre carrière.

**Premier pas :** Installez GCC, créez votre premier `hello.c`, et commencez le Module 1 cette semaine. Dans 6 mois, vous aurez un skillset que très peu de professionnels IT possèdent.

**Bon courage dans votre transition vers ingénieur système ! 🚀**

---


⏭️ [Embedded → Linux Embarqué](/parcours-recommandes/03-embedded-vers-linux-embarque.md)
