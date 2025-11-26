🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 17.3 Processus orphelins et zombies

## Introduction

Lorsque vous travaillez avec des processus sous Linux, deux situations particulières peuvent survenir : les **processus orphelins** et les **processus zombies**. Bien que ces termes puissent sembler inquiétants, ils décrivent simplement des états normaux (pour les orphelins) ou problématiques (pour les zombies) dans le cycle de vie des processus.

Comprendre ces concepts est essentiel pour écrire des applications multi-processus robustes et éviter les fuites de ressources.

## Les processus orphelins

### Définition

Un **processus orphelin** est un processus dont le **parent s'est terminé avant lui**.

```
Situation initiale:          Après terminaison du parent:
Parent (PID 1000)            systemd (PID 1)
  └── Enfant (PID 1001)        └── Enfant (PID 1001) [ORPHELIN]
```

### Comment un processus devient orphelin

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // PROCESSUS ENFANT
        printf("[Enfant] Mon PID: %d\n", getpid());
        printf("[Enfant] Mon parent initial: %d\n", getppid());

        sleep(3);  // Parent va se terminer pendant ce temps

        printf("[Enfant] Mon parent maintenant: %d\n", getppid());
        printf("[Enfant] Je suis devenu orphelin, adopté par systemd!\n");

        sleep(2);
        return 0;
    }
    else {
        // PROCESSUS PARENT
        printf("[Parent] Mon PID: %d\n", getpid());
        printf("[Parent] J'ai créé l'enfant: %d\n", pid);
        printf("[Parent] Je me termine immédiatement!\n");

        // Parent se termine sans attendre l'enfant
        return 0;
    }
}
```

**Sortie typique :**
```
[Parent] Mon PID: 5000
[Parent] J'ai créé l'enfant: 5001
[Parent] Je me termine immédiatement!
[Enfant] Mon PID: 5001
[Enfant] Mon parent initial: 5000
[Enfant] Mon parent maintenant: 1
[Enfant] Je suis devenu orphelin, adopté par systemd!
```

### Le mécanisme d'adoption

Sous Linux, **aucun processus ne peut rester orphelin** au sens strict. Lorsqu'un parent se termine :

1. Le noyau détecte que ses enfants n'ont plus de parent
2. Il **réaffecte automatiquement** ces enfants à `systemd` (PID 1)
3. `systemd` devient le nouveau parent et se charge de leur nettoyage

Ce mécanisme s'appelle le **réparenting** (réaffectation parentale).

### Les orphelins sont-ils un problème ?

**Non, les processus orphelins ne sont PAS un problème** en soi :

- ✅ Ils continuent à s'exécuter normalement
- ✅ Ils ont un nouveau parent (`systemd`) qui les gère
- ✅ Quand ils se terminent, `systemd` les nettoie automatiquement
- ✅ Ils ne consomment pas de ressources supplémentaires

**Cas d'usage légitime : Les daemons**

Les daemons (services en arrière-plan) sont volontairement créés comme orphelins :

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void become_daemon(void) {
    pid_t pid = fork();

    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        // Parent se termine volontairement
        exit(EXIT_SUCCESS);
    }

    // L'enfant continue et devient orphelin (adopté par systemd)
    // C'est maintenant un daemon

    printf("Je suis un daemon, PID: %d, Parent: %d\n",
           getpid(), getppid());
}

int main(void) {
    printf("Processus initial: %d\n", getpid());
    become_daemon();

    // Code du daemon ici
    while (1) {
        sleep(10);
        // Travail en arrière-plan...
    }

    return 0;
}
```

## Les processus zombies

### Définition

Un **processus zombie** est un processus qui **s'est terminé** mais dont l'entrée dans la table des processus **n'a pas encore été nettoyée** par son parent.

```
État normal:                  État zombie:
Parent (actif)                Parent (actif, n'a pas attendu)
  └── Enfant (actif)            └── Enfant (ZOMBIE) ☠️
```

### Pourquoi les zombies existent-ils ?

Quand un processus se termine :

1. Il libère ses ressources (mémoire, fichiers ouverts, etc.)
2. **MAIS** il conserve une entrée minimale dans la table des processus contenant :
   - Son PID
   - Son code de retour
   - Ses statistiques d'utilisation (CPU, mémoire)

Cette entrée reste en place jusqu'à ce que le **parent lise ces informations** avec `wait()` ou `waitpid()`.

> **Analogie** : C'est comme si l'enfant laissait un "bulletin de notes" que le parent doit venir récupérer avant que l'enfant puisse vraiment "partir".

### Détecter les processus zombies

Les zombies apparaissent avec l'état `Z` dans les commandes système :

```bash
# Avec ps
ps aux | grep Z

# Sortie exemple:
# user  1234  0.0  0.0     0    0 ?   Z    10:00   0:00 [mon_prog] <defunct>
```

Le mot `<defunct>` et l'état `Z` indiquent un zombie.

### Créer un zombie (à des fins pédagogiques)

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // ENFANT : se termine rapidement
        printf("[Enfant] PID: %d, je me termine!\n", getpid());
        return 0;  // L'enfant se termine
    }
    else {
        // PARENT : ne fait PAS de wait()
        printf("[Parent] Mon enfant %d va devenir zombie...\n", pid);

        sleep(2);  // Laisse le temps à l'enfant de se terminer

        printf("[Parent] Vérifiez avec 'ps aux | grep Z' dans un autre terminal!\n");
        printf("[Parent] Appuyez sur Entrée pour continuer...\n");
        getchar();

        // Toujours pas de wait(), l'enfant reste zombie
        printf("[Parent] Je me termine sans avoir attendu mon enfant.\n");

        return 0;
    }
}
```

Pendant l'exécution, si vous exécutez `ps aux | grep defunct`, vous verrez le processus enfant en état zombie.

### Pourquoi les zombies sont-ils un problème ?

Les zombies posent plusieurs problèmes :

#### 1. Fuite de PIDs

Chaque zombie **occupe un PID**. Le système a un nombre limité de PIDs disponibles :

```bash
cat /proc/sys/kernel/pid_max
# Exemple : 32768 ou 4194304
```

Si vous créez trop de zombies, vous pouvez **épuiser les PIDs disponibles**, empêchant la création de nouveaux processus !

#### 2. Pollution de la table des processus

Les zombies encombrent la table des processus, rendant la surveillance et le débogage plus difficiles.

#### 3. Indicateur de mauvaise conception

Un grand nombre de zombies indique généralement un bug dans le programme parent qui ne gère pas correctement ses enfants.

### Exemple de problème avec de nombreux zombies

```c
#include <stdio.h>
#include <unistd.h>

int main(void) {
    printf("Création de 1000 processus zombies...\n");

    for (int i = 0; i < 1000; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            // Enfant se termine immédiatement
            return 0;
        }

        // Parent ne fait JAMAIS de wait()
    }

    printf("1000 zombies créés! Vérifiez avec 'ps aux | grep Z'\n");
    sleep(30);  // Garde les zombies actifs

    return 0;
}
```

**Attention :** N'exécutez pas ce code sur un système de production !

## La solution : La fonction `wait()`

### Présentation de `wait()`

La fonction `wait()` permet au parent d'**attendre** la terminaison d'un de ses enfants et de **récupérer son code de retour**. Cela nettoie l'entrée zombie de la table des processus.

```c
#include <sys/types.h>
#include <sys/wait.h>

pid_t wait(int *status);
```

**Comportement :**
- Bloque le parent jusqu'à ce qu'un enfant se termine
- Récupère le code de retour de l'enfant dans `status`
- Retourne le PID de l'enfant terminé
- Retourne -1 en cas d'erreur (par exemple, aucun enfant)

### Exemple basique avec `wait()`

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // ENFANT
        printf("[Enfant] PID: %d, je travaille...\n", getpid());
        sleep(2);
        printf("[Enfant] Travail terminé, je me termine.\n");
        return 42;  // Code de retour
    }
    else {
        // PARENT
        int status;

        printf("[Parent] J'attends mon enfant %d...\n", pid);

        pid_t terminated_pid = wait(&status);

        printf("[Parent] L'enfant %d s'est terminé.\n", terminated_pid);

        // Pas de zombie! L'enfant a été correctement nettoyé

        return 0;
    }
}
```

**Sortie :**
```
[Parent] J'attends mon enfant 1001...
[Enfant] PID: 1001, je travaille...
[Enfant] Travail terminé, je me termine.
[Parent] L'enfant 1001 s'est terminé.
```

### Analyser le code de retour

Le paramètre `status` de `wait()` contient des informations sur comment l'enfant s'est terminé. On utilise des macros pour l'interpréter :

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void) {
    pid_t pid = fork();

    if (pid == 0) {
        // Enfant retourne un code
        printf("[Enfant] Je retourne le code 42\n");
        exit(42);
    }

    // Parent
    int status;
    wait(&status);

    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        printf("[Parent] Enfant terminé normalement avec le code: %d\n",
               exit_code);
    }
    else if (WIFSIGNALED(status)) {
        int signal = WTERMSIG(status);
        printf("[Parent] Enfant terminé par le signal: %d\n", signal);
    }

    return 0;
}
```

**Macros principales :**

| Macro | Description |
|-------|-------------|
| `WIFEXITED(status)` | Vrai si l'enfant s'est terminé normalement (`exit()` ou `return`) |
| `WEXITSTATUS(status)` | Code de retour de l'enfant (si `WIFEXITED` est vrai) |
| `WIFSIGNALED(status)` | Vrai si l'enfant a été terminé par un signal |
| `WTERMSIG(status)` | Numéro du signal qui a terminé l'enfant |
| `WIFSTOPPED(status)` | Vrai si l'enfant a été stoppé (SIGSTOP) |
| `WIFCONTINUED(status)` | Vrai si l'enfant a été relancé (SIGCONT) |

### Attendre plusieurs enfants

Si vous créez plusieurs enfants, vous devez appeler `wait()` pour chacun d'eux :

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int num_children = 3;

    // Créer 3 enfants
    for (int i = 0; i < num_children; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            // Enfant
            printf("[Enfant %d] PID: %d, je dors %d secondes\n",
                   i, getpid(), i+1);
            sleep(i + 1);
            printf("[Enfant %d] Terminé!\n", i);
            return i;
        }
    }

    // Parent attend TOUS ses enfants
    printf("[Parent] J'attends mes %d enfants...\n", num_children);

    for (int i = 0; i < num_children; i++) {
        int status;
        pid_t pid = wait(&status);

        if (WIFEXITED(status)) {
            printf("[Parent] Enfant %d terminé avec code %d\n",
                   pid, WEXITSTATUS(status));
        }
    }

    printf("[Parent] Tous mes enfants sont terminés, pas de zombies!\n");

    return 0;
}
```

**Point important :** `wait()` retourne dès qu'**un** enfant (n'importe lequel) se termine. L'ordre de retour n'est pas forcément l'ordre de création.

## La fonction `waitpid()` : Plus de contrôle

`waitpid()` offre plus de flexibilité que `wait()` :

```c
#include <sys/types.h>
#include <sys/wait.h>

pid_t waitpid(pid_t pid, int *status, int options);
```

**Paramètres :**

- **`pid`** :
  - `> 0` : Attendre l'enfant spécifique avec ce PID
  - `-1` : Attendre n'importe quel enfant (comme `wait()`)
  - `0` : Attendre un enfant du même groupe de processus
  - `< -1` : Attendre un enfant du groupe `-pid`

- **`status`** : Même que pour `wait()`

- **`options`** :
  - `0` : Comportement bloquant (par défaut)
  - `WNOHANG` : Non-bloquant (retourne immédiatement)
  - `WUNTRACED` : Signaler aussi les enfants stoppés
  - `WCONTINUED` : Signaler les enfants relancés

### Exemple : Attendre un enfant spécifique

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t child1, child2;

    // Premier enfant
    child1 = fork();
    if (child1 == 0) {
        printf("[Enfant 1] PID: %d, je dors 3 secondes\n", getpid());
        sleep(3);
        return 1;
    }

    // Deuxième enfant
    child2 = fork();
    if (child2 == 0) {
        printf("[Enfant 2] PID: %d, je dors 1 seconde\n", getpid());
        sleep(1);
        return 2;
    }

    // Parent attend spécifiquement child1
    printf("[Parent] J'attends spécifiquement l'enfant %d...\n", child1);

    int status;
    waitpid(child1, &status, 0);

    printf("[Parent] Enfant %d terminé (code: %d)\n",
           child1, WEXITSTATUS(status));

    // Puis attendre child2
    waitpid(child2, &status, 0);

    printf("[Parent] Enfant %d terminé (code: %d)\n",
           child2, WEXITSTATUS(status));

    return 0;
}
```

### Exemple : Mode non-bloquant avec `WNOHANG`

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();

    if (pid == 0) {
        // Enfant dort 5 secondes
        printf("[Enfant] Je dors 5 secondes...\n");
        sleep(5);
        return 0;
    }

    // Parent vérifie périodiquement sans bloquer
    printf("[Parent] Je vérifie l'état de mon enfant sans bloquer...\n");

    int status;
    pid_t result;
    int checks = 0;

    do {
        result = waitpid(pid, &status, WNOHANG);

        if (result == 0) {
            // Enfant pas encore terminé
            printf("[Parent] Check %d: enfant encore actif\n", ++checks);
            sleep(1);
        }
        else if (result == pid) {
            // Enfant terminé
            printf("[Parent] Enfant terminé!\n");
            break;
        }
        else {
            perror("waitpid");
            break;
        }

    } while (result == 0);

    return 0;
}
```

**Sortie typique :**
```
[Enfant] Je dors 5 secondes...
[Parent] Je vérifie l'état de mon enfant sans bloquer...
[Parent] Check 1: enfant encore actif
[Parent] Check 2: enfant encore actif
[Parent] Check 3: enfant encore actif
[Parent] Check 4: enfant encore actif
[Parent] Check 5: enfant encore actif
[Parent] Enfant terminé!
```

## Gestion des signaux : Éviter les zombies de manière asynchrone

Une approche élégante pour éviter les zombies est d'utiliser un **gestionnaire de signal** pour `SIGCHLD`.

### Le signal `SIGCHLD`

Quand un processus enfant se termine, le noyau envoie le signal **`SIGCHLD`** au parent. Le parent peut installer un gestionnaire pour ce signal et appeler `wait()` à ce moment-là.

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void sigchld_handler(int sig) {
    (void)sig;  // Paramètre non utilisé

    int status;
    pid_t pid;

    // Attendre tous les enfants terminés
    // WNOHANG pour ne pas bloquer s'il n'y a plus d'enfants
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("[Handler] Enfant %d nettoyé (code: %d)\n",
               pid, WEXITSTATUS(status));
    }
}

int main(void) {
    // Installer le gestionnaire de signal
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("[Parent] Gestionnaire SIGCHLD installé\n");

    // Créer plusieurs enfants
    for (int i = 0; i < 5; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            printf("[Enfant %d] PID: %d\n", i, getpid());
            sleep(i + 1);
            return i;
        }

        printf("[Parent] Créé enfant %d avec PID %d\n", i, pid);
    }

    printf("[Parent] Tous les enfants créés, j'attends...\n");

    // Parent peut faire autre chose
    sleep(10);

    printf("[Parent] Terminé, tous les zombies ont été nettoyés!\n");

    return 0;
}
```

### Pourquoi utiliser `WNOHANG` dans le handler ?

Dans le gestionnaire de signal, on utilise une boucle avec `WNOHANG` car :

1. **Plusieurs signaux peuvent être fusionnés** : Si plusieurs enfants se terminent rapidement, le système peut ne délivrer qu'un seul `SIGCHLD`
2. **On veut traiter tous les enfants terminés** en une seule invocation du handler
3. **On ne veut pas bloquer** dans un signal handler

## Cas particulier : `SIG_IGN` pour `SIGCHLD`

Une méthode simple (mais moins flexible) pour éviter les zombies :

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main(void) {
    // Dire au noyau de nettoyer automatiquement les enfants
    signal(SIGCHLD, SIG_IGN);

    printf("[Parent] SIGCHLD ignoré, pas de zombies possibles\n");

    for (int i = 0; i < 5; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            printf("[Enfant %d] PID: %d\n", i, getpid());
            sleep(1);
            return 0;
        }
    }

    sleep(5);
    printf("[Parent] Aucun zombie créé!\n");

    return 0;
}
```

**Avantages :**
- ✅ Simple
- ✅ Aucun zombie créé

**Inconvénients :**
- ❌ On ne peut pas récupérer le code de retour des enfants
- ❌ On ne sait pas quand un enfant se termine
- ❌ Moins de contrôle

## Stratégies pour éviter les zombies

### 1. Utiliser `wait()` ou `waitpid()` explicitement

**Quand :** Vous savez combien d'enfants vous créez et vous pouvez attendre leur terminaison.

```c
for (int i = 0; i < N; i++) {
    pid = fork();
    if (pid == 0) {
        // Enfant...
        return 0;
    }
}

// Attendre tous les enfants
for (int i = 0; i < N; i++) {
    wait(NULL);
}
```

### 2. Gestionnaire de signal `SIGCHLD`

**Quand :** Vous créez des enfants de manière asynchrone et ne savez pas exactement quand ils vont se terminer.

```c
void sigchld_handler(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

signal(SIGCHLD, sigchld_handler);
```

### 3. Double fork

**Quand :** Vous voulez créer un processus complètement détaché sans avoir à le gérer.

```c
pid_t pid = fork();
if (pid == 0) {
    // Premier enfant
    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Petit-enfant : fait le vrai travail
        // Son parent (premier enfant) va se terminer
        // Il sera adopté par systemd qui le nettoiera
        return 0;
    }
    // Premier enfant se termine immédiatement
    exit(0);
}

// Parent attend le premier enfant
wait(NULL);
// Le petit-enfant continue, mais n'est plus notre responsabilité
```

### 4. Ignorer `SIGCHLD`

**Quand :** Vous ne vous souciez pas du code de retour des enfants.

```c
signal(SIGCHLD, SIG_IGN);
```

## Tableau comparatif des stratégies

| Stratégie | Avantages | Inconvénients | Cas d'usage |
|-----------|-----------|---------------|-------------|
| `wait()` explicite | Simple, prévisible | Bloquant | Nombre fixe d'enfants |
| `waitpid(WNOHANG)` | Non-bloquant, contrôle précis | Polling nécessaire | Vérification périodique |
| Handler `SIGCHLD` | Asynchrone, efficace | Plus complexe | Production, serveurs |
| `SIG_IGN` | Très simple | Pas de code retour | Scripts simples |
| Double fork | Détachement complet | Complexe | Daemons |

## Débogage des zombies

### Trouver les processus zombies

```bash
# Lister tous les zombies
ps aux | grep Z

# Compter les zombies
ps aux | grep Z | wc -l

# Afficher les zombies avec leur parent
ps -eo pid,ppid,stat,cmd | grep Z
```

### Exemple de sortie

```
PID   PPID  STAT  CMD
1234  1000  Z     [mon_prog] <defunct>
1235  1000  Z     [mon_prog] <defunct>
```

Le `PPID` indique le parent responsable. C'est ce processus qui doit appeler `wait()`.

### Tuer le parent pour nettoyer les zombies

Si un parent a créé des zombies et ne les nettoie pas, la seule solution est de **terminer le parent** :

```bash
# Terminer le processus parent
kill 1000

# Si nécessaire, forcer
kill -9 1000
```

Quand le parent se termine, ses enfants zombies sont adoptés par `systemd` qui les nettoie immédiatement.

**⚠️ Attention :** Vous ne pouvez pas "tuer" directement un zombie (il est déjà mort). Vous devez agir sur le parent.

### Programme de diagnostic

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void check_zombies(void) {
    DIR *proc_dir = opendir("/proc");
    struct dirent *entry;
    int zombie_count = 0;

    if (!proc_dir) {
        perror("opendir");
        return;
    }

    printf("Recherche de processus zombies...\n\n");

    while ((entry = readdir(proc_dir)) != NULL) {
        if (entry->d_name[0] < '0' || entry->d_name[0] > '9')
            continue;

        char path[256];
        snprintf(path, sizeof(path), "/proc/%s/stat", entry->d_name);

        FILE *fp = fopen(path, "r");
        if (!fp) continue;

        char line[512];
        if (fgets(line, sizeof(line), fp)) {
            char *state = strchr(line, ')');
            if (state && state[2] == 'Z') {
                printf("Zombie trouvé: PID %s\n", entry->d_name);
                zombie_count++;
            }
        }

        fclose(fp);
    }

    closedir(proc_dir);

    printf("\nTotal de zombies: %d\n", zombie_count);
}

int main(void) {
    check_zombies();
    return 0;
}
```

## Erreurs courantes

### 1. Oublier d'attendre les enfants

```c
// ❌ MAUVAIS : Crée des zombies
for (int i = 0; i < 100; i++) {
    if (fork() == 0) {
        // Travail...
        exit(0);
    }
}
// Pas de wait() → 100 zombies!

// ✅ BON
for (int i = 0; i < 100; i++) {
    if (fork() == 0) {
        exit(0);
    }
}
for (int i = 0; i < 100; i++) {
    wait(NULL);
}
```

### 2. Attendre le mauvais nombre d'enfants

```c
// ❌ MAUVAIS
for (int i = 0; i < 10; i++) {
    fork();  // Crée 2^10 = 1024 processus!
}
wait(NULL);  // N'attend qu'un seul enfant!

// ✅ BON
int N = 10;
for (int i = 0; i < N; i++) {
    if (fork() == 0) {
        exit(0);
    }
}
for (int i = 0; i < N; i++) {
    wait(NULL);
}
```

### 3. Ignorer les erreurs de `wait()`

```c
// ❌ MAUVAIS
while (wait(NULL) > 0);  // Boucle infinie si pas d'enfants!

// ✅ BON
while (wait(NULL) > 0 || errno == EINTR);  // Continue si interrompu
```

## Bonnes pratiques

### 1. Toujours nettoyer les processus enfants

Chaque `fork()` doit avoir son `wait()` correspondant (sauf si `SIGCHLD` est ignoré).

### 2. Utiliser un gestionnaire `SIGCHLD` pour les serveurs

Les serveurs qui créent de nombreux processus enfants doivent utiliser un gestionnaire de signal.

### 3. Vérifier les codes de retour

Utilisez les macros `WIFEXITED`, `WEXITSTATUS`, etc. pour analyser correctement la terminaison.

### 4. Documenter la stratégie choisie

Indiquez clairement dans les commentaires comment vous gérez les processus enfants.

### 5. Tester avec des outils

Utilisez `ps`, `pstree`, et des moniteurs système pour vérifier qu'aucun zombie ne persiste.

## Résumé

### Processus orphelins

- ✅ **Normaux** : Pas un problème
- ✅ **Automatiquement adoptés** par `systemd`
- ✅ **Nettoyés automatiquement** par `systemd`
- 📌 **Cas d'usage légitime** : Création de daemons

### Processus zombies

- ❌ **Problématiques** : Consomment des PIDs
- ❌ **Doivent être évités**
- ✅ **Solution** : Le parent doit appeler `wait()` ou `waitpid()`
- ✅ **Alternative** : Gestionnaire de signal `SIGCHLD`
- ⚠️ **Ne peuvent pas être "tués"** : Il faut agir sur le parent

### Fonctions clés

| Fonction | Usage | Blocage |
|----------|-------|---------|
| `wait(status)` | Attendre n'importe quel enfant | Oui |
| `waitpid(pid, status, 0)` | Attendre un enfant spécifique | Oui |
| `waitpid(-1, status, WNOHANG)` | Vérifier sans bloquer | Non |

### Stratégies anti-zombies

1. **`wait()`/`waitpid()` explicites** - Simple et direct
2. **Gestionnaire `SIGCHLD`** - Recommandé pour production
3. **`signal(SIGCHLD, SIG_IGN)`** - Simple mais pas de code retour
4. **Double fork** - Pour détachement complet

La maîtrise de ces concepts est essentielle pour créer des applications multi-processus robustes et éviter les fuites de ressources sous Linux !

⏭️ [Variables d'environnement](/17-processus-et-signaux/04-variables-environnement.md)
