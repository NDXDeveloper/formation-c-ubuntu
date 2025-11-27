🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 19.5 Memory-mapped Files (mmap)

## Introduction

**Memory mapping** (ou mappage mémoire) est une technique qui permet de **traiter un fichier comme s'il était directement en mémoire**. Au lieu d'utiliser `read()` et `write()` pour accéder à un fichier, vous pouvez le "mapper" dans l'espace d'adressage de votre processus et y accéder comme un simple tableau.

### Analogie du monde réel

Imaginez deux façons de consulter un livre :

**Méthode classique (read/write) :**
- Vous demandez à un bibliothécaire de vous lire une page spécifique
- Pour chaque page, vous devez faire une nouvelle demande
- Le bibliothécaire copie le contenu pour vous

**Méthode mmap :**
- Le livre est posé devant vous sur la table
- Vous pouvez **directement** tourner les pages et lire ce que vous voulez
- Pas d'intermédiaire, accès instantané

C'est exactement ce que fait `mmap()` : il place le fichier directement dans votre espace mémoire.

---

## Pourquoi utiliser mmap ?

### Avantages

1. **Performance** ⚡
   - Pas de copies entre kernel et user space
   - Le système gère le cache automatiquement
   - Particulièrement efficace pour les accès aléatoires

2. **Simplicité** 🎯
   - Traiter un fichier comme un tableau
   - Pas besoin de `lseek()`, `read()`, `write()`
   - Code plus court et lisible

3. **Partage mémoire** 🔗
   - Plusieurs processus peuvent mapper le même fichier
   - Modifications visibles par tous (avec `MAP_SHARED`)
   - Base de la mémoire partagée POSIX

4. **Lazy loading** 💤
   - Le fichier n'est pas chargé entièrement
   - Seules les pages accédées sont chargées (demand paging)

### Cas d'usage typiques

- ✅ Lecture/écriture de gros fichiers (logs, bases de données)
- ✅ Accès aléatoire fréquent à un fichier
- ✅ Partage de données entre processus
- ✅ Fichiers de configuration en lecture seule
- ✅ Traitement d'images ou de données binaires
- ✅ Implémentation de bases de données (SQLite, LMDB)

---

## Concept : Comment fonctionne mmap ?

### Sans mmap : Approche traditionnelle

```
┌─────────────────┐
│   Processus     │
│                 │
│   Buffer (RAM)  │  ← Copie via read()
└────────┬────────┘
         │ read()/write()
         ▼
┌─────────────────┐
│  Kernel         │
│  Page Cache     │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│   Disque        │
│   Fichier.txt   │
└─────────────────┘
```

**Problème** : Copies multiples (disque → kernel → user space)

### Avec mmap : Accès direct

```
┌─────────────────────────────┐
│   Processus                 │
│                             │
│   Espace d'adressage        │
│   virtuel                   │
│                             │
│   0x7f000000 ────┐          │
│   [File mapped]  │          │
│                  │          │
└──────────────────┼──────────┘
                   │
                   ▼ Mapping direct
         ┌─────────────────┐
         │  Kernel         │
         │  Page Cache     │
         └────────┬────────┘
                  │
                  ▼
         ┌─────────────────┐
         │   Disque        │
         │   Fichier.txt   │
         └─────────────────┘
```

**Avantage** : Pas de copie, accès direct via la page cache

---

## L'API mmap

### Fonction principale : `mmap()`

```c
#include <sys/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
```

**Paramètres :**

1. **`addr`** : Adresse souhaitée (généralement `NULL` = laissez le système choisir)
2. **`length`** : Taille à mapper (en octets)
3. **`prot`** : Protections mémoire (lecture, écriture, exécution)
4. **`flags`** : Options de mapping
5. **`fd`** : Descripteur de fichier à mapper
6. **`offset`** : Position de départ dans le fichier (doit être multiple de la taille de page)

**Retour :**
- Pointeur vers la zone mappée en cas de succès
- `MAP_FAILED` (équivalent à `(void *) -1`) en cas d'erreur

### Protections (`prot`)

```c
// Protections de base
PROT_NONE   // Pas d'accès
PROT_READ   // Lecture seule
PROT_WRITE  // Écriture (implique généralement PROT_READ)
PROT_EXEC   // Exécution (pour charger du code)

// Combinaisons courantes
PROT_READ                    // Lecture seule
PROT_READ | PROT_WRITE       // Lecture/écriture
PROT_READ | PROT_EXEC        // Lecture/exécution (bibliothèques)
```

### Flags principaux

```c
// Type de mapping (un seul requis)
MAP_SHARED      // Modifications visibles par tous les processus et écrites sur le disque
MAP_PRIVATE     // Modifications privées au processus (copy-on-write)

// Options additionnelles
MAP_ANONYMOUS   // Pas de fichier (mémoire pure), fd = -1
MAP_FIXED       // Force l'adresse spécifiée (dangereux)
MAP_POPULATE    // Précharge les pages immédiatement
MAP_LOCKED      // Verrouille les pages en RAM (évite le swap)
```

**Différence cruciale** : `MAP_SHARED` vs `MAP_PRIVATE`

```
MAP_SHARED:
Processus A écrit → Fichier modifié ← Processus B voit les changements

MAP_PRIVATE:
Processus A écrit → Copie privée (pas le fichier) × Processus B ne voit rien
```

### Fonction de démapping : `munmap()`

```c
int munmap(void *addr, size_t length);
```

**Libère** le mapping et invalide le pointeur.

**Important** : Toujours appeler `munmap()` avant de fermer le descripteur de fichier.

---

## Exemple de base : Lire un fichier avec mmap

```c
// read_file_mmap.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fichier>\n", argv[0]);
        return 1;
    }

    // 1. Ouvrir le fichier
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // 2. Obtenir la taille du fichier
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        close(fd);
        return 1;
    }

    size_t file_size = sb.st_size;
    printf("Taille du fichier : %zu octets\n", file_size);

    // 3. Mapper le fichier en mémoire
    char *file_content = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_content == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    // 4. On peut fermer le descripteur maintenant (le mapping persiste)
    close(fd);

    // 5. Utiliser le contenu comme un simple pointeur
    printf("\nContenu du fichier :\n");
    printf("---\n");

    // Écrire sur stdout
    write(STDOUT_FILENO, file_content, file_size);

    printf("\n---\n");

    // Ou utiliser comme un tableau
    printf("Premier caractère : '%c'\n", file_content[0]);
    printf("Dernier caractère : '%c'\n", file_content[file_size - 1]);

    // 6. Libérer le mapping
    if (munmap(file_content, file_size) == -1) {
        perror("munmap");
        return 1;
    }

    printf("Mapping libéré avec succès\n");

    return 0;
}
```

### Compilation et test

```bash
# Compiler
gcc read_file_mmap.c -o read_file_mmap

# Créer un fichier de test
echo "Hello from mmap!" > test.txt

# Exécuter
./read_file_mmap test.txt
```

**Sortie :**
```
Taille du fichier : 17 octets

Contenu du fichier :
---
Hello from mmap!
---
Premier caractère : 'H'
Dernier caractère : '!'
Mapping libéré avec succès
```

---

## Exemple : Modifier un fichier avec mmap

```c
// modify_file_mmap.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fichier>\n", argv[0]);
        return 1;
    }

    // 1. Ouvrir en lecture/écriture
    int fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // 2. Obtenir la taille
    struct stat sb;
    fstat(fd, &sb);
    size_t file_size = sb.st_size;

    // 3. Mapper avec PROT_WRITE et MAP_SHARED
    //    MAP_SHARED = les modifications seront écrites dans le fichier
    char *data = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    close(fd);

    printf("Contenu original : %.*s\n", (int)file_size, data);

    // 4. Modifier le contenu (mettre en majuscules)
    for (size_t i = 0; i < file_size; i++) {
        data[i] = toupper(data[i]);
    }

    printf("Contenu modifié : %.*s\n", (int)file_size, data);

    // 5. Forcer la synchronisation sur disque (optionnel)
    if (msync(data, file_size, MS_SYNC) == -1) {
        perror("msync");
    }

    // 6. Libérer
    munmap(data, file_size);

    printf("Fichier modifié avec succès\n");

    return 0;
}
```

### Test

```bash
# Compiler
gcc modify_file_mmap.c -o modify_file_mmap

# Créer un fichier
echo "hello world" > test.txt

# Modifier
./modify_file_mmap test.txt

# Vérifier
cat test.txt
# Sortie : HELLO WORLD
```

**Explication** :
- `MAP_SHARED` + `PROT_WRITE` → Les modifications sont écrites dans le fichier
- `msync()` force l'écriture immédiate (sinon fait par le système plus tard)

---

## MAP_SHARED vs MAP_PRIVATE : Différences pratiques

### MAP_SHARED : Modifications persistantes

```c
// shared_example.c
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd = open("shared.txt", O_RDWR | O_CREAT, 0666);
    write(fd, "AAAA", 4);

    // MAP_SHARED : modifications visibles dans le fichier
    char *data = mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    printf("Avant : %.*s\n", 4, data);

    // Modifier
    data[0] = 'B';
    data[1] = 'B';

    printf("Après : %.*s\n", 4, data);

    // Forcer l'écriture
    msync(data, 4, MS_SYNC);

    munmap(data, 4);

    // Vérifier que le fichier est modifié
    fd = open("shared.txt", O_RDONLY);
    char buf[4];
    read(fd, buf, 4);
    printf("Fichier : %.*s\n", 4, buf);  // Affiche : BBAA
    close(fd);

    return 0;
}
```

**Résultat** : Le fichier `shared.txt` contient maintenant `BBAA`

### MAP_PRIVATE : Copy-on-Write

```c
// private_example.c
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd = open("private.txt", O_RDWR | O_CREAT, 0666);
    write(fd, "AAAA", 4);

    // MAP_PRIVATE : modifications privées (copy-on-write)
    char *data = mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    close(fd);

    printf("Avant : %.*s\n", 4, data);

    // Modifier
    data[0] = 'B';
    data[1] = 'B';

    printf("Après mapping : %.*s\n", 4, data);

    munmap(data, 4);

    // Vérifier le fichier (reste inchangé)
    fd = open("private.txt", O_RDONLY);
    char buf[4];
    read(fd, buf, 4);
    printf("Fichier : %.*s\n", 4, buf);  // Affiche : AAAA (inchangé)
    close(fd);

    return 0;
}
```

**Résultat** : Le fichier `private.txt` contient toujours `AAAA`

**Mécanisme Copy-on-Write** :
1. Initialement, le processus partage les pages avec le fichier
2. Lors de la première écriture, le noyau crée une **copie privée**
3. Les modifications vont dans la copie, pas dans le fichier

**Quand utiliser** :
- `MAP_SHARED` : IPC, modifications persistantes, bases de données
- `MAP_PRIVATE` : Lecture avec modifications temporaires, chargement d'exécutables

---

## Synchronisation : msync()

La fonction `msync()` force l'écriture des modifications sur disque.

```c
#include <sys/mman.h>

int msync(void *addr, size_t length, int flags);
```

**Flags :**

```c
MS_SYNC      // Bloquant : attend la fin de l'écriture
MS_ASYNC     // Asynchrone : lance l'écriture et retourne immédiatement
MS_INVALIDATE // Invalide les autres mappings du même fichier
```

**Exemple d'utilisation :**

```c
// Modifier des données
data[0] = 'X';
data[1] = 'Y';

// Forcer l'écriture immédiate (bloquant)
if (msync(data, file_size, MS_SYNC) == -1) {
    perror("msync");
}

// Maintenant garanti sur disque
printf("Données sauvegardées\n");
```

**Quand utiliser `msync()` ?**
- Avant de fermer une application critique
- Après des modifications importantes (checkpoint)
- Pour garantir la cohérence des données
- En pratique, rarement nécessaire (le système le fait automatiquement)

---

## Mémoire anonyme : MAP_ANONYMOUS

`mmap()` peut aussi allouer de la **mémoire pure** (sans fichier associé).

```c
// anonymous_mmap.c
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>

int main() {
    size_t size = 4096;  // 1 page

    // Allouer de la mémoire (pas de fichier)
    // fd = -1, offset = 0
    int *data = mmap(NULL, size, PROT_READ | PROT_WRITE,
                     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (data == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Utiliser comme un tableau normal
    data[0] = 42;
    data[1] = 100;

    printf("data[0] = %d\n", data[0]);
    printf("data[1] = %d\n", data[1]);

    // Libérer
    munmap(data, size);

    return 0;
}
```

**Avantages vs `malloc()` :**
- ✅ Initialisation à zéro garantie
- ✅ Contrôle fin des protections (`mprotect()`)
- ✅ Alignement sur les pages
- ❌ Plus lent que `malloc()` pour de petites allocations
- ❌ Granularité de la taille de page (4096 octets minimum)

**Use case** : Allocations de grandes zones, arenas mémoire personnalisées, implémentation d'allocateurs

---

## Mémoire partagée entre processus

`mmap()` avec `MAP_SHARED` + `MAP_ANONYMOUS` permet de partager de la mémoire entre processus liés (parent-enfant).

```c
// shared_memory_fork.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main() {
    // Créer une zone partagée AVANT fork()
    size_t size = sizeof(int);
    int *shared_counter = mmap(NULL, size, PROT_READ | PROT_WRITE,
                                MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (shared_counter == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    *shared_counter = 0;  // Initialiser

    // Fork
    pid_t pid = fork();

    if (pid == 0) {
        // ===== PROCESSUS FILS =====
        for (int i = 0; i < 5; i++) {
            (*shared_counter)++;
            printf("[FILS] Compteur : %d\n", *shared_counter);
            usleep(100000);  // 100ms
        }
        exit(0);

    } else {
        // ===== PROCESSUS PARENT =====
        for (int i = 0; i < 5; i++) {
            (*shared_counter)++;
            printf("[PARENT] Compteur : %d\n", *shared_counter);
            usleep(100000);
        }

        wait(NULL);  // Attendre le fils

        printf("\nValeur finale : %d\n", *shared_counter);

        munmap(shared_counter, size);
    }

    return 0;
}
```

**Sortie (peut varier - race condition !) :**
```
[PARENT] Compteur : 1
[FILS] Compteur : 2
[PARENT] Compteur : 3
[FILS] Compteur : 4
[PARENT] Compteur : 5
[FILS] Compteur : 6
...
Valeur finale : 10
```

⚠️ **Attention** : Il y a une **race condition** ! Les deux processus modifient `shared_counter` sans synchronisation. Pour un code production, utilisez des sémaphores.

---

## Protéger les accès : mprotect()

La fonction `mprotect()` permet de changer les permissions d'une région mappée.

```c
#include <sys/mman.h>

int mprotect(void *addr, size_t len, int prot);
```

**Exemple : Rendre une zone read-only après initialisation**

```c
// mprotect_example.c
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <signal.h>

void segfault_handler(int sig) {
    printf("SEGFAULT attrapé ! Tentative d'écriture interdite.\n");
    exit(1);
}

int main() {
    signal(SIGSEGV, segfault_handler);

    size_t size = 4096;
    char *data = mmap(NULL, size, PROT_READ | PROT_WRITE,
                      MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    // Écrire des données
    strcpy(data, "Configuration importante");
    printf("Données : %s\n", data);

    // Rendre read-only
    if (mprotect(data, size, PROT_READ) == -1) {
        perror("mprotect");
        return 1;
    }

    printf("Zone maintenant en lecture seule\n");

    // Tentative d'écriture → SEGFAULT
    data[0] = 'X';  // Crash ici !

    munmap(data, size);
    return 0;
}
```

**Use case** : Protéger des configurations, implémenter des tables de dispatch read-only, sécurité

---

## Conseils et optimisation

### 1. Taille de page et alignement

Les mappings doivent être alignés sur la **taille de page** (généralement 4096 octets).

```c
#include <unistd.h>

long page_size = sysconf(_SC_PAGESIZE);
printf("Taille de page : %ld octets\n", page_size);  // Généralement 4096

// L'offset doit être un multiple de page_size
off_t offset = 4096;  // OK
off_t offset_bad = 100;  // Erreur EINVAL
```

### 2. Préchargement avec MAP_POPULATE

```c
// Charge toutes les pages immédiatement au lieu de demand-paging
char *data = mmap(NULL, size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);
```

**Avantage** : Pas de page faults ultérieurs
**Inconvénient** : Plus lent au démarrage

### 3. Verrouillage en mémoire avec MAP_LOCKED

```c
// Empêche le swap (nécessite souvent des privilèges)
char *data = mmap(NULL, size, PROT_READ | PROT_WRITE,
                  MAP_ANONYMOUS | MAP_PRIVATE | MAP_LOCKED, -1, 0);
```

**Use case** : Applications temps réel, données sensibles (clés crypto)

### 4. Éviter les copies inutiles

```c
// ❌ Mauvais : copie tout le fichier en mémoire
FILE *f = fopen("large_file.bin", "rb");
char *buffer = malloc(file_size);
fread(buffer, 1, file_size, f);

// ✅ Bon : mapping direct, pas de copie
int fd = open("large_file.bin", O_RDONLY);
char *data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
```

---

## Comparaison : mmap vs read/write

### Benchmark simple

```c
// benchmark.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define FILE_SIZE (100 * 1024 * 1024)  // 100 MB

double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

void benchmark_read() {
    int fd = open("testfile.bin", O_RDONLY);
    char buffer[4096];
    long sum = 0;

    double start = get_time();

    while (read(fd, buffer, sizeof(buffer)) > 0) {
        sum += buffer[0];  // Accès pour forcer la lecture
    }

    double elapsed = get_time() - start;
    close(fd);

    printf("read() : %.3f secondes\n", elapsed);
}

void benchmark_mmap() {
    int fd = open("testfile.bin", O_RDONLY);
    struct stat sb;
    fstat(fd, &sb);

    double start = get_time();

    char *data = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    long sum = 0;
    for (size_t i = 0; i < sb.st_size; i += 4096) {
        sum += data[i];
    }

    munmap(data, sb.st_size);
    close(fd);

    double elapsed = get_time() - start;

    printf("mmap() : %.3f secondes\n", elapsed);
}

int main() {
    // Créer un fichier de test
    printf("Création du fichier de test...\n");
    int fd = open("testfile.bin", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    char buffer[4096] = {0};
    for (int i = 0; i < FILE_SIZE / 4096; i++) {
        write(fd, buffer, sizeof(buffer));
    }
    close(fd);

    // Benchmarks
    printf("Benchmarking...\n\n");
    benchmark_read();
    benchmark_mmap();

    // Nettoyer
    unlink("testfile.bin");

    return 0;
}
```

**Résultats typiques** (lecture séquentielle) :
```
read() : 0.450 secondes
mmap() : 0.380 secondes
```

**Conclusion** : `mmap()` est généralement **15-20% plus rapide** pour les accès séquentiels, et encore plus pour les accès aléatoires.

---

## Tableau récapitulatif : Quand utiliser mmap ?

| Situation | Utilisez | Raison |
|-----------|----------|--------|
| **Gros fichiers** (>10 MB) | ✅ mmap | Pas de copie, lazy loading |
| **Petits fichiers** (<4 KB) | ❌ read/write | Overhead de mapping |
| **Accès aléatoire** | ✅ mmap | Pas de lseek() |
| **Accès séquentiel** | 🟡 Les deux | Performance similaire |
| **Modifications fréquentes** | ✅ mmap + MAP_SHARED | Écriture directe |
| **Lecture seule** | ✅ mmap | Simplicité |
| **Partage entre processus** | ✅ mmap | IPC efficace |
| **Streaming** (données infinies) | ❌ read() | mmap nécessite taille fixe |
| **Fichiers réseau** | ⚠️ Dépend | Peut être inefficace |

---

## Cas d'usage réels

### 1. Base de données : SQLite

SQLite utilise massivement `mmap()` pour accéder aux fichiers de base de données :

```c
// Simplifié : comment SQLite mappe un fichier DB
int fd = open("database.db", O_RDWR);
struct stat sb;
fstat(fd, &sb);

// Mapper toute la base en mémoire
void *db_memory = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE,
                       MAP_SHARED, fd, 0);

// Accéder aux pages de la DB comme un tableau
db_page_t *page = (db_page_t *)(db_memory + page_offset);
```

### 2. Éditeur de texte : Vim

Vim utilise `mmap()` pour éditer de gros fichiers sans tout charger en RAM.

### 3. Traitement d'images

```c
// load_image.c
// Charger une image BMP directement avec mmap
typedef struct {
    uint16_t type;
    uint32_t size;
    // ... autres champs BMP
} __attribute__((packed)) bmp_header_t;

int fd = open("image.bmp", O_RDONLY);
struct stat sb;
fstat(fd, &sb);

bmp_header_t *img = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

// Accès direct aux pixels
uint8_t *pixels = (uint8_t *)img + img->pixel_offset;
printf("Premier pixel RGB : %d %d %d\n", pixels[0], pixels[1], pixels[2]);

munmap(img, sb.st_size);
close(fd);
```

### 4. Configuration partagée

```c
// config_shared.c
// Plusieurs processus lisent une config sans IPC complexe

// Processus 1 (writer)
int fd = open("/tmp/config.bin", O_RDWR | O_CREAT, 0666);
ftruncate(fd, sizeof(config_t));
config_t *cfg = mmap(NULL, sizeof(config_t), PROT_READ | PROT_WRITE,
                     MAP_SHARED, fd, 0);
cfg->timeout = 30;
cfg->max_connections = 100;
msync(cfg, sizeof(config_t), MS_SYNC);

// Processus 2, 3, 4... (readers)
int fd = open("/tmp/config.bin", O_RDONLY);
config_t *cfg = mmap(NULL, sizeof(config_t), PROT_READ, MAP_SHARED, fd, 0);
printf("Timeout : %d\n", cfg->timeout);  // Lit directement
```

---

## Pièges et erreurs courantes

### 1. Oublier de vérifier MAP_FAILED

```c
// ❌ Dangereux
char *data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
data[0] = 'X';  // Peut crasher si mmap a échoué !

// ✅ Correct
char *data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
if (data == MAP_FAILED) {
    perror("mmap");
    return 1;
}
```

### 2. Mapper plus que la taille du fichier

```c
// Fichier de 100 octets
struct stat sb;
fstat(fd, &sb);  // sb.st_size = 100

// ❌ Erreur : mapper 1000 octets
char *data = mmap(NULL, 1000, PROT_READ, MAP_PRIVATE, fd, 0);
// Accès à data[500] → SIGBUS !

// ✅ Correct : mapper exactement la taille
char *data = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
```

### 3. Offset non aligné

```c
// ❌ Erreur : offset doit être multiple de la taille de page
off_t offset = 100;  // EINVAL !
char *data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, offset);

// ✅ Correct
off_t offset = 4096;  // Multiple de page_size
char *data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, offset);
```

### 4. Accès après munmap()

```c
char *data = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
data[0] = 'A';

munmap(data, 4096);

// ❌ Use-after-free : data est invalide !
printf("%c\n", data[0]);  // SEGFAULT
```

### 5. SIGBUS : Accès au-delà du fichier

Si vous mappez un fichier puis qu'un autre processus le tronque :

```c
// Processus A
char *data = mmap(NULL, 10000, PROT_READ, MAP_SHARED, fd, 0);

// Processus B
ftruncate(fd, 100);  // Réduit le fichier à 100 octets

// Processus A
char c = data[5000];  // SIGBUS ! (au-delà de la nouvelle taille)
```

**Solution** : Utiliser `MAP_PRIVATE` si d'autres peuvent modifier la taille.

### 6. Modifications non synchronisées

```c
// Modifier avec MAP_SHARED
data[0] = 'X';

// ❌ Fermer immédiatement sans msync
close(fd);
// Les modifications peuvent ne pas être écrites !

// ✅ Forcer la synchronisation
msync(data, size, MS_SYNC);
munmap(data, size);
close(fd);
```

---

## Bonnes pratiques

### ✅ À faire

1. **Toujours vérifier `MAP_FAILED`**
   ```c
   if (ptr == MAP_FAILED) { /* erreur */ }
   ```

2. **Utiliser `fstat()` pour la taille exacte**
   ```c
   struct stat sb;
   fstat(fd, &sb);
   mmap(NULL, sb.st_size, ...);
   ```

3. **Fermer le fd après mmap**
   ```c
   char *data = mmap(..., fd, 0);
   close(fd);  // Le mapping persiste
   ```

4. **Utiliser `MAP_SHARED` pour IPC**
   ```c
   mmap(..., MAP_SHARED, fd, 0);
   ```

5. **Appeler `munmap()` avant de quitter**
   ```c
   munmap(data, size);
   ```

6. **Utiliser `msync()` pour garantir l'écriture**
   ```c
   msync(data, size, MS_SYNC);
   ```

### ❌ À éviter

1. **Ne pas** supposer que les modifications sont instantanées sur disque
2. **Ne pas** mapper des fichiers réseau (NFS) sans précaution
3. **Ne pas** oublier les permissions lors de l'ouverture (`O_RDWR` pour `PROT_WRITE`)
4. **Ne pas** utiliser `mmap()` pour de très petits fichiers (<4 KB)
5. **Ne pas** accéder au-delà de la taille mappée
6. **Ne pas** mélanger `MAP_SHARED` et `MAP_PRIVATE` sur le même fichier

---

## Résumé

### Points clés sur mmap()

1. ✅ **Mappe un fichier directement en mémoire**
   - Traiter un fichier comme un tableau
   - Pas de copies explicites

2. ✅ **Performance supérieure pour gros fichiers**
   - Lazy loading (demand paging)
   - Gestion automatique du cache par le kernel

3. ✅ **Deux modes principaux**
   - `MAP_SHARED` : modifications persistantes, IPC
   - `MAP_PRIVATE` : copy-on-write, modifications temporaires

4. ✅ **API simple**
   ```c
   void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
   int munmap(void *addr, size_t length);
   int msync(void *addr, size_t length, int flags);
   ```

5. ✅ **Use cases**
   - Bases de données
   - Traitement d'images/vidéos
   - Fichiers de configuration partagés
   - IPC performant
   - Chargement de bibliothèques dynamiques

### Quand utiliser mmap ?

| Critère | mmap recommandé ? |
|---------|-------------------|
| Fichier > 10 MB | ✅ Oui |
| Accès aléatoire | ✅ Oui |
| Lecture seule | ✅ Oui |
| Partage IPC | ✅ Oui |
| Fichier < 4 KB | ❌ Non (read/write) |
| Streaming infini | ❌ Non |
| Fichiers réseau | ⚠️ Dépend |

---

## Aller plus loin

### Fonctions avancées

```c
// Redimensionner un mapping
void *mremap(void *old_addr, size_t old_size, size_t new_size, int flags);

// Donner des conseils au kernel sur l'utilisation
int madvise(void *addr, size_t length, int advice);
// MADV_SEQUENTIAL : accès séquentiel prévu
// MADV_RANDOM : accès aléatoire
// MADV_WILLNEED : précharger
// MADV_DONTNEED : libérer les pages physiques
```

### Huge pages

Pour des performances extrêmes sur de très gros mappings :

```c
#define MAP_HUGETLB 0x40000

void *data = mmap(NULL, 2 * 1024 * 1024,  // 2 MB
                  PROT_READ | PROT_WRITE,
                  MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB,
                  -1, 0);
```

**Nécessite** : Configuration système (`/proc/sys/vm/nr_hugepages`)

---

## Références

- Man pages : `man mmap`, `man munmap`, `man msync`, `man mprotect`
- POSIX.1-2001, POSIX.1-2008
- Stevens & Rago, *Advanced Programming in the UNIX Environment*, Chapitre 14
- Linux Programmer's Manual
- Kernel documentation : `Documentation/vm/mmap.txt`

---

**Prochaine section** : Module 7 - Techniques Avancées et Optimisation

⏭️ [Réseau (Sockets)](/20-reseau-sockets/README.md)
