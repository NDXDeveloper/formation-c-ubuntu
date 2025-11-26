🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 16.1 Descripteurs de fichiers

## Introduction

Les **descripteurs de fichiers** (file descriptors en anglais, souvent abrégés **fd**) sont un concept fondamental de la programmation système sous Linux et UNIX. Ils constituent l'interface entre votre programme C et les ressources du système d'exploitation.

> **💡 Analogie simple :** Imaginez un descripteur de fichier comme un **ticket de vestiaire**. Quand vous donnez votre manteau au vestiaire, on vous remet un ticket numéroté. Ce ticket ne contient pas votre manteau, mais il vous permet d'y accéder. De la même manière, un descripteur de fichier est un simple numéro qui vous permet d'accéder à un fichier ouvert.

## Qu'est-ce qu'un descripteur de fichier ?

Un descripteur de fichier est **un entier non négatif** (généralement de type `int`) qui identifie de manière unique un fichier ouvert dans un processus. C'est une abstraction fournie par le noyau Linux pour manipuler des fichiers, des sockets, des pipes, et d'autres ressources d'entrée/sortie.

### Caractéristiques importantes

- **Simple entier** : Un fd est juste un nombre (0, 1, 2, 3, 4, ...)
- **Propre à chaque processus** : Le fd 3 dans un processus A peut correspondre à un fichier différent du fd 3 dans un processus B
- **Représente un fichier ouvert** : Pas le fichier lui-même, mais une "connexion" ouverte vers ce fichier
- **Géré par le noyau** : C'est le système d'exploitation qui alloue et gère ces numéros

## Les trois descripteurs standard

**Tout programme C dispose automatiquement de trois descripteurs de fichiers pré-ouverts :**

| Descripteur | Nom symbolique | Rôle | Utilisation typique |
|-------------|----------------|------|---------------------|
| **0** | `STDIN_FILENO` | Entrée standard | Lire depuis le clavier |
| **1** | `STDOUT_FILENO` | Sortie standard | Afficher à l'écran |
| **2** | `STDERR_FILENO` | Erreur standard | Afficher les erreurs |

Ces constantes sont définies dans `<unistd.h>`.

### Exemple conceptuel

```c
#include <unistd.h>
#include <string.h>

int main(void) {
    const char *message = "Bonjour depuis stdout\n";
    const char *erreur = "Ceci est une erreur\n";

    // Écrire sur la sortie standard (fd = 1)
    write(STDOUT_FILENO, message, strlen(message));

    // Écrire sur l'erreur standard (fd = 2)
    write(STDERR_FILENO, erreur, strlen(erreur));

    return 0;
}
```

**Explication :**
- `write()` est un appel système qui écrit dans un descripteur de fichier
- On utilise les constantes symboliques plutôt que les nombres bruts (meilleure lisibilité)
- stdout et stderr sont deux descripteurs différents (utile pour la redirection)

## Table des descripteurs de fichiers

Chaque processus possède une **table des descripteurs de fichiers** maintenue par le noyau. Cette table fait le lien entre les numéros de fd et les fichiers réellement ouverts.

```
┌─────────────────────────────────────┐
│   Table des descripteurs (Process)  │
├─────┬───────────────────────────────┤
│ fd  │  Pointeur vers fichier ouvert │
├─────┼───────────────────────────────┤
│  0  │  → stdin  (clavier)           │
│  1  │  → stdout (terminal)          │
│  2  │  → stderr (terminal)          │
│  3  │  → /home/user/data.txt        │
│  4  │  → /var/log/app.log           │
│  5  │  → socket TCP                 │
│ ... │  ...                          │
└─────┴───────────────────────────────┘
```

**Points clés :**
- Les fd sont alloués séquentiellement à partir du plus petit disponible
- Quand vous ouvrez un nouveau fichier, il obtient généralement le fd 3 (si 0, 1, 2 sont déjà pris)
- Fermer un fd libère ce numéro pour une réutilisation future

## Ouvrir un fichier : obtenir un descripteur

Pour obtenir un descripteur de fichier vers un fichier réel, on utilise l'appel système `open()`.

### Syntaxe de base

```c
#include <fcntl.h>    // Pour open() et les flags O_*
#include <unistd.h>   // Pour close()

int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
```

**Paramètres :**
- `pathname` : Chemin du fichier à ouvrir (chaîne de caractères)
- `flags` : Mode d'ouverture (lecture, écriture, création, etc.)
- `mode` : Permissions (seulement si on crée un fichier)

**Valeur de retour :**
- Un entier ≥ 0 (le descripteur de fichier) en cas de succès
- `-1` en cas d'erreur (avec `errno` positionné)

### Exemple : ouvrir un fichier en lecture

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    int fd;

    // Ouvrir le fichier en lecture seule
    fd = open("/home/user/document.txt", O_RDONLY);

    if (fd == -1) {
        perror("Erreur lors de l'ouverture");
        return 1;
    }

    printf("Fichier ouvert avec succès, descripteur = %d\n", fd);

    // ... utiliser le fichier ...

    // IMPORTANT : toujours fermer le fichier
    close(fd);

    return 0;
}
```

**Explication détaillée :**
1. `open()` demande au système d'ouvrir le fichier
2. Si réussi, on obtient un fd (probablement 3)
3. `perror()` affiche un message d'erreur lisible si `fd == -1`
4. `close()` ferme le fichier et libère le descripteur

### Flags courants pour `open()`

| Flag | Signification | Usage |
|------|---------------|-------|
| `O_RDONLY` | Lecture seule | Lire un fichier existant |
| `O_WRONLY` | Écriture seule | Écrire dans un fichier |
| `O_RDWR` | Lecture et écriture | Modifier un fichier |
| `O_CREAT` | Créer si n'existe pas | Créer un nouveau fichier |
| `O_TRUNC` | Vider le fichier | Effacer le contenu existant |
| `O_APPEND` | Écrire à la fin | Ajouter sans effacer |

**Combinaison de flags :** On peut combiner plusieurs flags avec l'opérateur `|` (OU bit-à-bit).

### Exemple : créer et écrire dans un fichier

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    int fd;
    const char *texte = "Première ligne\n";

    // Créer un fichier avec permissions 0644 (rw-r--r--)
    fd = open("nouveau.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        perror("Erreur création fichier");
        return 1;
    }

    // Écrire dans le fichier
    write(fd, texte, strlen(texte));

    close(fd);
    return 0;
}
```

**Explications :**
- `O_WRONLY | O_CREAT | O_TRUNC` : ouvre en écriture, crée si besoin, vide si existe
- `0644` : permissions Unix (propriétaire : rw, groupe : r, autres : r)
- On doit toujours vérifier que `open()` a réussi avant d'utiliser le fd

## Fermer un descripteur : `close()`

**Règle d'or :** Tout descripteur de fichier ouvert doit être fermé avec `close()`.

```c
#include <unistd.h>

int close(int fd);
```

**Retourne :**
- `0` en cas de succès
- `-1` en cas d'erreur

### Pourquoi fermer est crucial ?

1. **Fuite de ressources** : Le système a un nombre limité de descripteurs disponibles (typiquement 1024 par processus)
2. **Corruption de données** : Les données en cache peuvent ne pas être écrites sur disque
3. **Blocage d'autres processus** : Un fichier peut rester verrouillé

### Exemple de bonne pratique

```c
int fd = open("fichier.txt", O_RDONLY);
if (fd == -1) {
    perror("open");
    return 1;
}

// ... utiliser fd ...

if (close(fd) == -1) {
    perror("close");
    return 1;
}
```

## Lire et écrire avec les descripteurs

### Lecture : `read()`

```c
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t count);
```

**Paramètres :**
- `fd` : Descripteur du fichier à lire
- `buf` : Buffer où stocker les données lues
- `count` : Nombre maximum d'octets à lire

**Retourne :**
- Nombre d'octets effectivement lus (peut être < count)
- `0` si fin de fichier (EOF)
- `-1` en cas d'erreur

#### Exemple complet de lecture

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define BUFFER_SIZE 4096

int main(void) {
    int fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    fd = open("fichier.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Lire le fichier par blocs
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0';  // Terminer la chaîne
        printf("%s", buffer);
    }

    if (bytes_read == -1) {
        perror("read");
    }

    close(fd);
    return 0;
}
```

**Points importants :**
- `read()` peut lire moins que demandé (pas une erreur !)
- On boucle jusqu'à obtenir 0 (EOF) ou -1 (erreur)
- On ajoute `\0` seulement si on traite du texte

### Écriture : `write()`

```c
#include <unistd.h>

ssize_t write(int fd, const void *buf, size_t count);
```

**Paramètres :**
- `fd` : Descripteur du fichier où écrire
- `buf` : Buffer contenant les données à écrire
- `count` : Nombre d'octets à écrire

**Retourne :**
- Nombre d'octets effectivement écrits
- `-1` en cas d'erreur

#### Exemple d'écriture

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    int fd;
    const char *texte = "Bonjour Linux!\n";
    ssize_t bytes_written;

    fd = open("sortie.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    bytes_written = write(fd, texte, strlen(texte));
    if (bytes_written == -1) {
        perror("write");
        close(fd);
        return 1;
    }

    printf("%zd octets écrits\n", bytes_written);

    close(fd);
    return 0;
}
```

## Différences avec les fonctions de la bibliothèque C

Il existe deux niveaux pour manipuler les fichiers en C :

### Appels système (bas niveau)

- Fonctions : `open()`, `read()`, `write()`, `close()`
- Manipulent des **descripteurs de fichiers** (entiers)
- Appels directs au noyau Linux
- Pas de buffering automatique
- Plus rapides pour de grosses opérations
- Définis dans `<unistd.h>` et `<fcntl.h>`

### Bibliothèque C standard (haut niveau)

- Fonctions : `fopen()`, `fread()`, `fwrite()`, `fclose()`
- Manipulent des **pointeurs FILE***
- Buffering automatique intégré
- Plus pratiques pour du texte (`fprintf`, `fgets`)
- Portables (Windows, macOS, etc.)
- Définis dans `<stdio.h>`

### Comparaison pratique

```c
// Approche bas niveau (descripteurs)
int fd = open("fichier.txt", O_RDONLY);
read(fd, buffer, size);
close(fd);

// Approche haut niveau (FILE*)
FILE *fp = fopen("fichier.txt", "r");
fread(buffer, 1, size, fp);
fclose(fp);
```

**Quand utiliser quoi ?**

| Situation | Recommandation |
|-----------|----------------|
| Fichiers texte simples | `FILE*` (fopen, fprintf, etc.) |
| Performance critique | Descripteurs (open, read, write) |
| Programmation système | Descripteurs (sockets, pipes, etc.) |
| Portabilité maximale | `FILE*` |
| Contrôle fin (signaux, timeout) | Descripteurs |

## Limites et gestion des ressources

### Nombre maximum de descripteurs

Chaque processus a une limite sur le nombre de fichiers ouverts simultanément.

```c
#include <sys/resource.h>
#include <stdio.h>

int main(void) {
    struct rlimit limit;

    getrlimit(RLIMIT_NOFILE, &limit);

    printf("Limite souple : %lu\n", limit.rlim_cur);
    printf("Limite dure   : %lu\n", limit.rlim_max);

    return 0;
}
```

**Typiquement :**
- Limite par défaut : **1024** descripteurs
- Peut être augmentée avec `ulimit -n` ou `setrlimit()`

### Bonnes pratiques

1. **Toujours vérifier les valeurs de retour**
   ```c
   if (fd == -1) {
       perror("open");
       return 1;
   }
   ```

2. **Fermer immédiatement après usage**
   ```c
   int fd = open(...);
   // ... utiliser fd ...
   close(fd);
   ```

3. **Gérer les erreurs de close()**
   ```c
   if (close(fd) == -1) {
       perror("close");
   }
   ```

4. **Utiliser des constantes symboliques**
   ```c
   // ✅ BON
   write(STDOUT_FILENO, msg, len);

   // ❌ MAUVAIS
   write(1, msg, len);
   ```

5. **Ne jamais fermer les descripteurs standard sans raison**
   - Fermer stdin/stdout/stderr peut causer des comportements étranges

## Résumé des concepts clés

| Concept | Description |
|---------|-------------|
| **Descripteur de fichier** | Un entier identifiant un fichier ouvert |
| **0, 1, 2** | stdin, stdout, stderr (pré-ouverts) |
| **open()** | Ouvre un fichier, retourne un fd |
| **close()** | Ferme un fd, libère la ressource |
| **read()** | Lit depuis un fd |
| **write()** | Écrit dans un fd |
| **Table des fd** | Structure noyau mappant fd → fichiers |
| **-1** | Valeur de retour indiquant une erreur |

## Points à retenir

✅ **Un descripteur de fichier est un simple entier** qui représente un fichier ouvert

✅ **0, 1, 2 sont réservés** pour stdin, stdout, stderr

✅ **Toujours vérifier** que `open()` n'a pas retourné -1

✅ **Toujours fermer** les fichiers avec `close()` pour éviter les fuites

✅ **Les appels système** (`open`, `read`, `write`) sont bas niveau et puissants

✅ **Les fonctions stdio** (`fopen`, `fprintf`) sont plus pratiques pour du texte simple

✅ **Chaque processus** a sa propre table de descripteurs

✅ **Les descripteurs sont réutilisés** : le système donne toujours le plus petit fd disponible

---

## Prochaines étapes

Maintenant que vous comprenez les descripteurs de fichiers, vous êtes prêt à explorer :
- **Section 16.2** : Les appels système en détail
- **Section 16.3** : Différences appels système vs bibliothèque
- **Section 16.6** : `dup` et `dup2` pour la redirection
- **Section 16.7** : I/O multiplexing avec `select` et `poll`

Les descripteurs de fichiers sont la fondation de toute la programmation système sous Linux. Ils sont utilisés non seulement pour les fichiers, mais aussi pour les sockets réseau, les pipes, les périphériques, et bien plus encore !

⏭️ [Appels système](/16-fichiers-et-io/02-appels-systeme.md)
