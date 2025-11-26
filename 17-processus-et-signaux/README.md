🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 17. Processus et Signaux

## Introduction au chapitre

Bienvenue dans le **chapitre 6 : Programmation Système Linux**, et plus précisément dans le chapitre consacré aux **processus et signaux**. Ce chapitre est fondamental pour comprendre comment fonctionnent les systèmes d'exploitation modernes et comment créer des applications système robustes.

### Qu'est-ce qu'un processus ?

Un **processus** est une instance d'un programme en cours d'exécution. Lorsque vous lancez un programme (comme votre éditeur de texte, votre navigateur web, ou un script), le système d'exploitation crée un processus pour l'exécuter.

**Analogie :** Si un programme est une recette de cuisine, un processus est l'acte de cuisiner cette recette. Vous pouvez avoir plusieurs personnes (processus) qui suivent la même recette (programme) en même temps, chacune avec ses propres ingrédients (données) et son propre état d'avancement.

### Composants d'un processus

Chaque processus possède :

- **Un identifiant unique (PID)** : Process ID, un numéro qui identifie le processus
- **Son propre espace mémoire** : Code, données, pile (stack), tas (heap)
- **Son état d'exécution** : En cours, en attente, stoppé, terminé
- **Ses ressources** : Fichiers ouverts, connexions réseau, etc.
- **Ses variables d'environnement** : Configuration héritée du parent
- **Son contexte** : Registres CPU, pointeur d'instruction, etc.

### Qu'est-ce qu'un signal ?

Un **signal** est une notification asynchrone envoyée à un processus pour lui signaler un événement. C'est comme une alarme ou un message urgent qui peut interrompre le processus à tout moment.

**Exemples de signaux :**
- Ctrl+C → Signal `SIGINT` (interruption)
- Erreur de segmentation → Signal `SIGSEGV`
- Enfant terminé → Signal `SIGCHLD`
- Demande d'arrêt → Signal `SIGTERM`

### Pourquoi ce chapitre est-il important ?

La maîtrise des processus et des signaux est essentielle pour :

1. **Créer des applications multi-processus** : Paralléliser les tâches, améliorer les performances
2. **Gérer le cycle de vie des processus** : Création, surveillance, terminaison propre
3. **Gérer les communications** : Faire communiquer des processus entre eux
4. **Réagir aux événements système** : Gérer les interruptions, les erreurs, les demandes d'arrêt
5. **Développer des services et daemons** : Applications en arrière-plan robustes
6. **Comprendre le fonctionnement de Linux** : Fondations du système d'exploitation

## Vue d'ensemble du système de processus Linux

### La hiérarchie des processus

Sous Linux, les processus sont organisés en **arbre hiérarchique** :

```
systemd (PID 1)
│
├── sshd
│   └── bash
│       └── votre_programme
│
├── gnome-shell
│   ├── firefox
│   └── vscode
│
└── cron
    └── script_backup
```

- **systemd (PID 1)** : Le premier processus lancé au démarrage, ancêtre de tous les autres
- Chaque processus a **un parent** et peut avoir **plusieurs enfants**
- Relation parent-enfant créée par l'appel système `fork()`

### États d'un processus

Un processus peut être dans l'un de ces états :

| État | Symbole | Description |
|------|---------|-------------|
| **Running** | R | En cours d'exécution sur le CPU |
| **Sleeping** | S | En attente (interruptible) |
| **Uninterruptible** | D | En attente (non interruptible, I/O) |
| **Stopped** | T | Stoppé (Ctrl+Z) |
| **Zombie** | Z | Terminé mais pas nettoyé |

### Le cycle de vie d'un processus

```
┌─────────────┐
│   CRÉATION  │  fork() / exec()
└──────┬──────┘
       │
       ↓
┌─────────────┐
│  EXÉCUTION  │  Le processus travaille
└──────┬──────┘
       │
       ↓
┌─────────────┐
│ TERMINAISON │  exit() / signal / erreur
└──────┬──────┘
       │
       ↓
┌─────────────┐
│   NETTOYAGE │  wait() par le parent
└─────────────┘
```

## Concepts clés que vous allez apprendre

### 1. Création et gestion de processus (Sections 17.1 - 17.3)

Vous apprendrez à :
- **Créer des processus** avec `fork()`
- **Comprendre les PIDs** et la hiérarchie
- **Gérer les processus orphelins et zombies**
- **Attendre la terminaison** avec `wait()` et `waitpid()`

**Exemple aperçu :**
```c
pid_t pid = fork();

if (pid == 0) {
    // Code du processus enfant
    printf("Je suis l'enfant!\n");
} else {
    // Code du processus parent
    printf("Je suis le parent, mon enfant a le PID %d\n", pid);
    wait(NULL);  // Attendre l'enfant
}
```

### 2. Variables d'environnement (Section 17.4)

Les variables d'environnement permettent de configurer les processus :
- `PATH` : Chemins de recherche des exécutables
- `HOME` : Répertoire personnel
- `LANG` : Langue et encodage
- Variables personnalisées pour vos applications

**Exemple aperçu :**
```c
char *path = getenv("PATH");
setenv("MA_VARIABLE", "valeur", 1);
```

### 3. Gestion des signaux (Sections 17.5 - 17.7)

Les signaux sont essentiels pour :
- **Interruption propre** : Réagir à Ctrl+C
- **Gestion des erreurs** : Détecter les segfaults
- **Communication** : Envoyer des notifications entre processus
- **Masquage** : Protéger les sections critiques

**Signaux courants :**
- `SIGINT` (2) : Interruption (Ctrl+C)
- `SIGTERM` (15) : Terminaison propre
- `SIGKILL` (9) : Tuer immédiatement (non capturable)
- `SIGCHLD` (17) : Enfant terminé

**Exemple aperçu :**
```c
void signal_handler(int signum) {
    printf("Signal %d reçu!\n", signum);
}

signal(SIGINT, signal_handler);
```

### 4. Communication inter-processus (Sections 17.8 - 17.9)

Faire communiquer des processus avec :
- **Pipes anonymes** : Communication parent-enfant
- **Named pipes (FIFO)** : Communication entre processus indépendants

**Exemple aperçu :**
```c
int pipefd[2];
pipe(pipefd);

if (fork() == 0) {
    // Enfant lit
    read(pipefd[0], buffer, size);
} else {
    // Parent écrit
    write(pipefd[1], "Hello!", 7);
}
```

## Exemples d'applications réelles

### 1. Serveur web (Apache, Nginx)

```
Processus principal (master)
├── Processus worker 1 (gère les clients)
├── Processus worker 2 (gère les clients)
├── Processus worker 3 (gère les clients)
└── Processus worker 4 (gère les clients)
```

- Utilise `fork()` pour créer des workers
- Gère `SIGHUP` pour recharger la configuration
- Utilise `SIGTERM` pour l'arrêt propre

### 2. Shell (bash, zsh)

Quand vous tapez une commande :
```bash
ls -l | grep txt
```

Le shell :
1. Crée des processus avec `fork()`
2. Connecte les processus avec des pipes
3. Exécute les commandes avec `exec()`
4. Attend leur terminaison avec `wait()`

### 3. Daemon (service en arrière-plan)

Un daemon comme `sshd` :
- Se détache du terminal avec `fork()` et `setsid()`
- Gère les signaux pour recharger sa configuration
- Crée des processus enfants pour chaque connexion
- Nettoie les enfants terminés avec `SIGCHLD`

### 4. Traitement parallèle

Pour traiter un gros fichier en parallèle :
```
Processus principal
├── Worker 1 (lignes 1-1000)
├── Worker 2 (lignes 1001-2000)
├── Worker 3 (lignes 2001-3000)
└── Worker 4 (lignes 3001-4000)
```

Chaque worker traite une partie indépendamment, le parent attend la fin de tous.

## Outils de surveillance des processus

Avant de commencer à programmer, familiarisons-nous avec les outils système :

### Commande `ps`

```bash
# Lister vos processus
ps

# Tous les processus avec détails
ps aux

# Arbre des processus
ps auxf

# Processus d'un utilisateur
ps -u utilisateur
```

### Commande `top` et `htop`

Surveillance en temps réel :
```bash
top      # Moniteur système basique
htop     # Version améliorée (à installer)
```

### Commande `pstree`

Affiche l'arbre hiérarchique :
```bash
pstree           # Arbre complet
pstree -p        # Avec les PIDs
pstree -p 1234   # Arbre d'un processus spécifique
```

### Commande `kill`

Envoyer des signaux :
```bash
kill PID            # SIGTERM (arrêt propre)
kill -9 PID         # SIGKILL (force)
kill -USR1 PID      # Signal utilisateur
killall firefox     # Tuer par nom
```

### Répertoire `/proc`

Linux expose les informations des processus dans `/proc` :
```bash
# Informations sur le processus 1234
cat /proc/1234/status     # État général
cat /proc/1234/cmdline    # Ligne de commande
cat /proc/1234/environ    # Variables d'environnement
ls -l /proc/1234/fd       # Fichiers ouverts
```

## Prérequis et concepts nécessaires

### Connaissances requises

Avant d'aborder ce chapitre, vous devriez maîtriser :

1. **Les bases du C** :
   - Pointeurs et gestion de la mémoire
   - Structures et types de données
   - Compilation et linking

2. **Les descripteurs de fichiers** :
   - `open()`, `read()`, `write()`, `close()`
   - stdin (0), stdout (1), stderr (2)

3. **Les bases de Linux** :
   - Ligne de commande
   - Système de fichiers
   - Permissions

### Concepts qui seront approfondis

Ce chapitre approfondira :
- La mémoire et les espaces d'adressage
- Les appels système (system calls)
- La gestion des ressources système
- La synchronisation et la concurrence

## Structure du chapitre

Voici comment ce chapitre est organisé :

### 📚 Partie 1 : Les processus de base
- **17.1** Création de processus (`fork()`)
- **17.2** Les PIDs et hiérarchie
- **17.3** Processus orphelins et zombies

### 🔔 Partie 2 : Configuration et signaux
- **17.4** Variables d'environnement
- **17.5** Gestion des signaux
- **17.6** Signaux courants (détails)
- **17.7** Masquage de signaux

### 🔗 Partie 3 : Communication inter-processus
- **17.8** Pipes anonymes
- **17.9** Named pipes (FIFO)

## Approche pédagogique

### Comment étudier ce chapitre ?

1. **Lisez les sections dans l'ordre** : Chaque section s'appuie sur les précédentes
2. **Testez tous les exemples** : Compilez et exécutez chaque programme
3. **Expérimentez** : Modifiez les exemples pour voir ce qui se passe
4. **Utilisez les outils système** : `ps`, `top`, `strace` pour observer
5. **Déboguez activement** : Utilisez `gdb` pour comprendre les processus

### Erreurs courantes (que nous éviterons)

⚠️ **Erreurs fréquentes chez les débutants :**

1. **Oublier d'attendre les enfants** → Processus zombies
2. **Ne pas gérer les signaux** → Terminaison brutale
3. **Fermeture incorrecte des descripteurs** → Fuites de ressources
4. **Confusion PID dans l'enfant** → Logique incorrecte
5. **Ne pas vérifier les erreurs** → Comportement imprévisible

### Conseils pour réussir

✅ **Bonnes pratiques :**

1. **Toujours vérifier les valeurs de retour** des appels système
2. **Gérer proprement les signaux** (au minimum `SIGINT` et `SIGTERM`)
3. **Nettoyer les ressources** (fermer les descripteurs, attendre les enfants)
4. **Utiliser les outils de diagnostic** (`strace`, `lsof`, `/proc`)
5. **Tester les cas limites** (que se passe-t-il si `fork()` échoue ?)

## Compilation et test

### Compiler vos programmes

```bash
# Compilation basique
gcc -Wall -Wextra -o mon_programme mon_programme.c

# Avec symboles de debug
gcc -Wall -Wextra -g -o mon_programme mon_programme.c

# Afficher les warnings supplémentaires
gcc -Wall -Wextra -Wpedantic -o mon_programme mon_programme.c
```

### Déboguer avec `strace`

`strace` affiche tous les appels système d'un programme :

```bash
# Tracer un programme
strace ./mon_programme

# Tracer uniquement les signaux et processus
strace -e trace=signal,process ./mon_programme

# Tracer un processus en cours
strace -p PID
```

### Déboguer avec `gdb`

```bash
# Lancer avec gdb
gdb ./mon_programme

# Commandes gdb utiles
(gdb) run              # Exécuter
(gdb) break fork       # Breakpoint sur fork
(gdb) info inferiors   # Voir les processus
(gdb) set follow-fork-mode child  # Suivre l'enfant
```

## Ressources et documentation

### Pages de manuel (man)

Les pages de manuel sont votre meilleure ressource :

```bash
man 2 fork      # Appel système fork()
man 2 wait      # Appel système wait()
man 2 kill      # Appel système kill()
man 2 signal    # Gestion des signaux
man 2 pipe      # Création de pipes
man 7 signal    # Vue d'ensemble des signaux
```

**Sections du manuel :**
- Section 1 : Commandes utilisateur
- Section 2 : Appels système
- Section 3 : Fonctions de bibliothèque
- Section 7 : Conventions et divers

### Références en ligne

- [The Linux Programming Interface](http://man7.org/tlpi/) - Livre de référence
- [Beej's Guide to Unix IPC](https://beej.us/guide/bgipc/) - Guide des IPC
- [Linux kernel documentation](https://www.kernel.org/doc/)

## À quoi s'attendre

### Compétences que vous développerez

À la fin de ce chapitre, vous serez capable de :

- ✅ Créer et gérer des processus avec `fork()` et `wait()`
- ✅ Comprendre et naviguer dans la hiérarchie des processus
- ✅ Gérer les signaux pour créer des applications robustes
- ✅ Utiliser les pipes pour la communication inter-processus
- ✅ Configurer les processus avec les variables d'environnement
- ✅ Éviter les pièges courants (zombies, fuites de ressources)
- ✅ Créer des architectures multi-processus simples
- ✅ Déboguer des applications système

### Applications que vous pourrez créer

Avec ces connaissances, vous pourrez développer :

- 🖥️ **Serveurs concurrents** : Gérer plusieurs clients simultanément
- 🤖 **Daemons et services** : Applications en arrière-plan
- 🔧 **Outils système** : Scripts et utilitaires personnalisés
- 📊 **Applications de monitoring** : Surveillance de processus
- 🔄 **Gestionnaires de tâches** : Orchestrer des workers
- 🌐 **Applications client-serveur** : Architecture distribuée simple

## Motivation finale

La programmation système peut sembler intimidante au début, mais c'est l'une des compétences les plus gratifiantes en informatique. Comprendre comment fonctionnent les processus vous donne une vision profonde du système d'exploitation et vous permet de créer des applications puissantes et efficaces.

**Citations inspirantes :**

> "Les processus sont les citoyens de votre système d'exploitation."
> — Concept fondamental des systèmes Unix

> "Un bon programmeur système pense en processus et en ressources."
> — Sagesse Unix

## Prêt à commencer ?

Vous avez maintenant une vue d'ensemble de ce qui vous attend dans ce chapitre. Les concepts peuvent sembler abstraits pour le moment, mais ils deviendront concrets et naturels au fur et à mesure que vous progresserez dans les sections.

**Rappel important :**
- Testez chaque exemple
- N'hésitez pas à revenir sur les sections précédentes
- Utilisez les outils système pour observer
- Faites des expérimentations

### Prochaine étape

Direction la **Section 17.1 : Création de processus** où vous apprendrez à utiliser `fork()` pour créer votre premier processus !

---

**Bon apprentissage !** 🚀

Les processus et les signaux sont au cœur de Linux. Maîtrisez-les, et vous maîtriserez une partie essentielle de la programmation système.

⏭️ [Création de processus](/17-processus-et-signaux/01-creation-processus.md)
