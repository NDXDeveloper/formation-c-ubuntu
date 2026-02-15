🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 16.5 I/O bufferisé vs non bufferisé

## Introduction

Quand votre programme lit ou écrit des données, ces opérations peuvent être **bufferisées** (avec un tampon en mémoire) ou **non bufferisées** (directement vers le disque/périphérique). Cette différence a un impact majeur sur les performances et le comportement de votre programme.

> **💡 Analogie :** Imaginez que vous devez transporter des briques d'un endroit à un autre :
> - **Non bufferisé** : Vous portez une brique à la fois. C'est simple, mais vous faites beaucoup d'allers-retours (lent).
> - **Bufferisé** : Vous utilisez une brouette pour transporter plusieurs briques à la fois. Un peu plus complexe, mais beaucoup plus efficace (rapide).

## Qu'est-ce que le buffering ?

Le **buffering** (mise en tampon) consiste à stocker temporairement des données en **mémoire RAM** avant de les transférer vers leur destination finale (disque, réseau, écran, etc.).

### Schéma conceptuel

```
┌─────────────────────────────────────────────────────────┐
│                    VOTRE PROGRAMME                      │
└──────────────┬──────────────────────────────────────────┘
               │
               ▼
       ┌───────────────┐
       │  Écriture     │
       └───────┬───────┘
               │
    ┌──────────┴──────────┐
    │                     │
    ▼                     ▼
┌─────────────────┐  ┌─────────────────┐
│  NON BUFFERISÉ  │  │   BUFFERISÉ     │
│                 │  │                 │
│  write()        │  │  fprintf()      │
│  read()         │  │  fwrite()       │
│      │          │  │      │          │
│      ▼          │  │      ▼          │
│  [Syscall]      │  │  [Buffer RAM]   │
│      │          │  │      │          │
│      ▼          │  │      ▼ (quand   │
│   KERNEL        │  │   plein/flush)  │
│      │          │  │  [Syscall]      │
│      ▼          │  │      │          │
│   DISQUE        │  │      ▼          │
│                 │  │   KERNEL        │
│   LENT          │  │      │          │
│   (1000+ µs)    │  │      ▼          │
│                 │  │   DISQUE        │
│                 │  │                 │
│                 │  │   RAPIDE        │
│                 │  │   (moins de     │
│                 │  │   syscalls)     │
└─────────────────┘  └─────────────────┘
```

## I/O non bufferisé (Appels système)

### Caractéristiques

L'I/O non bufferisé utilise les **appels système** directement : `open()`, `read()`, `write()`, `close()`.

**Caractéristiques principales :**
- ✅ Chaque opération = 1 appel système
- ✅ Contrôle précis du timing
- ✅ Données écrites immédiatement (ou presque)
- ⚠️ Overhead important pour petites opérations
- ⚠️ Performances réduites avec beaucoup d'opérations

### Exemple basique

```c
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    // Chaque write() = 1 appel système
    write(fd, "Ligne 1\n", 8);  // Syscall #1
    write(fd, "Ligne 2\n", 8);  // Syscall #2
    write(fd, "Ligne 3\n", 8);  // Syscall #3

    // Total : 3 appels système
    close(fd);
    return 0;
}
```

**Analyse du comportement :**
```
write() → [Syscall] → Kernel → Disque
  ↓ (attente ~100-1000 µs)
write() → [Syscall] → Kernel → Disque
  ↓ (attente ~100-1000 µs)
write() → [Syscall] → Kernel → Disque
  ↓ (attente ~100-1000 µs)
```

**Overhead total :** ~300-3000 microsecondes juste pour les syscalls !

### Quand les données sont-elles réellement écrites ?

Même avec les appels système "non bufferisés", le **noyau Linux** a ses propres buffers :

```
write(fd, data, size)
       ↓
[Syscall rapide]
       ↓
Kernel page cache ← Les données sont ICI
       ↓ (quelques secondes plus tard)
Disque physique
```

**Important :** `write()` retourne dès que les données sont dans le cache noyau, pas sur le disque !

#### Forcer l'écriture physique immédiate

```c
#include <unistd.h>

int fd = open("file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);  
write(fd, data, size);  

// Forcer l'écriture sur disque
fsync(fd);  // Bloque jusqu'à ce que les données soient physiquement écrites

close(fd);
```

**Ou avec un flag d'ouverture :**
```c
// O_SYNC : Chaque write() attend l'écriture physique (TRÈS LENT)
int fd = open("file.txt", O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, 0644);  
write(fd, data, size);  // Bloque jusqu'à l'écriture sur disque  
```

## I/O bufferisé (Bibliothèque C)

### Caractéristiques

L'I/O bufferisé utilise la **bibliothèque C standard** : `fopen()`, `fread()`, `fwrite()`, `fprintf()`, `fclose()`.

**Caractéristiques principales :**
- ✅ Buffer automatique en RAM (4-8 Ko typiquement)
- ✅ Réduit drastiquement le nombre de syscalls
- ✅ Performances excellentes pour petites opérations fréquentes
- ✅ Fonctions pratiques de formatage (`fprintf()`, `fscanf()`)
- ⚠️ Données pas immédiatement sur disque
- ⚠️ Risque de perte en cas de crash

### Exemple basique

```c
#include <stdio.h>

int main(void) {
    FILE *fp = fopen("output.txt", "w");

    // Ces écritures vont dans le buffer, PAS directement sur disque
    fprintf(fp, "Ligne 1\n");  // → Buffer
    fprintf(fp, "Ligne 2\n");  // → Buffer
    fprintf(fp, "Ligne 3\n");  // → Buffer

    // Le buffer est vidé automatiquement à la fermeture
    fclose(fp);  // ICI : 1 seul appel système write()

    return 0;
}
```

**Analyse du comportement :**
```
fprintf() → [Buffer RAM] (rapide, ~1-10 ns)  
fprintf() → [Buffer RAM] (rapide, ~1-10 ns)  
fprintf() → [Buffer RAM] (rapide, ~1-10 ns)  
    ↓
fclose() → [Buffer plein ou fermeture]
    ↓
write() → [1 Syscall] → Kernel → Disque
```

**Overhead total :** ~100-1000 microsecondes pour UN SEUL syscall au lieu de trois !

### Structure du buffer

```c
// Simplifié : structure FILE interne (opaque)
struct _IO_FILE {
    char *buffer;        // Pointeur vers le buffer
    size_t buffer_size;  // Taille du buffer (ex: 8192)
    size_t buffer_pos;   // Position actuelle dans le buffer
    int fd;              // Descripteur de fichier sous-jacent
    // ... autres champs
};
```

**Visualisation :**
```
Buffer (8192 octets) :
┌────────────────────────────────────────────┐
│ Ligne 1\n Ligne 2\n Ligne 3\n              │ ← buffer_pos = 24
│ [0-23: données] [24-8191: vide]            │
└────────────────────────────────────────────┘
        ↑
    Sera écrit sur disque quand :
    1. Le buffer est plein
    2. fflush() est appelé
    3. fclose() est appelé
    4. Un '\n' est rencontré (si line buffered)
```

## Les trois types de buffering

La bibliothèque C propose **trois modes de buffering** :

### 1. Fully Buffered (Buffering complet)

**Comportement :** Les données sont écrites sur disque uniquement quand le buffer est plein.

**Utilisé pour :** Fichiers réguliers ouverts en lecture/écriture.

**Taille typique :** 4096 ou 8192 octets (1-2 pages mémoire).

```c
#include <stdio.h>

FILE *fp = fopen("data.txt", "w");
// Buffering complet par défaut pour les fichiers

for (int i = 0; i < 10000; i++) {
    fprintf(fp, "Ligne %d\n", i);
    // Chaque fprintf() remplit le buffer
    // Syscall seulement quand le buffer est plein
}

fclose(fp);
```

**Illustration :**
```
fprintf() → Buffer [████████░░░░░░░░░░] 40%  
fprintf() → Buffer [████████████░░░░░░] 60%  
fprintf() → Buffer [████████████████░░] 80%  
fprintf() → Buffer [████████████████████] 100% → [Syscall write()]  
fprintf() → Buffer [████░░░░░░░░░░░░░░░] 20% (nouveau buffer)  
```

### 2. Line Buffered (Buffering ligne par ligne)

**Comportement :** Les données sont écrites quand un `\n` (newline) est rencontré.

**Utilisé pour :** Terminaux (stdout connecté à un terminal).

```c
#include <stdio.h>

int main(void) {
    // stdout est line buffered par défaut (si terminal)
    printf("Bonjour");     // → Buffer (pas encore écrit)
    printf(" monde");      // → Buffer (toujours pas écrit)
    printf("!\n");         // → '\n' détecté → Syscall write() ici !

    printf("Autre ligne"); // → Buffer
    fflush(stdout);        // → Force l'écriture (sans attendre \n)

    return 0;
}
```

**Exemple concret :**
```c
printf("Entrez votre nom : ");  // Pas de \n → reste dans buffer
// L'utilisateur ne voit rien !

printf("Entrez votre nom : \n"); // Avec \n → immédiatement affiché
// Ou :
printf("Entrez votre nom : ");  
fflush(stdout);  // Force l'affichage  
```

### 3. Unbuffered (Sans buffering)

**Comportement :** Chaque opération d'écriture est immédiatement transférée (syscall).

**Utilisé pour :** stderr (pour voir les erreurs immédiatement, même en cas de crash).

```c
#include <stdio.h>

int main(void) {
    // stderr est unbuffered par défaut
    fprintf(stderr, "ERREUR: ");      // → Syscall immédiat
    fprintf(stderr, "Fichier introuvable\n"); // → Syscall immédiat

    // Même sans \n, c'est écrit immédiatement :
    fprintf(stderr, "Debug");  // → Visible tout de suite

    return 0;
}
```

**Pourquoi stderr est unbuffered ?**
Si le programme crash, les messages d'erreur bufferisés seraient perdus. Avec unbuffered, ils sont toujours écrits immédiatement.

### Tableau récapitulatif

| Type | Quand le buffer est vidé ? | Utilisé pour | Exemple |
|------|---------------------------|--------------|---------|
| **Fully Buffered** | Quand le buffer est plein | Fichiers normaux | `fopen("file.txt", "w")` |
| **Line Buffered** | À chaque `\n` ou quand plein | Terminaux (stdout) | `printf()` vers terminal |
| **Unbuffered** | Immédiatement | Flux d'erreur (stderr) | `fprintf(stderr, ...)` |

## Contrôler le buffering

### Déterminer le type de buffering

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>

int main(void) {
    FILE *fp = fopen("test.txt", "w");

    // Le type de buffering dépend du type de fichier
    if (isatty(fileno(fp))) {
        printf("Buffering ligne par ligne (terminal)\n");
    } else {
        printf("Buffering complet (fichier)\n");
    }

    fclose(fp);
    return 0;
}
```

### Changer le type de buffering avec `setvbuf()`

```c
#include <stdio.h>

int setvbuf(FILE *stream, char *buf, int mode, size_t size);
```

**Modes disponibles :**
- `_IOFBF` : Fully buffered
- `_IOLBF` : Line buffered
- `_IONBF` : Unbuffered

#### Désactiver complètement le buffering

```c
#include <stdio.h>

int main(void) {
    FILE *fp = fopen("log.txt", "w");

    // Désactiver le buffering
    setvbuf(fp, NULL, _IONBF, 0);

    // Maintenant chaque fprintf() est écrit immédiatement
    fprintf(fp, "Log 1\n");  // → Syscall immédiat
    fprintf(fp, "Log 2\n");  // → Syscall immédiat

    fclose(fp);
    return 0;
}
```

**Équivalent avec `setbuf()` (plus simple) :**
```c
FILE *fp = fopen("log.txt", "w");  
setbuf(fp, NULL);  // Désactive le buffering  
```

#### Activer le buffering complet

```c
FILE *fp = fopen("output.txt", "w");

// Buffer de 16 Ko
char buffer[16384];  
setvbuf(fp, buffer, _IOFBF, sizeof(buffer));  

// Maintenant les écritures utilisent ce buffer
fprintf(fp, "...");
```

**⚠️ Attention :** Le buffer doit rester valide tant que le fichier est ouvert !

#### Forcer le line buffering

```c
FILE *fp = fopen("output.txt", "w");

// Activer le line buffering
setvbuf(fp, NULL, _IOLBF, 0);

fprintf(fp, "Ligne 1");   // → Buffer  
fprintf(fp, "\n");        // → Écrit immédiatement  
```

### Vider le buffer manuellement avec `fflush()`

```c
#include <stdio.h>

int fflush(FILE *stream);
```

**Usage :**
```c
FILE *fp = fopen("data.txt", "w");

fprintf(fp, "Données importantes\n");  
fflush(fp);  // ✅ Force l'écriture immédiate sur disque  

fprintf(fp, "Autres données\n");
// Reste dans le buffer jusqu'à fclose() ou fflush()

fclose(fp);
```

**Vider tous les buffers ouverts :**
```c
fflush(NULL);  // Vide tous les fichiers ouverts en écriture
```

### Exemple : Log critique avec flush

```c
#include <stdio.h>
#include <time.h>
#include <string.h>

void log_message(const char *msg) {
    static FILE *log_fp = NULL;

    if (log_fp == NULL) {
        log_fp = fopen("app.log", "a");
        if (log_fp == NULL) return;
    }

    time_t now = time(NULL);
    char *timestr = ctime(&now);
    timestr[strlen(timestr) - 1] = '\0';  // Enlever le \n de ctime()
    fprintf(log_fp, "[%s] %s\n", timestr, msg);

    // Force l'écriture immédiate (en cas de crash)
    fflush(log_fp);
}

int main(void) {
    log_message("Application démarrée");
    // ... code ...
    log_message("Traitement terminé");

    return 0;
}
```

## Impact sur les performances

### Benchmark : Écriture de 10 000 lignes

```c
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define ITERATIONS 10000

double get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

void test_unbuffered(void) {
    int fd = open("unbuffered.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    for (int i = 0; i < ITERATIONS; i++) {
        char buf[64];
        int len = snprintf(buf, sizeof(buf), "Ligne %d\n", i);
        write(fd, buf, len);
    }

    close(fd);
}

void test_buffered(void) {
    FILE *fp = fopen("buffered.txt", "w");

    for (int i = 0; i < ITERATIONS; i++) {
        fprintf(fp, "Ligne %d\n", i);
    }

    fclose(fp);
}

void test_buffered_fflush(void) {
    FILE *fp = fopen("buffered_flush.txt", "w");

    for (int i = 0; i < ITERATIONS; i++) {
        fprintf(fp, "Ligne %d\n", i);
        fflush(fp);  // Force l'écriture à chaque ligne
    }

    fclose(fp);
}

void test_no_buffering(void) {
    FILE *fp = fopen("no_buffer.txt", "w");
    setbuf(fp, NULL);  // Désactiver le buffering

    for (int i = 0; i < ITERATIONS; i++) {
        fprintf(fp, "Ligne %d\n", i);
    }

    fclose(fp);
}

int main(void) {
    double start, end;

    printf("Benchmark avec %d itérations\n\n", ITERATIONS);

    // Test 1 : Non bufferisé (appels système)
    start = get_time_ms();
    test_unbuffered();
    end = get_time_ms();
    printf("1. write() non bufferisé : %.2f ms\n", end - start);

    // Test 2 : Bufferisé (bibliothèque C)
    start = get_time_ms();
    test_buffered();
    end = get_time_ms();
    printf("2. fprintf() bufferisé   : %.2f ms\n", end - start);

    // Test 3 : Bufferisé avec fflush à chaque ligne
    start = get_time_ms();
    test_buffered_fflush();
    end = get_time_ms();
    printf("3. fprintf() + fflush()  : %.2f ms\n", end - start);

    // Test 4 : Sans buffering via setbuf()
    start = get_time_ms();
    test_no_buffering();
    end = get_time_ms();
    printf("4. fprintf() sans buffer : %.2f ms\n", end - start);

    return 0;
}
```

**Résultats typiques :**
```
Benchmark avec 10000 itérations

1. write() non bufferisé : 385.23 ms
2. fprintf() bufferisé   :   8.47 ms  ← 45x plus rapide !
3. fprintf() + fflush()  : 392.18 ms  ← Aussi lent que write()
4. fprintf() sans buffer : 388.91 ms  ← Pas de gain
```

**Analyse :**
- Bufferisé = **~45x plus rapide** pour petites écritures fréquentes
- fflush() après chaque écriture = annule le bénéfice du buffering
- Désactiver le buffering = performances équivalentes aux appels système

### Visualisation de l'impact

```
Non bufferisé (10 000 écritures) :  
write() → [Syscall] → Kernel (100 µs)  
write() → [Syscall] → Kernel (100 µs)  
... (10 000 fois)
Total : ~1 000 000 µs = 1 seconde

Bufferisé (10 000 écritures) :  
fprintf() → Buffer (1 ns)  
fprintf() → Buffer (1 ns)  
... (10 000 fois dans buffer)
fclose() → [1 Syscall] → Kernel (100 µs)  
Total : ~10 000 ns + 100 µs ≈ 110 µs  

Gain : ~9000x plus rapide !
```

## Cas d'usage appropriés

### Utilisez I/O bufferisé (FILE*) pour :

✅ **Fichiers texte avec beaucoup de petites écritures**
```c
FILE *fp = fopen("rapport.txt", "w");  
for (int i = 0; i < 10000; i++) {  
    fprintf(fp, "Ligne %d: données...\n", i);
}
fclose(fp);
```

✅ **Formatage de données**
```c
fprintf(fp, "Nom: %s, Age: %d, Score: %.2f\n", nom, age, score);
// Beaucoup plus simple qu'avec write()
```

✅ **Lecture ligne par ligne**
```c
char ligne[256];  
while (fgets(ligne, sizeof(ligne), fp) != NULL) {  
    // Traiter chaque ligne
}
```

✅ **Portabilité maximale**
```c
// Code qui doit fonctionner sur Linux, macOS, Windows
```

### Utilisez I/O non bufferisé (fd) pour :

✅ **Gros transferts de données**
```c
// Copier un fichier de 1 Go
char buffer[1024 * 1024];  // Buffer de 1 Mo  
while ((n = read(fd_in, buffer, sizeof(buffer))) > 0) {  
    write(fd_out, buffer, n);
}
```

✅ **Contrôle précis du timing**
```c
// Communication en temps réel, protocoles réseau
int sock = socket(...);  
write(sock, data, size);  // Envoi immédiat  
```

✅ **Programmation système bas niveau**
```c
// Sockets, pipes, fichiers spéciaux
int pipe_fd[2];  
pipe(pipe_fd);  
write(pipe_fd[1], data, size);  
```

✅ **Opérations non-standard**
```c
// Utilisation de flags spéciaux
int fd = open("file", O_RDONLY | O_DIRECT | O_NOATIME);
```

### Utilisez fflush() pour :

✅ **Logs critiques**
```c
fprintf(log_fp, "ERREUR CRITIQUE: %s\n", message);  
fflush(log_fp);  // En cas de crash, le log est écrit  
```

✅ **Debugging interactif**
```c
printf("Valeur de x = %d", x);  
fflush(stdout);  // Affichage immédiat sans \n  
```

✅ **Avant fork() ou exec()**
```c
printf("Avant fork...");  
fflush(stdout);  // Évite la duplication du buffer  
pid_t pid = fork();  
```

✅ **Communication inter-processus via fichiers**
```c
fprintf(shared_file, "message");  
fflush(shared_file);  // L'autre processus peut lire immédiatement  
```

## Pièges courants

### 1. Oublier de flusher avant un crash potentiel

```c
// ❌ DANGEREUX
FILE *log = fopen("app.log", "a");  
fprintf(log, "Opération critique commencée\n");  
// Si crash ici → log perdu !
operation_dangereuse();

// ✅ SÉCURISÉ
FILE *log = fopen("app.log", "a");  
fprintf(log, "Opération critique commencée\n");  
fflush(log);  // Ou : setbuf(log, NULL);  
operation_dangereuse();  
```

### 2. Mélanger I/O bufferisé et non bufferisé

```c
// ❌ PROBLÉMATIQUE
FILE *fp = fopen("file.txt", "r+");  
int fd = fileno(fp);  

fgets(line, sizeof(line), fp);  // Lit avec buffer  
read(fd, buf, size);            // Lit sans buffer → désynchronisé !  
```

**Solution :** Utilisez `fflush()` avant de passer à l'autre niveau :
```c
fgets(line, sizeof(line), fp);  
fflush(fp);  // Synchronise  
read(fd, buf, size);  // OK maintenant  
```

### 3. Buffer sur la pile qui disparaît

```c
// ❌ ERREUR
void mauvaise_fonction(void) {
    FILE *fp = fopen("file.txt", "w");
    char buffer[8192];
    setvbuf(fp, buffer, _IOFBF, sizeof(buffer));
    // ...
}  // buffer est détruit ici, mais fp l'utilise encore !

// ✅ CORRECT
static char buffer[8192];  // ou malloc()  
void bonne_fonction(void) {  
    FILE *fp = fopen("file.txt", "w");
    setvbuf(fp, buffer, _IOFBF, sizeof(buffer));
    // ...
}
```

### 4. Supposer que fclose() ne peut pas échouer

```c
// ❌ INCOMPLET
FILE *fp = fopen("file.txt", "w");  
fprintf(fp, "données importantes");  
fclose(fp);  // Peut échouer (disque plein, etc.)  

// ✅ COMPLET
FILE *fp = fopen("file.txt", "w");  
fprintf(fp, "données importantes");  
if (fclose(fp) == EOF) {  
    perror("fclose");  // Les données n'ont peut-être pas été écrites !
    return -1;
}
```

### 5. Utiliser printf() dans du code critique en performance

```c
// ❌ LENT pour logs intensifs
for (int i = 0; i < 1000000; i++) {
    printf("Traitement %d\n", i);  // Buffering ligne → beaucoup de syscalls
}

// ✅ RAPIDE
FILE *fp = fopen("log.txt", "w");  
for (int i = 0; i < 1000000; i++) {  
    fprintf(fp, "Traitement %d\n", i);  // Buffering complet
}
fclose(fp);
```

## Buffering et sécurité

### Risque de perte de données

**Scénario :**
```c
FILE *fp = fopen("transactions.log", "a");

fprintf(fp, "Transaction: +1000€ sur compte A\n");
// Données dans le buffer, pas sur disque

// CRASH ICI (coupure électrique, kill -9, segfault)
// → Log perdu !
```

**Solutions :**

1. **Utiliser fflush() après chaque opération critique**
```c
fprintf(fp, "Transaction: +1000€ sur compte A\n");  
fflush(fp);  // Force l'écriture  
```

2. **Désactiver le buffering pour les logs critiques**
```c
FILE *fp = fopen("transactions.log", "a");  
setbuf(fp, NULL);  // Unbuffered  
fprintf(fp, "Transaction: +1000€\n");  // Écrit immédiatement  
```

3. **Utiliser fsync() pour garantir l'écriture physique**
```c
fprintf(fp, "Transaction: +1000€\n");  
fflush(fp);                // Vide le buffer stdio  
fsync(fileno(fp));         // Force l'écriture sur disque  
```

### Buffering et multiprocessing

**Problème avec fork() :**
```c
printf("Avant fork");  // Reste dans le buffer  
pid_t pid = fork();  
// Le buffer est dupliqué → "Avant fork" sera affiché 2 fois !
```

**Solution :**
```c
printf("Avant fork");  
fflush(stdout);  // Vide le buffer avant fork()  
pid_t pid = fork();  
```

## Exemple complet : Logger flexible

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

typedef enum {
    LOG_BUFFERED,    // Performance maximale
    LOG_LINE,        // Balance entre performance et sécurité
    LOG_IMMEDIATE    // Sécurité maximale (unbuffered)
} log_mode_t;

typedef struct {
    FILE *fp;
    log_mode_t mode;
} logger_t;

logger_t* logger_create(const char *filename, log_mode_t mode) {
    logger_t *logger = malloc(sizeof(logger_t));
    if (!logger) return NULL;

    logger->fp = fopen(filename, "a");
    if (!logger->fp) {
        free(logger);
        return NULL;
    }

    logger->mode = mode;

    // Configurer le buffering selon le mode
    switch (mode) {
        case LOG_BUFFERED:
            // Par défaut : fully buffered (rapide)
            break;
        case LOG_LINE:
            setvbuf(logger->fp, NULL, _IOLBF, 0);
            break;
        case LOG_IMMEDIATE:
            setbuf(logger->fp, NULL);  // Unbuffered
            break;
    }

    return logger;
}

void logger_log(logger_t *logger, const char *level, const char *fmt, ...) {
    if (!logger || !logger->fp) return;

    // Timestamp
    time_t now = time(NULL);
    char *timestr = ctime(&now);
    timestr[strlen(timestr) - 1] = '\0';  // Enlever le \n

    // Écrire l'en-tête
    fprintf(logger->fp, "[%s] [%s] ", timestr, level);

    // Écrire le message avec arguments variables
    va_list args;
    va_start(args, fmt);
    vfprintf(logger->fp, fmt, args);
    va_end(args);

    fprintf(logger->fp, "\n");

    // Flush si mode LINE (déjà fait par \n) ou IMMEDIATE (déjà unbuffered)
    // Rien à faire, c'est automatique !
}

void logger_destroy(logger_t *logger) {
    if (logger) {
        if (logger->fp) fclose(logger->fp);
        free(logger);
    }
}

int main(void) {
    // Logger haute performance (bufferisé)
    logger_t *fast_log = logger_create("fast.log", LOG_BUFFERED);

    // Logger équilibré (line buffered)
    logger_t *normal_log = logger_create("normal.log", LOG_LINE);

    // Logger sécurisé (unbuffered)
    logger_t *critical_log = logger_create("critical.log", LOG_IMMEDIATE);

    // Utilisation
    for (int i = 0; i < 1000; i++) {
        logger_log(fast_log, "DEBUG", "Itération %d", i);
    }

    logger_log(normal_log, "INFO", "Traitement terminé");
    logger_log(critical_log, "ERROR", "Erreur critique détectée");

    // Nettoyage
    logger_destroy(fast_log);
    logger_destroy(normal_log);
    logger_destroy(critical_log);

    return 0;
}
```

## Tableau récapitulatif complet

| Aspect | I/O Non bufferisé (fd) | I/O Bufferisé (FILE*) |
|--------|------------------------|------------------------|
| **Fonctions** | `open()`, `read()`, `write()` | `fopen()`, `fread()`, `fprintf()` |
| **Headers** | `<unistd.h>`, `<fcntl.h>` | `<stdio.h>` |
| **Handle** | `int fd` | `FILE *fp` |
| **Buffer** | ❌ Non (direct syscall) | ✅ Oui (4-8 Ko) |
| **Syscalls** | 1 par opération | 1 tous les 4-8 Ko |
| **Performance (petites E/S)** | ⚠️ Lent (~100-1000 µs) | ✅ Rapide (~1-10 ns) |
| **Performance (grosses E/S)** | ✅ Bon | ✅ Bon |
| **Contrôle timing** | ✅ Précis | ⚠️ Retardé |
| **Sécurité (crash)** | ✅ Meilleure* | ⚠️ Risque de perte |
| **Formatage** | ❌ Manuel | ✅ Intégré |
| **Portabilité** | ⚠️ UNIX/Linux | ✅ Standard C |
| **Complexité** | ⚠️ Plus complexe | ✅ Simple |

\* *Avec `fsync()` pour garantir l'écriture physique*

## Points clés à retenir

✅ **Buffering** = Stocker temporairement des données en RAM avant écriture

✅ **I/O bufferisé** (FILE*) = Rapide pour petites opérations fréquentes (~45x plus rapide)

✅ **I/O non bufferisé** (fd) = Contrôle précis, bon pour gros transferts

✅ **Trois types** : Fully buffered (fichiers), Line buffered (terminaux), Unbuffered (stderr)

✅ **fflush()** force le vidage du buffer (logs critiques, avant fork())

✅ **setbuf(fp, NULL)** désactive le buffering complètement

✅ **fclose()** vide automatiquement le buffer (et peut échouer !)

✅ **Risque de perte** en cas de crash si données bufferisées

✅ **fsync()** garantit l'écriture physique sur disque

✅ **Ne pas mélanger** FILE* et fd sans synchronisation

---

## Prochaines étapes

Maintenant que vous maîtrisez le buffering, explorez :
- **Section 16.6** : `dup` et `dup2` pour la redirection
- **Section 16.7** : I/O multiplexing (`select`, `poll`, `epoll`)
- **Section 27.3** : Profiling pour mesurer l'impact du buffering
- **Module 6** : Programmation système complète

Le buffering est un concept fondamental qui affecte directement les performances de vos programmes. Comprendre quand et comment l'utiliser est essentiel pour écrire du code efficace et robuste !

⏭️ [dup, dup2 et redirection](/16-fichiers-et-io/06-dup-redirection.md)
