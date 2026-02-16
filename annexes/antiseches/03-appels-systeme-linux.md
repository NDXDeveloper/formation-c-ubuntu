🔝 Retour au [Sommaire](/SOMMAIRE.md)

# B.3 Appels système Linux (Cheat Sheet)

## Introduction

Les **appels système** (syscalls) sont l'interface entre votre programme et le noyau Linux. Ils permettent d'accéder aux ressources système : fichiers, processus, mémoire, réseau, etc.

💡 **Pour les débutants :** Les appels système sont des fonctions "spéciales" qui demandent au système d'exploitation de faire quelque chose que votre programme ne peut pas faire seul (ouvrir un fichier, créer un processus, etc.).

---

## Appels système vs Fonctions de bibliothèque

### Différences fondamentales

| Aspect | Appels système | Fonctions de bibliothèque |
|--------|----------------|---------------------------|
| **Définition** | Interface kernel | Fonctions en espace utilisateur |
| **Exemples** | `open()`, `read()`, `write()` | `fopen()`, `fread()`, `fwrite()` |
| **Performance** | Plus lent (changement de contexte) | Plus rapide (bufferisé) |
| **Portabilité** | Spécifique UNIX/Linux | Standard C (portable) |
| **Buffering** | Non bufferisé | Bufferisé |
| **Retour d'erreur** | -1 + `errno` | NULL ou -1 + `errno` |

### Exemple comparatif

**Avec appels système (bas niveau) :**
```c
#include <fcntl.h>
#include <unistd.h>

int fd = open("file.txt", O_RDONLY);  
char buffer[100];  
read(fd, buffer, 100);  
close(fd);  
```

**Avec bibliothèque standard (haut niveau) :**
```c
#include <stdio.h>

FILE *fp = fopen("file.txt", "r");  
char buffer[100];  
fread(buffer, 1, 100, fp);  
fclose(fp);  
```

### Quand utiliser quoi ?

**Utilisez les fonctions de bibliothèque (`fopen`, `printf`) quand :**
- ✅ Vous voulez du code portable
- ✅ Vous avez besoin de buffering (performance)
- ✅ Vous travaillez avec des fichiers texte
- ✅ Vous débutez en C

**Utilisez les appels système (`open`, `write`) quand :**
- ✅ Vous avez besoin de contrôle fin
- ✅ Vous faites de la programmation système
- ✅ Vous travaillez avec des fichiers binaires/devices
- ✅ Vous gérez des processus/signaux/réseau

---

## Gestion des erreurs

### La variable `errno`

Tous les appels système retournent **-1** en cas d'erreur et positionnent la variable globale `errno`.

```c
#include <errno.h>
#include <string.h>
#include <stdio.h>

int fd = open("nonexistent.txt", O_RDONLY);  
if (fd == -1) {  
    printf("Erreur: %s\n", strerror(errno));
    // Ou :
    perror("open");
}
```

### Codes d'erreur courants

| Code | Constante | Signification |
|------|-----------|---------------|
| 1 | `EPERM` | Operation not permitted |
| 2 | `ENOENT` | No such file or directory |
| 3 | `ESRCH` | No such process |
| 4 | `EINTR` | Interrupted system call |
| 5 | `EIO` | I/O error |
| 9 | `EBADF` | Bad file descriptor |
| 11 | `EAGAIN` | Resource temporarily unavailable |
| 12 | `ENOMEM` | Out of memory |
| 13 | `EACCES` | Permission denied |
| 22 | `EINVAL` | Invalid argument |

**Référence complète :** `man errno` ou `/usr/include/asm-generic/errno-base.h`

---

## 📁 Appels système : Fichiers et I/O

### `open()` - Ouvrir un fichier

```c
#include <fcntl.h>

int open(const char *pathname, int flags);  
int open(const char *pathname, int flags, mode_t mode);  
```

**Flags courants :**
- `O_RDONLY` : Lecture seule
- `O_WRONLY` : Écriture seule
- `O_RDWR` : Lecture et écriture
- `O_CREAT` : Créer si n'existe pas
- `O_APPEND` : Ajouter à la fin
- `O_TRUNC` : Tronquer à 0
- `O_EXCL` : Échec si existe (avec `O_CREAT`)
- `O_NONBLOCK` : Mode non bloquant

**Mode (permissions) :**
- `S_IRUSR` (0400) : Lecture propriétaire
- `S_IWUSR` (0200) : Écriture propriétaire
- `S_IXUSR` (0100) : Exécution propriétaire
- Ou octal : `0644` = rw-r--r--

**Exemple :**
```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main() {
    // Ouvrir en lecture
    int fd = open("file.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Créer un nouveau fichier
    int fd2 = open("newfile.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd2 == -1) {
        perror("open");
        close(fd);
        return 1;
    }

    close(fd);
    close(fd2);
    return 0;
}
```

**Retour :** File descriptor (≥ 0) en succès, -1 en erreur.

---

### `close()` - Fermer un fichier

```c
#include <unistd.h>

int close(int fd);
```

**Exemple :**
```c
close(fd);
```

**Retour :** 0 en succès, -1 en erreur.

**Important :** Toujours fermer les fichiers ouverts !

---

### `read()` - Lire des données

```c
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t count);
```

**Paramètres :**
- `fd` : File descriptor
- `buf` : Buffer où stocker les données
- `count` : Nombre d'octets à lire

**Exemple :**
```c
char buffer[1024];  
ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);  

if (bytes_read == -1) {
    perror("read");
} else if (bytes_read == 0) {
    printf("Fin de fichier\n");
} else {
    buffer[bytes_read] = '\0';  // Null-terminate
    printf("Lu: %s\n", buffer);
}
```

**Retour :**
- Nombre d'octets lus en succès
- 0 = fin de fichier (EOF)
- -1 = erreur

**Note :** `read()` peut lire moins que demandé !

---

### `write()` - Écrire des données

```c
#include <unistd.h>

ssize_t write(int fd, const void *buf, size_t count);
```

**Exemple :**
```c
const char *message = "Hello, World!\n";  
ssize_t bytes_written = write(fd, message, strlen(message));  

if (bytes_written == -1) {
    perror("write");
} else if (bytes_written < strlen(message)) {
    printf("Écriture partielle\n");
}
```

**Retour :**
- Nombre d'octets écrits en succès
- -1 = erreur

**Note :** `write()` peut écrire moins que demandé !

---

### `lseek()` - Déplacer le curseur de fichier

```c
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence);
```

**Whence (origine) :**
- `SEEK_SET` : Début du fichier
- `SEEK_CUR` : Position actuelle
- `SEEK_END` : Fin du fichier

**Exemples :**
```c
// Aller au début
lseek(fd, 0, SEEK_SET);

// Avancer de 100 octets
lseek(fd, 100, SEEK_CUR);

// Aller à la fin
lseek(fd, 0, SEEK_END);

// Obtenir la position actuelle
off_t pos = lseek(fd, 0, SEEK_CUR);

// Obtenir la taille du fichier
off_t size = lseek(fd, 0, SEEK_END);
```

**Retour :** Nouvelle position, ou -1 en erreur.

---

### `dup()` / `dup2()` - Dupliquer un file descriptor

```c
#include <unistd.h>

int dup(int oldfd);  
int dup2(int oldfd, int newfd);  
```

**Exemple : Redirection de sortie**
```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Rediriger stdout vers le fichier
    dup2(fd, STDOUT_FILENO);
    close(fd);

    printf("Ceci sera écrit dans output.txt\n");

    return 0;
}
```

---

### `pipe()` - Créer un pipe

```c
#include <unistd.h>

int pipe(int pipefd[2]);
```

**Usage :**
- `pipefd[0]` : Lecture
- `pipefd[1]` : Écriture

**Exemple : Communication parent-enfant**
```c
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

int main() {
    int pipefd[2];
    char buffer[100];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {  // Enfant
        close(pipefd[1]);  // Fermer écriture

        read(pipefd[0], buffer, sizeof(buffer));
        printf("Enfant a reçu: %s\n", buffer);

        close(pipefd[0]);
    } else {  // Parent
        close(pipefd[0]);  // Fermer lecture

        const char *msg = "Hello from parent";
        write(pipefd[1], msg, strlen(msg) + 1);

        close(pipefd[1]);
        wait(NULL);
    }

    return 0;
}
```

---

### `stat()` / `fstat()` / `lstat()` - Informations sur fichier

```c
#include <sys/stat.h>

int stat(const char *pathname, struct stat *statbuf);  
int fstat(int fd, struct stat *statbuf);  
int lstat(const char *pathname, struct stat *statbuf);  
```

**Structure `stat` :**
```c
struct stat {
    dev_t     st_dev;      // Device ID
    ino_t     st_ino;      // Inode number
    mode_t    st_mode;     // File type and mode
    nlink_t   st_nlink;    // Number of hard links
    uid_t     st_uid;      // User ID
    gid_t     st_gid;      // Group ID
    off_t     st_size;     // Size in bytes
    time_t    st_atime;    // Access time
    time_t    st_mtime;    // Modification time
    time_t    st_ctime;    // Change time
};
```

**Exemple :**
```c
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>

int main() {
    struct stat st;

    if (stat("file.txt", &st) == -1) {
        perror("stat");
        return 1;
    }

    printf("Taille: %ld octets\n", st.st_size);
    printf("Permissions: %o\n", st.st_mode & 0777);
    printf("Modification: %s", ctime(&st.st_mtime));

    if (S_ISREG(st.st_mode))  printf("Fichier régulier\n");
    if (S_ISDIR(st.st_mode))  printf("Répertoire\n");
    if (S_ISLNK(st.st_mode))  printf("Lien symbolique\n");

    return 0;
}
```

---

### `access()` - Vérifier les permissions

```c
#include <unistd.h>

int access(const char *pathname, int mode);
```

**Modes :**
- `F_OK` : Existence
- `R_OK` : Lecture
- `W_OK` : Écriture
- `X_OK` : Exécution

**Exemple :**
```c
if (access("file.txt", F_OK) == 0) {
    printf("Le fichier existe\n");
}

if (access("file.txt", R_OK | W_OK) == 0) {
    printf("Lecture et écriture possibles\n");
}
```

---

## 🔄 Appels système : Processus

### `fork()` - Créer un processus enfant

```c
#include <unistd.h>

pid_t fork(void);
```

**Retour :**
- Dans le parent : PID de l'enfant
- Dans l'enfant : 0
- Erreur : -1

**Exemple :**
```c
#include <unistd.h>
#include <stdio.h>

int main() {
    printf("Avant fork\n");

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // Code de l'enfant
        printf("Je suis l'enfant (PID: %d)\n", getpid());
    } else {
        // Code du parent
        printf("Je suis le parent (PID: %d), enfant: %d\n", getpid(), pid);
    }

    printf("Après fork (exécuté par les deux)\n");
    return 0;
}
```

---

### `exec` famille - Remplacer le processus

```c
#include <unistd.h>

int execl(const char *path, const char *arg, ...);  
int execlp(const char *file, const char *arg, ...);  
int execle(const char *path, const char *arg, ..., char *const envp[]);  
int execv(const char *path, char *const argv[]);  
int execvp(const char *file, char *const argv[]);  
int execve(const char *path, char *const argv[], char *const envp[]);  
```

**Différences :**
- `l` : Liste d'arguments
- `v` : Vecteur (tableau) d'arguments
- `p` : Cherche dans PATH
- `e` : Variables d'environnement personnalisées

**Exemple avec `execl` :**
```c
#include <unistd.h>
#include <stdio.h>

int main() {
    printf("Avant exec\n");

    execl("/bin/ls", "ls", "-l", NULL);

    // Si on arrive ici, c'est une erreur
    perror("execl");
    return 1;
}
```

**Exemple avec `fork` + `exec` :**
```c
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // Enfant exécute ls
        execl("/bin/ls", "ls", "-l", NULL);
        perror("execl");
        return 1;
    } else {
        // Parent attend
        wait(NULL);
        printf("Enfant terminé\n");
    }

    return 0;
}
```

---

### `wait()` / `waitpid()` - Attendre un processus enfant

```c
#include <sys/wait.h>

pid_t wait(int *status);  
pid_t waitpid(pid_t pid, int *status, int options);  
```

**Options `waitpid` :**
- `WNOHANG` : Non bloquant
- `WUNTRACED` : Retourner si enfant arrêté
- `WCONTINUED` : Retourner si enfant repris

**Macros de status :**
- `WIFEXITED(status)` : Terminé normalement ?
- `WEXITSTATUS(status)` : Code de retour
- `WIFSIGNALED(status)` : Tué par signal ?
- `WTERMSIG(status)` : Quel signal ?

**Exemple :**
```c
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        printf("Enfant travaille...\n");
        sleep(2);
        return 42;  // Code de retour
    } else {
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Enfant terminé avec code: %d\n", WEXITSTATUS(status));
        }
    }

    return 0;
}
```

---

### `getpid()` / `getppid()` - Obtenir les PID

```c
#include <unistd.h>

pid_t getpid(void);   // PID du processus actuel  
pid_t getppid(void);  // PID du processus parent  
```

**Exemple :**
```c
printf("Mon PID: %d\n", getpid());  
printf("PID de mon parent: %d\n", getppid());  
```

---

### `exit()` / `_exit()` - Terminer le processus

```c
#include <stdlib.h>
#include <unistd.h>

void exit(int status);  
void _exit(int status);  
```

**Différence :**
- `exit()` : Appelle les handlers `atexit()`, flush les buffers
- `_exit()` : Terminaison immédiate, pas de cleanup

**Exemple :**
```c
if (error) {
    exit(1);  // Terminaison avec erreur
}
exit(0);  // Terminaison normale
```

---

### `getuid()` / `geteuid()` - Obtenir l'UID

```c
#include <unistd.h>

uid_t getuid(void);   // UID réel  
uid_t geteuid(void);  // UID effectif  
gid_t getgid(void);   // GID réel  
gid_t getegid(void);  // GID effectif  
```

**Exemple :**
```c
printf("UID: %d, GID: %d\n", getuid(), getgid());
```

---

## ⚡ Appels système : Signaux

### `kill()` - Envoyer un signal

```c
#include <signal.h>

int kill(pid_t pid, int sig);
```

**Signaux courants :**
- `SIGTERM` (15) : Terminaison gracieuse
- `SIGKILL` (9) : Terminaison forcée (non interceptable)
- `SIGINT` (2) : Interruption (Ctrl+C)
- `SIGUSR1` / `SIGUSR2` : Signaux définis par l'utilisateur
- `SIGCHLD` : Enfant terminé
- `SIGSEGV` (11) : Segmentation fault

**Exemple :**
```c
// Envoyer SIGTERM au processus 1234
kill(1234, SIGTERM);

// Envoyer à tous les enfants
kill(0, SIGUSR1);
```

---

### `signal()` - Installer un handler de signal (simple)

```c
#include <signal.h>

typedef void (*sighandler_t)(int);  
sighandler_t signal(int signum, sighandler_t handler);  
```

**Handlers spéciaux :**
- `SIG_DFL` : Comportement par défaut
- `SIG_IGN` : Ignorer le signal

**Exemple :**
```c
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void handler(int sig) {
    printf("Signal %d reçu\n", sig);
}

int main() {
    signal(SIGINT, handler);

    printf("Appuyez sur Ctrl+C...\n");
    while (1) {
        sleep(1);
    }

    return 0;
}
```

**⚠️ Limitation :** `signal()` n'est pas portable. Préférer `sigaction()`.

---

### `sigaction()` - Installer un handler de signal (recommandé)

```c
#include <signal.h>

int sigaction(int signum, const struct sigaction *act,
              struct sigaction *oldact);
```

**Structure `sigaction` :**
```c
struct sigaction {
    void (*sa_handler)(int);            // Handler simple
    void (*sa_sigaction)(int, siginfo_t *, void *);  // Handler avancé
    sigset_t sa_mask;                   // Signaux à bloquer
    int sa_flags;                       // Flags
};
```

**Exemple :**
```c
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void handler(int sig) {
    const char *msg = "Signal reçu\n";
    write(STDOUT_FILENO, msg, strlen(msg));  // async-signal-safe
}

int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);

    printf("Appuyez sur Ctrl+C...\n");
    while (1) {
        sleep(1);
    }

    return 0;
}
```

---

### `pause()` - Attendre un signal

```c
#include <unistd.h>

int pause(void);
```

**Exemple :**
```c
printf("En attente d'un signal...\n");  
pause();  // Bloque jusqu'à réception d'un signal  
printf("Signal reçu !\n");  
```

---

### `alarm()` - Programmer un signal SIGALRM

```c
#include <unistd.h>

unsigned int alarm(unsigned int seconds);
```

**Exemple :**
```c
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

void alarm_handler(int sig) {
    printf("Temps écoulé !\n");
}

int main() {
    signal(SIGALRM, alarm_handler);
    alarm(5);  // SIGALRM dans 5 secondes

    pause();  // Attendre
    return 0;
}
```

---

## 💾 Appels système : Mémoire

### `brk()` / `sbrk()` - Modifier le segment de données

```c
#include <unistd.h>

int brk(void *addr);  
void *sbrk(intptr_t increment);  
```

**Note :** Rarement utilisés directement. `malloc()` les utilise en interne.

**Exemple :**
```c
void *old_brk = sbrk(0);  // Position actuelle  
void *new_brk = sbrk(4096);  // Allouer 4096 octets  

if (new_brk == (void *)-1) {
    perror("sbrk");
}
```

---

### `mmap()` / `munmap()` - Mapper de la mémoire

```c
#include <sys/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags,
           int fd, off_t offset);
int munmap(void *addr, size_t length);
```

**Protection (`prot`) :**
- `PROT_READ` : Lecture
- `PROT_WRITE` : Écriture
- `PROT_EXEC` : Exécution
- `PROT_NONE` : Aucun accès

**Flags :**
- `MAP_SHARED` : Partagé entre processus
- `MAP_PRIVATE` : Privé (copy-on-write)
- `MAP_ANONYMOUS` : Pas de fichier (mémoire pure)
- `MAP_FIXED` : Adresse exacte

**Exemple : Mémoire anonyme**
```c
#include <sys/mman.h>
#include <stdio.h>

int main() {
    size_t size = 4096;
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Utiliser la mémoire
    int *data = (int *)ptr;
    data[0] = 42;
    printf("data[0] = %d\n", data[0]);

    munmap(ptr, size);
    return 0;
}
```

**Exemple : Mapper un fichier**
```c
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>

int main() {
    int fd = open("file.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct stat st;
    fstat(fd, &st);

    char *mapped = mmap(NULL, st.st_size, PROT_READ,
                        MAP_PRIVATE, fd, 0);
    close(fd);  // Peut fermer après mmap

    if (mapped == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Lire le fichier en mémoire
    printf("%.*s", (int)st.st_size, mapped);

    munmap(mapped, st.st_size);
    return 0;
}
```

---

### `mprotect()` - Modifier les permissions mémoire

```c
#include <sys/mman.h>

int mprotect(void *addr, size_t len, int prot);
```

**Exemple :**
```c
// Rendre une zone read-only
mprotect(ptr, size, PROT_READ);

// Rendre exécutable
mprotect(ptr, size, PROT_READ | PROT_EXEC);
```

---

## 🕐 Appels système : Temps

### `time()` - Obtenir le temps actuel

```c
#include <time.h>

time_t time(time_t *tloc);
```

**Exemple :**
```c
time_t now = time(NULL);  
printf("Timestamp: %ld\n", now);  
```

---

### `gettimeofday()` - Temps avec microsecondes

```c
#include <sys/time.h>

int gettimeofday(struct timeval *tv, struct timezone *tz);
```

**Exemple :**
```c
struct timeval tv;  
gettimeofday(&tv, NULL);  
printf("Secondes: %ld, Microsecondes: %ld\n", tv.tv_sec, tv.tv_usec);  
```

---

### `clock_gettime()` - Horloge haute précision

```c
#include <time.h>

int clock_gettime(clockid_t clk_id, struct timespec *tp);
```

**Types d'horloge :**
- `CLOCK_REALTIME` : Temps réel (wall-clock)
- `CLOCK_MONOTONIC` : Monotone (ne recule jamais)
- `CLOCK_PROCESS_CPUTIME_ID` : CPU time du processus
- `CLOCK_THREAD_CPUTIME_ID` : CPU time du thread

**Exemple : Mesurer le temps**
```c
#include <time.h>
#include <stdio.h>

int main() {
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    // Code à mesurer
    for (int i = 0; i < 1000000; i++);

    clock_gettime(CLOCK_MONOTONIC, &end);

    long seconds = end.tv_sec - start.tv_sec;
    long nanoseconds = end.tv_nsec - start.tv_nsec;
    double elapsed = seconds + nanoseconds * 1e-9;

    printf("Temps: %.6f secondes\n", elapsed);
    return 0;
}
```

---

### `nanosleep()` - Dormir avec précision

```c
#include <time.h>

int nanosleep(const struct timespec *req, struct timespec *rem);
```

**Exemple :**
```c
struct timespec ts = {
    .tv_sec = 1,           // 1 seconde
    .tv_nsec = 500000000   // 500 millisecondes
};

nanosleep(&ts, NULL);  // Dormir 1.5 secondes
```

---

## 📂 Appels système : Répertoires

### `mkdir()` - Créer un répertoire

```c
#include <sys/stat.h>

int mkdir(const char *pathname, mode_t mode);
```

**Exemple :**
```c
if (mkdir("newdir", 0755) == -1) {
    perror("mkdir");
}
```

---

### `rmdir()` - Supprimer un répertoire (vide)

```c
#include <unistd.h>

int rmdir(const char *pathname);
```

---

### `opendir()` / `readdir()` / `closedir()` - Lire un répertoire

```c
#include <dirent.h>

DIR *opendir(const char *name);  
struct dirent *readdir(DIR *dirp);  
int closedir(DIR *dirp);  
```

**Structure `dirent` :**
```c
struct dirent {
    ino_t          d_ino;       // Inode number
    off_t          d_off;       // Offset
    unsigned short d_reclen;    // Record length
    unsigned char  d_type;      // Type (DT_REG, DT_DIR, etc.)
    char           d_name[256]; // Filename
};
```

**Exemple :**
```c
#include <dirent.h>
#include <stdio.h>

int main() {
    DIR *dir = opendir(".");
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s", entry->d_name);

        if (entry->d_type == DT_DIR)
            printf(" [DIR]");
        else if (entry->d_type == DT_REG)
            printf(" [FILE]");

        printf("\n");
    }

    closedir(dir);
    return 0;
}
```

---

### `chdir()` / `getcwd()` - Changer/obtenir le répertoire courant

```c
#include <unistd.h>

int chdir(const char *path);  
char *getcwd(char *buf, size_t size);  
```

**Exemple :**
```c
char cwd[PATH_MAX];  
getcwd(cwd, sizeof(cwd));  
printf("Répertoire actuel: %s\n", cwd);  

chdir("/tmp");  
getcwd(cwd, sizeof(cwd));  
printf("Nouveau répertoire: %s\n", cwd);  
```

---

## 🌐 Appels système : Réseau (Sockets)

### `socket()` - Créer un socket

```c
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```

**Domaines :**
- `AF_INET` : IPv4
- `AF_INET6` : IPv6
- `AF_UNIX` : Sockets locaux

**Types :**
- `SOCK_STREAM` : TCP (fiable, orienté connexion)
- `SOCK_DGRAM` : UDP (non fiable, sans connexion)
- `SOCK_RAW` : Accès direct au réseau

**Exemple :**
```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0);  
if (sockfd == -1) {  
    perror("socket");
    return 1;
}
```

---

### `bind()` - Lier un socket à une adresse

```c
#include <sys/socket.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

**Exemple :**
```c
#include <arpa/inet.h>

struct sockaddr_in addr;  
addr.sin_family = AF_INET;  
addr.sin_port = htons(8080);  
addr.sin_addr.s_addr = INADDR_ANY;  

if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("bind");
    return 1;
}
```

---

### `listen()` - Écouter les connexions

```c
#include <sys/socket.h>

int listen(int sockfd, int backlog);
```

**Exemple :**
```c
listen(sockfd, 5);  // Queue de 5 connexions max
```

---

### `accept()` - Accepter une connexion

```c
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

**Exemple :**
```c
struct sockaddr_in client_addr;  
socklen_t client_len = sizeof(client_addr);  

int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);  
if (client_fd == -1) {  
    perror("accept");
    return 1;
}

printf("Connexion depuis: %s:%d\n",
       inet_ntoa(client_addr.sin_addr),
       ntohs(client_addr.sin_port));
```

---

### `connect()` - Se connecter à un serveur

```c
#include <sys/socket.h>

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

**Exemple :**
```c
struct sockaddr_in server_addr;  
server_addr.sin_family = AF_INET;  
server_addr.sin_port = htons(8080);  
inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);  

if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    perror("connect");
    return 1;
}
```

---

### `send()` / `recv()` - Envoyer/recevoir sur socket

```c
#include <sys/socket.h>

ssize_t send(int sockfd, const void *buf, size_t len, int flags);  
ssize_t recv(int sockfd, void *buf, size_t len, int flags);  
```

**Exemple :**
```c
const char *msg = "Hello, server!";  
send(sockfd, msg, strlen(msg), 0);  

char buffer[1024];  
ssize_t bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);  
if (bytes > 0) {  
    buffer[bytes] = '\0';
    printf("Reçu: %s\n", buffer);
}
```

---

### `sendto()` / `recvfrom()` - UDP

```c
#include <sys/socket.h>

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);
```

---

### Exemple complet : Serveur TCP simple

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(1);
    }

    printf("Serveur en écoute sur le port %d...\n", PORT);

    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("accept");
        exit(1);
    }

    printf("Client connecté!\n");

    char buffer[1024] = {0};
    read(client_fd, buffer, 1024);
    printf("Message reçu: %s\n", buffer);

    const char *response = "Message bien reçu!";
    send(client_fd, response, strlen(response), 0);

    close(client_fd);
    close(server_fd);

    return 0;
}
```

---

## 📊 Tableau récapitulatif par catégorie

### Fichiers et I/O

| Appel système | Description | Man page |
|---------------|-------------|----------|
| `open()` | Ouvrir un fichier | `man 2 open` |
| `close()` | Fermer un fichier | `man 2 close` |
| `read()` | Lire des données | `man 2 read` |
| `write()` | Écrire des données | `man 2 write` |
| `lseek()` | Déplacer le curseur | `man 2 lseek` |
| `dup()` / `dup2()` | Dupliquer un fd | `man 2 dup` |
| `pipe()` | Créer un pipe | `man 2 pipe` |
| `stat()` / `fstat()` | Info sur fichier | `man 2 stat` |
| `access()` | Vérifier permissions | `man 2 access` |

### Processus

| Appel système | Description | Man page |
|---------------|-------------|----------|
| `fork()` | Créer un processus | `man 2 fork` |
| `exec*()` | Remplacer le processus | `man 3 exec` |
| `wait()` / `waitpid()` | Attendre un enfant | `man 2 wait` |
| `getpid()` | PID actuel | `man 2 getpid` |
| `getppid()` | PID du parent | `man 2 getppid` |
| `exit()` / `_exit()` | Terminer | `man 3 exit` |

### Signaux

| Appel système | Description | Man page |
|---------------|-------------|----------|
| `kill()` | Envoyer un signal | `man 2 kill` |
| `signal()` | Handler simple | `man 2 signal` |
| `sigaction()` | Handler avancé | `man 2 sigaction` |
| `pause()` | Attendre un signal | `man 2 pause` |
| `alarm()` | Programmer SIGALRM | `man 2 alarm` |

### Mémoire

| Appel système | Description | Man page |
|---------------|-------------|----------|
| `brk()` / `sbrk()` | Modifier le heap | `man 2 brk` |
| `mmap()` | Mapper de la mémoire | `man 2 mmap` |
| `munmap()` | Démapper | `man 2 munmap` |
| `mprotect()` | Changer permissions | `man 2 mprotect` |

### Temps

| Appel système | Description | Man page |
|---------------|-------------|----------|
| `time()` | Temps actuel | `man 2 time` |
| `gettimeofday()` | Avec microsecondes | `man 2 gettimeofday` |
| `clock_gettime()` | Haute précision | `man 2 clock_gettime` |
| `nanosleep()` | Dormir | `man 2 nanosleep` |

### Répertoires

| Appel système | Description | Man page |
|---------------|-------------|----------|
| `mkdir()` | Créer répertoire | `man 2 mkdir` |
| `rmdir()` | Supprimer répertoire | `man 2 rmdir` |
| `opendir()` | Ouvrir répertoire | `man 3 opendir` |
| `readdir()` | Lire entrée | `man 3 readdir` |
| `chdir()` | Changer de répertoire | `man 2 chdir` |
| `getcwd()` | Répertoire actuel | `man 3 getcwd` |

### Réseau (Sockets)

| Appel système | Description | Man page |
|---------------|-------------|----------|
| `socket()` | Créer un socket | `man 2 socket` |
| `bind()` | Lier à une adresse | `man 2 bind` |
| `listen()` | Écouter | `man 2 listen` |
| `accept()` | Accepter connexion | `man 2 accept` |
| `connect()` | Se connecter | `man 2 connect` |
| `send()` / `recv()` | Envoyer/recevoir | `man 2 send` |
| `sendto()` / `recvfrom()` | UDP | `man 2 sendto` |

---

## 🎯 Patterns courants

### Pattern : Copier un fichier

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int copy_file(const char *src, const char *dest) {
    int fd_src = open(src, O_RDONLY);
    if (fd_src == -1) {
        perror("open source");
        return -1;
    }

    int fd_dest = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dest == -1) {
        perror("open dest");
        close(fd_src);
        return -1;
    }

    char buffer[4096];
    ssize_t bytes;

    while ((bytes = read(fd_src, buffer, sizeof(buffer))) > 0) {
        if (write(fd_dest, buffer, bytes) != bytes) {
            perror("write");
            close(fd_src);
            close(fd_dest);
            return -1;
        }
    }

    if (bytes == -1) {
        perror("read");
    }

    close(fd_src);
    close(fd_dest);
    return bytes == -1 ? -1 : 0;
}
```

---

### Pattern : Exécuter une commande et capturer la sortie

```c
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

int main() {
    int pipefd[2];
    char buffer[1024];

    pipe(pipefd);

    pid_t pid = fork();
    if (pid == 0) {
        // Enfant
        close(pipefd[0]);  // Fermer lecture
        dup2(pipefd[1], STDOUT_FILENO);  // Rediriger stdout
        close(pipefd[1]);

        execlp("ls", "ls", "-l", NULL);
        perror("execlp");
        return 1;
    } else {
        // Parent
        close(pipefd[1]);  // Fermer écriture

        ssize_t bytes;
        while ((bytes = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes] = '\0';
            printf("%s", buffer);
        }

        close(pipefd[0]);
        wait(NULL);
    }

    return 0;
}
```

---

### Pattern : Daemon (processus en arrière-plan)

```c
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

void daemonize() {
    // 1. Fork et terminer le parent
    pid_t pid = fork();
    if (pid < 0) exit(1);
    if (pid > 0) exit(0);  // Parent se termine

    // 2. Créer une nouvelle session
    if (setsid() < 0) exit(1);

    // 3. Fork à nouveau pour ne pas être session leader
    pid = fork();
    if (pid < 0) exit(1);
    if (pid > 0) exit(0);

    // 4. Changer le répertoire courant
    chdir("/");

    // 5. Fermer les descripteurs standard
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // 6. Rediriger vers /dev/null
    open("/dev/null", O_RDONLY);  // stdin
    open("/dev/null", O_WRONLY);  // stdout
    open("/dev/null", O_WRONLY);  // stderr

    // 7. Définir umask
    umask(0);
}

int main() {
    daemonize();

    // Code du daemon
    while (1) {
        // Faire quelque chose...
        sleep(60);
    }

    return 0;
}
```

---

## 💡 Bonnes pratiques

### 1. Toujours vérifier les retours

```c
// ❌ Mauvais
int fd = open("file.txt", O_RDONLY);  
read(fd, buffer, 100);  

// ✅ Bon
int fd = open("file.txt", O_RDONLY);  
if (fd == -1) {  
    perror("open");
    return 1;
}

ssize_t bytes = read(fd, buffer, 100);  
if (bytes == -1) {  
    perror("read");
    close(fd);
    return 1;
}
```

---

### 2. Fermer les descripteurs

```c
// ❌ Mauvais
int fd = open("file.txt", O_RDONLY);
// ... travail ...
return 0;  // fd pas fermé = fuite de ressource

// ✅ Bon
int fd = open("file.txt", O_RDONLY);  
if (fd == -1) return 1;  
// ... travail ...
close(fd);  
return 0;  
```

---

### 3. Gérer les interruptions (EINTR)

```c
// ❌ Peut échouer si signal reçu
ssize_t bytes = read(fd, buffer, size);

// ✅ Réessayer si interrompu
ssize_t bytes;  
do {  
    bytes = read(fd, buffer, size);
} while (bytes == -1 && errno == EINTR);
```

---

### 4. Vérifier les retours partiels

```c
// ❌ Suppose que tout est écrit
write(fd, buffer, size);

// ✅ Vérifier que tout est écrit
ssize_t total = 0;  
while (total < size) {  
    ssize_t written = write(fd, buffer + total, size - total);
    if (written == -1) {
        if (errno == EINTR) continue;
        perror("write");
        return -1;
    }
    total += written;
}
```

---

## 🎓 Checklist : Appels système à connaître

### ✅ Niveau Débutant
- [ ] `open()` / `close()` : Fichiers
- [ ] `read()` / `write()` : I/O
- [ ] `fork()` : Créer processus
- [ ] `wait()` / `waitpid()` : Attendre enfant
- [ ] `exec*()` : Remplacer processus

### ✅ Niveau Intermédiaire
- [ ] `pipe()` : Communication inter-processus
- [ ] `dup2()` : Redirection
- [ ] `signal()` / `sigaction()` : Signaux
- [ ] `kill()` : Envoyer signaux
- [ ] `stat()` : Info fichiers

### ✅ Niveau Avancé
- [ ] `mmap()` / `munmap()` : Mémoire mappée
- [ ] `socket()` / `bind()` / `listen()` : Réseau
- [ ] `select()` / `poll()` / `epoll()` : I/O multiplexing
- [ ] `pthread_*()` : Threads POSIX
- [ ] `clock_gettime()` : Mesures précises

---

## 📝 Aide-mémoire rapide

```c
// Fichiers
int fd = open("file.txt", O_RDONLY);  
read(fd, buffer, size);  
write(fd, buffer, size);  
lseek(fd, 0, SEEK_SET);  
close(fd);  

// Processus
pid_t pid = fork();  
if (pid == 0) {  
    // Enfant
    execl("/bin/ls", "ls", "-l", NULL);
} else {
    // Parent
    wait(NULL);
}

// Signaux
signal(SIGINT, handler);  
kill(pid, SIGTERM);  

// Sockets
int sock = socket(AF_INET, SOCK_STREAM, 0);  
bind(sock, (struct sockaddr *)&addr, sizeof(addr));  
listen(sock, 5);  
int client = accept(sock, NULL, NULL);  
send(client, buffer, size, 0);  
recv(client, buffer, size, 0);  
```

---

## 🔗 Pour aller plus loin

### Documentation

```bash
# Man pages section 2 (appels système)
man 2 open  
man 2 fork  
man 2 signal  

# Lister tous les appels système
man syscalls
```

### Ressources en ligne

- **Linux man pages** : [https://man7.org/linux/man-pages/](https://man7.org/linux/man-pages/)
- **The Linux Programming Interface** (livre de Michael Kerrisk)
- **Advanced Programming in UNIX Environment** (livre de Stevens)

---

## 📝 Conclusion

Les appels système sont le **pont entre votre code et le système d'exploitation**. Maîtriser les appels système essentiels est crucial pour :

- ✅ Comprendre comment fonctionne vraiment le système
- ✅ Écrire du code système performant
- ✅ Débugger efficacement
- ✅ Développer des applications réseau, multi-processus, etc.

**Commencez par les bases** (fichiers, processus) puis progressez vers les sujets avancés (signaux, mémoire, réseau).

**N'oubliez pas :** Chaque appel système a sa man page détaillée. `man 2 <syscall>` est votre meilleur ami !

---


⏭️ [POSIX threads API](/annexes/antiseches/04-posix-threads-api.md)
