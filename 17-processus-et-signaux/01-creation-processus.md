🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 17.1 Création de processus

## Introduction

Un **processus** est une instance d'un programme en cours d'exécution. Sous Linux, chaque programme que vous lancez devient un processus qui possède :
- Son propre espace mémoire
- Son propre identifiant (PID - Process ID)
- Ses propres ressources système (fichiers ouverts, variables d'environnement, etc.)

La capacité de créer de nouveaux processus est fondamentale en programmation système Linux. C'est ce qui permet, par exemple, à votre shell de lancer des commandes, ou à un serveur web de traiter plusieurs requêtes simultanément.

## L'appel système `fork()`

### Concept fondamental

L'appel système `fork()` est **la** méthode pour créer un nouveau processus sous Linux. Son fonctionnement est unique et peut sembler déroutant au premier abord :

> **`fork()` crée une copie quasi-identique du processus appelant.**

Après l'appel à `fork()`, vous avez :
- Le **processus parent** (l'original)
- Le **processus enfant** (la copie)

Les deux processus continuent leur exécution **à partir de la ligne suivant l'appel à `fork()`**.

### Prototype de la fonction

```c
#include <unistd.h>
#include <sys/types.h>

pid_t fork(void);
```

**Valeurs de retour :**
- Dans le **processus parent** : `fork()` retourne le PID du processus enfant (un nombre positif)
- Dans le **processus enfant** : `fork()` retourne `0`
- En cas d'**erreur** : `fork()` retourne `-1` (aucun processus enfant n'est créé)

### Pourquoi cette distinction ?

Cette distinction permet à chaque processus de savoir qui il est et d'exécuter du code différent. C'est le mécanisme clé pour créer des comportements distincts entre parent et enfant.

## Premier exemple simple

Voici un exemple minimal pour comprendre le fonctionnement de `fork()` :

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid;

    printf("Avant fork() - PID: %d\n", getpid());

    pid = fork();

    // À partir d'ici, le code est exécuté par 2 processus !

    if (pid == -1) {
        // Erreur lors du fork
        perror("Erreur fork");
        return 1;
    }
    else if (pid == 0) {
        // Code exécuté par l'ENFANT
        printf("Je suis le processus ENFANT - PID: %d, Parent PID: %d\n",
               getpid(), getppid());
    }
    else {
        // Code exécuté par le PARENT
        printf("Je suis le processus PARENT - PID: %d, Enfant PID: %d\n",
               getpid(), pid);
    }

    printf("Fin du processus PID: %d\n", getpid());

    return 0;
}
```

**Sortie possible :**
```
Avant fork() - PID: 1234
Je suis le processus PARENT - PID: 1234, Enfant PID: 1235
Fin du processus PID: 1234
Je suis le processus ENFANT - PID: 1235, Parent PID: 1234
Fin du processus PID: 1235
```

### Analyse du code

1. **Avant `fork()`** : Un seul processus s'exécute
2. **Après `fork()`** : Deux processus identiques continuent l'exécution
3. **Test de `pid`** : Permet de différencier parent et enfant
4. **`getpid()`** : Retourne le PID du processus courant
5. **`getppid()`** : Retourne le PID du processus parent

## Ce qui est copié lors d'un `fork()`

Lorsque `fork()` crée un processus enfant, celui-ci hérite de nombreux éléments du parent :

### ✅ Copié (duplicated)

- **Code du programme** : L'enfant exécute le même code
- **Données** : Variables globales et locales (valeurs au moment du fork)
- **Stack et Heap** : Copie complète de la mémoire
- **Descripteurs de fichiers ouverts** : L'enfant partage les mêmes fichiers ouverts
- **Variables d'environnement**
- **Répertoire de travail courant**
- **Masque de création de fichiers (umask)**
- **Gestionnaires de signaux**

### ❌ Différent

- **PID** : Chaque processus a un identifiant unique
- **PPID** (Parent PID) : Le parent de l'enfant est le processus d'origine
- **Temps d'exécution CPU** : Remis à zéro pour l'enfant
- **Verrous de fichiers** : Non hérités
- **Signaux en attente** : Effacés pour l'enfant

## Exemple avec modification de variables

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    int compteur = 100;
    pid_t pid;

    printf("Avant fork() - compteur = %d\n", compteur);

    pid = fork();

    if (pid == -1) {
        perror("Erreur fork");
        return 1;
    }
    else if (pid == 0) {
        // ENFANT : modifie sa copie
        compteur += 10;
        printf("ENFANT - compteur = %d (adresse: %p)\n",
               compteur, (void*)&compteur);
    }
    else {
        // PARENT : modifie sa copie
        compteur += 50;
        printf("PARENT - compteur = %d (adresse: %p)\n",
               compteur, (void*)&compteur);
    }

    return 0;
}
```

**Sortie possible :**
```
Avant fork() - compteur = 100
PARENT - compteur = 150 (adresse: 0x7ffd12345678)
ENFANT - compteur = 110 (adresse: 0x7ffd12345678)
```

### Observation importante

Bien que les adresses mémoires **apparaissent** identiques, ce sont en réalité des **espaces mémoire séparés**. Chaque processus a sa propre copie de `compteur` et les modifications dans l'un n'affectent pas l'autre.

## Ordre d'exécution

L'ordre d'exécution entre parent et enfant après un `fork()` est **non déterministe**. Le système d'exploitation (scheduler) décide quel processus s'exécute en premier.

```c
#include <stdio.h>
#include <unistd.h>

int main(void) {
    pid_t pid = fork();

    if (pid == 0) {
        printf("A\n");
        printf("B\n");
    } else {
        printf("C\n");
        printf("D\n");
    }

    return 0;
}
```

**Sorties possibles :**
```
C    ou    A    ou    C    ou    A
D          B          A          C
A          C          D          D
B          D          B          B
```

### Implication pratique

Ne faites **jamais** d'hypothèses sur l'ordre d'exécution. Si vous avez besoin de synchronisation, utilisez des mécanismes appropriés (signaux, pipes, sémaphores, etc.).

## Gestion des erreurs de `fork()`

`fork()` peut échouer dans plusieurs situations :

```c
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(void) {
    pid_t pid = fork();

    if (pid == -1) {
        // fork() a échoué
        fprintf(stderr, "Erreur fork: %s\n", strerror(errno));

        // Causes possibles :
        // EAGAIN : Nombre max de processus atteint
        // ENOMEM : Mémoire insuffisante

        return 1;
    }

    // Code normal...

    return 0;
}
```

### Causes communes d'échec

1. **Limite de processus atteinte** : Chaque utilisateur a un nombre maximum de processus autorisés
2. **Mémoire insuffisante** : Le système n'a pas assez de ressources pour créer un nouveau processus
3. **Limites système** : Limite globale du nombre de processus sur le système

## Création de multiples processus

Il est possible de créer plusieurs processus en appelant `fork()` plusieurs fois :

```c
#include <stdio.h>
#include <unistd.h>

int main(void) {
    pid_t pid1, pid2;

    printf("Processus principal - PID: %d\n", getpid());

    // Premier enfant
    pid1 = fork();

    if (pid1 == 0) {
        printf("Premier enfant - PID: %d\n", getpid());
        return 0;
    }

    // Deuxième enfant (créé par le parent uniquement)
    pid2 = fork();

    if (pid2 == 0) {
        printf("Deuxième enfant - PID: %d\n", getpid());
        return 0;
    }

    // Code du parent
    printf("Parent - Mes enfants: %d et %d\n", pid1, pid2);

    return 0;
}
```

### Attention : Multiplication exponentielle

Si vous ne faites pas attention, `fork()` peut créer un nombre exponentiel de processus :

```c
fork();
fork();
fork();
```

Ce code crée **8 processus** au total (2³) ! Soyez prudent avec les appels successifs à `fork()`.

## Exemple pratique : Création d'un processus de travail

Voici un exemple plus réaliste où le parent et l'enfant ont des tâches différentes :

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid;

    pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // PROCESSUS ENFANT : Effectue un calcul
        printf("[Enfant %d] Début du travail...\n", getpid());

        long somme = 0;
        for (long i = 0; i < 1000000000L; i++) {
            somme += i;
        }

        printf("[Enfant %d] Calcul terminé: somme = %ld\n",
               getpid(), somme);

        return 0;  // L'enfant se termine
    }
    else {
        // PROCESSUS PARENT : Continue son travail
        printf("[Parent %d] J'ai créé l'enfant %d\n", getpid(), pid);
        printf("[Parent %d] Je continue mon travail pendant que l'enfant calcule\n",
               getpid());

        // Le parent pourrait faire autre chose ici
        sleep(2);  // Simule du travail

        printf("[Parent %d] Mon travail est terminé\n", getpid());

        return 0;
    }
}
```

## Concepts clés à retenir

### 1. `fork()` crée une copie du processus

Le processus enfant est une copie quasi-identique du parent au moment de l'appel à `fork()`. Ils partagent initialement le même code et les mêmes données (copiées).

### 2. Deux valeurs de retour différentes

- **Parent** : reçoit le PID de l'enfant
- **Enfant** : reçoit 0

C'est ce qui permet de différencier les deux processus et de leur faire exécuter du code différent.

### 3. Espaces mémoire séparés

Après le `fork()`, chaque processus a son propre espace mémoire. Les modifications dans l'un n'affectent pas l'autre.

### 4. Héritage et indépendance

L'enfant hérite de nombreux attributs du parent, mais devient un processus indépendant avec son propre PID, sa propre durée de vie, etc.

### 5. Ordre d'exécution non garanti

Ne faites pas d'hypothèses sur quel processus (parent ou enfant) s'exécutera en premier après le `fork()`.

## Optimisation : Copy-on-Write (COW)

### Note technique avancée

Bien que conceptuellement `fork()` "copie" toute la mémoire du parent, Linux utilise une technique appelée **Copy-on-Write** pour optimiser ce processus :

- Immédiatement après `fork()`, parent et enfant **partagent** physiquement les mêmes pages mémoire
- Les pages sont marquées en lecture seule
- Dès qu'un processus tente de **modifier** une page, le système en crée alors une copie privée
- Cela rend `fork()` très rapide, même pour de gros processus

Cette optimisation est transparente pour le programmeur, mais explique pourquoi `fork()` est si efficace sous Linux.

## Fonctions utiles liées aux processus

```c
#include <unistd.h>
#include <sys/types.h>

// Obtenir le PID du processus courant
pid_t getpid(void);

// Obtenir le PID du processus parent
pid_t getppid(void);

// Obtenir le groupe de processus
pid_t getpgrp(void);

// Obtenir l'ID utilisateur réel
uid_t getuid(void);
```

### Exemple d'utilisation

```c
#include <stdio.h>
#include <unistd.h>

int main(void) {
    printf("Mon PID: %d\n", getpid());
    printf("PID de mon parent: %d\n", getppid());
    printf("Mon groupe de processus: %d\n", getpgrp());
    printf("Mon UID: %d\n", getuid());

    return 0;
}
```

## Erreurs courantes à éviter

### 1. Oublier de gérer l'erreur de `fork()`

```c
// ❌ MAUVAIS
pid_t pid = fork();
if (pid == 0) {
    // enfant
} else {
    // parent - mais que se passe-t-il si fork() a échoué ?
}

// ✅ BON
pid_t pid = fork();
if (pid == -1) {
    perror("fork");
    return 1;
}
if (pid == 0) {
    // enfant
} else {
    // parent
}
```

### 2. Confondre PID et valeur de retour

```c
// ❌ MAUVAIS : pid dans l'enfant vaut 0, pas son PID !
pid_t pid = fork();
printf("Mon PID est: %d\n", pid);  // Faux dans l'enfant !

// ✅ BON
pid_t pid = fork();
printf("Mon PID est: %d\n", getpid());
```

### 3. Ne pas gérer la terminaison des processus enfants

Nous verrons dans les sections suivantes comment attendre la terminaison des enfants avec `wait()` pour éviter les processus zombies.

## Compilation et exécution

Pour compiler et tester les exemples :

```bash
# Compilation
gcc -Wall -Wextra -o exemple_fork exemple_fork.c

# Exécution
./exemple_fork

# Voir les processus créés (dans un autre terminal pendant l'exécution)
ps aux | grep exemple_fork
```

## Cas d'usage courants

### 1. Serveur réseau

Un serveur crée un processus enfant pour gérer chaque nouvelle connexion client.

### 2. Shell

Votre shell (bash, zsh) utilise `fork()` suivi de `exec()` pour lancer les commandes que vous tapez.

### 3. Parallélisation

Créer plusieurs processus pour effectuer des tâches en parallèle (calculs, traitement de données).

### 4. Isolation

Créer un processus séparé pour exécuter du code non fiable de manière isolée.

## Pour aller plus loin

Cette section a couvert les bases de la création de processus avec `fork()`. Les sections suivantes exploreront :

- **17.2** : Les PIDs et la hiérarchie des processus
- **17.3** : Les processus orphelins et zombies (et comment les éviter)
- **17.4** : Les variables d'environnement
- **17.5-17.7** : La gestion des signaux
- **17.8-17.9** : La communication entre processus avec les pipes

La combinaison de `fork()` avec d'autres appels système (comme `exec()`, `wait()`, et les signaux) permet de créer des applications système puissantes et robustes.

---

## Résumé

- `fork()` crée un nouveau processus en dupliquant le processus appelant
- Le parent reçoit le PID de l'enfant, l'enfant reçoit 0
- Les deux processus s'exécutent indépendamment avec leurs propres espaces mémoire
- Toujours vérifier si `fork()` a échoué (retour == -1)
- L'ordre d'exécution entre parent et enfant n'est pas garanti
- Linux optimise `fork()` avec le mécanisme Copy-on-Write

La maîtrise de `fork()` est essentielle pour la programmation système sous Linux !

⏭️ [Les PIDs et hiérarchie](/17-processus-et-signaux/02-pids-hierarchie.md)
