🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 19.1 Shared Memory (Mémoire Partagée)

## Introduction

La **mémoire partagée** (shared memory) est un mécanisme de communication inter-processus (IPC) qui permet à plusieurs processus d'accéder à une même région de mémoire. C'est la méthode IPC la plus rapide car les données ne sont pas copiées entre les processus : tous les processus accèdent directement à la même zone mémoire physique.

### Pourquoi utiliser la mémoire partagée ?

Imaginez deux programmes qui doivent échanger des données :
- **Sans mémoire partagée** : Les données doivent transiter par le noyau (pipes, sockets), ce qui implique des copies multiples
- **Avec mémoire partagée** : Les deux programmes lisent et écrivent directement dans la même zone mémoire

**Cas d'usage typiques :**
- Partage de grosses structures de données entre processus
- Communication haute performance (bases de données, serveurs)
- Caches partagés
- Traitement parallèle de données volumineuses

---

## Concepts fondamentaux

### Architecture de la mémoire partagée

```
┌─────────────┐         ┌─────────────┐
│ Processus A │         │ Processus B │
│             │         │             │
│  Espace     │         │  Espace     │
│  mémoire    │         │  mémoire    │
│  virtuel    │         │  virtuel    │
└──────┬──────┘         └──────┬──────┘
       │                       │
       │   Mapping virtuel     │
       └───────────┬───────────┘
                   │
           ┌───────▼────────┐
           │  Shared Memory │
           │   Segment      │
           │  (RAM physique)│
           └────────────────┘
```

Chaque processus "mappe" (attache) le segment de mémoire partagée dans son propre espace d'adressage virtuel. Les modifications faites par un processus sont immédiatement visibles par les autres.

### Les deux APIs principales

Sous Linux, il existe deux APIs pour gérer la mémoire partagée :

1. **POSIX Shared Memory** (moderne, recommandée)
   - API standardisée et portable
   - Utilise des noms symboliques
   - Fonctions : `shm_open()`, `shm_unlink()`

2. **System V Shared Memory** (legacy)
   - API plus ancienne
   - Utilise des clés numériques
   - Fonctions : `shmget()`, `shmat()`, `shmdt()`, `shmctl()`

Nous nous concentrerons sur **POSIX** dans cette section, car c'est l'approche moderne.

---

## POSIX Shared Memory

### Étapes d'utilisation

1. **Créer/Ouvrir** un segment de mémoire partagée avec `shm_open()`
2. **Dimensionner** le segment avec `ftruncate()`
3. **Mapper** le segment dans l'espace d'adressage avec `mmap()`
4. **Utiliser** la mémoire (lecture/écriture)
5. **Détacher** le mapping avec `munmap()`
6. **Fermer** le descripteur avec `close()`
7. **Supprimer** le segment avec `shm_unlink()` (quand terminé)

### Fonctions principales

#### `shm_open()` - Créer ou ouvrir un segment

```c
#include <sys/mman.h>
#include <fcntl.h>

int shm_open(const char *name, int oflag, mode_t mode);
```

**Paramètres :**
- `name` : Nom du segment (doit commencer par `/`, ex: `/my_shm`)
- `oflag` : Flags d'ouverture
  - `O_CREAT` : Créer si n'existe pas
  - `O_EXCL` : Erreur si existe déjà (avec O_CREAT)
  - `O_RDONLY` : Lecture seule
  - `O_RDWR` : Lecture/écriture
- `mode` : Permissions (comme `0666`)

**Retour :** Descripteur de fichier (comme `open()`), ou -1 en cas d'erreur

#### `ftruncate()` - Définir la taille

```c
#include <unistd.h>

int ftruncate(int fd, off_t length);
```

Définit la taille du segment de mémoire partagée.

#### `mmap()` - Mapper en mémoire

```c
#include <sys/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
```

**Paramètres importants :**
- `addr` : Généralement `NULL` (le système choisit l'adresse)
- `length` : Taille à mapper
- `prot` : Protection
  - `PROT_READ` : Lecture
  - `PROT_WRITE` : Écriture
  - `PROT_READ | PROT_WRITE` : Les deux
- `flags` : Doit contenir `MAP_SHARED` pour la mémoire partagée
- `fd` : Descripteur retourné par `shm_open()`

**Retour :** Pointeur vers la zone mappée, ou `MAP_FAILED` en cas d'erreur

#### `munmap()` - Détacher le mapping

```c
int munmap(void *addr, size_t length);
```

#### `shm_unlink()` - Supprimer le segment

```c
int shm_unlink(const char *name);
```

Supprime le segment de mémoire partagée du système (équivalent à `unlink()` pour les fichiers).

---

## Exemple complet : Processus Producteur-Consommateur

### Structure de données partagée

```c
// shared_data.h
#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#define SHM_NAME "/my_shared_memory"
#define SHM_SIZE sizeof(struct shared_data)

struct shared_data {
    int counter;
    char message[256];
};

#endif
```

### Processus Producteur (Écrivain)

```c
// producer.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "shared_data.h"

int main() {
    // 1. Créer le segment de mémoire partagée
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // 2. Définir la taille du segment
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("ftruncate");
        exit(1);
    }

    // 3. Mapper le segment en mémoire
    struct shared_data *data = mmap(NULL, SHM_SIZE,
                                     PROT_READ | PROT_WRITE,
                                     MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // 4. Utiliser la mémoire partagée
    printf("Producteur : Écriture des données...\n");
    data->counter = 42;
    strcpy(data->message, "Hello from producer!");

    printf("Producteur : counter = %d\n", data->counter);
    printf("Producteur : message = %s\n", data->message);

    // Attendre que l'utilisateur appuie sur Entrée
    printf("Appuyez sur Entrée pour terminer...\n");
    getchar();

    // 5. Détacher le mapping
    if (munmap(data, SHM_SIZE) == -1) {
        perror("munmap");
    }

    // 6. Fermer le descripteur
    close(shm_fd);

    // 7. Supprimer le segment (optionnel - généralement fait par le dernier processus)
    // shm_unlink(SHM_NAME);

    return 0;
}
```

### Processus Consommateur (Lecteur)

```c
// consumer.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "shared_data.h"

int main() {
    // 1. Ouvrir le segment existant
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open (le segment existe-t-il ?)");
        exit(1);
    }

    // 2. Mapper le segment en mémoire
    struct shared_data *data = mmap(NULL, SHM_SIZE,
                                     PROT_READ | PROT_WRITE,
                                     MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // 3. Lire les données
    printf("Consommateur : Lecture des données...\n");
    printf("Consommateur : counter = %d\n", data->counter);
    printf("Consommateur : message = %s\n", data->message);

    // Modifier les données
    data->counter += 10;
    printf("Consommateur : nouveau counter = %d\n", data->counter);

    // 4. Détacher le mapping
    if (munmap(data, SHM_SIZE) == -1) {
        perror("munmap");
    }

    // 5. Fermer le descripteur
    close(shm_fd);

    // 6. Supprimer le segment
    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink");
    }

    return 0;
}
```

### Compilation et exécution

```bash
# Compiler (nécessite -lrt sur certains systèmes)
gcc producer.c -o producer -lrt  
gcc consumer.c -o consumer -lrt  

# Terminal 1 : Lancer le producteur
./producer

# Terminal 2 : Lancer le consommateur (pendant que producer tourne)
./consumer
```

**Sortie attendue :**

Terminal 1 (producer) :
```
Producteur : Écriture des données...  
Producteur : counter = 42  
Producteur : message = Hello from producer!  
Appuyez sur Entrée pour terminer...  
```

Terminal 2 (consumer) :
```
Consommateur : Lecture des données...  
Consommateur : counter = 42  
Consommateur : message = Hello from producer!  
Consommateur : nouveau counter = 52  
```

---

## Inspection de la mémoire partagée

Sous Linux, les segments de mémoire partagée POSIX sont stockés dans `/dev/shm/` :

```bash
# Lister les segments existants
ls -l /dev/shm/

# Exemple de sortie
-rw-r--r-- 1 user user 264 Nov 27 10:30 my_shared_memory

# Afficher le contenu (binaire)
xxd /dev/shm/my_shared_memory
```

---

## Problèmes de synchronisation

⚠️ **ATTENTION CRITIQUE** : La mémoire partagée ne fournit **aucune synchronisation** par défaut !

### Le problème

Quand plusieurs processus accèdent simultanément à la mémoire partagée, des **race conditions** peuvent survenir :

```c
// Processus A                  // Processus B
int val = data->counter;        int val = data->counter;  // Tous deux lisent 10  
val++;                          val++;                     // Tous deux incrémentent à 11  
data->counter = val;            data->counter = val;       // Résultat final : 11 au lieu de 12 !  
```

### Solutions de synchronisation

Il est **obligatoire** d'utiliser des mécanismes de synchronisation :

1. **Sémaphores POSIX** (`sem_t`) - Recommandé
2. **Mutex dans la mémoire partagée** (pthread avec attributs `PTHREAD_PROCESS_SHARED`)
3. **Sémaphores System V**
4. **File locks** (`fcntl()`)

#### Exemple avec sémaphore POSIX

```c
// shared_data.h (version avec sémaphore)
#include <semaphore.h>

struct shared_data {
    sem_t semaphore;      // Sémaphore pour synchronisation
    int counter;
    char message[256];
};

// Initialisation dans le producteur
sem_init(&data->semaphore, 1, 1);  // 1 = partagé entre processus, 1 = valeur initiale

// Utilisation dans tous les processus
sem_wait(&data->semaphore);        // Entrer en section critique
// ... accès aux données partagées ...
data->counter++;  
sem_post(&data->semaphore);        // Sortir de la section critique  

// Destruction dans le dernier processus
sem_destroy(&data->semaphore);
```

---

## Comparaison : Mémoire partagée vs autres IPC

| Méthode | Vitesse | Complexité | Synchronisation | Use Case |
|---------|---------|------------|-----------------|----------|
| **Shared Memory** | ⚡ Très rapide | 🔴 Élevée | ❌ Manuelle | Gros volumes de données |
| **Pipes** | 🐢 Lente | 🟢 Faible | ✅ Intégrée | Communication simple |
| **Message Queues** | 🟡 Moyenne | 🟡 Moyenne | ✅ Intégrée | Messages structurés |
| **Sockets** | 🟡 Moyenne | 🟡 Moyenne | ✅ Intégrée | Réseau / IPC flexible |

---

## Bonnes pratiques

### ✅ À faire

1. **Toujours synchroniser** les accès concurrents
2. **Vérifier les erreurs** de chaque appel système
3. **Détacher** (`munmap()`) avant de fermer (`close()`)
4. **Supprimer** (`shm_unlink()`) le segment quand plus nécessaire
5. **Initialiser** les données lors de la création
6. **Documenter** la structure de données partagée

### ❌ À éviter

1. **Ne jamais** accéder sans synchronisation
2. **Ne pas** oublier `ftruncate()` lors de la création
3. **Ne pas** supposer que les données sont initialisées à zéro (utiliser `memset()`)
4. **Ne pas** laisser des segments orphelins (utiliser `shm_unlink()`)
5. **Ne pas** mapper plus que la taille réelle du segment

---

## Gestion des erreurs courantes

```c
// Vérification complète avec gestion d'erreurs
int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);  
if (shm_fd == -1) {  
    perror("shm_open");
    // Erreurs possibles :
    // - EACCES : Permissions insuffisantes
    // - EEXIST : Déjà existant (avec O_CREAT | O_EXCL)
    // - EMFILE : Trop de descripteurs ouverts
    exit(EXIT_FAILURE);
}

if (ftruncate(shm_fd, SHM_SIZE) == -1) {
    perror("ftruncate");
    close(shm_fd);
    shm_unlink(SHM_NAME);
    exit(EXIT_FAILURE);
}

void *ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);  
if (ptr == MAP_FAILED) {  
    perror("mmap");
    // Erreurs possibles :
    // - EBADF : Descripteur invalide
    // - ENOMEM : Pas assez de mémoire
    close(shm_fd);
    shm_unlink(SHM_NAME);
    exit(EXIT_FAILURE);
}
```

---

## Nettoyage des segments orphelins

Les segments de mémoire partagée persistent même après la fin des processus. Pour nettoyer manuellement :

```bash
# Lister
ls -l /dev/shm/

# Supprimer un segment spécifique
rm /dev/shm/my_shared_memory

# Ou utiliser shm_unlink dans le code
```

---

## System V Shared Memory (pour référence)

L'ancienne API System V utilise des fonctions différentes :

```c
#include <sys/ipc.h>
#include <sys/shm.h>

// Créer/obtenir un segment
int shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0666);

// Attacher
void *ptr = shmat(shmid, NULL, 0);

// Détacher
shmdt(ptr);

// Supprimer
shmctl(shmid, IPC_RMID, NULL);
```

**Différences principales :**
- Utilise des **clés numériques** au lieu de noms
- API moins intuitive
- Commandes `ipcs` et `ipcrm` pour inspection
- Moins portable que POSIX

⚠️ **Recommandation** : Privilégiez POSIX sauf si vous devez maintenir du code legacy.

---

## Commandes utiles

```bash
# Voir les segments POSIX
ls -lh /dev/shm/

# Voir les segments System V
ipcs -m

# Supprimer un segment System V
ipcrm -m <shmid>

# Surveiller l'utilisation mémoire
free -h  
df -h /dev/shm/  
```

---

## Résumé

La **mémoire partagée** est l'IPC le plus performant mais nécessite :

1. ✅ Création avec `shm_open()` + `ftruncate()`
2. ✅ Mapping avec `mmap()`
3. ✅ **Synchronisation obligatoire** (sémaphores, mutex)
4. ✅ Nettoyage avec `munmap()` + `shm_unlink()`

**Quand l'utiliser** : Partage de gros volumes de données entre processus avec haute performance.

**Quand l'éviter** : Communication simple (préférer pipes), données de petite taille, besoin de simplicité.

---

## Références

- Man pages : `man shm_open`, `man mmap`, `man sem_init`
- POSIX.1-2001, POSIX.1-2008
- Linux Programmer's Manual
- Stevens & Rago, *Advanced Programming in the UNIX Environment*

---

**Prochaine section** : 19.2 Sémaphores System V

⏭️ [Sémaphores System V](/19-memoire-partagee-ipc/02-semaphores-system-v.md)
