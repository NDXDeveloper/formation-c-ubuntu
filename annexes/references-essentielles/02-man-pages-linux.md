🔝 Retour au [Sommaire](/SOMMAIRE.md)

# A.2 Man pages Linux essentielles

## Introduction aux Man Pages

### Qu'est-ce qu'une man page ?

Les **man pages** (de l'anglais *manual pages*) sont la documentation officielle intégrée aux systèmes UNIX et Linux. Chaque commande, fonction, fichier de configuration dispose d'une page de manuel qui explique :
- Son utilité
- Sa syntaxe
- Ses options
- Des exemples d'utilisation
- Les fichiers associés
- Les bugs connus

💡 **Pour les débutants :** Les man pages sont comme une encyclopédie toujours disponible sur votre système, sans besoin d'Internet.

### Pourquoi les man pages sont essentielles pour programmer en C ?

En programmation C, les man pages sont votre meilleure amie car :
- ✅ Elles documentent **toutes** les fonctions de la bibliothèque standard
- ✅ Elles documentent les appels système Linux
- ✅ Elles sont **toujours à jour** avec votre système
- ✅ Elles contiennent des exemples de code
- ✅ Elles précisent les fichiers d'en-tête nécessaires
- ✅ Elles listent les codes d'erreur possibles

**En résumé :** Ne pas lire les man pages revient à coder les yeux bandés !

---

## Comment utiliser les man pages

### Syntaxe de base

```bash
man <nom_de_la_commande_ou_fonction>
```

**Exemples :**
```bash
man printf     # Documentation de printf
man malloc     # Documentation de malloc
man ls         # Documentation de la commande ls
```

### Navigation dans une man page

Une fois dans une man page, utilisez ces raccourcis clavier :

| Touche | Action |
|--------|--------|
| `Espace` ou `Page Down` | Avancer d'une page |
| `b` ou `Page Up` | Reculer d'une page |
| `↓` ou `Enter` | Avancer d'une ligne |
| `↑` | Reculer d'une ligne |
| `/motif` | Rechercher "motif" vers le bas |
| `n` | Aller à l'occurrence suivante |
| `N` | Aller à l'occurrence précédente |
| `h` | Afficher l'aide de navigation |
| `q` | Quitter |
| `g` | Aller au début |
| `G` | Aller à la fin |

**Exemple pratique :**
```bash
man printf
# Tapez /format pour chercher "format"
# Tapez n pour voir les occurrences suivantes
# Tapez q pour quitter
```

---

## Les sections des man pages

Les man pages sont organisées en **sections numérotées** selon le type de contenu :

### Tableau des sections

| Section | Contenu | Exemples |
|---------|---------|----------|
| **1** | Commandes utilisateur | `ls`, `cp`, `grep`, `gcc` |
| **2** | Appels système | `open`, `read`, `write`, `fork` |
| **3** | Fonctions de bibliothèque C | `printf`, `malloc`, `strlen` |
| **4** | Fichiers spéciaux (devices) | `/dev/null`, `/dev/random` |
| **5** | Formats de fichiers | `/etc/passwd`, `fstab` |
| **6** | Jeux et économiseurs d'écran | - |
| **7** | Divers (conventions, protocoles) | `ascii`, `utf-8`, `ip` |
| **8** | Commandes d'administration | `mount`, `iptables` |

### Pourquoi les sections sont importantes ?

Certains noms existent dans plusieurs sections !

**Exemple : `printf` existe en section 1 et 3**

```bash
man 1 printf    # Commande shell printf
man 3 printf    # Fonction C printf
```

Sans préciser la section, `man` affiche la première trouvée (généralement section 1).

### Connaître toutes les sections d'un nom

```bash
man -f printf
# ou
whatis printf
```

**Sortie :**
```
printf (1)    - format and print data
printf (3)    - formatted output conversion
```

### Rechercher dans toutes les sections

```bash
man -a printf    # Affiche toutes les pages une par une
man -k printf    # Recherche "printf" dans toutes les descriptions
```

---

## Structure typique d'une man page

Voici les sections que vous trouverez dans la plupart des man pages :

### 1. **NAME** (NOM)
Le nom de la commande/fonction et une description en une ligne.

### 2. **SYNOPSIS** (SYNOPSIS)
La signature de la fonction ou la syntaxe de la commande.

**Exemple pour `malloc` :**
```c
#include <stdlib.h>

void *malloc(size_t size);
```

💡 Cette section vous dit **quel fichier d'en-tête inclure**.

### 3. **DESCRIPTION** (DESCRIPTION)
Explication détaillée du fonctionnement.

### 4. **RETURN VALUE** (VALEUR DE RETOUR)
Ce que retourne la fonction en cas de succès et d'échec.

### 5. **ERRORS** (ERREURS)
Les codes d'erreur possibles (via `errno`).

### 6. **EXAMPLES** (EXEMPLES)
Code d'exemple (pas toujours présent).

### 7. **SEE ALSO** (VOIR AUSSI)
Fonctions ou commandes liées.

### 8. **BUGS** (BUGS)
Bugs connus ou limitations.

---

## Man pages essentielles pour la programmation C

Voici les man pages que vous devez connaître, classées par catégorie.

---

## 📚 Catégorie 1 : Entrées/Sorties standard (stdio.h)

### `printf(3)` - Affichage formaté

```bash
man 3 printf
```

**Fonctions associées :**
- `printf` : Affichage sur la sortie standard
- `fprintf` : Affichage dans un fichier
- `sprintf` : Affichage dans une chaîne
- `snprintf` : Affichage dans une chaîne avec limite de taille

**Pourquoi c'est essentiel :**
- Comprendre les spécificateurs de format (`%d`, `%s`, `%p`, etc.)
- Maîtriser les options de formatage
- Éviter les bugs de format string

**Ce que vous apprendrez :**
```c
// Exemples tirés de la man page
printf("%d", 42);           // Entier
printf("%#x", 255);         // Hexadécimal avec préfixe 0x
printf("%10s", "hello");    // Chaîne alignée à droite sur 10 caractères
printf("%-10s", "hello");   // Chaîne alignée à gauche
printf("%.2f", 3.14159);    // Float avec 2 décimales
```

### `scanf(3)` - Lecture formatée

```bash
man 3 scanf
```

**Fonctions associées :**
- `scanf` : Lecture depuis stdin
- `fscanf` : Lecture depuis un fichier
- `sscanf` : Lecture depuis une chaîne

**Ce que vous apprendrez :**
- Format de lecture
- Gestion des espaces blancs
- Limitations et dangers

### `fopen(3)`, `fclose(3)` - Gestion de fichiers

```bash
man 3 fopen
man 3 fclose
```

**Pourquoi c'est essentiel :**
- Ouverture/fermeture de fichiers
- Les différents modes (`"r"`, `"w"`, `"a"`, `"r+"`, etc.)
- Gestion des erreurs

**Modes à retenir :**
```c
"r"   // Lecture seule
"w"   // Écriture (écrase le fichier)
"a"   // Ajout (append)
"r+"  // Lecture/écriture
"w+"  // Lecture/écriture (écrase)
"rb"  // Lecture binaire
```

### `fread(3)`, `fwrite(3)` - I/O binaires

```bash
man 3 fread
man 3 fwrite
```

**Pourquoi c'est essentiel :**
- Lecture/écriture de données binaires
- Manipulation de structures
- Performance des E/S

### `fgets(3)`, `fputs(3)` - Lignes de texte

```bash
man 3 fgets
```

**Pourquoi c'est essentiel :**
- Alternative sûre à `gets()` (qui est dangereuse)
- Lecture ligne par ligne

---

## 📚 Catégorie 2 : Gestion de la mémoire (stdlib.h)

### `malloc(3)`, `calloc(3)`, `realloc(3)`, `free(3)`

```bash
man 3 malloc
```

**Pourquoi c'est LA man page la plus importante :**
- Allocation dynamique de mémoire
- Différences entre `malloc` et `calloc`
- Comment utiliser `realloc` correctement
- Quand et comment appeler `free`
- Les causes de fuites mémoire

**Points clés de la man page :**
```c
// malloc retourne NULL en cas d'échec
void *ptr = malloc(100);
if (ptr == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
}

// calloc initialise à zéro
int *array = calloc(10, sizeof(int));

// realloc peut déplacer le bloc
ptr = realloc(ptr, 200);

// free libère la mémoire
free(ptr);
ptr = NULL;  // Bonne pratique
```

### `exit(3)`, `atexit(3)` - Terminaison de programme

```bash
man 3 exit
```

**Pourquoi c'est essentiel :**
- Codes de retour standards
- Nettoyage avant terminaison
- `EXIT_SUCCESS` vs `EXIT_FAILURE`

---

## 📚 Catégorie 3 : Manipulation de chaînes (string.h)

### `strlen(3)` - Longueur d'une chaîne

```bash
man 3 strlen
```

**Ce que vous apprendrez :**
- Calcul de longueur (sans le `\0`)
- Complexité O(n)

### `strcpy(3)`, `strncpy(3)` - Copie de chaînes

```bash
man 3 strcpy
```

**⚠️ Avertissement dans la man page :**
- `strcpy` est dangereuse (pas de vérification de taille)
- Préférer `strncpy` ou mieux `strlcpy` (BSD)

### `strcmp(3)`, `strncmp(3)` - Comparaison

```bash
man 3 strcmp
```

**Ce que vous apprendrez :**
- Comparaison lexicographique
- Valeur de retour : `< 0`, `0`, `> 0`

### `strcat(3)`, `strncat(3)` - Concaténation

```bash
man 3 strcat
```

### `strchr(3)`, `strrchr(3)` - Recherche de caractère

```bash
man 3 strchr
```

### `strstr(3)` - Recherche de sous-chaîne

```bash
man 3 strstr
```

### `strtok(3)` - Découpage de chaîne

```bash
man 3 strtok
```

**⚠️ Attention :**
- Modifie la chaîne d'origine
- N'est pas thread-safe (voir `strtok_r`)

### `memcpy(3)`, `memmove(3)`, `memset(3)`

```bash
man 3 memcpy
man 3 memmove
man 3 memset
```

**Différence cruciale :**
- `memcpy` : Zones mémoire non chevauchantes
- `memmove` : Zones peuvent se chevaucher (plus sûr, légèrement plus lent)

---

## 📚 Catégorie 4 : Appels système (Section 2)

### `open(2)`, `close(2)` - Descripteurs de fichiers

```bash
man 2 open
```

**Pourquoi c'est essentiel :**
- Différence avec `fopen` (bas niveau vs haut niveau)
- Flags : `O_RDONLY`, `O_WRONLY`, `O_CREAT`, `O_APPEND`
- Permissions (mode)
- Retourne un file descriptor (fd)

**Exemple de lecture de la man page :**
```c
#include <fcntl.h>
#include <unistd.h>

int fd = open("/tmp/file.txt", O_RDONLY);
if (fd == -1) {
    perror("open");
    return 1;
}
// ...
close(fd);
```

### `read(2)`, `write(2)` - I/O bas niveau

```bash
man 2 read
man 2 write
```

**Ce que vous apprendrez :**
- Lecture/écriture par chunks
- Gestion des erreurs (`EINTR`, `EAGAIN`)
- Valeur de retour (nombre d'octets lus/écrits)

### `fork(2)` - Création de processus

```bash
man 2 fork
```

**Pourquoi c'est fascinant :**
- Comprendre comment Linux crée des processus
- Le concept de processus parent/enfant
- Valeur de retour : PID dans le parent, 0 dans l'enfant

### `exec(3)` - Remplacement de processus

```bash
man 3 exec
```

**Famille de fonctions :**
- `execl`, `execlp`, `execle`, `execv`, `execvp`, `execvpe`

### `wait(2)`, `waitpid(2)` - Attendre un processus enfant

```bash
man 2 wait
```

### `pipe(2)` - Communication inter-processus

```bash
man 2 pipe
```

### `signal(2)`, `sigaction(2)` - Gestion des signaux

```bash
man 2 signal
man 2 sigaction
```

**⚠️ La man page recommande :**
- Préférer `sigaction` à `signal` (plus portable)

### `kill(2)` - Envoyer un signal

```bash
man 2 kill
```

### `mmap(2)`, `munmap(2)` - Mapping mémoire

```bash
man 2 mmap
```

**Pourquoi c'est avancé mais crucial :**
- Fichiers mappés en mémoire
- Mémoire partagée entre processus
- Performance des E/S

---

## 📚 Catégorie 5 : Threads POSIX (pthread.h)

### `pthread_create(3)` - Créer un thread

```bash
man 3 pthread_create
```

### `pthread_join(3)` - Attendre un thread

```bash
man 3 pthread_join
```

### `pthread_mutex_init(3)`, `pthread_mutex_lock(3)`

```bash
man 3 pthread_mutex_lock
```

**Pourquoi c'est essentiel :**
- Synchronisation entre threads
- Éviter les race conditions

### `pthread_cond_init(3)` - Variables de condition

```bash
man 3 pthread_cond_init
```

---

## 📚 Catégorie 6 : Erreurs et diagnostics

### `errno(3)` - Variable d'erreur globale

```bash
man 3 errno
```

**Pourquoi c'est crucial :**
- Toutes les fonctions système utilisent `errno`
- Doit être vérifié après chaque appel système

**Exemple de lecture :**
```c
#include <errno.h>
#include <string.h>

int fd = open("file.txt", O_RDONLY);
if (fd == -1) {
    printf("Erreur: %s\n", strerror(errno));
}
```

### `perror(3)` - Afficher l'erreur

```bash
man 3 perror
```

**Utilisation recommandée :**
```c
if (fd == -1) {
    perror("open");  // Affiche "open: No such file or directory"
}
```

### `strerror(3)` - Convertir errno en chaîne

```bash
man 3 strerror
```

---

## 📚 Catégorie 7 : Conversions et utilitaires

### `atoi(3)`, `atol(3)`, `atof(3)` - Conversions ASCII vers nombre

```bash
man 3 atoi
```

**⚠️ Limitation mentionnée dans la man page :**
- Pas de détection d'erreur
- Préférer `strtol`, `strtoll`, `strtod`

### `strtol(3)`, `strtoll(3)` - Conversions robustes

```bash
man 3 strtol
```

**Pourquoi c'est mieux :**
- Détection d'erreur
- Gestion de différentes bases (décimal, hexa, octal)

### `qsort(3)` - Tri générique

```bash
man 3 qsort
```

**Pourquoi c'est intéressant :**
- Tri avec fonction de comparaison personnalisée
- Introduction aux pointeurs de fonctions

### `bsearch(3)` - Recherche binaire

```bash
man 3 bsearch
```

---

## 📚 Catégorie 8 : Temps et dates

### `time(2)` - Temps actuel

```bash
man 2 time
```

### `localtime(3)`, `gmtime(3)` - Conversion de temps

```bash
man 3 localtime
```

### `strftime(3)` - Formatage de date

```bash
man 3 strftime
```

---

## 📚 Catégorie 9 : Divers et conventions (Section 7)

### `ascii(7)` - Table ASCII

```bash
man 7 ascii
```

**Pourquoi c'est utile :**
- Référence rapide des codes ASCII
- Valeurs décimales, hexadécimales, octales

### `utf-8(7)` - Encodage UTF-8

```bash
man 7 utf-8
```

### `signal(7)` - Liste des signaux

```bash
man 7 signal
```

**Signaux à connaître :**
- `SIGINT` : Ctrl+C
- `SIGTERM` : Terminaison propre
- `SIGKILL` : Terminaison forcée
- `SIGSEGV` : Segmentation fault
- `SIGCHLD` : Enfant terminé

### `ip(7)` - Protocole IP

```bash
man 7 ip
```

### `socket(7)` - Sockets réseau

```bash
man 7 socket
```

---

## Astuces avancées pour les man pages

### 1. Recherche dans toutes les man pages

```bash
# Rechercher "thread" dans toutes les descriptions
man -k thread

# Rechercher exactement "pthread_create"
man -k '^pthread_create$'

# Rechercher dans le contenu complet
man -K "mutex lock"  # Attention : très lent !
```

### 2. Afficher uniquement la section EXAMPLES

```bash
man printf | sed -n '/^EXAMPLES/,/^[A-Z]/p'
```

### 3. Exporter une man page en PDF

```bash
man -t printf | ps2pdf - printf.pdf
```

### 4. Afficher la man page dans le navigateur

```bash
man -H printf   # Nécessite un navigateur configuré
```

### 5. Man pages en français

```bash
# Installer les man pages en français
sudo apt install manpages-fr manpages-fr-dev

# Utiliser
LANG=fr_FR.UTF-8 man printf
```

### 6. Créer un alias pour la section 3

```bash
# Dans votre .bashrc ou .zshrc
alias man3='man 3'
alias man2='man 2'

# Utilisation
man3 printf
man2 open
```

### 7. Lister toutes les man pages d'une section

```bash
# Toutes les man pages de section 3
man -s 3 -k . | less

# Avec apropos (plus lisible)
apropos -s 3 '.*'
```

---

## Configuration de man

### Fichier de configuration

Le fichier `/etc/man_db.conf` (ou `/etc/manpath.config`) contrôle :
- Les chemins de recherche
- Les sections affichées
- Le pager utilisé

### Personnaliser le pager

Par défaut, `man` utilise `less`. Vous pouvez le changer :

```bash
export MANPAGER="most"      # Pager coloré
export MANPAGER="vim -M -"  # Utiliser vim comme pager
```

### Activer les couleurs

```bash
# Ajouter dans votre .bashrc ou .zshrc
export LESS_TERMCAP_mb=$'\e[1;32m'     # Begin bold
export LESS_TERMCAP_md=$'\e[1;34m'     # Begin blink
export LESS_TERMCAP_me=$'\e[0m'        # End mode
export LESS_TERMCAP_se=$'\e[0m'        # End standout
export LESS_TERMCAP_so=$'\e[01;44;33m' # Begin standout
export LESS_TERMCAP_ue=$'\e[0m'        # End underline
export LESS_TERMCAP_us=$'\e[1;4;31m'   # Begin underline
```

---

## Alternatives modernes aux man pages

### 1. tldr - Pages simplifiées et exemples

```bash
# Installation
sudo apt install tldr
tldr --update

# Utilisation
tldr tar
tldr rsync
```

**Avantage :** Exemples concrets immédiatement exploitables.

### 2. cheat - Cheat sheets communautaires

```bash
# Installation
pip install cheat

# Utilisation
cheat tar
cheat ssh
```

### 3. info - Système GNU Info

```bash
info libc   # Documentation complète de la glibc
```

### 4. Documentation en ligne

- **cppreference.com/c** : Documentation C moderne et claire
- **man7.org** : Man pages Linux en ligne
- **pubs.opengroup.org** : Standards POSIX officiels

---

## Exercice de navigation pratique

Voici un parcours typique pour apprendre à utiliser les man pages :

### Scénario : Vous voulez écrire dans un fichier

**1. Trouvez les fonctions disponibles**
```bash
man -k "write file"
```

**2. Lisez la description de `fwrite`**
```bash
man 3 fwrite
```

**3. Notez le prototype**
```c
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
```

**4. Lisez la section RETURN VALUE**
> Returns the number of items successfully written

**5. Lisez les ERRORS**
> Check ferror() to detect write errors

**6. Regardez les fonctions liées (SEE ALSO)**
> fread(3), fopen(3), fclose(3), fprintf(3)

**7. Vérifiez un exemple**
```c
// Exemple typique dans la man page
FILE *fp = fopen("output.dat", "wb");
if (fp == NULL) {
    perror("fopen");
    return 1;
}

size_t written = fwrite(data, sizeof(data[0]), n, fp);
if (written != n) {
    if (ferror(fp))
        fprintf(stderr, "Write error\n");
}

fclose(fp);
```

---

## Checklist : Man pages à maîtriser par niveau

### 🟢 Niveau Débutant
- [ ] `man(1)` - Comment utiliser man
- [ ] `printf(3)` - Affichage formaté
- [ ] `scanf(3)` - Lecture formatée
- [ ] `malloc(3)` - Allocation mémoire
- [ ] `strlen(3)`, `strcpy(3)`, `strcmp(3)` - Chaînes de base
- [ ] `fopen(3)`, `fclose(3)` - Fichiers

### 🟡 Niveau Intermédiaire
- [ ] `open(2)`, `read(2)`, `write(2)` - I/O bas niveau
- [ ] `errno(3)`, `perror(3)` - Gestion d'erreurs
- [ ] `fork(2)`, `exec(3)`, `wait(2)` - Processus
- [ ] `signal(7)` - Signaux système
- [ ] `strtol(3)` - Conversions robustes
- [ ] `memcpy(3)`, `memmove(3)` - Manipulation mémoire

### 🔴 Niveau Avancé
- [ ] `pthread_create(3)` et famille - Threads
- [ ] `mmap(2)` - Mémoire mappée
- [ ] `pipe(2)` - IPC
- [ ] `socket(2)`, `bind(2)`, `listen(2)` - Réseau
- [ ] `sigaction(2)` - Gestion avancée des signaux
- [ ] `select(2)`, `poll(2)`, `epoll(7)` - I/O multiplexing

---

## Conclusion et bonnes pratiques

### Les réflexes à adopter

✅ **AVANT d'écrire du code :**
1. Lisez la man page de la fonction
2. Notez les headers nécessaires
3. Comprenez la valeur de retour
4. Identifiez les erreurs possibles

✅ **Quand vous avez un bug :**
1. Relisez la man page
2. Vérifiez que vous respectez les préconditions
3. Consultez la section BUGS

✅ **Pour progresser :**
1. Lisez une nouvelle man page par jour
2. Reproduisez les exemples
3. Explorez les liens "SEE ALSO"

### Commandes à retenir

```bash
# Les 5 commandes man les plus utiles
man <fonction>           # Lire une man page
man -k <mot_clé>         # Rechercher dans les descriptions
man <section> <nom>      # Spécifier la section
man -f <nom>             # Lister toutes les sections
/<terme>                 # Rechercher dans la page (une fois ouverte)
```

### Man pages comme référence ultime

Les man pages ne sont pas seulement de la documentation, ce sont :
- **La source de vérité** : Elles reflètent exactement ce qui est implémenté sur votre système
- **Toujours disponibles** : Pas besoin d'Internet
- **Mises à jour automatiquement** : Quand vous mettez à jour votre système
- **Standards POSIX** : Garantie de portabilité

**En tant que programmeur C, vous devez considérer les man pages comme votre extension de mémoire. Ne les évitez pas, embrassez-les !**

---


⏭️ [Livres de référence recommandés](/annexes/references-essentielles/03-livres-reference.md)
