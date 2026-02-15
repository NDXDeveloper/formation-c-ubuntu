🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 19.2 Sémaphores System V

## Introduction

Les **sémaphores** sont des mécanismes de synchronisation qui permettent de contrôler l'accès concurrent à des ressources partagées. Imaginez un sémaphore comme un **feu de signalisation** ou un **gardien** qui contrôle qui peut accéder à une zone critique.

### Analogie du monde réel

Pensez à une **salle de réunion** avec un nombre limité de places :
- Le sémaphore représente le **nombre de places disponibles**
- Avant d'entrer, vous devez **"prendre" une place** (décrementer le sémaphore)
- Si toutes les places sont occupées (sémaphore = 0), vous **attendez**
- Quand quelqu'un sort, il **"libère" une place** (incrémenter le sémaphore)
- Vous pouvez alors entrer

C'est exactement ce que fait un sémaphore en programmation !

### Pourquoi les sémaphores ?

Sans synchronisation, l'accès concurrent à une ressource partagée produit des résultats imprévisibles :

```
Processus A                    Processus B
-----------                    -----------
Lire counter (= 10)           Lire counter (= 10)  
Incrémenter (11)              Incrémenter (11)  
Écrire counter (11)           Écrire counter (11)

Résultat : counter = 11 au lieu de 12 !
```

Avec un sémaphore :

```
Processus A                    Processus B
-----------                    -----------
Attendre sémaphore (entrer)   [BLOQUÉ - attend]  
Lire counter (= 10)  
Incrémenter (11)  
Écrire counter (11)
Libérer sémaphore (sortir)
                              Attendre sémaphore (entrer)
                              Lire counter (= 11)
                              Incrémenter (12)
                              Écrire counter (12)
                              Libérer sémaphore

Résultat : counter = 12 ✓
```

---

## POSIX vs System V : Deux mondes

Il existe **deux APIs différentes** pour les sémaphores sous Linux :

### POSIX Sémaphores (moderne, recommandée)

```c
#include <semaphore.h>

sem_t sem;  
sem_init(&sem, 0, 1);    // Simple et intuitif  
sem_wait(&sem);          // Attendre  
sem_post(&sem);          // Signaler  
sem_destroy(&sem);  
```

**Avantages :**
- ✅ API simple et moderne
- ✅ Portable (standard POSIX)
- ✅ Nommage clair (`sem_wait`, `sem_post`)

### System V Sémaphores (legacy, mais répandu)

```c
#include <sys/sem.h>

int semid = semget(key, 1, IPC_CREAT | 0666);  // Plus complexe  
struct sembuf op = {0, -1, 0};  
semop(semid, &op, 1);    // Opération opaque  
```

**Caractéristiques :**
- ⚠️ API plus complexe
- ⚠️ Utilise des clés numériques
- ✅ Permet des ensembles de sémaphores
- ✅ Très répandu dans les systèmes legacy
- ✅ Opérations atomiques sur plusieurs sémaphores

**Dans cette section**, nous nous concentrons sur **System V** car :
1. Beaucoup de code legacy l'utilise encore
2. Comprendre System V aide à apprécier la simplicité de POSIX
3. Certaines fonctionnalités avancées n'existent qu'en System V

---

## Concepts fondamentaux System V

### Ensembles de sémaphores

Contrairement à POSIX qui manipule des sémaphores individuels, System V gère des **ensembles** (arrays) de sémaphores :

```
┌─────────────────────────────────┐
│   Ensemble de sémaphores        │
│   (semid = 12345)               │
├─────────────────────────────────┤
│  Sémaphore 0 : valeur = 1       │  ← Mutex pour ressource A
│  Sémaphore 1 : valeur = 5       │  ← Pool de 5 connexions
│  Sémaphore 2 : valeur = 0       │  ← Attente d'un événement
└─────────────────────────────────┘
```

Chaque ensemble a :
- Un **identifiant unique** (semid)
- Un nombre défini de sémaphores
- Chaque sémaphore a une **valeur entière** ≥ 0

### Clés IPC (IPC Keys)

System V utilise des **clés numériques** pour identifier les ressources :

```c
key_t key = ftok("/tmp/myfile", 'A');  // Génère une clé unique  
int semid = semget(key, 1, IPC_CREAT | 0666);  
```

- `ftok()` génère une clé à partir d'un chemin de fichier et d'un ID
- Deux processus utilisant la même clé accèdent au **même ensemble**

Vous pouvez aussi utiliser :
- `IPC_PRIVATE` : Crée un ensemble privé (souvent avec fork)
- Une valeur hardcodée : `0x12345678` (déconseillé)

---

## L'API System V : Les 3 fonctions essentielles

### 1. `semget()` - Créer ou obtenir un ensemble

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int semget(key_t key, int nsems, int semflg);
```

**Paramètres :**
- `key` : Clé IPC (ou `IPC_PRIVATE`)
- `nsems` : Nombre de sémaphores dans l'ensemble
- `semflg` : Flags de création
  - `IPC_CREAT` : Créer si n'existe pas
  - `IPC_EXCL` : Erreur si existe déjà (avec `IPC_CREAT`)
  - `0666` : Permissions (lecture/écriture pour tous)

**Retour :** Identifiant de l'ensemble (semid), ou -1 en cas d'erreur

**Exemple :**
```c
// Créer un ensemble de 1 sémaphore
key_t key = ftok("/tmp/mysem", 'S');  
int semid = semget(key, 1, IPC_CREAT | 0666);  
if (semid == -1) {  
    perror("semget");
    exit(1);
}
```

### 2. `semop()` - Opérations sur les sémaphores

```c
int semop(int semid, struct sembuf *sops, size_t nsops);
```

C'est la fonction **la plus importante** : elle effectue des opérations atomiques sur les sémaphores.

**Structure `sembuf` :**
```c
struct sembuf {
    unsigned short sem_num;  // Numéro du sémaphore dans l'ensemble
    short sem_op;            // Opération à effectuer
    short sem_flg;           // Flags (IPC_NOWAIT, SEM_UNDO)
};
```

**Types d'opérations (`sem_op`) :**

| `sem_op` | Action | Équivalent | Usage |
|----------|--------|------------|-------|
| **-1** | Décrémenter (attendre) | `sem_wait()` (POSIX) | Entrer en section critique |
| **+1** | Incrémenter (signaler) | `sem_post()` (POSIX) | Sortir de section critique |
| **0** | Attendre que valeur = 0 | - | Attendre la fin d'un traitement |

**Flags importants :**
- `IPC_NOWAIT` : Ne pas bloquer si opération impossible (retourne -1 avec errno = EAGAIN)
- `SEM_UNDO` : Annuler l'opération si le processus se termine (évite les deadlocks)

**Exemples d'opérations :**

```c
struct sembuf op;

// Attendre (décrementer, P operation, "prendre")
op.sem_num = 0;        // Sémaphore numéro 0  
op.sem_op = -1;        // Décrémenter  
op.sem_flg = SEM_UNDO; // Annuler si crash  
semop(semid, &op, 1);  

// Signaler (incrémenter, V operation, "libérer")
op.sem_num = 0;  
op.sem_op = +1;        // Incrémenter  
op.sem_flg = SEM_UNDO;  
semop(semid, &op, 1);  
```

### 3. `semctl()` - Contrôle et configuration

```c
int semctl(int semid, int semnum, int cmd, ...);
```

Fonction de contrôle multifonction (comme `ioctl()`).

**Commandes principales :**

| Commande | Description | Argument |
|----------|-------------|----------|
| `SETVAL` | Initialiser la valeur d'un sémaphore | `int val` |
| `GETVAL` | Lire la valeur d'un sémaphore | - |
| `IPC_RMID` | Supprimer l'ensemble | - |
| `IPC_STAT` | Obtenir les infos | `struct semid_ds` |
| `SETALL` | Initialiser tous les sémaphores | `unsigned short *array` |
| `GETALL` | Lire tous les sémaphores | `unsigned short *array` |

**Union requise (selon la page man) :**
```c
union semun {
    int val;                // Pour SETVAL
    struct semid_ds *buf;   // Pour IPC_STAT
    unsigned short *array;  // Pour SETALL/GETALL
};
```

⚠️ **Note importante** : Cette union n'est **pas définie** par le système, vous devez la déclarer vous-même.

**Exemples :**

```c
// Initialiser un sémaphore à 1 (mutex)
union semun arg;  
arg.val = 1;  
semctl(semid, 0, SETVAL, arg);  

// Lire la valeur
int val = semctl(semid, 0, GETVAL);  
printf("Valeur actuelle : %d\n", val);  

// Supprimer l'ensemble
semctl(semid, 0, IPC_RMID);
```

---

## Exemple complet : Mutex simple avec System V

### Programme complet avec commentaires

```c
// semaphore_demo.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <errno.h>

// Union requise pour semctl (non définie par le système)
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

// Fonction utilitaire : Attendre (P operation)
void sem_wait_sysv(int semid, int semnum) {
    struct sembuf op;
    op.sem_num = semnum;
    op.sem_op = -1;          // Décrémenter
    op.sem_flg = SEM_UNDO;   // Protection contre les crashes

    if (semop(semid, &op, 1) == -1) {
        perror("sem_wait");
        exit(1);
    }
}

// Fonction utilitaire : Signaler (V operation)
void sem_post_sysv(int semid, int semnum) {
    struct sembuf op;
    op.sem_num = semnum;
    op.sem_op = +1;          // Incrémenter
    op.sem_flg = SEM_UNDO;

    if (semop(semid, &op, 1) == -1) {
        perror("sem_post");
        exit(1);
    }
}

int main() {
    key_t key;
    int semid;
    union semun arg;

    // 1. Générer une clé unique
    key = ftok("/tmp", 'S');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    // 2. Créer un ensemble de 1 sémaphore
    semid = semget(key, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    // 3. Initialiser le sémaphore à 1 (mutex libre)
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("semctl SETVAL");
        exit(1);
    }

    printf("Sémaphore créé (ID: %d)\n", semid);

    // 4. Créer un processus fils
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // ===== PROCESSUS FILS =====
        printf("[FILS] Tentative d'accès à la section critique...\n");

        sem_wait_sysv(semid, 0);  // Attendre le sémaphore
        printf("[FILS] Entrée en section critique\n");

        // Simuler un traitement
        for (int i = 0; i < 5; i++) {
            printf("[FILS] Travail... %d/5\n", i + 1);
            sleep(1);
        }

        printf("[FILS] Sortie de la section critique\n");
        sem_post_sysv(semid, 0);  // Libérer le sémaphore

        exit(0);

    } else {
        // ===== PROCESSUS PARENT =====
        sleep(1);  // Laisser le fils démarrer

        printf("[PARENT] Tentative d'accès à la section critique...\n");

        sem_wait_sysv(semid, 0);  // Attendre le sémaphore (bloqué tant que fils est dedans)
        printf("[PARENT] Entrée en section critique\n");

        // Simuler un traitement
        for (int i = 0; i < 3; i++) {
            printf("[PARENT] Travail... %d/3\n", i + 1);
            sleep(1);
        }

        printf("[PARENT] Sortie de la section critique\n");
        sem_post_sysv(semid, 0);  // Libérer le sémaphore

        // Attendre la fin du fils
        wait(NULL);

        // 5. Supprimer le sémaphore
        printf("Suppression du sémaphore\n");
        if (semctl(semid, 0, IPC_RMID) == -1) {
            perror("semctl IPC_RMID");
        }
    }

    return 0;
}
```

### Compilation et exécution

```bash
# Compiler
gcc semaphore_demo.c -o semaphore_demo

# Exécuter
./semaphore_demo
```

**Sortie attendue :**
```
Sémaphore créé (ID: 123456)
[FILS] Tentative d'accès à la section critique...
[FILS] Entrée en section critique
[PARENT] Tentative d'accès à la section critique...
[FILS] Travail... 1/5
[FILS] Travail... 2/5
[FILS] Travail... 3/5
[FILS] Travail... 4/5
[FILS] Travail... 5/5
[FILS] Sortie de la section critique
[PARENT] Entrée en section critique     ← Le parent attendait ici !
[PARENT] Travail... 1/3
[PARENT] Travail... 2/3
[PARENT] Travail... 3/3
[PARENT] Sortie de la section critique
Suppression du sémaphore
```

**Observation clé** : Le parent est **bloqué** tant que le fils est dans la section critique, grâce au sémaphore.

---

## Opérations atomiques multiples

Un avantage de System V : effectuer **plusieurs opérations atomiquement** :

```c
struct sembuf ops[2];

// Opération atomique : décrémenter sem 0 ET incrémenter sem 1
ops[0].sem_num = 0;  
ops[0].sem_op = -1;  
ops[0].sem_flg = SEM_UNDO;  

ops[1].sem_num = 1;  
ops[1].sem_op = +1;  
ops[1].sem_flg = SEM_UNDO;  

// Les deux opérations sont effectuées atomiquement (ou aucune)
semop(semid, ops, 2);
```

**Cas d'usage** : Transférer une ressource d'un pool à un autre sans race condition.

---

## Le flag `SEM_UNDO` : Pourquoi c'est important

Sans `SEM_UNDO` :
```c
// Processus acquiert le sémaphore
sem_wait_sysv(semid, 0);

// CRASH ! Le processus meurt avant de libérer
// → Le sémaphore reste bloqué à jamais (deadlock)
```

Avec `SEM_UNDO` :
```c
op.sem_flg = SEM_UNDO;  // ✅  
sem_wait_sysv(semid, 0);  

// Si le processus crash, le système annule automatiquement
// l'opération → le sémaphore est libéré
```

**Recommandation** : Utilisez **toujours** `SEM_UNDO` sauf si vous avez une raison spécifique.

---

## Inspection des sémaphores System V

Linux fournit des commandes pour inspecter les ressources IPC :

### Lister les sémaphores actifs

```bash
ipcs -s
```

**Sortie exemple :**
```
------ Semaphore Arrays --------
key        semid      owner      perms      nsems
0x53001234 123456     user       666        1
```

### Afficher les détails

```bash
ipcs -s -i 123456
```

### Supprimer un sémaphore

```bash
ipcrm -s 123456
```

Ou depuis le code :
```c
semctl(semid, 0, IPC_RMID);
```

---

## Patterns courants

### 1. Mutex (Exclusion Mutuelle)

```c
// Initialisation : valeur = 1
arg.val = 1;  
semctl(semid, 0, SETVAL, arg);  

// Utilisation
sem_wait_sysv(semid, 0);  // Acquérir
// ... section critique ...
sem_post_sysv(semid, 0);  // Libérer
```

### 2. Compteur de ressources (Pool)

```c
// Initialisation : valeur = N (nombre de ressources disponibles)
arg.val = 5;  // Pool de 5 connexions  
semctl(semid, 0, SETVAL, arg);  

// Utilisation
sem_wait_sysv(semid, 0);  // Prendre une ressource (décrémente)
// ... utiliser la ressource ...
sem_post_sysv(semid, 0);  // Rendre la ressource (incrémente)
```

### 3. Barrière de synchronisation

```c
// Sémaphore initialisé à 0
arg.val = 0;  
semctl(semid, 0, SETVAL, arg);  

// Processus A termine son travail
printf("Processus A : travail terminé\n");  
sem_post_sysv(semid, 0);  // Signaler  

// Processus B attend
printf("Processus B : attente...\n");  
sem_wait_sysv(semid, 0);  // Bloqué jusqu'au signal  
printf("Processus B : peut continuer\n");  
```

### 4. Producteur-Consommateur

```c
// Ensemble de 2 sémaphores
int semid = semget(key, 2, IPC_CREAT | 0666);

// sem[0] = places vides dans le buffer (initialement N)
// sem[1] = éléments disponibles (initialement 0)
unsigned short init_vals[2] = {BUFFER_SIZE, 0};  
arg.array = init_vals;  
semctl(semid, 0, SETALL, arg);  

// Producteur
sem_wait_sysv(semid, 0);  // Attendre place vide
// ... produire ...
sem_post_sysv(semid, 1);  // Signaler élément disponible

// Consommateur
sem_wait_sysv(semid, 1);  // Attendre élément disponible
// ... consommer ...
sem_post_sysv(semid, 0);  // Signaler place vide
```

---

## Pièges et problèmes courants

### 1. Oublier d'initialiser

```c
int semid = semget(key, 1, IPC_CREAT | 0666);
// ❌ Valeur non initialisée ! Comportement imprévisible
sem_wait_sysv(semid, 0);

// ✅ Toujours initialiser
union semun arg;  
arg.val = 1;  
semctl(semid, 0, SETVAL, arg);  
```

### 2. Ne pas supprimer les sémaphores

Les sémaphores System V **persistent** après la fin du programme :

```c
// ✅ Toujours nettoyer avant de terminer
semctl(semid, 0, IPC_RMID);
```

Sans cela, vous verrez des sémaphores orphelins dans `ipcs -s`.

### 3. Deadlock par ordre inversé

```c
// Processus A
sem_wait_sysv(semid, 0);  // Acquiert sem 0  
sem_wait_sysv(semid, 1);  // Veut sem 1  

// Processus B
sem_wait_sysv(semid, 1);  // Acquiert sem 1  
sem_wait_sysv(semid, 0);  // Veut sem 0  

// → DEADLOCK !
```

**Solution** : Toujours acquérir les sémaphores dans le **même ordre**.

### 4. Oublier `SEM_UNDO`

Sans `SEM_UNDO`, un crash laisse le sémaphore dans un état incohérent :

```c
// ❌ Dangereux
op.sem_flg = 0;

// ✅ Sûr
op.sem_flg = SEM_UNDO;
```

---

## Comparaison détaillée : POSIX vs System V

| Caractéristique | POSIX | System V |
|----------------|-------|----------|
| **Simplicité** | ⭐⭐⭐⭐⭐ Simple | ⭐⭐ Complexe |
| **Portabilité** | ⭐⭐⭐⭐⭐ Standard | ⭐⭐⭐ Répandu mais moins portable |
| **Ensembles** | ❌ Un seul sémaphore | ✅ Arrays de sémaphores |
| **Opérations atomiques multiples** | ❌ Non | ✅ Oui |
| **Nommage** | Noms symboliques (`/sem_name`) | Clés numériques |
| **Persistance** | Disparaît avec dernier processus | Persiste jusqu'à `IPC_RMID` |
| **Inspection** | `ls /dev/shm/` | `ipcs -s` |
| **Undo automatique** | ❌ Non | ✅ `SEM_UNDO` |
| **Use case** | Applications modernes | Systèmes legacy, besoins avancés |

**Recommandation générale** : Préférez **POSIX** pour les nouveaux projets, sauf si vous avez besoin des fonctionnalités avancées de System V.

---

## Exemple avancé : Producteur-Consommateur complet

```c
// producer_consumer.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define BUFFER_SIZE 5
#define SEM_EMPTY 0    // Places vides
#define SEM_FULL 1     // Éléments disponibles
#define SEM_MUTEX 2    // Mutex pour accès buffer

union semun {
    int val;
    unsigned short *array;
};

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;   // Index d'insertion
    int out;  // Index d'extraction
} shared_buffer_t;

void sem_wait_sysv(int semid, int semnum) {
    struct sembuf op = {semnum, -1, 0};
    semop(semid, &op, 1);
}

void sem_post_sysv(int semid, int semnum) {
    struct sembuf op = {semnum, +1, 0};
    semop(semid, &op, 1);
}

int main() {
    key_t key = ftok("/tmp", 'P');

    // Créer ensemble de 3 sémaphores
    int semid = semget(key, 3, IPC_CREAT | 0666);

    // Initialiser
    union semun arg;
    unsigned short init_vals[3] = {BUFFER_SIZE, 0, 1};  // empty=5, full=0, mutex=1
    arg.array = init_vals;
    semctl(semid, 0, SETALL, arg);

    // Créer mémoire partagée pour le buffer
    int shmid = shmget(key, sizeof(shared_buffer_t), IPC_CREAT | 0666);
    shared_buffer_t *sb = shmat(shmid, NULL, 0);
    sb->in = 0;
    sb->out = 0;

    pid_t pid = fork();

    if (pid == 0) {
        // ===== PRODUCTEUR =====
        for (int i = 0; i < 10; i++) {
            int item = i * 10;

            sem_wait_sysv(semid, SEM_EMPTY);  // Attendre place vide
            sem_wait_sysv(semid, SEM_MUTEX);  // Exclusion mutuelle

            // Section critique
            sb->buffer[sb->in] = item;
            printf("[PROD] Produit : %d à l'index %d\n", item, sb->in);
            sb->in = (sb->in + 1) % BUFFER_SIZE;

            sem_post_sysv(semid, SEM_MUTEX);  // Libérer mutex
            sem_post_sysv(semid, SEM_FULL);   // Signaler élément disponible

            sleep(1);
        }
        exit(0);

    } else {
        // ===== CONSOMMATEUR =====
        for (int i = 0; i < 10; i++) {
            sem_wait_sysv(semid, SEM_FULL);   // Attendre élément
            sem_wait_sysv(semid, SEM_MUTEX);  // Exclusion mutuelle

            // Section critique
            int item = sb->buffer[sb->out];
            printf("[CONS] Consommé : %d de l'index %d\n", item, sb->out);
            sb->out = (sb->out + 1) % BUFFER_SIZE;

            sem_post_sysv(semid, SEM_MUTEX);  // Libérer mutex
            sem_post_sysv(semid, SEM_EMPTY);  // Signaler place vide

            sleep(2);  // Consomme plus lentement
        }

        wait(NULL);

        // Nettoyage
        shmdt(sb);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID);

        printf("Terminé.\n");
    }

    return 0;
}
```

**Explication** :
- `SEM_EMPTY` : Compte les places vides (initialement 5)
- `SEM_FULL` : Compte les éléments disponibles (initialement 0)
- `SEM_MUTEX` : Protège l'accès concurrent au buffer (initialement 1)

Le producteur attend une place vide, puis protège l'accès avec le mutex, insère, et signale un élément disponible. Le consommateur fait l'inverse.

---

## Debugging des problèmes de sémaphores

### Voir l'état actuel

```bash
# Lister tous les sémaphores
ipcs -s

# Afficher les valeurs
ipcs -s -i <semid>
```

### Problème : Sémaphore bloqué

Si un processus est bloqué indéfiniment :

1. Vérifier la valeur actuelle :
```bash
ipcs -s -i <semid>
```

2. Si valeur = 0 et aucun processus ne libère :
```bash
# Réinitialiser manuellement (depuis un programme)
union semun arg;  
arg.val = 1;  
semctl(semid, 0, SETVAL, arg);  
```

3. Ou supprimer complètement :
```bash
ipcrm -s <semid>
```

### Problème : Sémaphores orphelins

Après un crash, des sémaphores peuvent rester :

```bash
# Nettoyer tous les sémaphores d'un utilisateur
ipcs -s | grep $USER | awk '{print $2}' | xargs -I {} ipcrm -s {}
```

---

## Bonnes pratiques

### ✅ À faire

1. **Toujours initialiser** les sémaphores après création
2. **Utiliser `SEM_UNDO`** systématiquement
3. **Supprimer** les sémaphores (`IPC_RMID`) quand terminé
4. **Vérifier les erreurs** de chaque appel système
5. **Documenter** le protocole de synchronisation
6. **Utiliser `ftok()`** pour générer des clés uniques
7. **Acquérir dans le même ordre** pour éviter les deadlocks

### ❌ À éviter

1. **Ne pas** supposer que les sémaphores sont initialisés à 0
2. **Ne pas** oublier de libérer (`sem_post`) après acquisition
3. **Ne pas** utiliser des clés hardcodées (collision possible)
4. **Ne pas** négliger le nettoyage (orphelins)
5. **Ne pas** mélanger POSIX et System V (confusion)

---

## Gestion robuste des erreurs

```c
#include <errno.h>

int semid = semget(key, 1, IPC_CREAT | 0666);  
if (semid == -1) {  
    if (errno == EEXIST) {
        fprintf(stderr, "Sémaphore déjà existant\n");
        // Essayer de l'obtenir sans IPC_CREAT
        semid = semget(key, 1, 0666);
    } else if (errno == ENOSPC) {
        fprintf(stderr, "Limite système de sémaphores atteinte\n");
        exit(1);
    } else {
        perror("semget");
        exit(1);
    }
}

// Opération avec timeout (non standard, Linux-specific)
struct sembuf op = {0, -1, IPC_NOWAIT | SEM_UNDO};  
if (semop(semid, &op, 1) == -1) {  
    if (errno == EAGAIN) {
        fprintf(stderr, "Sémaphore non disponible (non bloquant)\n");
    } else {
        perror("semop");
    }
}
```

---

## Migration de System V vers POSIX

Si vous maintenez du code legacy et souhaitez moderniser :

### Équivalences

| System V | POSIX | Notes |
|----------|-------|-------|
| `semget()` + `SETVAL` | `sem_init()` | POSIX plus simple |
| `semop(..., -1, ...)` | `sem_wait()` | Même sémantique |
| `semop(..., +1, ...)` | `sem_post()` | Même sémantique |
| `IPC_RMID` | `sem_destroy()` | POSIX nettoie automatiquement |

### Exemple de migration

**Avant (System V) :**
```c
int semid = semget(key, 1, IPC_CREAT | 0666);  
union semun arg;  
arg.val = 1;  
semctl(semid, 0, SETVAL, arg);  

struct sembuf op = {0, -1, SEM_UNDO};  
semop(semid, &op, 1);  
```

**Après (POSIX) :**
```c
sem_t sem;  
sem_init(&sem, 1, 1);  // 1 = partagé entre processus  

sem_wait(&sem);
```

**Avantages** : Code plus lisible, moins de lignes, moins d'erreurs potentielles.

---

## Limites système

Linux impose des limites sur les sémaphores System V :

```bash
# Voir les limites
cat /proc/sys/kernel/sem
# Format : SEMMSL SEMMNS SEMOPM SEMMNI
# Exemple : 250 32000 32 128

# SEMMSL : Sémaphores max par ensemble
# SEMMNS : Sémaphores max sur le système
# SEMOPM : Opérations max par appel semop()
# SEMMNI : Ensembles max sur le système
```

Si vous atteignez ces limites :
- Nettoyez les sémaphores orphelins (`ipcrm`)
- Augmentez les limites (root requis) :
```bash
sudo sysctl -w kernel.sem="250 32000 32 256"
```

---

## Cas d'usage réels

### 1. Bases de données

PostgreSQL utilise System V pour la synchronisation de la shared memory :

```bash
# Voir les sémaphores utilisés par PostgreSQL
ipcs -s | grep postgres
```

### 2. Apache HTTP Server

Mode prefork utilise des sémaphores pour coordonner les workers.

### 3. Systèmes embarqués

Souvent préféré sur les systèmes sans support POSIX complet.

---

## Résumé

Les **sémaphores System V** sont un mécanisme de synchronisation puissant mais complexe :

### Points clés

1. ✅ **API en 3 fonctions** : `semget()`, `semop()`, `semctl()`
2. ✅ **Ensembles de sémaphores** : Gère des arrays, pas des unités
3. ✅ **Opérations atomiques multiples** : Unique à System V
4. ✅ **`SEM_UNDO`** : Protection contre les crashes
5. ⚠️ **Persistance** : Ne disparaît pas automatiquement
6. ⚠️ **Complexité** : Plus difficile que POSIX

### Quand utiliser System V ?

- ✅ Maintien de code legacy
- ✅ Besoin d'opérations atomiques sur plusieurs sémaphores
- ✅ Environnements sans support POSIX complet
- ❌ Nouveaux projets → Préférez **POSIX**

### Commandes essentielles

```bash
ipcs -s          # Lister les sémaphores  
ipcrm -s <id>    # Supprimer un sémaphore  
```

---

## Références

- Man pages : `man semget`, `man semop`, `man semctl`
- Stevens & Rago, *Advanced Programming in the UNIX Environment*, Chapitre 15
- POSIX.1-2001, POSIX.1-2008
- Linux Programmer's Manual : Section 2 (System Calls)

---

**Prochaine section** : 19.3 Message Queues System V

⏭️ [Message queues System V](/19-memoire-partagee-ipc/03-message-queues.md)
