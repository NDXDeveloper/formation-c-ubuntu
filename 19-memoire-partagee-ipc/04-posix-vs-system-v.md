🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 19.4 POSIX IPC vs System V IPC

## Introduction

Sous Linux, il existe **deux familles complètes** de mécanismes de communication inter-processus (IPC) :

1. **System V IPC** - L'ancien standard (années 1980)
2. **POSIX IPC** - Le standard moderne (années 1990-2000)

Chaque famille offre les trois mêmes types de mécanismes :
- **Sémaphores** (synchronisation)
- **Mémoire partagée** (partage de données)
- **Files de messages** (communication asynchrone)

Mais les deux standards diffèrent **profondément** dans leur philosophie, leur API et leur utilisation.

### Pourquoi deux standards ?

**Contexte historique :**
- **System V IPC** : Développé par AT&T pour UNIX System V dans les années 1980
- **POSIX IPC** : Standardisé plus tard pour unifier les variantes UNIX et améliorer la portabilité

**Aujourd'hui :**
- Les deux coexistent sur Linux
- System V reste très répandu dans le code legacy
- POSIX est recommandé pour les nouveaux projets
- Comprendre les deux est essentiel pour les développeurs système

---

## Philosophies divergentes

### System V : Approche centralisée avec clés numériques

```
┌──────────────────────────────────────┐
│    Espace global du système IPC      │
│                                      │
│  Clé 0x12345 → Sémaphore ID 1        │
│  Clé 0x67890 → Shared Memory ID 2    │
│  Clé 0xABCDE → Message Queue ID 3    │
└──────────────────────────────────────┘
         ▲
         │ ftok() génère des clés
         │
   /tmp/myfile + 'S'
```

**Caractéristiques System V :**
- Utilise des **clés numériques** (générées par `ftok()`)
- Les ressources ont des **identifiants système** (semid, shmid, msqid)
- **Persiste après la fin des processus** jusqu'à suppression explicite
- Nécessite des commandes spéciales : `ipcs`, `ipcrm`
- API complexe avec structures multiples

### POSIX : Approche fichiers avec nommage symbolique

```
┌──────────────────────────────────────┐
│         Filesystem namespace         │
│                                      │
│  /dev/shm/my_sem      (sémaphore)    │
│  /dev/shm/my_shm      (shm)          │
│  /dev/mqueue/my_queue (msg queue)    │
└──────────────────────────────────────┘
         ▲
         │ Noms symboliques
         │
    "/my_resource"
```

**Caractéristiques POSIX :**
- Utilise des **noms symboliques** (chemins style `/name`)
- S'intègre au **système de fichiers virtuel** (`/dev/shm`, `/dev/mqueue`)
- Suppression via **`*_unlink()`** (ressource libérée après fermeture par le dernier processus)
- Utilisable avec des **outils standards** : `ls`, `rm`
- API simple et cohérente

---

## Comparaison détaillée par mécanisme

### 1. Sémaphores

#### System V Sémaphores

```c
#include <sys/sem.h>

// Créer un ensemble de sémaphores
key_t key = ftok("/tmp", 'S');  
int semid = semget(key, 1, IPC_CREAT | 0666);  

// Initialiser
union semun {
    int val;
} arg;
arg.val = 1;  
semctl(semid, 0, SETVAL, arg);  

// Attendre (P)
struct sembuf op = {0, -1, SEM_UNDO};  
semop(semid, &op, 1);  

// Signaler (V)
op.sem_op = +1;  
semop(semid, &op, 1);  

// Supprimer
semctl(semid, 0, IPC_RMID);
```

**Complexité** : 🔴 Élevée (union, struct sembuf, flags multiples)

#### POSIX Sémaphores

```c
#include <semaphore.h>

// Créer un sémaphore nommé
sem_t *sem = sem_open("/my_sem", O_CREAT, 0666, 1);

// Attendre
sem_wait(sem);

// Signaler
sem_post(sem);

// Fermer
sem_close(sem);

// Supprimer
sem_unlink("/my_sem");
```

**Complexité** : 🟢 Faible (API intuitive, nommage clair)

#### Comparaison sémaphores

| Caractéristique | System V | POSIX |
|----------------|----------|-------|
| **API** | `semget()`, `semop()`, `semctl()` | `sem_open()`, `sem_wait()`, `sem_post()` |
| **Nommage** | Clés numériques (`ftok()`) | Noms symboliques (`"/name"`) |
| **Ensembles** | ✅ Arrays de sémaphores | ❌ Sémaphores individuels |
| **Opérations atomiques multiples** | ✅ Oui (`semop()` sur plusieurs) | ❌ Non |
| **Undo automatique** | ✅ `SEM_UNDO` | ❌ Non |
| **Inspection** | `ipcs -s` | `ls /dev/shm/sem.*` |
| **Suppression** | `ipcrm -s` ou `semctl()` | `rm` ou `sem_unlink()` |
| **Simplicité** | 🔴 Complexe | 🟢 Simple |
| **Use case** | Besoin d'opérations atomiques complexes | Simplicité et portabilité |

---

### 2. Mémoire partagée

#### System V Shared Memory

```c
#include <sys/shm.h>

// Créer
key_t key = ftok("/tmp", 'M');  
int shmid = shmget(key, 4096, IPC_CREAT | 0666);  

// Attacher
void *ptr = shmat(shmid, NULL, 0);

// Utiliser
strcpy(ptr, "Hello");

// Détacher
shmdt(ptr);

// Supprimer
shmctl(shmid, IPC_RMID, NULL);
```

#### POSIX Shared Memory

```c
#include <sys/mman.h>
#include <fcntl.h>

// Créer
int fd = shm_open("/my_shm", O_CREAT | O_RDWR, 0666);  
ftruncate(fd, 4096);  

// Mapper
void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

// Utiliser
strcpy(ptr, "Hello");

// Détacher
munmap(ptr, 4096);  
close(fd);  

// Supprimer
shm_unlink("/my_shm");
```

#### Comparaison mémoire partagée

| Caractéristique | System V | POSIX |
|----------------|----------|-------|
| **API** | `shmget()`, `shmat()`, `shmdt()` | `shm_open()`, `mmap()`, `munmap()` |
| **Création/Dimensionnement** | En une étape (`shmget()`) | Deux étapes (`shm_open()` + `ftruncate()`) |
| **Mapping** | `shmat()` (spécifique) | `mmap()` (générique, aussi pour fichiers) |
| **Nommage** | Clés numériques | Noms symboliques |
| **Fichiers** | Non visible dans filesystem | Visible dans `/dev/shm/` |
| **Portabilité** | Très répandu | Standard moderne |
| **Simplicité** | 🟡 Moyenne | 🟡 Moyenne (mais cohérent avec mmap) |
| **Inspection** | `ipcs -m` | `ls -lh /dev/shm/` |
| **Use case** | Code legacy | Intégration avec mmap |

---

### 3. Files de messages

#### System V Message Queues

```c
#include <sys/msg.h>

// Créer
key_t key = ftok("/tmp", 'Q');  
int msqid = msgget(key, IPC_CREAT | 0666);  

// Structure
struct message {
    long mtype;
    char text[100];
};

// Envoyer
struct message msg = {1, "Hello"};  
msgsnd(msqid, &msg, sizeof(msg.text), 0);  

// Recevoir (avec filtrage par type)
msgrcv(msqid, &msg, sizeof(msg.text), 1, 0);

// Supprimer
msgctl(msqid, IPC_RMID, NULL);
```

#### POSIX Message Queues

```c
#include <mqueue.h>

// Créer
struct mq_attr attr = {0, 10, 8192, 0};  // max 10 msgs, 8KB chacun  
mqd_t mq = mq_open("/my_queue", O_CREAT | O_RDWR, 0666, &attr);  

// Envoyer (avec priorité)
char msg[100] = "Hello";  
mq_send(mq, msg, strlen(msg) + 1, 5);  // priorité 5  

// Recevoir
unsigned int prio;  
mq_receive(mq, msg, sizeof(msg), &prio);  

// Fermer et supprimer
mq_close(mq);  
mq_unlink("/my_queue");  
```

#### Comparaison files de messages

| Caractéristique | System V | POSIX |
|----------------|----------|-------|
| **API** | `msgget()`, `msgsnd()`, `msgrcv()` | `mq_open()`, `mq_send()`, `mq_receive()` |
| **Types de messages** | ✅ Filtrage par `mtype` | ❌ Pas de types |
| **Priorités** | ❌ Non | ✅ 0-31 (MQ_PRIO_MAX) |
| **Notifications** | ❌ Polling uniquement | ✅ Signaux, threads (`mq_notify()`) |
| **Taille max** | Limitée système (~8KB) | Configurable (`mq_attr`) |
| **Structure** | Doit inclure `long mtype` | Buffer simple |
| **Inspection** | `ipcs -q` | `ls /dev/mqueue/` |
| **Simplicité** | 🟡 Moyenne (struct spéciale) | 🟢 Simple |
| **Use case** | Filtrage par type nécessaire | Priorités et notifications |

---

## Tableau récapitulatif général

| Aspect | System V IPC | POSIX IPC |
|--------|--------------|-----------|
| **Année d'introduction** | ~1983 | ~1993 |
| **Standardisation** | UNIX System V | POSIX.1b (IEEE 1003.1b-1993) |
| **Nommage** | Clés numériques (`key_t`) | Noms symboliques (`"/name"`) |
| **Création de clés** | `ftok()` | Pas nécessaire |
| **Identifiants** | IDs opaques (int) | Descripteurs/pointeurs |
| **Persistance** | ✅ Persiste jusqu'à `IPC_RMID` | ⚠️ Dépend du type |
| **Inspection** | Outils spéciaux (`ipcs`) | Outils standards (`ls`, `cat`) |
| **Suppression** | Outils spéciaux (`ipcrm`) | Outils standards (`rm`, `unlink()`) |
| **Complexité API** | 🔴 Complexe | 🟢 Simple et cohérente |
| **Portabilité** | Très répandu (legacy) | Standard moderne |
| **Performance** | ⚡ Très bonne | ⚡ Très bonne (équivalente) |
| **Préféré pour** | Maintien code legacy, ops atomiques | Nouveaux projets, simplicité |

---

## Exemples côte à côte

### Mutex simple : Les deux approches

#### System V : Sémaphore comme mutex

```c
// sysv_mutex.c
#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>

union semun {
    int val;
};

int main() {
    // Créer
    key_t key = ftok("/tmp", 'M');
    int semid = semget(key, 1, IPC_CREAT | 0666);

    // Initialiser à 1 (mutex libre)
    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);

    // Acquérir (P)
    struct sembuf lock = {0, -1, SEM_UNDO};
    semop(semid, &lock, 1);

    printf("Section critique\n");

    // Libérer (V)
    struct sembuf unlock = {0, +1, SEM_UNDO};
    semop(semid, &unlock, 1);

    // Supprimer
    semctl(semid, 0, IPC_RMID);

    return 0;
}
```

**Lignes de code** : ~25 (sans gestion erreurs)

#### POSIX : Sémaphore comme mutex

```c
// posix_mutex.c
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>

int main() {
    // Créer et initialiser à 1
    sem_t *sem = sem_open("/my_mutex", O_CREAT, 0666, 1);

    // Acquérir
    sem_wait(sem);

    printf("Section critique\n");

    // Libérer
    sem_post(sem);

    // Nettoyer
    sem_close(sem);
    sem_unlink("/my_mutex");

    return 0;
}
```

**Lignes de code** : ~15 (sans gestion erreurs)

**Verdict** : POSIX est **40% plus concis** et plus lisible.

---

### Partage de données : Compteur partagé

#### System V Shared Memory

```c
// sysv_shared_counter.c
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
    // Créer segment
    key_t key = ftok("/tmp", 'C');
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);

    // Attacher
    int *counter = (int *)shmat(shmid, NULL, 0);

    // Utiliser
    *counter = 42;
    printf("Compteur : %d\n", *counter);

    // Détacher
    shmdt(counter);

    // Supprimer
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

// Compilation
// gcc sysv_shared_counter.c -o sysv_shared_counter
```

#### POSIX Shared Memory

```c
// posix_shared_counter.c
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    // Créer et dimensionner
    int fd = shm_open("/my_counter", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(int));

    // Mapper
    int *counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                        MAP_SHARED, fd, 0);
    close(fd);  // Peut fermer immédiatement

    // Utiliser
    *counter = 42;
    printf("Compteur : %d\n", *counter);

    // Détacher
    munmap(counter, sizeof(int));

    // Supprimer
    shm_unlink("/my_counter");

    return 0;
}

// Compilation : nécessite -lrt sur certains systèmes
// gcc posix_shared_counter.c -o posix_shared_counter -lrt
```

**Différences clés** :
- POSIX nécessite `-lrt` (library realtime) sur certains systèmes
- System V : API dédiée (`shmat`/`shmdt`)
- POSIX : Réutilise `mmap()` (cohérent avec fichiers mappés)

---

## Critères de choix

### Quand utiliser System V IPC ?

✅ **Situations appropriées :**

1. **Maintien de code existant**
   - Projet legacy utilisant déjà System V
   - Éviter le refactoring massif

2. **Ensembles de sémaphores**
   - Besoin de gérer plusieurs sémaphores liés
   - Opérations atomiques sur plusieurs sémaphores

3. **Filtrage par type de messages**
   - Pattern producteur-consommateur avec catégories
   - Priorités basées sur types personnalisés

4. **Persistance garantie**
   - Ressources IPC devant survivre aux redémarrages d'applications
   - Administration centralisée avec `ipcs`/`ipcrm`

5. **Environnements embedded sans POSIX complet**
   - Systèmes très contraints
   - RTOS sans support POSIX

❌ **À éviter pour :**
- Nouveaux projets (préférer POSIX)
- Besoin de simplicité maximale
- Équipes peu familières avec System V

---

### Quand utiliser POSIX IPC ?

✅ **Situations appropriées :**

1. **Nouveaux projets** 🎯
   - Standard moderne et recommandé
   - Meilleure lisibilité du code

2. **Portabilité maximale**
   - Code destiné à plusieurs UNIX (Linux, macOS, BSD)
   - Standard IEEE bien supporté

3. **Intégration système de fichiers**
   - Besoin de voir les ressources dans `/dev/shm`
   - Utilisation d'outils standards (ls, rm)

4. **Notifications asynchrones** (message queues)
   - Signaux via `mq_notify()`
   - Callbacks de threads

5. **Simplicité et maintenabilité**
   - Équipes débutantes en IPC
   - Code devant être facilement compréhensible

6. **Cohérence avec `mmap()`**
   - Projet utilisant déjà memory-mapped files
   - Architecture unifiée autour de `mmap()`

✅ **Recommandé par défaut** sauf besoin spécifique System V

---

## Migration System V → POSIX

### Tableau d'équivalences

| System V | POSIX | Notes |
|----------|-------|-------|
| `semget()` + `semctl(SETVAL)` | `sem_open()` | Créer sémaphore |
| `semop(..., -1, ...)` | `sem_wait()` | Attendre |
| `semop(..., +1, ...)` | `sem_post()` | Signaler |
| `semctl(IPC_RMID)` | `sem_unlink()` | Supprimer |
| `shmget()` | `shm_open()` + `ftruncate()` | Créer shm |
| `shmat()` | `mmap()` | Attacher shm |
| `shmdt()` | `munmap()` | Détacher shm |
| `shmctl(IPC_RMID)` | `shm_unlink()` | Supprimer shm |
| `msgget()` | `mq_open()` | Créer queue |
| `msgsnd()` | `mq_send()` | Envoyer message |
| `msgrcv()` | `mq_receive()` | Recevoir message |
| `msgctl(IPC_RMID)` | `mq_unlink()` | Supprimer queue |

### Exemple de migration : Sémaphore

**Avant (System V) :**

```c
key_t key = ftok("/tmp", 'S');  
int semid = semget(key, 1, IPC_CREAT | 0666);  

union semun arg;  
arg.val = 1;  
semctl(semid, 0, SETVAL, arg);  

struct sembuf wait_op = {0, -1, SEM_UNDO};  
semop(semid, &wait_op, 1);  

// Section critique

struct sembuf post_op = {0, +1, SEM_UNDO};  
semop(semid, &post_op, 1);  

semctl(semid, 0, IPC_RMID);
```

**Après (POSIX) :**

```c
sem_t *sem = sem_open("/my_sem", O_CREAT, 0666, 1);

sem_wait(sem);

// Section critique

sem_post(sem);

sem_close(sem);  
sem_unlink("/my_sem");  
```

**Bénéfices** :
- ✅ Code 60% plus court
- ✅ Pas de `union` ni `struct` complexes
- ✅ Nommage explicite
- ❌ Perte de `SEM_UNDO` (compensation nécessaire)

---

## Défis de la migration

### 1. `SEM_UNDO` n'existe pas en POSIX

**Problème** : System V annule automatiquement les opérations si le processus crash.

**Solution POSIX** :
```c
// Utiliser des handlers de signaux
void cleanup_handler(int sig) {
    sem_post(global_sem);  // Libérer explicitement
    exit(1);
}

signal(SIGTERM, cleanup_handler);  
signal(SIGINT, cleanup_handler);  
```

### 2. Ensembles de sémaphores

**Problème** : POSIX n'a pas d'équivalent direct aux ensembles.

**Solution** :
```c
// System V : 1 ensemble de 3 sémaphores
int semid = semget(key, 3, IPC_CREAT | 0666);

// POSIX : 3 sémaphores individuels
sem_t *sem1 = sem_open("/sem1", O_CREAT, 0666, 1);  
sem_t *sem2 = sem_open("/sem2", O_CREAT, 0666, 1);  
sem_t *sem3 = sem_open("/sem3", O_CREAT, 0666, 1);  
```

### 3. Filtrage de messages par type

**Problème** : POSIX n'a pas le concept de `mtype`.

**Solution** : Utiliser les priorités ou plusieurs queues.

```c
// System V : 1 queue, filtrage par type
msgrcv(msqid, &msg, size, TYPE_URGENT, 0);

// POSIX : Utiliser les priorités (0-31)
mq_receive(mq, buffer, size, &priority);  
if (priority >= 20) {  
    // Message urgent
}

// Ou : Plusieurs queues dédiées
mqd_t mq_urgent = mq_open("/queue_urgent", O_RDONLY);  
mqd_t mq_normal = mq_open("/queue_normal", O_RDONLY);  
```

---

## Outils et inspection

### System V : Outils spécialisés

```bash
# Lister toutes les ressources IPC
ipcs

# Sémaphores uniquement
ipcs -s

# Mémoire partagée uniquement
ipcs -m

# Files de messages uniquement
ipcs -q

# Détails d'une ressource
ipcs -s -i <semid>

# Supprimer
ipcrm -s <semid>  
ipcrm -m <shmid>  
ipcrm -q <msqid>  

# Nettoyer toutes les ressources d'un utilisateur
ipcs -s | grep $USER | awk '{print $2}' | xargs -I {} ipcrm -s {}
```

### POSIX : Outils standards du système de fichiers

```bash
# Sémaphores (sur certains systèmes)
ls -la /dev/shm/sem.*

# Mémoire partagée
ls -lh /dev/shm/

# Files de messages
ls -la /dev/mqueue/

# Voir le contenu (hexadécimal)
xxd /dev/shm/my_shared_memory

# Supprimer
rm /dev/shm/my_shm  
rm /dev/mqueue/my_queue  

# Taille utilisée
du -sh /dev/shm/*  
df -h /dev/shm  
```

**Avantage POSIX** : Pas besoin d'apprendre de nouveaux outils !

---

## Performance : Quel est le plus rapide ?

### Tests de performance

Des benchmarks montrent que **les performances sont équivalentes** :

| Opération | System V | POSIX | Différence |
|-----------|----------|-------|------------|
| Sémaphore wait/post | ~150 ns | ~150 ns | Négligeable |
| Shared memory read | ~5 ns | ~5 ns | Identique |
| Message send/receive | ~2 μs | ~2 μs | Négligeable |

**Conclusion** : **Pas de différence significative** de performance.

Le choix doit se baser sur :
- ✅ Simplicité de l'API (avantage POSIX)
- ✅ Fonctionnalités spécifiques (avantage System V pour ensembles)
- ✅ Maintenabilité du code

---

## Coexistence : Peut-on mélanger ?

**Réponse courte** : Techniquement oui, mais **fortement déconseillé**.

### Pourquoi éviter de mélanger ?

1. **Confusion** : Deux APIs différentes pour les mêmes concepts
2. **Inspection compliquée** : Outils différents (`ipcs` vs `ls`)
3. **Formation** : L'équipe doit connaître les deux standards
4. **Maintenance** : Code hétérogène difficile à maintenir

### Cas acceptable

```c
// OK : POSIX pour nouveau code, System V pour intégration legacy
sem_t *posix_sem = sem_open("/new_feature", O_CREAT, 0666, 1);

// Communiquer avec un daemon legacy System V existant
int legacy_msqid = msgget(existing_key, 0666);  
msgsnd(legacy_msqid, &msg, size, 0);  
```

**Recommandation** : Choisissez **un seul standard par projet** et tenez-vous-y.

---

## Recommandations finales

### Pour les nouveaux projets 🎯

**Utilisez POSIX IPC** sauf si :
- Vous avez besoin d'ensembles de sémaphores avec opérations atomiques multiples
- Votre système ne supporte pas POSIX complet

**Avantages POSIX** :
- ✅ Code plus lisible et maintenable
- ✅ API moderne et cohérente
- ✅ Intégration naturelle au système de fichiers
- ✅ Standard actuel recommandé

### Pour les projets existants

**Gardez System V si** :
- Le code fonctionne bien
- Pas de problèmes de maintenabilité
- L'équipe connaît déjà System V

**Migrez vers POSIX si** :
- Refactoring prévu
- Problèmes de compréhension du code
- Onboarding difficile pour nouveaux développeurs

### Matrice de décision

```
┌─────────────────────────────────────────┐
│ Vous démarrez un nouveau projet ?       │
│                                         │
│  OUI → Utilisez POSIX                   │
│  NON → Continuez avec l'existant        │
│        (sauf si refactoring)            │
└─────────────────────────────────────────┘

┌─────────────────────────────────────────┐
│ Besoin d'opérations atomiques           │
│ sur plusieurs sémaphores ?              │
│                                         │
│  OUI → System V peut être justifié      │
│  NON → POSIX recommandé                 │
└─────────────────────────────────────────┘

┌─────────────────────────────────────────┐
│ Équipe familière avec System V ?        │
│                                         │
│  OUI → OK de continuer                  │
│  NON → POSIX plus facile à apprendre    │
└─────────────────────────────────────────┘
```

---

## Tableau final : Choix rapide

| Critère | Choisir | Pourquoi |
|---------|---------|----------|
| **Nouveau projet** | POSIX ⭐ | Standard moderne |
| **Simplicité** | POSIX ⭐ | API intuitive |
| **Portabilité** | POSIX ⭐ | Standard IEEE |
| **Ensembles de sémaphores** | System V ⭐ | Seule option |
| **Opérations atomiques multiples** | System V ⭐ | Unique à System V |
| **Filtrage messages par type** | System V ⭐ | `mtype` natif |
| **Notifications asynchrones** | POSIX ⭐ | `mq_notify()` |
| **Intégration filesystem** | POSIX ⭐ | `/dev/shm`, `/dev/mqueue` |
| **Code legacy** | System V ⭐ | Maintenir l'existant |
| **Apprentissage** | POSIX ⭐ | Plus facile |

---

## Exemple complet : Les deux approches

### Problème : Producteur-Consommateur avec buffer circulaire

#### Version System V

```c
// producer_consumer_sysv.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define BUFFER_SIZE 5

union semun {
    int val;
    unsigned short *array;
};

typedef struct {
    int buffer[BUFFER_SIZE];
    int in, out;
} shared_buffer_t;

void sem_wait_sysv(int semid, int num) {
    struct sembuf op = {num, -1, 0};
    semop(semid, &op, 1);
}

void sem_post_sysv(int semid, int num) {
    struct sembuf op = {num, +1, 0};
    semop(semid, &op, 1);
}

int main() {
    key_t key = ftok("/tmp", 'P');

    // Créer ensemble de 3 sémaphores (empty, full, mutex)
    int semid = semget(key, 3, IPC_CREAT | 0666);
    union semun arg;
    unsigned short vals[3] = {BUFFER_SIZE, 0, 1};
    arg.array = vals;
    semctl(semid, 0, SETALL, arg);

    // Créer mémoire partagée
    int shmid = shmget(key, sizeof(shared_buffer_t), IPC_CREAT | 0666);
    shared_buffer_t *sb = shmat(shmid, NULL, 0);
    sb->in = sb->out = 0;

    // ... logique producteur-consommateur avec sem_wait_sysv/sem_post_sysv ...

    // Nettoyage
    shmdt(sb);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);

    return 0;
}
```

#### Version POSIX

```c
// producer_consumer_posix.c
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 5

typedef struct {
    int buffer[BUFFER_SIZE];
    int in, out;
} shared_buffer_t;

int main() {
    // Créer sémaphores
    sem_t *empty = sem_open("/sem_empty", O_CREAT, 0666, BUFFER_SIZE);
    sem_t *full = sem_open("/sem_full", O_CREAT, 0666, 0);
    sem_t *mutex = sem_open("/sem_mutex", O_CREAT, 0666, 1);

    // Créer mémoire partagée
    int fd = shm_open("/my_buffer", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(shared_buffer_t));
    shared_buffer_t *sb = mmap(NULL, sizeof(shared_buffer_t),
                                PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    sb->in = sb->out = 0;

    // ... logique producteur-consommateur avec sem_wait/sem_post ...

    // Nettoyage
    munmap(sb, sizeof(shared_buffer_t));
    shm_unlink("/my_buffer");
    sem_close(empty);
    sem_close(full);
    sem_close(mutex);
    sem_unlink("/sem_empty");
    sem_unlink("/sem_full");
    sem_unlink("/sem_mutex");

    return 0;
}
```

**Comparaison** :
- POSIX : 3 sémaphores nommés séparément vs System V : 1 ensemble
- POSIX : Nommage explicite (`/sem_empty`) vs System V : indices numériques
- POSIX : Pas de `union semun` nécessaire
- Lines of code : Similaires, mais POSIX plus lisible

---

## Résumé

### System V IPC

**Points forts** ⭐ :
- Ensembles de ressources
- Opérations atomiques multiples (sémaphores)
- Filtrage sophistiqué (message queues)
- Très répandu dans le code existant

**Points faibles** ⚠️ :
- API complexe (structures, unions, flags)
- Clés numériques opaques
- Outils spécialisés requis
- Courbe d'apprentissage abrupte

**Utilisez pour** : Code legacy, besoins avancés spécifiques

---

### POSIX IPC

**Points forts** ⭐ :
- API simple et intuitive
- Nommage symbolique clair
- Intégration au filesystem
- Outils standards (ls, rm)
- Portabilité maximale

**Points faibles** ⚠️ :
- Pas d'ensembles de sémaphores
- Pas d'opérations atomiques multiples
- Pas de filtrage natif par type (messages)

**Utilisez pour** : Nouveaux projets, simplicité, maintenabilité

---

### Le verdict final 🏆

**Pour 90% des projets : Choisissez POSIX**

- ✅ Plus simple à comprendre et maintenir
- ✅ Standard moderne et bien supporté
- ✅ Courbe d'apprentissage douce
- ✅ Performance équivalente à System V

**System V reste pertinent pour** :
- Maintien de code existant bien fonctionnel
- Besoins spécifiques (ensembles, opérations atomiques)
- Environnements sans POSIX complet

---

## Références

- POSIX.1-2001, POSIX.1-2008 (IEEE 1003.1)
- Stevens & Rago, *Advanced Programming in the UNIX Environment* (Chapitres 15-16)
- Linux man pages : `man 7 sem_overview`, `man 7 shm_overview`, `man 7 mq_overview`
- Single UNIX Specification (SUS)
- System V Interface Definition (SVID)

---

**Prochaine section** : 19.5 Memory-mapped files (mmap)

⏭️ [Memory-mapped files (mmap)](/19-memoire-partagee-ipc/05-mmap.md)
