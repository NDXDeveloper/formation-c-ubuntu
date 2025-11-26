🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 16.2 Appels système

## Introduction

Les **appels système** (system calls en anglais, souvent abrégés **syscalls**) sont le pont entre votre programme et le noyau Linux. Ils constituent l'interface fondamentale qui permet à votre code d'interagir avec le matériel et les ressources du système d'exploitation.

> **💡 Analogie simple :** Imaginez que votre programme est un client dans un restaurant (l'espace utilisateur) et le noyau Linux est la cuisine (l'espace noyau). Vous ne pouvez pas entrer dans la cuisine pour préparer votre repas vous-même. À la place, vous devez passer commande via un serveur (l'appel système). Le serveur transmet votre demande à la cuisine, qui prépare le plat et vous le rapporte.

## Qu'est-ce qu'un appel système ?

Un appel système est une **fonction spéciale** qui demande au noyau Linux d'effectuer une opération privilégiée que votre programme ne peut pas faire directement.

### Pourquoi les appels système existent-ils ?

Pour des raisons de **sécurité** et de **stabilité**, Linux divise la mémoire en deux espaces distincts :

```
┌─────────────────────────────────────┐
│      ESPACE UTILISATEUR             │
│   (User Space / Ring 3)             │
│                                     │
│   • Votre programme C               │
│   • Applications                    │
│   • Bibliothèques                   │
│   • Accès limité au matériel        │
│                                     │
├─────────────────────────────────────┤
│         APPELS SYSTÈME              │
│      (Interface Kernel)             │
├─────────────────────────────────────┤
│      ESPACE NOYAU                   │
│   (Kernel Space / Ring 0)           │
│                                     │
│   • Noyau Linux                     │
│   • Pilotes matériels               │
│   • Gestion mémoire                 │
│   • Accès complet au matériel       │
│                                     │
└─────────────────────────────────────┘
```

**Séparation des privilèges :**
- **Espace utilisateur** : Votre programme s'exécute avec des droits limités
- **Espace noyau** : Le noyau a tous les droits (accès disque, réseau, mémoire, etc.)
- **Appels système** : Le seul moyen légal de passer de l'un à l'autre

### Que peuvent faire les appels système ?

Les appels système permettent de :
- 📁 **Manipuler des fichiers** : `open()`, `read()`, `write()`, `close()`
- 🔄 **Gérer des processus** : `fork()`, `exec()`, `wait()`, `exit()`
- 💾 **Allouer de la mémoire** : `brk()`, `mmap()`
- 🌐 **Communiquer en réseau** : `socket()`, `connect()`, `send()`, `recv()`
- ⏰ **Gérer le temps** : `time()`, `sleep()`, `clock_gettime()`
- 🔐 **Contrôler les permissions** : `chmod()`, `chown()`
- 📡 **Envoyer des signaux** : `kill()`, `signal()`

## Comment fonctionne un appel système ?

### Le cycle de vie d'un appel système

Quand vous appelez `open()` dans votre code C, voici ce qui se passe :

```
1. VOTRE CODE C
   ↓
   int fd = open("/tmp/file.txt", O_RDONLY);
   ↓

2. BIBLIOTHÈQUE C (libc)
   ↓
   • Prépare les paramètres
   • Place le numéro de syscall dans un registre
   ↓

3. INTERRUPTION LOGICIELLE (int 0x80 ou syscall)
   ↓
   • Transition vers le mode noyau
   • Sauvegarde du contexte
   ↓

4. NOYAU LINUX
   ↓
   • Vérifie les permissions
   • Accède au système de fichiers
   • Effectue l'opération
   ↓

5. RETOUR AU PROGRAMME
   ↓
   • Restaure le contexte
   • Retourne la valeur (fd ou -1)
   ↓

6. VOTRE CODE C
   ↓
   if (fd == -1) { /* erreur */ }
```

### Les étapes en détail

**Étape 1 : Préparation**
```c
// Votre code
int fd = open("/tmp/file.txt", O_RDONLY);
```

**Étape 2 : La bibliothèque C (wrapper)**
- La fonction `open()` dans la libc est un **wrapper** (enveloppe)
- Elle prépare les arguments selon les conventions du système
- Elle place le numéro de l'appel système dans un registre (sur x86-64 : `rax`)

**Étape 3 : Transition vers le noyau**
- Une instruction spéciale (`syscall` sur x86-64) déclenche le passage en mode noyau
- Le processeur change de niveau de privilège (Ring 3 → Ring 0)
- Le contexte actuel est sauvegardé

**Étape 4 : Exécution dans le noyau**
- Le noyau identifie quel syscall exécuter (grâce au numéro)
- Il vérifie les permissions et la validité des paramètres
- Il effectue l'opération demandée
- Il prépare la valeur de retour

**Étape 5 : Retour au programme**
- Le noyau restaure le contexte
- Le processeur repasse en mode utilisateur (Ring 0 → Ring 3)
- La valeur de retour est placée dans un registre (`rax`)

**Étape 6 : Traitement du résultat**
- Votre programme récupère la valeur de retour
- En cas d'erreur, `errno` est positionné

### Coût en performance

Les appels système sont **relativement lents** comparés aux fonctions normales :

| Opération | Temps approximatif |
|-----------|-------------------|
| Fonction C normale | ~1-10 nanosecondes |
| Appel système | ~100-1000 nanosecondes |
| Lecture disque | ~1-10 millisecondes |

**Raisons :**
- Changement de contexte (user → kernel → user)
- Vérifications de sécurité
- Sauvegarde/restauration de l'état du processeur

**Conséquence pratique :** Il faut minimiser le nombre d'appels système dans les parties critiques du code.

## Convention des appels système

### Valeurs de retour

**Presque tous les appels système suivent cette convention :**

```c
// Succès : retourne une valeur ≥ 0
int result = syscall(...);
if (result >= 0) {
    // Succès
}

// Échec : retourne -1 et positionne errno
if (result == -1) {
    // Erreur, consulter errno
    perror("syscall");
}
```

**Exemples concrets :**

```c
// open() : retourne le fd (≥ 0) ou -1
int fd = open("file.txt", O_RDONLY);
if (fd == -1) {
    perror("open");
}

// read() : retourne nombre d'octets lus ou -1
ssize_t n = read(fd, buffer, 100);
if (n == -1) {
    perror("read");
}

// fork() : retourne le PID (> 0), 0 (enfant), ou -1
pid_t pid = fork();
if (pid == -1) {
    perror("fork");
}
```

### La variable `errno`

Quand un appel système échoue, il positionne la variable globale **`errno`** avec un code d'erreur spécifique.

```c
#include <errno.h>
#include <string.h>
#include <stdio.h>

int fd = open("fichier_inexistant.txt", O_RDONLY);
if (fd == -1) {
    printf("Erreur numéro : %d\n", errno);
    printf("Message : %s\n", strerror(errno));
    // Ou simplement :
    perror("open");
}
```

**Codes d'erreur courants :**

| errno | Constante | Signification |
|-------|-----------|---------------|
| 1 | `EPERM` | Opération non permise |
| 2 | `ENOENT` | Fichier ou répertoire inexistant |
| 9 | `EBADF` | Mauvais descripteur de fichier |
| 12 | `ENOMEM` | Mémoire insuffisante |
| 13 | `EACCES` | Permission refusée |
| 17 | `EEXIST` | Le fichier existe déjà |
| 22 | `EINVAL` | Argument invalide |

**Utilisation correcte de errno :**

```c
#include <errno.h>
#include <string.h>
#include <stdio.h>

// ✅ BON
int fd = open("file.txt", O_RDONLY);
if (fd == -1) {
    if (errno == ENOENT) {
        printf("Fichier non trouvé\n");
    } else if (errno == EACCES) {
        printf("Permission refusée\n");
    } else {
        perror("open");
    }
}

// ❌ MAUVAIS : ne pas vérifier errno sans avoir vérifié l'échec
int fd = open("file.txt", O_RDONLY);
if (errno == ENOENT) {  // ERREUR : fd peut être valide !
    // ...
}
```

**Important :** `errno` n'est positionné que si la fonction retourne une erreur (-1). Ne testez jamais `errno` sans avoir d'abord vérifié la valeur de retour.

## Les principaux appels système

### Gestion des fichiers

#### `open()` - Ouvrir un fichier

```c
#include <fcntl.h>
#include <sys/stat.h>

int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
```

**Exemple :**
```c
int fd = open("/tmp/data.txt", O_RDWR | O_CREAT, 0644);
if (fd == -1) {
    perror("open");
    return 1;
}
```

#### `read()` - Lire depuis un fichier

```c
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t count);
```

**Exemple :**
```c
char buffer[256];
ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
if (n == -1) {
    perror("read");
} else if (n == 0) {
    printf("Fin de fichier\n");
} else {
    buffer[n] = '\0';
    printf("Lu %zd octets : %s\n", n, buffer);
}
```

#### `write()` - Écrire dans un fichier

```c
#include <unistd.h>

ssize_t write(int fd, const void *buf, size_t count);
```

**Exemple :**
```c
const char *message = "Bonjour Linux\n";
ssize_t n = write(fd, message, strlen(message));
if (n == -1) {
    perror("write");
} else {
    printf("Écrit %zd octets\n", n);
}
```

#### `close()` - Fermer un fichier

```c
#include <unistd.h>

int close(int fd);
```

**Exemple :**
```c
if (close(fd) == -1) {
    perror("close");
}
```

#### `lseek()` - Déplacer le curseur de lecture

```c
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence);
```

**Exemple :**
```c
// Aller au début du fichier
lseek(fd, 0, SEEK_SET);

// Aller à la fin
lseek(fd, 0, SEEK_END);

// Avancer de 100 octets
lseek(fd, 100, SEEK_CUR);
```

### Gestion des répertoires

#### `mkdir()` - Créer un répertoire

```c
#include <sys/stat.h>

int mkdir(const char *pathname, mode_t mode);
```

**Exemple :**
```c
if (mkdir("/tmp/nouveau_dossier", 0755) == -1) {
    perror("mkdir");
}
```

#### `rmdir()` - Supprimer un répertoire vide

```c
#include <unistd.h>

int rmdir(const char *pathname);
```

#### `chdir()` - Changer de répertoire courant

```c
#include <unistd.h>

int chdir(const char *path);
```

**Exemple :**
```c
if (chdir("/home/user") == -1) {
    perror("chdir");
}
```

### Gestion des processus

#### `fork()` - Créer un processus fils

```c
#include <unistd.h>

pid_t fork(void);
```

**Exemple :**
```c
pid_t pid = fork();
if (pid == -1) {
    perror("fork");
} else if (pid == 0) {
    printf("Je suis le processus enfant\n");
} else {
    printf("Je suis le parent, mon enfant a le PID %d\n", pid);
}
```

#### `execve()` - Remplacer le processus courant

```c
#include <unistd.h>

int execve(const char *pathname, char *const argv[], char *const envp[]);
```

#### `exit()` - Terminer le processus

```c
#include <unistd.h>

void _exit(int status);
```

**Note :** Préférez `exit()` de la libc qui fait du nettoyage avant d'appeler `_exit()`.

#### `wait()` / `waitpid()` - Attendre la fin d'un processus fils

```c
#include <sys/wait.h>

pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);
```

**Exemple :**
```c
pid_t pid = fork();
if (pid == 0) {
    // Processus enfant
    _exit(42);
} else {
    // Processus parent
    int status;
    waitpid(pid, &status, 0);
    printf("L'enfant s'est terminé avec le code %d\n", WEXITSTATUS(status));
}
```

### Gestion de la mémoire

#### `brk()` / `sbrk()` - Modifier la taille du tas

```c
#include <unistd.h>

int brk(void *addr);
void *sbrk(intptr_t increment);
```

**Note :** Ces appels sont rarement utilisés directement. Utilisez plutôt `malloc()` qui les utilise en interne.

#### `mmap()` - Mapper de la mémoire

```c
#include <sys/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags,
           int fd, off_t offset);
```

**Exemple (allocation anonyme) :**
```c
size_t size = 4096; // 1 page
void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
if (ptr == MAP_FAILED) {
    perror("mmap");
} else {
    // Utiliser ptr comme un tableau
    memset(ptr, 0, size);
    munmap(ptr, size);  // Libérer
}
```

### Signaux

#### `kill()` - Envoyer un signal

```c
#include <signal.h>

int kill(pid_t pid, int sig);
```

**Exemple :**
```c
// Envoyer SIGTERM au processus 1234
kill(1234, SIGTERM);

// Envoyer SIGKILL à soi-même
kill(getpid(), SIGKILL);
```

#### `signal()` / `sigaction()` - Gérer les signaux

```c
#include <signal.h>

typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
```

### Informations système

#### `getpid()` - Obtenir le PID du processus

```c
#include <unistd.h>

pid_t getpid(void);
```

**Exemple :**
```c
printf("Mon PID : %d\n", getpid());
```

#### `getppid()` - Obtenir le PID du parent

```c
#include <unistd.h>

pid_t getppid(void);
```

#### `getuid()` / `getgid()` - Obtenir UID/GID

```c
#include <unistd.h>

uid_t getuid(void);
gid_t getgid(void);
```

#### `gettimeofday()` - Obtenir l'heure actuelle

```c
#include <sys/time.h>

int gettimeofday(struct timeval *tv, struct timezone *tz);
```

**Exemple :**
```c
struct timeval tv;
gettimeofday(&tv, NULL);
printf("Secondes depuis epoch : %ld\n", tv.tv_sec);
printf("Microsecondes : %ld\n", tv.tv_usec);
```

## Appels système vs Fonctions de bibliothèque

### Les wrappers de la bibliothèque C

La plupart des appels système que vous utilisez en C sont en réalité des **wrappers** fournis par la **glibc** (GNU C Library).

```
┌──────────────────────┐
│   Votre code C       │
│   open("file.txt")   │
└──────────┬───────────┘
           │
           ↓
┌──────────────────────┐
│   Wrapper glibc      │
│   (bibliothèque C)   │
└──────────┬───────────┘
           │
           ↓
┌──────────────────────┐
│   Vrai syscall       │
│   (kernel)           │
└──────────────────────┘
```

**Pourquoi des wrappers ?**

1. **Portabilité** : Les wrappers masquent les différences entre architectures
2. **Commodité** : Gestion automatique de certains détails (errno, etc.)
3. **Sécurité** : Validation des paramètres avant l'appel
4. **Compatibilité** : Interface stable même si le noyau change

### Exemple : `write()` en détail

```c
#include <unistd.h>

// Ce que vous appelez (wrapper glibc)
ssize_t write(int fd, const void *buf, size_t count);

// Ce qui se passe en interne (pseudo-code)
ssize_t write(int fd, const void *buf, size_t count) {
    long result;

    // 1. Validation des paramètres
    if (buf == NULL) {
        errno = EINVAL;
        return -1;
    }

    // 2. Préparation des registres
    // Sur x86-64 :
    // rax = numéro de syscall (1 pour write)
    // rdi = fd
    // rsi = buf
    // rdx = count

    // 3. Appel du vrai syscall
    asm volatile (
        "syscall"
        : "=a" (result)
        : "0" (__NR_write), "D" (fd), "S" (buf), "d" (count)
        : "rcx", "r11", "memory"
    );

    // 4. Gestion du résultat
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}
```

### Fonctions de bibliothèque qui N'utilisent PAS de syscalls

Certaines fonctions de la libc n'appellent aucun syscall :

```c
// Ces fonctions sont purement en espace utilisateur
strlen(str);        // Parcours de chaîne
strcpy(dest, src);  // Copie de chaîne
memset(ptr, 0, n);  // Remplissage mémoire
sqrt(x);            // Calcul mathématique
```

### Fonctions de bibliothèque qui utilisent PLUSIEURS syscalls

Certaines fonctions font plusieurs appels système :

```c
// printf() peut appeler :
// - write() plusieurs fois
// - brk() si besoin d'allouer un buffer

// malloc() peut appeler :
// - brk() ou mmap() pour allouer
// - munmap() pour libérer
```

## Lister les appels système d'un programme

### Utiliser `strace`

L'outil **`strace`** permet de tracer tous les appels système effectués par un programme.

```bash
# Compiler un programme
gcc -o hello hello.c

# Tracer ses syscalls
strace ./hello

# Tracer uniquement certains syscalls
strace -e open,read,write ./hello

# Compter les syscalls
strace -c ./hello
```

**Exemple de sortie :**
```
execve("./hello", ["./hello"], 0x7ffe...) = 0
brk(NULL)                               = 0x55a123000
mmap(NULL, 8192, PROT_READ|PROT_WRITE, ...) = 0x7f8a...
open("/tmp/file.txt", O_RDONLY)         = 3
read(3, "Hello\n", 4096)                = 6
write(1, "Hello\n", 6)                  = 6
close(3)                                = 0
exit_group(0)                           = ?
```

### Exemple pratique avec strace

```c
// simple.c
#include <stdio.h>

int main(void) {
    printf("Bonjour\n");
    return 0;
}
```

```bash
$ gcc -o simple simple.c
$ strace -e write ./simple
```

**Sortie :**
```
write(1, "Bonjour\n", 8)               = 8
+++ exited with 0 +++
```

**Observation :** `printf()` a utilisé l'appel système `write()` sur le fd 1 (stdout).

## Appeler directement un syscall

Dans de rares cas, vous pouvez vouloir appeler directement un syscall sans passer par le wrapper.

### Avec `syscall()`

```c
#include <unistd.h>
#include <sys/syscall.h>

long syscall(long number, ...);
```

**Exemple :**
```c
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>

int main(void) {
    // Appeler gettid() directement
    pid_t tid = syscall(SYS_gettid);
    printf("Thread ID : %d\n", tid);

    // Appeler write() directement
    const char *msg = "Direct syscall\n";
    syscall(SYS_write, STDOUT_FILENO, msg, 15);

    return 0;
}
```

**Numéros de syscalls :** Définis dans `/usr/include/asm/unistd_64.h` (pour x86-64).

**Exemple de numéros :**
```c
#define __NR_read     0
#define __NR_write    1
#define __NR_open     2
#define __NR_close    3
#define __NR_fork    57
#define __NR_execve  59
```

### En assembleur inline (avancé)

```c
#include <stdio.h>

int main(void) {
    const char *msg = "Hello from asm!\n";
    long ret;

    // Appel système write() en assembleur x86-64
    asm volatile (
        "mov $1, %%rax\n"      // syscall number (write)
        "mov $1, %%rdi\n"      // fd (stdout)
        "mov %1, %%rsi\n"      // buffer
        "mov $16, %%rdx\n"     // count
        "syscall\n"
        "mov %%rax, %0\n"
        : "=r" (ret)
        : "r" (msg)
        : "rax", "rdi", "rsi", "rdx", "rcx", "r11", "memory"
    );

    printf("Syscall returned: %ld\n", ret);
    return 0;
}
```

**Note :** Cette approche est rarement nécessaire et non portable. Préférez toujours les wrappers de la libc.

## Bonnes pratiques

### 1. Toujours vérifier les valeurs de retour

```c
// ✅ BON
int fd = open("file.txt", O_RDONLY);
if (fd == -1) {
    perror("open");
    return 1;
}

// ❌ MAUVAIS
int fd = open("file.txt", O_RDONLY);
read(fd, buffer, 100);  // fd pourrait être -1 !
```

### 2. Utiliser `perror()` ou `strerror()`

```c
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Méthode 1 : perror()
if (open("file.txt", O_RDONLY) == -1) {
    perror("Erreur open");  // Affiche : "Erreur open: No such file or directory"
}

// Méthode 2 : strerror()
if (open("file.txt", O_RDONLY) == -1) {
    fprintf(stderr, "Erreur : %s\n", strerror(errno));
}
```

### 3. Gérer les interruptions (EINTR)

Certains syscalls peuvent être interrompus par un signal :

```c
#include <errno.h>

ssize_t n;
do {
    n = read(fd, buffer, size);
} while (n == -1 && errno == EINTR);

if (n == -1) {
    perror("read");
}
```

**Ou avec une macro :**
```c
#define TEMP_FAILURE_RETRY(expression) \
    ({ \
        long int _result; \
        do _result = (long int) (expression); \
        while (_result == -1 && errno == EINTR); \
        _result; \
    })

ssize_t n = TEMP_FAILURE_RETRY(read(fd, buffer, size));
```

### 4. Minimiser le nombre de syscalls

```c
// ❌ INEFFICACE (1000 syscalls)
for (int i = 0; i < 1000; i++) {
    write(fd, &data[i], 1);  // 1 octet à la fois
}

// ✅ EFFICACE (1 syscall)
write(fd, data, 1000);  // Tout d'un coup
```

### 5. Utiliser les fonctions bufferisées quand approprié

```c
// Pour du texte, préférez les fonctions bufferisées
FILE *fp = fopen("file.txt", "w");
fprintf(fp, "Line %d\n", i);  // Bufferisé automatiquement
fclose(fp);

// Pour des opérations binaires à haute performance
int fd = open("file.bin", O_WRONLY);
write(fd, large_buffer, size);  // Accès direct
close(fd);
```

## Tableau récapitulatif

| Catégorie | Appels système courants |
|-----------|------------------------|
| **Fichiers** | `open`, `read`, `write`, `close`, `lseek`, `stat`, `unlink` |
| **Répertoires** | `mkdir`, `rmdir`, `chdir`, `getcwd`, `opendir`, `readdir` |
| **Processus** | `fork`, `execve`, `exit`, `wait`, `waitpid`, `getpid` |
| **Signaux** | `kill`, `signal`, `sigaction`, `pause`, `alarm` |
| **Mémoire** | `brk`, `sbrk`, `mmap`, `munmap`, `mprotect` |
| **IPC** | `pipe`, `socket`, `connect`, `send`, `recv`, `shmget` |
| **Threads** | `clone`, `futex` (via pthread) |
| **Temps** | `time`, `gettimeofday`, `clock_gettime`, `nanosleep` |

## Points clés à retenir

✅ **Les appels système** sont l'interface entre votre programme et le noyau Linux

✅ **Transition coûteuse** : user space → kernel space → user space

✅ **Convention** : retourne ≥ 0 en cas de succès, -1 en cas d'erreur + errno

✅ **Toujours vérifier** la valeur de retour de chaque syscall

✅ **`errno`** contient le code d'erreur détaillé

✅ **`perror()` et `strerror()`** permettent d'afficher des messages d'erreur lisibles

✅ **Les wrappers glibc** simplifient l'utilisation des syscalls

✅ **`strace`** est votre meilleur ami pour comprendre ce qui se passe

✅ **Minimiser les syscalls** améliore les performances

---

## Prochaines étapes

Maintenant que vous comprenez les appels système, vous pouvez explorer :
- **Section 16.3** : Différences appels système vs bibliothèque standard
- **Section 16.4** : Permissions et modes de fichiers
- **Section 16.5** : I/O bufferisé vs non bufferisé
- **Chapitre 17** : Gestion des processus avec `fork()` et `exec()`

Les appels système sont le fondement de toute interaction avec le système d'exploitation. Ils sont omniprésents dans la programmation système Linux et comprendre leur fonctionnement est essentiel pour écrire du code système robuste et efficace !

⏭️ [Différence appels système vs bibliothèque](/16-fichiers-et-io/03-systeme-vs-bibliotheque.md)
