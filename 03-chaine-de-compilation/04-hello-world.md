🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 3.4 Démystification du "Hello World"

## Introduction

Tout programmeur C a commencé par écrire ce programme :

```c
#include <stdio.h>

int main(void) {
    printf("Hello, World!\n");
    return 0;
}
```

Ces quelques lignes semblent simples, mais elles cachent une machinerie complexe. Dans cette section, nous allons **décortiquer entièrement** ce programme pour comprendre ce qui se passe réellement, de la compilation jusqu'à l'affichage du texte à l'écran.

Préparez-vous à découvrir que même le plus simple des programmes est une merveille d'ingénierie !

---

## Le Code Source : Analyse Ligne par Ligne

Reprenons notre programme et examinons chaque ligne en détail.

### Ligne 1 : `#include <stdio.h>`

```c
#include <stdio.h>
```

**Que fait cette ligne ?**

C'est une **directive de préprocesseur**. Elle demande au préprocesseur d'insérer le contenu du fichier `stdio.h` à cet endroit.

**Qu'est-ce que `stdio.h` ?**

- `stdio` = **St**andar**d** **I**nput/**O**utput (Entrées/Sorties standard)
- `.h` = fichier d'en-tête (*header*)
- C'est un fichier qui contient les **déclarations** des fonctions d'entrée/sortie comme `printf`, `scanf`, `fopen`, etc.

**Pourquoi est-ce nécessaire ?**

En C, vous devez **déclarer** une fonction avant de l'utiliser. Sans `#include <stdio.h>`, le compilateur ne connaîtrait pas `printf` et afficherait une erreur (ou un warning selon la version).

**Les chevrons `< >` vs guillemets `" "`**

```c
#include <stdio.h>    // Cherche dans les répertoires système
#include "monfichier.h"  // Cherche d'abord dans le répertoire courant
```

### Ligne 3 : `int main(void)`

```c
int main(void)
```

**Que représente cette ligne ?**

C'est la **déclaration de la fonction `main`**, le point d'entrée de tout programme C.

**Décomposition :**

| Élément | Signification |
|---------|---------------|
| `int` | Type de retour : la fonction renvoie un entier |
| `main` | Nom de la fonction (imposé, pas de choix) |
| `void` | La fonction ne prend aucun paramètre |

**Pourquoi `main` est-elle spéciale ?**

- C'est la **première fonction** appelée quand votre programme démarre
- Son nom est imposé par le standard C
- Tout programme C doit avoir exactement une fonction `main`

**Variantes de `main` :**

```c
int main(void)                    // Sans arguments  
int main(int argc, char *argv[])  // Avec arguments de ligne de commande  
int main(int argc, char **argv)   // Équivalent
```

### Ligne 4 : L'Accolade Ouvrante `{`

```c
{
```

Marque le **début du corps** de la fonction `main`. Tout le code entre `{` et `}` appartient à cette fonction.

### Ligne 5 : `printf("Hello, World!\n");`

```c
    printf("Hello, World!\n");
```

C'est le cœur de notre programme. Analysons-le en détail.

**`printf`** : Fonction de la bibliothèque standard C qui affiche du texte formaté.

**`"Hello, World!\n"`** : Une **chaîne de caractères** (string literal).

**Le `\n`** : Un **caractère d'échappement** représentant un saut de ligne (*newline*).

**Le `;`** : Termine l'instruction (obligatoire en C).

**Les caractères d'échappement courants :**

| Séquence | Signification |
|----------|---------------|
| `\n` | Nouvelle ligne (newline) |
| `\t` | Tabulation |
| `\\` | Antislash littéral |
| `\"` | Guillemet double |
| `\0` | Caractère nul (fin de chaîne) |

### Ligne 6 : `return 0;`

```c
    return 0;
```

**Que fait cette ligne ?**

- Termine l'exécution de `main`
- Renvoie la valeur `0` au système d'exploitation

**Que signifie la valeur de retour ?**

| Valeur | Signification conventionnelle |
|--------|-------------------------------|
| `0` | Succès, tout s'est bien passé |
| `!= 0` | Erreur (le code indique le type d'erreur) |

**Comment vérifier cette valeur ?**

```bash
./hello
echo $?  # Affiche le code de retour du dernier programme
# Résultat : 0
```

### Ligne 7 : L'Accolade Fermante `}`

```c
}
```

Marque la **fin du corps** de la fonction `main`.

---

## Ce Qui Se Passe à la Compilation

Rappelons les 4 étapes de la compilation et voyons ce qui arrive à notre Hello World.

### Étape 1 : Préprocesseur

Le préprocesseur remplace `#include <stdio.h>` par le contenu de ce fichier.

**Avant :**
```c
#include <stdio.h>

int main(void) {
    printf("Hello, World!\n");
    return 0;
}
```

**Après (simplifié) :**
```c
/* Contenu de stdio.h : environ 800 lignes */
typedef struct _IO_FILE FILE;  
extern int printf(const char *format, ...);
/* ... des centaines d'autres déclarations ... */

int main(void) {
    printf("Hello, World!\n");
    return 0;
}
```

Notre fichier de 7 lignes devient un fichier de **~800 lignes** !

### Étape 2 : Compilation

Le compilateur transforme le code C en assembleur :

```asm
    .file   "hello.c"
    .section    .rodata
.LC0:
    .string "Hello, World!"
    .text
    .globl  main
    .type   main, @function
main:
    pushq   %rbp
    movq    %rsp, %rbp
    leaq    .LC0(%rip), %rdi
    call    puts@PLT
    movl    $0, %eax
    popq    %rbp
    ret
```

**Observations intéressantes :**

1. Notre chaîne `"Hello, World!"` est stockée dans `.rodata` (données en lecture seule)
2. Le `\n` a disparu car GCC a optimisé `printf("...\n")` en `puts("...")`
3. Le `return 0` devient `movl $0, %eax` (la valeur de retour est placée dans le registre `eax`)

### Étape 3 : Assemblage

L'assembleur convertit le code assembleur en code machine (binaire).

```bash
gcc -c hello.c -o hello.o  
hexdump -C hello.o | head -5
# 00000000  7f 45 4c 46 02 01 01 00  00 00 00 00 00 00 00 00  |.ELF............|
```

Le fichier commence par `7f 45 4c 46` qui correspond à `.ELF` — le format des exécutables Linux.

### Étape 4 : Édition de Liens

Le linker assemble notre code avec la bibliothèque C standard pour créer l'exécutable final.

```
┌─────────────────────────────────────────────────────────────┐
│  ÉDITION DE LIENS DU HELLO WORLD                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   hello.o                    libc.so.6                      │
│   ┌─────────────────┐        ┌─────────────────────┐        │
│   │ main:           │        │ printf: ●───────────┼──┐     │
│   │   call puts ────┼───────▶│ puts: ●             │  │     │
│   │   movl $0, %eax │        │ exit: ●             │  │     │
│   └─────────────────┘        │ _start: ●           │  │     │
│                              └─────────────────────┘  │     │
│                                                       │     │
│                              ld-linux.so              │     │
│                              (chargeur dynamique)     │     │
│                              ┌─────────────────────┐  │     │
│                              │ Résout les symboles │◀─┘     │
│                              │ au démarrage        │        │
│                              └─────────────────────┘        │
│                                       │                     │
│                                       ▼                     │
│                              ┌─────────────────────┐        │
│                              │      hello          │        │
│                              │   (exécutable)      │        │
│                              └─────────────────────┘        │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## Ce Qui Se Passe à l'Exécution

Quand vous tapez `./hello` et appuyez sur Entrée, une série d'événements se produit.

### 1. Le Shell Interprète la Commande

Votre shell (bash, zsh, etc.) lit `./hello` et comprend que vous voulez exécuter le fichier `hello` dans le répertoire courant.

### 2. Le Système Crée un Nouveau Processus

Le shell demande au noyau Linux de créer un nouveau processus via l'appel système `fork()`, puis de remplacer ce processus par votre programme via `execve()`.

### 3. Le Chargeur Dynamique Entre en Jeu

Avant même que `main` ne s'exécute, le **chargeur dynamique** (`ld-linux.so`) :

1. Charge votre exécutable en mémoire
2. Charge les bibliothèques partagées nécessaires (`libc.so.6`)
3. Résout les symboles (connecte `puts` à son code dans la libc)
4. Effectue les relocations (ajuste les adresses)

### 4. Le Code d'Initialisation S'exécute

Surprise ! `main` n'est pas la **vraie** première fonction exécutée. La libc exécute d'abord du code d'initialisation :

```
┌─────────────────────────────────────────────────────────────┐
│  SÉQUENCE DE DÉMARRAGE RÉELLE                               │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ./hello                                                    │
│      │                                                      │
│      ▼                                                      │
│  ┌─────────────────────────────────────┐                    │
│  │ _start (point d'entrée ELF)         │                    │
│  │ Fourni par la libc (crt1.o)         │                    │
│  └──────────────┬──────────────────────┘                    │
│                 │                                           │
│                 ▼                                           │
│  ┌─────────────────────────────────────┐                    │
│  │ __libc_start_main                   │                    │
│  │ - Initialise la libc                │                    │
│  │ - Configure argc, argv, envp        │                    │
│  │ - Enregistre les fonctions atexit   │                    │
│  └──────────────┬──────────────────────┘                    │
│                 │                                           │
│                 ▼                                           │
│  ┌─────────────────────────────────────┐                    │
│  │ main (votre code)                   │  ◀── ENFIN !       │
│  │ - printf("Hello, World!\n")         │                    │
│  │ - return 0                          │                    │
│  └──────────────┬──────────────────────┘                    │
│                 │                                           │
│                 ▼                                           │
│  ┌─────────────────────────────────────┐                    │
│  │ exit                                │                    │
│  │ - Appelle les fonctions atexit      │                    │
│  │ - Ferme les flux (stdout, etc.)     │                    │
│  │ - Appelle _exit (syscall)           │                    │
│  └─────────────────────────────────────┘                    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 5. `main` S'exécute

Finalement, votre fonction `main` est appelée. Elle exécute `printf("Hello, World!\n")`.

### 6. `printf` Fait Son Travail

`printf` est une fonction complexe qui :

1. Parse la chaîne de format (cherche les `%d`, `%s`, etc.)
2. Formate la sortie en mémoire (dans un buffer)
3. Écrit le résultat vers la sortie standard (stdout)

### 7. Les Données Atteignent l'Écran

Voici le chemin parcouru par notre texte :

```
┌─────────────────────────────────────────────────────────────┐
│  VOYAGE DE "Hello, World!" JUSQU'À L'ÉCRAN                  │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  printf("Hello, World!\n")                                  │
│      │                                                      │
│      │  (1) Formate la chaîne                               │
│      ▼                                                      │
│  ┌─────────────────────────────────────┐                    │
│  │ Buffer stdio (en mémoire utilisateur)│                   │
│  │ "Hello, World!\n"                   │                    │
│  └──────────────┬──────────────────────┘                    │
│                 │                                           │
│                 │  (2) Le buffer est "flushé"               │
│                 │      (vidé) car il y a un \n              │
│                 ▼                                           │
│  ┌─────────────────────────────────────┐                    │
│  │ Appel système write()               │                    │
│  │ write(1, "Hello, World!\n", 14)     │                    │
│  └──────────────┬──────────────────────┘                    │
│                 │                                           │
│  ═══════════════╪═══════════════════════  (frontière        │
│                 │                          user/kernel)     │
│                 ▼                                           │
│  ┌─────────────────────────────────────┐                    │
│  │ Noyau Linux                         │                    │
│  │ - Vérifie le descripteur de fichier │                    │
│  │ - Copie les données                 │                    │
│  └──────────────┬──────────────────────┘                    │
│                 │                                           │
│                 ▼                                           │
│  ┌─────────────────────────────────────┐                    │
│  │ Pilote du terminal (tty)            │                    │
│  │ - Interprète les caractères         │                    │
│  │ - Gère le \n (nouvelle ligne)       │                    │
│  └──────────────┬──────────────────────┘                    │
│                 │                                           │
│                 ▼                                           │
│  ┌─────────────────────────────────────┐                    │
│  │ Émulateur de terminal / Console     │                    │
│  │                                     │                    │
│  │     Hello, World!                   │                    │
│  │     █                               │  ◀── Vous voyez    │
│  │                                     │      le texte !    │
│  └─────────────────────────────────────┘                    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 8. Le Programme Se Termine

Quand `main` retourne `0` :

1. La valeur `0` est passée à `exit()`
2. `exit()` effectue le nettoyage (ferme les fichiers, vide les buffers)
3. L'appel système `_exit()` est invoqué
4. Le noyau libère les ressources du processus
5. Le code de retour `0` est transmis au shell

---

## Les Coulisses de `printf`

`printf` est bien plus qu'une simple fonction d'affichage. C'est un véritable interpréteur de format.

### La Signature de `printf`

```c
int printf(const char *format, ...);
```

- `const char *format` : La chaîne de format
- `...` : Un nombre variable d'arguments (fonction variadique)
- Retourne : Le nombre de caractères écrits (ou négatif si erreur)

### Les Spécificateurs de Format

Même si notre Hello World n'en utilise pas, `printf` peut interpréter :

| Spécificateur | Type attendu | Exemple |
|---------------|--------------|---------|
| `%d` ou `%i` | `int` | `printf("%d", 42)` → `42` |
| `%u` | `unsigned int` | `printf("%u", 42)` → `42` |
| `%f` | `double` | `printf("%f", 3.14)` → `3.140000` |
| `%c` | `char` | `printf("%c", 'A')` → `A` |
| `%s` | `char *` | `printf("%s", "Hi")` → `Hi` |
| `%p` | `void *` | `printf("%p", ptr)` → `0x7fff...` |
| `%x` | `unsigned int` (hex) | `printf("%x", 255)` → `ff` |
| `%%` | (aucun) | `printf("%%")` → `%` |

### Ce Que `printf` Fait Vraiment

```
┌─────────────────────────────────────────────────────────────┐
│  FONCTIONNEMENT INTERNE DE printf                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  printf("Valeur: %d, Nom: %s\n", 42, "Alice");              │
│                                                             │
│  1. Parse la chaîne de format :                             │
│     ┌─────────────────────────────────────────────┐         │
│     │ "Valeur: " │ %d │ ", Nom: " │ %s │ "\n"     │         │
│     └─────────────────────────────────────────────┘         │
│         texte      spec   texte     spec  texte             │
│                                                             │
│  2. Pour chaque élément :                                   │
│     - Texte → copie directement dans le buffer              │
│     - Spécificateur → récupère l'argument correspondant     │
│                       et le convertit en texte              │
│                                                             │
│  3. Résultat dans le buffer :                               │
│     ┌─────────────────────────────────────────────┐         │
│     │ V │ a │ l │ e │ u │ r │ : │   │ 4 │ 2 │...  │         │
│     └─────────────────────────────────────────────┘         │
│     "Valeur: 42, Nom: Alice\n"                              │
│                                                             │
│  4. Écrit le buffer vers stdout                             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Pourquoi `puts` et non `printf` ?

Si vous examinez le code assembleur généré, vous verrez souvent `puts` au lieu de `printf`. C'est une **optimisation** de GCC :

- `printf("Hello\n")` → Pas de formatage nécessaire
- GCC le remplace par `puts("Hello")` qui est plus simple et plus rapide

`puts` ajoute automatiquement un `\n`, donc GCC retire le `\n` de la chaîne.

---

## La Chaîne de Caractères en Mémoire

Notre chaîne `"Hello, World!\n"` est stockée d'une façon particulière en mémoire.

### Représentation en Mémoire

```
┌─────────────────────────────────────────────────────────────┐
│  "Hello, World!\n" EN MÉMOIRE                               │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Adresse   Contenu     Caractère   Code ASCII               │
│  ───────────────────────────────────────────────            │
│  0x1000    0x48        'H'         72                       │
│  0x1001    0x65        'e'         101                      │
│  0x1002    0x6C        'l'         108                      │
│  0x1003    0x6C        'l'         108                      │
│  0x1004    0x6F        'o'         111                      │
│  0x1005    0x2C        ','         44                       │
│  0x1006    0x20        ' '         32                       │
│  0x1007    0x57        'W'         87                       │
│  0x1008    0x6F        'o'         111                      │
│  0x1009    0x72        'r'         114                      │
│  0x100A    0x6C        'l'         108                      │
│  0x100B    0x64        'd'         100                      │
│  0x100C    0x21        '!'         33                       │
│  0x100D    0x0A        '\n'        10  (newline)            │
│  0x100E    0x00        '\0'        0   (terminateur)        │
│                                    ▲                        │
│                                    │                        │
│                            Caractère nul :                  │
│                            marque la fin de                 │
│                            la chaîne                        │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**Points importants :**

1. Chaque caractère occupe **1 octet**
2. La chaîne se termine par `\0` (caractère nul), ajouté automatiquement
3. La chaîne fait donc **15 octets** (14 caractères + le terminateur)
4. Elle est stockée dans la section `.rodata` (lecture seule)

---

## Anatomie de l'Exécutable Final

Notre minuscule programme de 7 lignes produit un exécutable d'environ **16 Ko**. Pourquoi ?

### Structure d'un Exécutable ELF

```
┌─────────────────────────────────────────────────────────────┐
│  STRUCTURE DE L'EXÉCUTABLE hello                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────┐                    │
│  │         En-tête ELF                 │  64 octets         │
│  │  - Magic number: 7f 45 4c 46        │  (identifie ELF)   │
│  │  - Architecture: x86-64             │                    │
│  │  - Point d'entrée: _start           │                    │
│  └─────────────────────────────────────┘                    │
│                                                             │
│  ┌─────────────────────────────────────┐                    │
│  │      Program Headers                │                    │
│  │  - Segments à charger en mémoire    │                    │
│  │  - Permissions (lecture/exécution)  │                    │
│  └─────────────────────────────────────┘                    │
│                                                             │
│  ┌─────────────────────────────────────┐                    │
│  │         .text (code)                │  ~200 octets       │
│  │  - main                             │                    │
│  │  - Code d'initialisation            │                    │
│  └─────────────────────────────────────┘                    │
│                                                             │
│  ┌─────────────────────────────────────┐                    │
│  │        .rodata (données RO)         │  ~50 octets        │
│  │  - "Hello, World!"                  │                    │
│  └─────────────────────────────────────┘                    │
│                                                             │
│  ┌─────────────────────────────────────┐                    │
│  │        .dynamic                     │                    │
│  │  - Liste des bibliothèques requises │                    │
│  │  - libc.so.6                        │                    │
│  └─────────────────────────────────────┘                    │
│                                                             │
│  ┌─────────────────────────────────────┐                    │
│  │        .symtab / .strtab            │                    │
│  │  - Table des symboles               │                    │
│  │  - Noms des fonctions               │                    │
│  └─────────────────────────────────────┘                    │
│                                                             │
│  ┌─────────────────────────────────────┐                    │
│  │       Section Headers               │                    │
│  │  - Métadonnées sur les sections     │                    │
│  └─────────────────────────────────────┘                    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Examiner l'Exécutable

```bash
# Taille du fichier
ls -l hello
# -rwxr-xr-x 1 user user 16696 ... hello

# Informations sur le format
file hello
# hello: ELF 64-bit LSB pie executable, x86-64, ...

# Voir les sections
size hello
#    text    data     bss     dec     hex filename
#    1418     600       8    2026     7ea hello

# Voir les bibliothèques liées
ldd hello
#     linux-vdso.so.1 (0x00007fff...)
#     libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f...)
#     /lib64/ld-linux-x86-64.so.2 (0x00007f...)
```

---

## Les Appels Système Impliqués

Même notre simple Hello World fait appel au noyau Linux via des **appels système** (syscalls).

### Tracer les Appels Système

L'outil `strace` permet de voir tous les appels système :

```bash
strace ./hello
```

**Sortie simplifiée :**

```
execve("./hello", ["./hello"], ...) = 0  
brk(NULL)                           = 0x55a...  
mmap(...)                           = 0x7f3...  # Allocation mémoire  
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", ...) = 3  
read(3, "\177ELF..."..., 832)       = 832  # Lecture de la libc  
close(3)                            = 0
...
write(1, "Hello, World!\n", 14)     = 14  # ← Notre printf !  
exit_group(0)                       = ?    # ← Notre return 0
```

### Les Syscalls Clés

| Syscall | Rôle dans notre programme |
|---------|---------------------------|
| `execve` | Lance notre programme |
| `mmap` | Alloue de la mémoire |
| `openat` | Ouvre la libc |
| `read` | Lit le contenu de la libc |
| `write` | Écrit "Hello, World!\n" sur stdout |
| `exit_group` | Termine le programme |

Notre petit `printf` se traduit par un appel système `write(1, "Hello, World!\n", 14)` :
- `1` = descripteur de fichier pour stdout
- `14` = nombre d'octets à écrire

---

## Récapitulatif : Du Code à l'Écran

Voici le voyage complet de notre Hello World :

```
┌─────────────────────────────────────────────────────────────┐
│  LE VOYAGE COMPLET DU HELLO WORLD                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. ÉCRITURE                                                │
│     Vous écrivez hello.c (7 lignes)                         │
│                     │                                       │
│                     ▼                                       │
│  2. PRÉPROCESSEUR                                           │
│     #include → 800 lignes                                   │
│                     │                                       │
│                     ▼                                       │
│  3. COMPILATION                                             │
│     C → Assembleur (~30 lignes)                             │
│                     │                                       │
│                     ▼                                       │
│  4. ASSEMBLAGE                                              │
│     Assembleur → Code machine                               │
│                     │                                       │
│                     ▼                                       │
│  5. ÉDITION DE LIENS                                        │
│     + libc → Exécutable (~16 Ko)                            │
│                     │                                       │
│                     ▼                                       │
│  6. EXÉCUTION                                               │
│     ./hello                                                 │
│         │                                                   │
│         ├── Chargement en mémoire                           │
│         ├── Résolution des symboles                         │
│         ├── _start → __libc_start_main → main               │
│         │                                                   │
│         ▼                                                   │
│  7. printf("Hello, World!\n")                               │
│         │                                                   │
│         ├── Formatage de la chaîne                          │
│         ├── Écriture dans le buffer                         │
│         ├── Flush du buffer (grâce au \n)                   │
│         │                                                   │
│         ▼                                                   │
│  8. write(1, "Hello, World!\n", 14)                         │
│         │                                                   │
│         ├── Appel système vers le noyau                     │
│         ├── Pilote TTY                                      │
│         │                                                   │
│         ▼                                                   │
│  9. AFFICHAGE                                               │
│                                                             │
│     ┌──────────────────────────────┐                        │
│     │  $ ./hello                   │                        │
│     │  Hello, World!               │                        │
│     │  $                           │                        │
│     └──────────────────────────────┘                        │
│                                                             │
│ 10. TERMINAISON                                             │
│     return 0 → exit(0) → code de retour au shell            │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## Conclusion

Ce simple programme de 7 lignes met en œuvre :

- Le **préprocesseur** et l'inclusion de fichiers
- Le **compilateur** et la génération de code
- L'**assembleur** et le code machine
- L'**éditeur de liens** et les bibliothèques partagées
- Le **chargeur dynamique** et la résolution de symboles
- La **bibliothèque C standard** et ses fonctions
- Les **appels système** Linux
- La gestion des **processus** par le noyau
- Le système de **fichiers** et les descripteurs
- L'affichage via le **terminal**

Le Hello World n'est pas qu'un rite de passage pour les programmeurs débutants. C'est une fenêtre sur l'ensemble de la pile logicielle, du code source jusqu'au matériel.

Maintenant, quand vous verrez `Hello, World!` s'afficher à l'écran, vous saurez tout ce qui se cache derrière ces quelques caractères !

⏭️ [Inspection des fichiers intermédiaires](/03-chaine-de-compilation/05-fichiers-intermediaires.md)
