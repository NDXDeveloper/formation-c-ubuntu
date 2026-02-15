🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 17.2 Les PIDs et hiérarchie

## Introduction

Chaque processus sous Linux possède un identifiant unique appelé **PID** (Process ID). Ce système d'identification permet au noyau Linux de gérer, suivre et contrôler tous les processus en cours d'exécution. Comprendre les PIDs et la hiérarchie des processus est essentiel pour maîtriser la programmation système.

## Qu'est-ce qu'un PID ?

### Définition

Un **PID** (Process IDentifier) est un nombre entier positif unique qui identifie de manière univoque un processus en cours d'exécution sur le système.

**Caractéristiques :**
- Type : `pid_t` (généralement un `int`)
- Valeurs : nombres entiers positifs (1 à 32768 par défaut, configurable jusqu'à 4194304)
- Unicité : Chaque processus actif a un PID unique
- Réutilisation : Lorsqu'un processus se termine, son PID peut être réattribué à un nouveau processus

### Le PID spécial : 1

Le processus avec le PID **1** est spécial. C'est le **premier processus** lancé par le noyau au démarrage du système :

- Sur les systèmes modernes : **systemd** (PID 1)
- Sur les anciens systèmes : **init** (PID 1)

Ce processus est l'**ancêtre de tous les autres processus** du système. Il ne se termine jamais (sauf à l'arrêt du système) et joue un rôle crucial dans la gestion des processus orphelins.

## Obtenir les PIDs en C

### Fonctions principales

```c
#include <unistd.h>
#include <sys/types.h>

// Obtenir le PID du processus courant
pid_t getpid(void);

// Obtenir le PID du processus parent (PPID)
pid_t getppid(void);
```

Ces fonctions ne peuvent **jamais échouer** et retournent toujours une valeur valide.

### Exemple basique

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t mon_pid = getpid();
    pid_t parent_pid = getppid();

    printf("Mon PID est: %d\n", mon_pid);
    printf("Le PID de mon parent est: %d\n", parent_pid);

    return 0;
}
```

**Sortie exemple :**
```
Mon PID est: 12345  
Le PID de mon parent est: 3456  
```

Le processus parent est généralement le shell (bash, zsh) qui a lancé votre programme.

## La hiérarchie des processus

### Structure arborescente

Sous Linux, les processus sont organisés en **arbre hiérarchique** :

```
systemd (PID 1)
│
├── sshd (PID 850)
│   └── bash (PID 2341)
│       └── mon_programme (PID 5678)
│
├── gdm (PID 1200)
│   └── gnome-shell (PID 1250)
│
└── cron (PID 920)
    └── backup.sh (PID 8901)
```

Chaque processus (sauf `systemd`) a exactement **un parent**. Un processus peut avoir **zéro, un ou plusieurs enfants**.

### Relations parent-enfant

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid;

    printf("=== AVANT fork() ===\n");
    printf("PID du processus: %d\n", getpid());
    printf("PID du parent: %d\n\n", getppid());

    pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // PROCESSUS ENFANT
        printf("=== PROCESSUS ENFANT ===\n");
        printf("Mon PID: %d\n", getpid());
        printf("PID de mon parent: %d\n", getppid());
        printf("Je suis l'enfant créé par fork()\n\n");
    }
    else {
        // PROCESSUS PARENT
        printf("=== PROCESSUS PARENT ===\n");
        printf("Mon PID: %d\n", getpid());
        printf("PID de mon parent: %d\n", getppid());
        printf("J'ai créé un enfant avec le PID: %d\n\n", pid);
    }

    return 0;
}
```

**Sortie possible :**
```
=== AVANT fork() ===
PID du processus: 1234  
PID du parent: 890  

=== PROCESSUS PARENT ===
Mon PID: 1234  
PID de mon parent: 890  
J'ai créé un enfant avec le PID: 1235  

=== PROCESSUS ENFANT ===
Mon PID: 1235  
PID de mon parent: 1234  
Je suis l'enfant créé par fork()  
```

### Observations importantes

1. Le **PID du parent** reste 890 pour le processus parent
2. Le **PID du parent de l'enfant** est 1234 (le processus parent)
3. L'enfant obtient un **nouveau PID unique** (1235)

## Visualisation de la hiérarchie

### Avec la commande `pstree`

La commande `pstree` affiche l'arbre des processus de manière visuelle :

```bash
# Afficher l'arbre complet
pstree

# Afficher avec les PIDs
pstree -p

# Afficher l'arbre d'un processus spécifique
pstree -p 1234

# Afficher uniquement la branche de votre utilisateur
pstree -p $USER
```

**Exemple de sortie :**
```
systemd(1)─┬─sshd(850)───bash(2341)───mon_programme(5678)
           ├─gdm(1200)───gnome-shell(1250)
           └─cron(920)───backup.sh(8901)
```

### Avec la commande `ps`

```bash
# Afficher tous les processus avec leurs relations
ps auxf

# Afficher les processus en format arbre
ps -ejH

# Afficher les PIDs avec leurs parents (PPID)
ps -eo pid,ppid,cmd

# Trouver tous les enfants d'un processus
ps --ppid 1234
```

**Exemple :**
```
  PID  PPID CMD
    1     0 /sbin/init
  850     1 /usr/sbin/sshd
 2341   850 bash
 5678  2341 ./mon_programme
```

### Avec `/proc`

Le système de fichiers `/proc` contient des informations détaillées sur chaque processus :

```bash
# Voir le parent d'un processus
cat /proc/5678/status | grep PPid

# Voir tous les détails d'un processus
cat /proc/5678/status
```

## Exemple : Tracer la chaîne de parenté

Voici un programme qui remonte toute la chaîne de processus jusqu'à `systemd` :

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CMD_LEN 256

// Fonction pour lire le nom du processus depuis /proc
void get_process_name(pid_t pid, char *name, size_t len) {
    char path[64];
    char cmdline[MAX_CMD_LEN];
    FILE *fp;

    snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);

    fp = fopen(path, "r");
    if (fp == NULL) {
        snprintf(name, len, "???");
        return;
    }

    if (fgets(cmdline, sizeof(cmdline), fp) != NULL) {
        // Extraire juste le nom du programme
        char *base = strrchr(cmdline, '/');
        if (base != NULL) {
            snprintf(name, len, "%s", base + 1);
        } else {
            snprintf(name, len, "%s", cmdline);
        }
    } else {
        snprintf(name, len, "???");
    }

    fclose(fp);
}

// Fonction pour obtenir le PPID depuis /proc
pid_t get_parent_pid(pid_t pid) {
    char path[64];
    char line[256];
    FILE *fp;
    pid_t ppid = 0;

    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    fp = fopen(path, "r");
    if (fp == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strncmp(line, "PPid:", 5) == 0) {
            sscanf(line, "PPid:\t%d", &ppid);
            break;
        }
    }

    fclose(fp);
    return ppid;
}

int main(void) {
    pid_t current_pid = getpid();
    char process_name[MAX_CMD_LEN];
    int level = 0;

    printf("Chaîne de parenté (du processus actuel vers systemd):\n");
    printf("========================================================\n\n");

    while (current_pid > 0) {
        get_process_name(current_pid, process_name, sizeof(process_name));

        // Indentation selon le niveau
        for (int i = 0; i < level; i++) {
            printf("  ");
        }

        printf("└─ PID %d: %s", current_pid, process_name);

        if (current_pid == 1) {
            printf(" [INIT/SYSTEMD - racine de l'arbre]\n");
            break;
        }

        printf("\n");

        // Remonter au parent
        pid_t parent = get_parent_pid(current_pid);

        if (parent == 0 || parent == current_pid) {
            break;
        }

        current_pid = parent;
        level++;
    }

    return 0;
}
```

**Sortie exemple :**
```
Chaîne de parenté (du processus actuel vers systemd):
========================================================

└─ PID 5678: mon_programme
  └─ PID 2341: bash
    └─ PID 850: sshd
      └─ PID 1: systemd [INIT/SYSTEMD - racine de l'arbre]
```

## Création d'une famille de processus

### Exemple : Parent avec plusieurs enfants

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid1, pid2, pid3;

    printf("Processus parent initial - PID: %d\n\n", getpid());

    // Création du premier enfant
    pid1 = fork();
    if (pid1 == 0) {
        printf("  [Enfant 1] PID: %d, Parent: %d\n", getpid(), getppid());
        sleep(10);  // Reste actif pour visualiser
        return 0;
    }

    // Création du deuxième enfant
    pid2 = fork();
    if (pid2 == 0) {
        printf("  [Enfant 2] PID: %d, Parent: %d\n", getpid(), getppid());
        sleep(10);
        return 0;
    }

    // Création du troisième enfant
    pid3 = fork();
    if (pid3 == 0) {
        printf("  [Enfant 3] PID: %d, Parent: %d\n", getpid(), getppid());
        sleep(10);
        return 0;
    }

    // Code du parent
    printf("\n[Parent] Mes enfants sont:\n");
    printf("  - Enfant 1: PID %d\n", pid1);
    printf("  - Enfant 2: PID %d\n", pid2);
    printf("  - Enfant 3: PID %d\n", pid3);

    printf("\nExécutez 'pstree -p %d' dans un autre terminal pour voir l'arbre!\n",
           getpid());

    sleep(10);  // Laisser le temps de visualiser

    return 0;
}
```

**Visualisation avec `pstree` pendant l'exécution :**
```bash
$ pstree -p 1234
mon_programme(1234)─┬─mon_programme(1235)
                     ├─mon_programme(1236)
                     └─mon_programme(1237)
```

### Exemple : Hiérarchie multi-générationnelle

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid;

    printf("[Génération 0] PID: %d (Arrière-grand-parent)\n", getpid());

    pid = fork();
    if (pid == 0) {
        printf("  [Génération 1] PID: %d, Parent: %d (Grand-parent)\n",
               getpid(), getppid());

        pid = fork();
        if (pid == 0) {
            printf("    [Génération 2] PID: %d, Parent: %d (Parent)\n",
                   getpid(), getppid());

            pid = fork();
            if (pid == 0) {
                printf("      [Génération 3] PID: %d, Parent: %d (Enfant)\n",
                       getpid(), getppid());
                sleep(5);
                return 0;
            }

            sleep(5);
            return 0;
        }

        sleep(5);
        return 0;
    }

    printf("\nArbre créé! Utilisez 'pstree -p %d' pour visualiser.\n", getpid());
    sleep(5);

    return 0;
}
```

**Structure créée :**
```
PID 1000
  └── PID 1001
      └── PID 1002
          └── PID 1003
```

## Groupes de processus et sessions

Au-delà de la relation parent-enfant simple, Linux organise aussi les processus en **groupes** et **sessions**.

### Groupe de processus (Process Group)

Un groupe de processus est un ensemble de processus partageant le même **PGID** (Process Group ID).

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid, pgid;

    pid = getpid();
    pgid = getpgrp();  // ou getpgid(0)

    printf("Mon PID: %d\n", pid);
    printf("Mon PGID (Process Group ID): %d\n", pgid);

    // Généralement, PID == PGID pour le leader du groupe
    if (pid == pgid) {
        printf("Je suis le LEADER de mon groupe de processus.\n");
    }

    return 0;
}
```

**Utilité des groupes :**
- Gestion des signaux : Envoyer un signal à tout un groupe
- Contrôle de terminal : Gérer les jobs en avant-plan/arrière-plan dans un shell

### Session

Une session est un ensemble de groupes de processus. Chaque session a un **SID** (Session ID).

```c
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid = getpid();
    pid_t sid = getsid(0);  // 0 = processus courant
    pid_t pgid = getpgrp();

    printf("Mon PID:  %d\n", pid);
    printf("Mon PGID: %d\n", pgid);
    printf("Mon SID:  %d\n", sid);

    return 0;
}
```

**Hiérarchie complète :**
```
Session (SID)
  ├── Groupe de processus 1 (PGID)
  │   ├── Processus A (PID)
  │   └── Processus B (PID)
  │
  └── Groupe de processus 2 (PGID)
      ├── Processus C (PID)
      └── Processus D (PID)
```

## Limites et configuration du système

### Plage de PIDs

```bash
# Voir la valeur maximale actuelle des PIDs
cat /proc/sys/kernel/pid_max

# Exemple de sortie: 32768 ou 4194304
```

### Modifier la limite (root uniquement)

```bash
# Augmenter temporairement la limite
sudo sysctl -w kernel.pid_max=4194304

# Rendre permanent (ajouter à /etc/sysctl.conf)
echo "kernel.pid_max = 4194304" | sudo tee -a /etc/sysctl.conf
```

### Pourquoi c'est important

Sur un système très actif (serveur avec de nombreux processus créés/détruits), augmenter `pid_max` réduit le risque de réutilisation rapide des PIDs, ce qui peut causer des bugs subtils.

## Outils de surveillance des processus

### Commandes essentielles

```bash
# Liste simple des processus
ps aux

# Arbre des processus avec PIDs
pstree -p

# Surveillance en temps réel
top  
htop  # version améliorée (à installer)  

# Informations détaillées sur un processus
cat /proc/PID/status  
cat /proc/PID/cmdline  
cat /proc/PID/environ  

# Trouver le PID d'un processus par nom
pgrep nom_processus  
pidof nom_programme  

# Tuer un processus par PID
kill PID  
kill -9 PID  # force (SIGKILL)  

# Tuer un processus par nom
pkill nom_processus  
killall nom_programme  
```

### Programme C pour lister ses enfants

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

void list_children(pid_t parent_pid) {
    DIR *proc_dir;
    struct dirent *entry;
    char path[256];
    char line[256];
    FILE *fp;

    proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        perror("opendir /proc");
        return;
    }

    printf("Enfants du processus %d:\n", parent_pid);

    while ((entry = readdir(proc_dir)) != NULL) {
        // Ignorer les entrées non numériques
        if (entry->d_name[0] < '0' || entry->d_name[0] > '9') {
            continue;
        }

        // Construire le chemin vers /proc/PID/status
        snprintf(path, sizeof(path), "/proc/%s/status", entry->d_name);

        fp = fopen(path, "r");
        if (fp == NULL) {
            continue;
        }

        // Chercher la ligne PPid
        while (fgets(line, sizeof(line), fp) != NULL) {
            if (strncmp(line, "PPid:", 5) == 0) {
                int ppid;
                sscanf(line, "PPid:\t%d", &ppid);

                if (ppid == parent_pid) {
                    printf("  └─ PID %s\n", entry->d_name);
                }
                break;
            }
        }

        fclose(fp);
    }

    closedir(proc_dir);
}

int main(void) {
    pid_t my_pid = getpid();

    printf("Mon PID: %d\n", my_pid);
    printf("Création de 3 processus enfants...\n\n");

    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            printf("  Enfant %d créé avec PID %d\n", i+1, getpid());
            sleep(5);  // Rester actif
            return 0;
        }
    }

    sleep(1);  // Laisser les enfants s'initialiser
    printf("\n");

    list_children(my_pid);

    sleep(5);  // Attendre que les enfants terminent

    return 0;
}
```

## Concepts avancés

### Le réparenting (adoption)

Lorsqu'un processus parent se termine avant ses enfants, ces enfants deviennent **orphelins**. Le noyau Linux les **réaffecte automatiquement** à `systemd` (PID 1).

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid = fork();

    if (pid == 0) {
        // ENFANT
        printf("[Enfant] PID: %d, Parent initial: %d\n",
               getpid(), getppid());

        sleep(3);  // Attendre que le parent se termine

        printf("[Enfant] Après sommeil, nouveau parent: %d\n",
               getppid());
        // Le PPID sera probablement 1 (systemd)

        sleep(2);
        return 0;
    }
    else {
        // PARENT
        printf("[Parent] PID: %d, enfant créé: %d\n",
               getpid(), pid);
        printf("[Parent] Je me termine immédiatement!\n");

        return 0;  // Parent se termine
    }
}
```

**Sortie typique :**
```
[Parent] PID: 5000, enfant créé: 5001
[Parent] Je me termine immédiatement!
[Enfant] PID: 5001, Parent initial: 5000
[Enfant] Après sommeil, nouveau parent: 1
```

### PIDs et threads

Sous Linux, chaque **thread** a aussi un identifiant unique appelé **TID** (Thread ID). Le thread principal d'un processus a un TID égal au PID du processus.

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

int main(void) {
    pid_t pid = getpid();
    pid_t tid = syscall(SYS_gettid);

    printf("PID du processus: %d\n", pid);
    printf("TID du thread principal: %d\n", tid);

    // Pour le thread principal: PID == TID
    if (pid == tid) {
        printf("Confirmation: je suis le thread principal.\n");
    }

    return 0;
}
```

## Bonnes pratiques

### 1. Toujours vérifier les valeurs de retour

```c
// ✅ BON
pid_t pid = fork();  
if (pid == -1) {  
    perror("fork");
    return 1;
}
```

### 2. Utiliser getpid() plutôt que stocker le PID

```c
// ❌ MAUVAIS dans l'enfant
pid_t pid = fork();  
if (pid == 0) {  
    // pid vaut 0, pas le PID de l'enfant!
    printf("Mon PID: %d\n", pid);  // Affiche 0!
}

// ✅ BON
pid_t pid = fork();  
if (pid == 0) {  
    printf("Mon PID: %d\n", getpid());  // Correct!
}
```

### 3. Nettoyer les processus enfants

Toujours attendre (`wait()`) la terminaison des enfants pour éviter les zombies (voir section 17.3).

### 4. Documentation

Commentez clairement les relations parent-enfant dans votre code pour faciliter la maintenance.

## Cas d'usage pratiques

### 1. Shell

Un shell crée un processus enfant pour chaque commande :

```bash
bash (PID 1000)
  └── ls (PID 1001)     # Commande exécutée
```

### 2. Serveur web

Un serveur peut forker pour gérer chaque requête :

```
nginx (PID 500)
  ├── worker process (PID 501)
  ├── worker process (PID 502)
  ├── worker process (PID 503)
  └── worker process (PID 504)
```

### 3. Daemon

Un daemon se détache de son parent et de son terminal en créant une nouvelle session :

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void daemonize(void) {
    pid_t pid = fork();

    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);  // Parent se termine
    }

    // Enfant continue et devient leader de session
    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    // Maintenant le processus est un daemon
    printf("Daemon PID: %d, SID: %d\n", getpid(), getsid(0));
}
```

## Débogage et diagnostic

### Tracer un processus avec strace

```bash
# Tracer tous les appels système d'un processus
strace -p PID

# Tracer un processus et tous ses enfants
strace -f ./mon_programme

# Tracer uniquement fork, clone, exec
strace -e trace=fork,clone,execve ./mon_programme
```

### Vérifier les relations avec /proc

```bash
# Parent d'un processus
cat /proc/PID/status | grep PPid

# Enfants d'un processus
pgrep -P PID

# Arbre complet autour d'un processus
ps --forest -p PID
```

## Résumé

- Chaque processus a un **PID unique** obtenu avec `getpid()`
- Le **PPID** (Parent PID) s'obtient avec `getppid()`
- Les processus forment un **arbre hiérarchique** avec `systemd` (PID 1) à la racine
- Après `fork()`, l'enfant hérite du PID du parent comme PPID
- Les processus orphelins sont adoptés par `systemd`
- Les outils `ps`, `pstree`, `top` permettent de visualiser la hiérarchie
- Les groupes de processus (PGID) et sessions (SID) organisent les processus au-delà de la relation parent-enfant

Comprendre la hiérarchie des processus est essentiel pour :
- Gérer correctement les processus créés par votre application
- Déboguer les problèmes de terminaison et de signaux
- Implémenter des architectures multi-processus robustes

La prochaine section (17.3) abordera les processus orphelins et zombies en détail.

⏭️ [Processus orphelins et zombies](/17-processus-et-signaux/03-orphelins-zombies.md)
