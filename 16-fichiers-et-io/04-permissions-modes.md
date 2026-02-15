🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 16.4 Permissions et modes

## Introduction

Sous Linux, **chaque fichier et répertoire** possède un système de permissions qui contrôle qui peut lire, modifier ou exécuter ce fichier. Ce système est fondamental pour la sécurité et l'organisation multi-utilisateurs de Linux.

> **💡 Analogie :** Imaginez que chaque fichier est comme une maison avec trois types de portes :
> - Une porte pour le **propriétaire** (vous)
> - Une porte pour la **famille** (votre groupe)
> - Une porte pour les **visiteurs** (tout le monde)
>
> Chaque porte peut avoir 3 serrures différentes : une pour **entrer et regarder** (lecture), une pour **modifier** (écriture), et une pour **utiliser** (exécution).

## Le système de permissions Unix

### Les trois types de permissions

Chaque fichier a **trois permissions de base** :

| Permission | Symbole | Valeur | Signification pour un fichier | Signification pour un répertoire |
|------------|---------|--------|-------------------------------|----------------------------------|
| **Read** (Lecture) | `r` | 4 | Lire le contenu du fichier | Lister le contenu du répertoire |
| **Write** (Écriture) | `w` | 2 | Modifier le fichier | Créer/supprimer des fichiers dans ce répertoire |
| **Execute** (Exécution) | `x` | 1 | Exécuter le fichier comme programme | Entrer dans le répertoire (`cd`) |

### Les trois catégories d'utilisateurs

Ces permissions s'appliquent à **trois catégories** d'utilisateurs :

| Catégorie | Symbole | Description |
|-----------|---------|-------------|
| **User** (Propriétaire) | `u` | L'utilisateur qui possède le fichier |
| **Group** (Groupe) | `g` | Les utilisateurs appartenant au groupe du fichier |
| **Others** (Autres) | `o` | Tous les autres utilisateurs du système |

### Visualisation des permissions

Quand vous faites `ls -l`, vous voyez les permissions :

```bash
$ ls -l fichier.txt
-rw-r--r-- 1 alice developers 1024 Nov 26 10:30 fichier.txt
 │││││││││
 │└┬┘└┬┘└┬┘
 │ │  │  └─── Others (autres) : r-- (lecture seule)
 │ │  └────── Group (groupe)  : r-- (lecture seule)
 │ └───────── User (propriétaire) : rw- (lecture et écriture)
 └─────────── Type de fichier : - (fichier normal)
```

**Décomposition complète :**

```
-rw-r--r--
│││││││││└── Others - Execute (non)
││││││││└─── Others - Write   (non)
│││││││└──── Others - Read    (oui)
││││││└───── Group - Execute  (non)
│││││└────── Group - Write    (non)
││││└─────── Group - Read     (oui)
│││└──────── User - Execute   (non)
││└───────── User - Write     (oui)
│└────────── User - Read      (oui)
└─────────── Type: - (fichier), d (répertoire), l (lien)
```

## Notation octale des permissions

### Principe de base

Chaque permission a une **valeur numérique** :
- `r` (read) = **4**
- `w` (write) = **2**
- `x` (execute) = **1**

On **additionne** ces valeurs pour obtenir un chiffre de 0 à 7 :

| Octal | Binaire | Symbole | Permissions |
|-------|---------|---------|-------------|
| **0** | 000 | `---` | Aucune permission |
| **1** | 001 | `--x` | Exécution seulement |
| **2** | 010 | `-w-` | Écriture seulement |
| **3** | 011 | `-wx` | Écriture + Exécution |
| **4** | 100 | `r--` | Lecture seulement |
| **5** | 101 | `r-x` | Lecture + Exécution |
| **6** | 110 | `rw-` | Lecture + Écriture |
| **7** | 111 | `rwx` | Toutes les permissions |

### Calcul des permissions

**Exemple : `rw-r--r--` en octal**

```
User:   rw-  = 4 + 2 + 0 = 6  
Group:  r--  = 4 + 0 + 0 = 4  
Others: r--  = 4 + 0 + 0 = 4  

Résultat : 0644
```

**Autre exemple : `rwxr-x---` en octal**

```
User:   rwx  = 4 + 2 + 1 = 7  
Group:  r-x  = 4 + 0 + 1 = 5  
Others: ---  = 0 + 0 + 0 = 0  

Résultat : 0750
```

### Permissions courantes

| Octal | Symbole | Usage typique |
|-------|---------|---------------|
| **0644** | `rw-r--r--` | Fichiers texte normaux |
| **0755** | `rwxr-xr-x` | Exécutables, scripts, répertoires |
| **0600** | `rw-------` | Fichiers privés (clés SSH, mots de passe) |
| **0700** | `rwx------` | Répertoires privés |
| **0666** | `rw-rw-rw-` | Fichiers accessibles à tous en lecture/écriture |
| **0777** | `rwxrwxrwx` | Tout le monde peut tout faire (⚠️ dangereux !) |

### Pourquoi le 0 devant ?

La notation **0644** commence par un `0` pour indiquer que c'est un **nombre octal** (base 8).

```c
// En C, ces deux lignes sont équivalentes :
mode_t permissions = 0644;  // Octal  
mode_t permissions = 420;   // Décimal (6×64 + 4×8 + 4 = 420)  

// ⚠️ Attention : toujours utiliser le 0 devant pour clarté !
```

## Créer des fichiers avec des permissions

### Avec `open()` - Appels système

La fonction `open()` avec le flag `O_CREAT` nécessite un troisième paramètre : le **mode**.

```c
#include <fcntl.h>
#include <unistd.h>

int fd = open("fichier.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
//                                                          ^^^^
//                                                       permissions
```

**Syntaxe complète :**
```c
int open(const char *pathname, int flags, mode_t mode);
```

**Exemple complet :**
```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    // Créer un fichier avec permissions rw-r--r-- (0644)
    int fd = open("data.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    write(fd, "Hello\n", 6);
    close(fd);

    // Vérifier avec ls -l
    system("ls -l data.txt");
    // Affichera : -rw-r--r-- 1 user group 6 Nov 26 10:30 data.txt

    return 0;
}
```

### Avec `fopen()` - Bibliothèque C

La fonction `fopen()` crée les fichiers avec des permissions par défaut (généralement 0666, modifiées par umask).

```c
#include <stdio.h>
#include <sys/stat.h>

FILE *fp = fopen("fichier.txt", "w");
// Permissions par défaut : 0666 & ~umask
```

**Pour contrôler les permissions avec fopen() :**

```c
#include <stdio.h>
#include <sys/stat.h>

// Méthode 1 : Modifier umask temporairement
mode_t old_umask = umask(0);  // Désactiver umask  
FILE *fp = fopen("fichier.txt", "w");  
umask(old_umask);  // Restaurer  

// Méthode 2 : Utiliser open() puis fdopen()
int fd = open("fichier.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600);  
FILE *fp = fdopen(fd, "w");  
```

### Permissions des répertoires

```c
#include <sys/stat.h>
#include <sys/types.h>

// Créer un répertoire avec permissions rwxr-xr-x (0755)
if (mkdir("/tmp/mon_dossier", 0755) == -1) {
    perror("mkdir");
    return 1;
}
```

**Permissions typiques pour répertoires :**
- **0755** (`rwxr-xr-x`) : Standard, tout le monde peut lister et entrer
- **0700** (`rwx------`) : Privé, seul le propriétaire peut accéder
- **0775** (`rwxrwxr-x`) : Groupe peut modifier le contenu
- **0777** (`rwxrwxrwx`) : Accessible à tous (⚠️ risque de sécurité)

## Le umask : Masque de permissions

### Qu'est-ce que le umask ?

Le **umask** est un masque qui **retire** des permissions par défaut lors de la création de fichiers.

**Formule :**
```
Permissions finales = Permissions demandées & ~umask
```

**En termes simples :** Le umask indique quelles permissions **enlever**.

### Valeurs courantes de umask

| umask | Fichiers créés | Répertoires créés | Usage |
|-------|----------------|-------------------|-------|
| **0022** | `rw-r--r--` (0644) | `rwxr-xr-x` (0755) | Défaut sur la plupart des systèmes |
| **0002** | `rw-rw-r--` (0664) | `rwxrwxr-x` (0775) | Groupe peut écrire |
| **0077** | `rw-------` (0600) | `rwx------` (0700) | Fichiers privés seulement |
| **0000** | `rw-rw-rw-` (0666) | `rwxrwxrwx` (0777) | Aucune restriction |

### Comment fonctionne le umask

**Exemple avec umask 0022 :**

```
Fichier créé avec open(..., 0666) :
  Permissions demandées :  0666  (rw-rw-rw-)
  Umask retire :           0022  (----w--w-)
  Permissions finales :    0644  (rw-r--r--)

  (Calcul : 0666 & ~0022 = 0666 & 0755 = 0644)

Répertoire créé avec mkdir(..., 0777) :
  Permissions demandées :  0777  (rwxrwxrwx)
  Umask retire :           0022  (----w--w-)
  Permissions finales :    0755  (rwxr-xr-x)

  (Calcul : 0777 & ~0022 = 0777 & 0755 = 0755)
```

### Manipuler le umask en C

```c
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    mode_t old_umask;

    // Lire le umask actuel (et le changer temporairement)
    old_umask = umask(0);
    printf("Umask actuel : %04o\n", old_umask);
    umask(old_umask);  // Restaurer

    // Changer le umask pour ce processus
    umask(0077);  // Tous les fichiers seront privés

    // Créer un fichier avec ce umask
    int fd = open("prive.txt", O_WRONLY | O_CREAT, 0666);
    // Permissions finales : 0666 & ~0077 = 0600 (rw-------)

    close(fd);

    // Restaurer l'ancien umask
    umask(old_umask);

    return 0;
}
```

### Voir le umask dans le shell

```bash
$ umask
0022

$ umask -S
u=rwx,g=rx,o=rx
```

## Modifier les permissions existantes

### Avec `chmod()` - Changer les permissions

```c
#include <sys/stat.h>

int chmod(const char *pathname, mode_t mode);
```

**Exemple :**
```c
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    // Créer un fichier
    int fd = open("script.sh", O_WRONLY | O_CREAT, 0644);
    close(fd);

    // Le rendre exécutable (0755)
    if (chmod("script.sh", 0755) == -1) {
        perror("chmod");
        return 1;
    }

    printf("script.sh est maintenant exécutable\n");
    return 0;
}
```

### Avec `fchmod()` - Via un descripteur de fichier

```c
#include <sys/stat.h>

int fchmod(int fd, mode_t mode);
```

**Exemple :**
```c
int fd = open("fichier.txt", O_RDWR);  
if (fd != -1) {  
    fchmod(fd, 0600);  // Rendre privé
    close(fd);
}
```

### Modification relative des permissions

En shell, on peut faire `chmod +x fichier` pour ajouter l'exécution. En C, il faut lire les permissions actuelles :

```c
#include <sys/stat.h>
#include <stdio.h>

int add_execute_permission(const char *path) {
    struct stat st;

    // Lire les permissions actuelles
    if (stat(path, &st) == -1) {
        perror("stat");
        return -1;
    }

    // Ajouter la permission d'exécution pour tout le monde
    mode_t new_mode = st.st_mode | S_IXUSR | S_IXGRP | S_IXOTH;

    // Appliquer
    if (chmod(path, new_mode) == -1) {
        perror("chmod");
        return -1;
    }

    return 0;
}
```

## Constantes symboliques pour les permissions

Plutôt que d'utiliser des nombres octaux, vous pouvez utiliser des **constantes prédéfinies** :

### Constantes de base

```c
#include <sys/stat.h>

// Permissions du propriétaire (User)
S_IRUSR  // 0400  r--------  Lecture  
S_IWUSR  // 0200  -w-------  Écriture  
S_IXUSR  // 0100  --x------  Exécution  

// Permissions du groupe (Group)
S_IRGRP  // 0040  ---r-----  Lecture  
S_IWGRP  // 0020  ----w----  Écriture  
S_IXGRP  // 0010  -----x---  Exécution  

// Permissions des autres (Others)
S_IROTH  // 0004  ------r--  Lecture  
S_IWOTH  // 0002  -------w-  Écriture  
S_IXOTH  // 0001  --------x  Exécution  
```

### Combinaisons pratiques

```c
// Équivalent à 0644 (rw-r--r--)
mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

// Équivalent à 0755 (rwxr-xr-x)
mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR |
              S_IRGRP | S_IXGRP |
              S_IROTH | S_IXOTH;

// Équivalent à 0600 (rw-------)
mode_t mode = S_IRUSR | S_IWUSR;

// Équivalent à 0700 (rwx------)
mode_t mode = S_IRWXU;  // Macro pour S_IRUSR | S_IWUSR | S_IXUSR
```

### Macros utiles

```c
S_IRWXU  // 0700  rwx------  Toutes permissions pour User  
S_IRWXG  // 0070  ---rwx---  Toutes permissions pour Group  
S_IRWXO  // 0007  ------rwx  Toutes permissions pour Others  
```

### Exemple complet avec constantes

```c
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    // Créer un fichier privé (rw-------, 0600)
    int fd = open("secret.txt",
                  O_WRONLY | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    write(fd, "Données confidentielles\n", 25);
    close(fd);

    // Créer un script exécutable (rwxr-xr-x, 0755)
    fd = open("script.sh",
              O_WRONLY | O_CREAT | O_TRUNC,
              S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    write(fd, "#!/bin/bash\necho 'Hello'\n", 25);
    close(fd);

    return 0;
}
```

## Lire les permissions d'un fichier

### Avec `stat()` ou `fstat()`

```c
#include <sys/stat.h>

int stat(const char *pathname, struct stat *statbuf);  
int fstat(int fd, struct stat *statbuf);  
```

**La structure `struct stat` :**
```c
struct stat {
    mode_t    st_mode;     // Permissions et type de fichier
    uid_t     st_uid;      // User ID du propriétaire
    gid_t     st_gid;      // Group ID
    off_t     st_size;     // Taille en octets
    time_t    st_atime;    // Dernier accès
    time_t    st_mtime;    // Dernière modification
    time_t    st_ctime;    // Changement de métadonnées
    // ... autres champs
};
```

### Exemple : Afficher les permissions

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>

void print_permissions(mode_t mode) {
    printf("Permissions: ");

    // User
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");

    // Group
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");

    // Others
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");

    printf(" (%04o)\n", mode & 0777);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <fichier>\n", argv[0]);
        return 1;
    }

    struct stat st;
    if (stat(argv[1], &st) == -1) {
        perror("stat");
        return 1;
    }

    print_permissions(st.st_mode);

    return 0;
}
```

**Sortie exemple :**
```bash
$ ./show_perms fichier.txt
Permissions: rw-r--r-- (0644)
```

### Vérifier un type spécifique de permission

```c
#include <sys/stat.h>
#include <stdio.h>

int main(void) {
    struct stat st;

    if (stat("fichier.txt", &st) == -1) {
        perror("stat");
        return 1;
    }

    // Vérifier si le propriétaire peut écrire
    if (st.st_mode & S_IWUSR) {
        printf("Le propriétaire peut écrire\n");
    }

    // Vérifier si tout le monde peut exécuter
    if (st.st_mode & S_IXOTH) {
        printf("Tout le monde peut exécuter\n");
    }

    // Vérifier si c'est un fichier régulier
    if (S_ISREG(st.st_mode)) {
        printf("C'est un fichier régulier\n");
    }

    // Vérifier si c'est un répertoire
    if (S_ISDIR(st.st_mode)) {
        printf("C'est un répertoire\n");
    }

    return 0;
}
```

## Changer le propriétaire et le groupe

### Avec `chown()` et `chgrp()`

```c
#include <unistd.h>

int chown(const char *pathname, uid_t owner, gid_t group);  
int fchown(int fd, uid_t owner, gid_t group);  
```

**Exemple :**
```c
#include <unistd.h>
#include <stdio.h>

int main(void) {
    // Changer le propriétaire (nécessite root généralement)
    if (chown("fichier.txt", 1000, 1000) == -1) {
        perror("chown");
        return 1;
    }

    // Changer seulement le groupe (possible si on appartient au groupe)
    if (chown("fichier.txt", -1, 100) == -1) {
        perror("chown");
        return 1;
    }

    return 0;
}
```

**Note :** `-1` signifie "ne pas changer cette valeur".

## Bits spéciaux de permissions

Au-delà des permissions de base, il existe **trois bits spéciaux** :

### 1. Setuid (Set User ID) - Bit 4000

Quand un exécutable a le bit **setuid**, il s'exécute avec les permissions du **propriétaire du fichier**, pas de l'utilisateur qui le lance.

**Exemple classique : `passwd`**
```bash
$ ls -l /usr/bin/passwd
-rwsr-xr-x 1 root root 68208 Jul 14 2021 /usr/bin/passwd
    ^
    └─ 's' au lieu de 'x' : bit setuid activé
```

**Pourquoi ?** L'utilisateur normal peut changer son mot de passe, mais le fichier `/etc/shadow` appartient à root. Le programme `passwd` a besoin de s'exécuter en tant que root.

**En C :**
```c
// Activer le bit setuid (nécessite root)
chmod("programme", 04755);  // rwsr-xr-x

// Avec constantes
chmod("programme", S_ISUID | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
```

### 2. Setgid (Set Group ID) - Bit 2000

#### Sur un fichier exécutable
Le programme s'exécute avec les permissions du **groupe du fichier**.

#### Sur un répertoire
Tous les fichiers créés dans ce répertoire **héritent du groupe du répertoire**, pas du groupe de l'utilisateur.

**Exemple :**
```bash
$ mkdir projet_partage
$ chmod 2775 projet_partage  # rwxrwsr-x
$ ls -ld projet_partage
drwxrwsr-x 2 alice devs 4096 Nov 26 10:30 projet_partage
       ^
       └─ 's' : bit setgid activé
```

**En C :**
```c
// Répertoire avec setgid
mkdir("projet", 0775);  
chmod("projet", 02775);  // rwxrwsr-x  

// Avec constantes
chmod("projet", S_ISGID | S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
```

### 3. Sticky bit - Bit 1000

Sur un **répertoire**, le sticky bit empêche les utilisateurs de supprimer ou renommer des fichiers qui ne leur appartiennent pas, même s'ils ont le droit d'écriture dans le répertoire.

**Exemple classique : `/tmp`**
```bash
$ ls -ld /tmp
drwxrwxrwt 20 root root 4096 Nov 26 10:30 /tmp
         ^
         └─ 't' au lieu de 'x' : sticky bit activé
```

**Utilité :** Dans `/tmp`, tout le monde peut créer des fichiers, mais personne ne peut supprimer les fichiers des autres.

**En C :**
```c
// Répertoire avec sticky bit
mkdir("shared", 0777);  
chmod("shared", 01777);  // rwxrwxrwt  

// Avec constantes
chmod("shared", S_ISVTX | S_IRWXU | S_IRWXG | S_IRWXO);
```

### Tableau récapitulatif des bits spéciaux

| Bit | Octal | Symbole | Nom | Usage |
|-----|-------|---------|-----|-------|
| **setuid** | 4000 | `s` à la place de `x` (user) | Set User ID | Exécutable s'exécute comme le propriétaire |
| **setgid** | 2000 | `s` à la place de `x` (group) | Set Group ID | Exécutable s'exécute comme le groupe / Héritage de groupe pour répertoire |
| **sticky** | 1000 | `t` à la place de `x` (others) | Sticky bit | Protège contre suppression dans répertoire partagé |

### Notation complète avec bits spéciaux

```c
// Format complet : [special][user][group][others]
chmod("fichier", 04755);  // setuid + rwxr-xr-x  
chmod("fichier", 02755);  // setgid + rwxr-xr-x  
chmod("fichier", 01777);  // sticky + rwxrwxrwx  
chmod("fichier", 06755);  // setuid + setgid + rwxr-xr-x  
```

## Vérifier les permissions d'accès

### Avec `access()`

Teste si le processus peut accéder à un fichier avec certaines permissions.

```c
#include <unistd.h>

int access(const char *pathname, int mode);
```

**Modes possibles :**
- `R_OK` : Test de lecture
- `W_OK` : Test d'écriture
- `X_OK` : Test d'exécution
- `F_OK` : Test d'existence

**Exemple :**
```c
#include <unistd.h>
#include <stdio.h>

int main(void) {
    const char *fichier = "test.txt";

    // Vérifier si le fichier existe
    if (access(fichier, F_OK) == 0) {
        printf("Le fichier existe\n");
    } else {
        printf("Le fichier n'existe pas\n");
        return 1;
    }

    // Vérifier les permissions
    if (access(fichier, R_OK) == 0) {
        printf("✓ Lecture autorisée\n");
    } else {
        printf("✗ Lecture refusée\n");
    }

    if (access(fichier, W_OK) == 0) {
        printf("✓ Écriture autorisée\n");
    } else {
        printf("✗ Écriture refusée\n");
    }

    if (access(fichier, X_OK) == 0) {
        printf("✓ Exécution autorisée\n");
    } else {
        printf("✗ Exécution refusée\n");
    }

    return 0;
}
```

## Permissions et sécurité

### Bonnes pratiques

#### 1. Principe du moindre privilège

Donnez uniquement les permissions nécessaires, pas plus.

```c
// ❌ MAUVAIS : Trop permissif
open("config.txt", O_WRONLY | O_CREAT, 0777);

// ✅ BON : Minimal nécessaire
open("config.txt", O_WRONLY | O_CREAT, 0644);

// ✅ MIEUX : Si fichier sensible
open("password.txt", O_WRONLY | O_CREAT, 0600);
```

#### 2. Fichiers sensibles : 0600

Les fichiers contenant des secrets doivent être lisibles uniquement par le propriétaire.

```c
// Clés SSH, tokens, mots de passe
int fd = open("id_rsa", O_WRONLY | O_CREAT | O_TRUNC, 0600);
```

#### 3. Exécutables : 0755

Les programmes et scripts doivent être exécutables par tous, mais modifiables uniquement par le propriétaire.

```c
// Scripts shell, binaires
int fd = open("deploy.sh", O_WRONLY | O_CREAT | O_TRUNC, 0755);
```

#### 4. Répertoires partagés : 1777 avec sticky bit

```c
mkdir("/tmp/shared", 0777);  
chmod("/tmp/shared", 01777);  // Avec sticky bit  
```

### Dangers des permissions trop permissives

**0777 sur tout est dangereux !**

```c
// ❌ TRÈS DANGEREUX
chmod("script.sh", 0777);
// → N'importe qui peut modifier votre script et y injecter du code malveillant !

// ✅ SÉCURISÉ
chmod("script.sh", 0755);
// → Seul le propriétaire peut modifier, les autres peuvent seulement exécuter
```

## Exemple complet : Gestionnaire de permissions

```c
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

void print_permissions(mode_t mode) {
    // Type de fichier
    if (S_ISREG(mode)) printf("-");
    else if (S_ISDIR(mode)) printf("d");
    else if (S_ISLNK(mode)) printf("l");
    else printf("?");

    // User
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    if (mode & S_ISUID)
        printf((mode & S_IXUSR) ? "s" : "S");
    else
        printf((mode & S_IXUSR) ? "x" : "-");

    // Group
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    if (mode & S_ISGID)
        printf((mode & S_IXGRP) ? "s" : "S");
    else
        printf((mode & S_IXGRP) ? "x" : "-");

    // Others
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    if (mode & S_ISVTX)
        printf((mode & S_IXOTH) ? "t" : "T");
    else
        printf((mode & S_IXOTH) ? "x" : "-");

    printf(" (%04o)", mode & 07777);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <fichier>\n", argv[0]);
        return 1;
    }

    struct stat st;
    if (stat(argv[1], &st) == -1) {
        perror("stat");
        return 1;
    }

    printf("Fichier : %s\n", argv[1]);
    printf("Permissions : ");
    print_permissions(st.st_mode);
    printf("\n");

    printf("Propriétaire : UID %d\n", st.st_uid);
    printf("Groupe : GID %d\n", st.st_gid);
    printf("Taille : %ld octets\n", st.st_size);

    // Tests d'accès
    printf("\nVos permissions :\n");
    printf("  Lecture :   %s\n", access(argv[1], R_OK) == 0 ? "✓ OUI" : "✗ NON");
    printf("  Écriture :  %s\n", access(argv[1], W_OK) == 0 ? "✓ OUI" : "✗ NON");
    printf("  Exécution : %s\n", access(argv[1], X_OK) == 0 ? "✓ OUI" : "✗ NON");

    return 0;
}
```

**Utilisation :**
```bash
$ ./perms /bin/bash
Fichier : /bin/bash  
Permissions : -rwxr-xr-x (0755)  
Propriétaire : UID 0  
Groupe : GID 0  
Taille : 1183448 octets  

Vos permissions :
  Lecture :   ✓ OUI
  Écriture :  ✗ NON
  Exécution : ✓ OUI
```

## Points clés à retenir

✅ **Trois types de permissions** : Read (4), Write (2), Execute (1)

✅ **Trois catégories** : User (propriétaire), Group, Others

✅ **Notation octale** : 0644 = rw-r--r--, 0755 = rwxr-xr-x

✅ **umask** retire des permissions par défaut (défaut 0022)

✅ **Permissions courantes** : 0644 (fichiers), 0755 (exécutables/répertoires), 0600 (privé)

✅ **Bits spéciaux** : setuid (4000), setgid (2000), sticky (1000)

✅ **Toujours le 0 devant** pour indiquer l'octal : 0644, pas 644

✅ **Principe du moindre privilège** : Donnez le minimum de permissions nécessaires

✅ **Fichiers sensibles** : Toujours 0600 pour clés, mots de passe, tokens

✅ **`chmod()`** pour modifier, `stat()` pour lire, `access()` pour tester

---

## Prochaines étapes

Maintenant que vous maîtrisez les permissions et modes, explorez :
- **Section 16.5** : I/O bufferisé vs non bufferisé
- **Section 16.6** : `dup` et `dup2` pour la redirection
- **Chapitre 26** : Sécurité et code défensif (validation des permissions)
- **Module 6** : Programmation système complète

Les permissions sont fondamentales pour la sécurité sous Linux. Une bonne compréhension de ce système vous permettra d'écrire des programmes sécurisés et de comprendre les erreurs d'accès refusé !

⏭️ [I/O bufferisé vs non bufferisé](/16-fichiers-et-io/05-io-bufferise.md)
