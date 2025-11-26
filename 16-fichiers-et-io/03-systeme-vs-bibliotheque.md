🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 16.3 Différence appels système vs bibliothèque

## Introduction

En programmation C sous Linux, vous disposez de **deux niveaux** pour manipuler les fichiers et les entrées/sorties :

1. **Les appels système** (system calls) : `open()`, `read()`, `write()`, `close()`
2. **Les fonctions de la bibliothèque C** (stdio) : `fopen()`, `fread()`, `fwrite()`, `fclose()`

> **💡 Analogie simple :** Imaginez que vous voulez vous déplacer en ville :
> - **Appels système** = Conduire votre propre voiture (contrôle total, mais plus de responsabilités)
> - **Bibliothèque C** = Prendre un bus (plus simple, automatique, mais moins de contrôle)

Ces deux approches permettent d'atteindre le même objectif (manipuler des fichiers), mais avec des caractéristiques très différentes.

## Vue d'ensemble

### Architecture en couches

```
┌─────────────────────────────────────────────┐
│         VOTRE PROGRAMME C                   │
└─────────────────┬───────────────────────────┘
                  │
        ┌─────────┴──────────┐
        │                    │
        ▼                    ▼
┌───────────────┐    ┌──────────────────┐
│  BIBLIOTHÈQUE │    │  APPELS SYSTÈME  │
│  C (stdio.h)  │    │  (unistd.h)      │
│               │    │                  │
│  fopen()      │    │  open()          │
│  fread()      │    │  read()          │
│  fwrite()     │    │  write()         │
│  fclose()     │    │  close()         │
│  fprintf()    │    │                  │
│  fgets()      │    │                  │
│               │    │                  │
│  + BUFFERING  │    │  (pas de buffer) │
└───────┬───────┘    └────────┬─────────┘
        │                     │
        └──────────┬──────────┘
                   ▼
        ┌─────────────────────┐
        │   NOYAU LINUX       │
        │  (Kernel Space)     │
        └─────────────────────┘
                   │
                   ▼
        ┌─────────────────────┐
        │  SYSTÈME DE         │
        │  FICHIERS           │
        └─────────────────────┘
```

**Points clés :**
- Les **fonctions de bibliothèque** utilisent les **appels système** en interne
- La bibliothèque C ajoute une couche d'abstraction avec du **buffering**
- Les appels système communiquent **directement** avec le noyau

## Les deux approches en détail

### Appels système (bas niveau)

**Caractéristiques :**
- Interface directe avec le noyau Linux
- Manipulent des **descripteurs de fichiers** (entiers : 0, 1, 2, 3...)
- Pas de buffering automatique
- Spécifiques à UNIX/Linux
- Maximum de contrôle
- Définis dans `<unistd.h>`, `<fcntl.h>`

**Exemple basique :**
```c
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    // 1. Ouvrir avec un descripteur de fichier (int)
    int fd = open("fichier.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    // 2. Écrire directement
    const char *texte = "Bonjour\n";
    write(fd, texte, strlen(texte));

    // 3. Fermer
    close(fd);

    return 0;
}
```

### Bibliothèque C standard (haut niveau)

**Caractéristiques :**
- Couche d'abstraction portable
- Manipulent des **pointeurs FILE*** (structures opaques)
- Buffering automatique intégré
- Portables (Windows, macOS, Linux, etc.)
- Plus pratiques pour du texte
- Définis dans `<stdio.h>`

**Exemple basique :**
```c
#include <stdio.h>

int main(void) {
    // 1. Ouvrir avec un pointeur FILE*
    FILE *fp = fopen("fichier.txt", "w");

    // 2. Écrire avec des fonctions formatées
    fprintf(fp, "Bonjour\n");

    // 3. Fermer (vide le buffer automatiquement)
    fclose(fp);

    return 0;
}
```

## Comparaison détaillée

### 1. Type de handle (identifiant)

| Aspect | Appels système | Bibliothèque C |
|--------|----------------|----------------|
| **Type** | `int` (descripteur de fichier) | `FILE*` (pointeur de structure) |
| **Exemples** | `3`, `4`, `5` | Pointeur opaque |
| **Pré-définis** | `0` (stdin), `1` (stdout), `2` (stderr) | `stdin`, `stdout`, `stderr` |
| **Déclaration** | `int fd;` | `FILE *fp;` |

**Exemple côte à côte :**
```c
// Appels système
int fd = open("file.txt", O_RDONLY);
if (fd == -1) { /* erreur */ }

// Bibliothèque C
FILE *fp = fopen("file.txt", "r");
if (fp == NULL) { /* erreur */ }
```

### 2. Fonctions d'ouverture

| Appels système | Bibliothèque C |
|----------------|----------------|
| `open(path, flags, mode)` | `fopen(path, mode)` |

**Comparaison pratique :**
```c
// APPELS SYSTÈME : Plus verbeux
int fd = open("data.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);

// BIBLIOTHÈQUE C : Plus simple
FILE *fp = fopen("data.txt", "w+");
```

**Correspondance des modes :**

| Bibliothèque C | Appels système équivalents |
|----------------|----------------------------|
| `"r"` | `O_RDONLY` |
| `"w"` | `O_WRONLY | O_CREAT | O_TRUNC` |
| `"a"` | `O_WRONLY | O_CREAT | O_APPEND` |
| `"r+"` | `O_RDWR` |
| `"w+"` | `O_RDWR | O_CREAT | O_TRUNC` |
| `"a+"` | `O_RDWR | O_CREAT | O_APPEND` |

### 3. Fonctions de lecture

| Appels système | Bibliothèque C |
|----------------|----------------|
| `read(fd, buf, count)` | `fread(buf, size, count, fp)` |
| - | `fgets(buf, size, fp)` |
| - | `fscanf(fp, format, ...)` |
| - | `fgetc(fp)` |

**Exemple de lecture :**
```c
char buffer[256];

// APPELS SYSTÈME
int fd = open("file.txt", O_RDONLY);
ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
buffer[n] = '\0';  // Vous devez ajouter le '\0'
close(fd);

// BIBLIOTHÈQUE C
FILE *fp = fopen("file.txt", "r");
fgets(buffer, sizeof(buffer), fp);  // '\0' ajouté automatiquement
fclose(fp);
```

### 4. Fonctions d'écriture

| Appels système | Bibliothèque C |
|----------------|----------------|
| `write(fd, buf, count)` | `fwrite(buf, size, count, fp)` |
| - | `fprintf(fp, format, ...)` |
| - | `fputs(str, fp)` |
| - | `fputc(c, fp)` |

**Exemple d'écriture formatée :**
```c
int age = 25;
const char *nom = "Alice";

// APPELS SYSTÈME : Vous devez formater manuellement
int fd = open("info.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
char buffer[256];
int len = snprintf(buffer, sizeof(buffer), "Nom: %s, Age: %d\n", nom, age);
write(fd, buffer, len);
close(fd);

// BIBLIOTHÈQUE C : Formatage intégré
FILE *fp = fopen("info.txt", "w");
fprintf(fp, "Nom: %s, Age: %d\n", nom, age);  // Beaucoup plus simple !
fclose(fp);
```

### 5. Fonctions de fermeture

| Appels système | Bibliothèque C |
|----------------|----------------|
| `close(fd)` | `fclose(fp)` |

**Différence importante :**
```c
// APPELS SYSTÈME : Fermeture immédiate
write(fd, data, size);
close(fd);  // Données écrites immédiatement (ou presque)

// BIBLIOTHÈQUE C : Vide le buffer d'abord
fprintf(fp, "texte");
fclose(fp);  // 1. Vide le buffer, 2. Appelle close() en interne
```

## Le buffering : La grande différence

### Qu'est-ce que le buffering ?

Le **buffering** (mise en tampon) consiste à accumuler des données en mémoire avant de les écrire réellement sur le disque.

```
Sans buffering (appels système) :
┌─────────┐
│ write() │ ──→ [Syscall] ──→ Kernel ──→ Disque
└─────────┘     (lent)

Avec buffering (bibliothèque C) :
┌──────────┐
│ fprintf()│ ──→ [Buffer en RAM] (rapide)
└──────────┘           │
                       │ (quand le buffer est plein)
                       ▼
                  [Syscall] ──→ Kernel ──→ Disque
```

### Types de buffering

La bibliothèque C utilise trois types de buffering :

| Type | Description | Utilisé pour |
|------|-------------|--------------|
| **Fully buffered** | Écrit quand le buffer est plein | Fichiers normaux |
| **Line buffered** | Écrit à chaque `\n` | Terminaux (stdout) |
| **Unbuffered** | Écrit immédiatement | stderr |

**Taille typique du buffer :** 4 Ko à 8 Ko (selon le système)

### Exemple concret de buffering

```c
#include <stdio.h>
#include <unistd.h>

int main(void) {
    FILE *fp = fopen("test.txt", "w");

    // Ces données vont dans le buffer, PAS sur le disque
    fprintf(fp, "Ligne 1\n");
    fprintf(fp, "Ligne 2\n");
    fprintf(fp, "Ligne 3\n");

    printf("Données dans le buffer, pas encore sur disque\n");
    sleep(5);  // Attendre 5 secondes

    // Maintenant on ferme : le buffer est vidé sur le disque
    fclose(fp);
    printf("Maintenant les données sont sur le disque\n");

    return 0;
}
```

**Si vous ouvrez `test.txt` pendant le sleep() :** Le fichier sera vide ou n'existera même pas !

### Forcer le vidage du buffer

**Fonction `fflush()` :**
```c
#include <stdio.h>

FILE *fp = fopen("log.txt", "w");

fprintf(fp, "Message important\n");
fflush(fp);  // ✅ Force l'écriture immédiate sur disque

fprintf(fp, "Autre message\n");
// Ce message reste dans le buffer jusqu'à fclose() ou fflush()

fclose(fp);
```

**Quand utiliser `fflush()` :**
- Logs critiques (en cas de crash, vous voulez que les logs soient écrits)
- Avant un `fork()` ou `exec()`
- Communication inter-processus via fichiers
- Debugging (pour voir les données immédiatement)

### Désactiver complètement le buffering

```c
#include <stdio.h>

FILE *fp = fopen("log.txt", "w");

// Désactiver le buffering
setbuf(fp, NULL);
// Ou : setvbuf(fp, NULL, _IONBF, 0);

// Maintenant chaque fprintf() écrit immédiatement
fprintf(fp, "Log 1\n");  // Écrit tout de suite
fprintf(fp, "Log 2\n");  // Écrit tout de suite

fclose(fp);
```

## Impact sur les performances

### Benchmark : Écriture de 10 000 lignes

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

// Test avec appels système
void test_syscalls(void) {
    int fd = open("syscall.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    for (int i = 0; i < 10000; i++) {
        char buf[32];
        int len = snprintf(buf, sizeof(buf), "Ligne %d\n", i);
        write(fd, buf, len);  // 10 000 appels système !
    }
    close(fd);
}

// Test avec bibliothèque C
void test_stdio(void) {
    FILE *fp = fopen("stdio.txt", "w");
    for (int i = 0; i < 10000; i++) {
        fprintf(fp, "Ligne %d\n", i);  // Bufferisé
    }
    fclose(fp);  // Un seul (ou quelques) appels système
}

int main(void) {
    clock_t start, end;

    // Test appels système
    start = clock();
    test_syscalls();
    end = clock();
    printf("Appels système : %.3f secondes\n",
           (double)(end - start) / CLOCKS_PER_SEC);

    // Test bibliothèque C
    start = clock();
    test_stdio();
    end = clock();
    printf("Bibliothèque C : %.3f secondes\n",
           (double)(end - start) / CLOCKS_PER_SEC);

    return 0;
}
```

**Résultats typiques :**
```
Appels système : 0.850 secondes
Bibliothèque C : 0.012 secondes
```

**Bibliothèque C = ~70x plus rapide !** (pour des petites écritures fréquentes)

### Pourquoi cette différence ?

| Aspect | Appels système | Bibliothèque C |
|--------|----------------|----------------|
| **Nombre de syscalls** | 10 000 | ~2-3 |
| **Changements de contexte** | 10 000 | ~2-3 |
| **Overhead** | Élevé | Faible |

**Cas où les appels système sont plus rapides :**
- Écriture de gros blocs (> 4 Ko)
- Une seule grande opération
- Besoin de contrôle précis du timing

## Portabilité

### Appels système (UNIX/Linux spécifiques)

```c
#include <fcntl.h>   // ❌ N'existe pas sur Windows
#include <unistd.h>  // ❌ N'existe pas sur Windows

int fd = open("file.txt", O_RDONLY);  // ❌ Windows utilise CreateFile()
```

**Systèmes supportés :**
- ✅ Linux
- ✅ macOS
- ✅ BSD
- ✅ Autres UNIX
- ❌ Windows (API différente)

### Bibliothèque C (portable)

```c
#include <stdio.h>  // ✅ Standard C, fonctionne partout

FILE *fp = fopen("file.txt", "r");  // ✅ Portable
```

**Systèmes supportés :**
- ✅ Linux
- ✅ macOS
- ✅ Windows
- ✅ BSD
- ✅ Systèmes embarqués
- ✅ Tout système avec un compilateur C standard

## Fonctionnalités avancées

### Ce que les appels système peuvent faire

**1. Opérations atomiques**
```c
// Ouvrir un fichier de manière atomique
int fd = open("lock.file", O_WRONLY | O_CREAT | O_EXCL, 0644);
// Échoue si le fichier existe déjà (utile pour les verrous)
```

**2. Contrôle fin des permissions**
```c
int fd = open("file.txt", O_WRONLY | O_CREAT, 0600);  // rw------- seulement
```

**3. Fichiers spéciaux**
```c
int fd = open("/dev/urandom", O_RDONLY);  // Générateur aléatoire
int fd = open("/proc/cpuinfo", O_RDONLY); // Informations système
```

**4. Manipulation directe de descripteurs**
```c
dup2(fd, STDOUT_FILENO);  // Rediriger stdout vers un fichier
```

**5. I/O non-bloquant**
```c
int fd = open("fifo", O_RDONLY | O_NONBLOCK);
// Ne bloque pas si le FIFO est vide
```

### Ce que la bibliothèque C fait mieux

**1. Formatage de texte**
```c
fprintf(fp, "Utilisateur: %s, Score: %d, Ratio: %.2f%%\n",
        nom, score, ratio);
// Beaucoup plus simple qu'avec write() !
```

**2. Lecture ligne par ligne**
```c
char ligne[256];
while (fgets(ligne, sizeof(ligne), fp) != NULL) {
    printf("%s", ligne);
}
// Très simple comparé à read()
```

**3. Parsing de texte**
```c
int age;
char nom[50];
fscanf(fp, "%s %d", nom, &age);
// Impossible directement avec read()
```

**4. Gestion automatique des erreurs de fin de ligne**
```c
// Gère automatiquement \n, \r\n, \r selon le système
fputs("ligne\n", fp);
```

## Interopérabilité

### Obtenir le fd depuis un FILE*

```c
#include <stdio.h>

FILE *fp = fopen("file.txt", "r");
int fd = fileno(fp);  // Obtenir le descripteur sous-jacent

// Maintenant on peut utiliser les deux
fgets(buffer, size, fp);  // Bibliothèque C
read(fd, buf, n);         // Appel système
```

**⚠️ Attention :** Mélanger les deux peut causer des problèmes de buffering !

### Créer un FILE* depuis un fd

```c
#include <stdio.h>

int fd = open("file.txt", O_RDONLY);
FILE *fp = fdopen(fd, "r");  // Envelopper le fd dans un FILE*

// Maintenant on peut utiliser les fonctions de stdio
fgets(buffer, size, fp);

fclose(fp);  // Ferme aussi le fd
```

## Quand utiliser quoi ?

### Utilisez les appels système quand :

✅ **Vous devez manipuler des descripteurs directement**
```c
dup2(fd, STDOUT_FILENO);  // Redirection
```

✅ **Performance critique avec gros transferts**
```c
// Copier un fichier de 1 Go
char buffer[1024 * 1024];  // 1 Mo
while ((n = read(fd_in, buffer, sizeof(buffer))) > 0) {
    write(fd_out, buffer, n);
}
```

✅ **Programmation système bas niveau**
```c
// Sockets, pipes, fichiers spéciaux
int sock = socket(AF_INET, SOCK_STREAM, 0);
```

✅ **Opérations non-standard**
```c
// Flags spéciaux, contrôle fin
open("file", O_RDONLY | O_NOATIME | O_DIRECT);
```

✅ **Code spécifique Linux/UNIX**
```c
// Pas besoin de portabilité Windows
```

### Utilisez la bibliothèque C quand :

✅ **Manipulation de fichiers texte**
```c
FILE *fp = fopen("config.txt", "r");
while (fgets(ligne, sizeof(ligne), fp) != NULL) {
    // Traiter chaque ligne
}
```

✅ **Formatage d'entrées/sorties**
```c
fprintf(fp, "Date: %02d/%02d/%04d\n", jour, mois, annee);
```

✅ **Code portable (multi-plateforme)**
```c
// Doit fonctionner sur Windows, macOS, Linux
```

✅ **Simplicité et lisibilité**
```c
// Plus facile à lire et maintenir
```

✅ **Petites écritures fréquentes**
```c
// Le buffering améliore les performances
for (int i = 0; i < 10000; i++) {
    fprintf(fp, "Ligne %d\n", i);
}
```

## Tableau récapitulatif complet

| Critère | Appels système | Bibliothèque C |
|---------|----------------|----------------|
| **Headers** | `<unistd.h>`, `<fcntl.h>` | `<stdio.h>` |
| **Handle** | `int fd` | `FILE *fp` |
| **Ouverture** | `open()` | `fopen()` |
| **Lecture** | `read()` | `fread()`, `fgets()`, `fscanf()` |
| **Écriture** | `write()` | `fwrite()`, `fprintf()`, `fputs()` |
| **Fermeture** | `close()` | `fclose()` |
| **Buffering** | ❌ Non | ✅ Oui (automatique) |
| **Portabilité** | ❌ UNIX/Linux | ✅ Toutes plateformes |
| **Performance (petites E/S)** | ⚠️ Lent | ✅ Rapide |
| **Performance (grosses E/S)** | ✅ Rapide | ⚠️ Comparable |
| **Contrôle** | ✅ Maximum | ⚠️ Limité |
| **Simplicité texte** | ❌ Complexe | ✅ Simple |
| **Formatage** | ❌ Manuel | ✅ Intégré |
| **Programmation système** | ✅ Idéal | ❌ Limité |
| **Complexité** | ⚠️ Plus complexe | ✅ Plus simple |

## Exemple complet : Les deux approches

### Copier un fichier avec appels système

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int copy_syscall(const char *src, const char *dst) {
    int fd_in, fd_out;
    char buffer[8192];
    ssize_t n;

    // Ouvrir source
    fd_in = open(src, O_RDONLY);
    if (fd_in == -1) {
        perror("open source");
        return -1;
    }

    // Créer destination
    fd_out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out == -1) {
        perror("open destination");
        close(fd_in);
        return -1;
    }

    // Copier
    while ((n = read(fd_in, buffer, sizeof(buffer))) > 0) {
        if (write(fd_out, buffer, n) != n) {
            perror("write");
            close(fd_in);
            close(fd_out);
            return -1;
        }
    }

    if (n == -1) {
        perror("read");
    }

    close(fd_in);
    close(fd_out);
    return 0;
}
```

### Copier un fichier avec bibliothèque C

```c
#include <stdio.h>

int copy_stdio(const char *src, const char *dst) {
    FILE *fp_in, *fp_out;
    char buffer[8192];
    size_t n;

    // Ouvrir source
    fp_in = fopen(src, "rb");  // "b" pour binaire
    if (fp_in == NULL) {
        perror("fopen source");
        return -1;
    }

    // Créer destination
    fp_out = fopen(dst, "wb");
    if (fp_out == NULL) {
        perror("fopen destination");
        fclose(fp_in);
        return -1;
    }

    // Copier
    while ((n = fread(buffer, 1, sizeof(buffer), fp_in)) > 0) {
        if (fwrite(buffer, 1, n, fp_out) != n) {
            perror("fwrite");
            fclose(fp_in);
            fclose(fp_out);
            return -1;
        }
    }

    if (ferror(fp_in)) {
        perror("fread");
    }

    fclose(fp_in);
    fclose(fp_out);
    return 0;
}
```

**Observations :**
- Les deux versions sont très similaires
- La version stdio est légèrement plus courte
- Les performances seront similaires (gros buffers)
- stdio est portable, syscalls est spécifique UNIX

## Pièges courants

### 1. Mélanger les deux approches

```c
// ❌ DANGEREUX
FILE *fp = fopen("file.txt", "r");
int fd = fileno(fp);

fgets(line, size, fp);  // Utilise le buffer de stdio
read(fd, buf, n);       // Bypass le buffer -> données décalées !
```

**Solution :** Si vous devez mélanger, utilisez `fflush()` :
```c
fgets(line, size, fp);
fflush(fp);           // Synchronise
read(fd, buf, n);     // OK maintenant
```

### 2. Oublier de vérifier les erreurs

```c
// ❌ MAUVAIS
FILE *fp = fopen("file.txt", "r");
fgets(buffer, size, fp);  // Crash si fp == NULL !

// ✅ BON
FILE *fp = fopen("file.txt", "r");
if (fp == NULL) {
    perror("fopen");
    return 1;
}
fgets(buffer, size, fp);
```

### 3. Ne pas fermer les fichiers

```c
// ❌ Fuite de ressources
for (int i = 0; i < 1000; i++) {
    FILE *fp = fopen("file.txt", "r");
    // ... utiliser fp ...
    // Oubli de fclose(fp) !
}
// Après ~1024 itérations : erreur "Too many open files"
```

### 4. Oublier le mode binaire sur Windows

```c
// ❌ Sur Windows : corruption de données binaires
FILE *fp = fopen("image.png", "r");  // Mode texte par défaut

// ✅ Toujours utiliser "b" pour les fichiers binaires
FILE *fp = fopen("image.png", "rb");
```

## Points clés à retenir

✅ **Deux niveaux** : appels système (bas niveau) vs bibliothèque C (haut niveau)

✅ **Appels système** = descripteurs de fichiers (`int`), pas de buffering, maximum de contrôle

✅ **Bibliothèque C** = pointeurs FILE*, buffering automatique, portable, simple

✅ **Buffering** = La grande différence de performance pour petites E/S fréquentes

✅ **Appels système** pour : programmation système, performance (gros fichiers), contrôle fin

✅ **Bibliothèque C** pour : texte, formatage, portabilité, simplicité

✅ **`fflush()`** pour forcer le vidage du buffer

✅ **Ne pas mélanger** les deux approches sans précautions

✅ **stdio utilise syscalls en interne** mais ajoute une couche d'abstraction

---

## Prochaines étapes

Maintenant que vous comprenez les différences entre appels système et bibliothèque, explorez :
- **Section 16.4** : Permissions et modes de fichiers
- **Section 16.5** : I/O bufferisé vs non bufferisé (détails avancés)
- **Section 15.2** : Débogage avec GDB pour observer le buffering
- **Module 6** : Programmation système complète avec processus et threads

Le choix entre appels système et bibliothèque C dépend de votre cas d'usage. Dans la plupart des situations quotidiennes, la bibliothèque C est le meilleur choix. Pour de la programmation système ou des besoins spécifiques, les appels système offrent le contrôle nécessaire !

⏭️ [Permissions et modes](/16-fichiers-et-io/04-permissions-modes.md)
