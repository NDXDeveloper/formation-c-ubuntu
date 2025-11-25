🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 3.2 Visualisation du Pipeline de Compilation

## Introduction

Dans la section précédente, nous avons découvert les 4 étapes de la chaîne de compilation. Maintenant, nous allons **visualiser concrètement** ce pipeline en action. Voir les transformations de votre code à chaque étape vous aidera à mieux comprendre ce qui se passe « sous le capot » quand vous compilez un programme.

---

## Le Pipeline Complet en Un Coup d'Œil

Voici une représentation visuelle complète du chemin parcouru par votre code :

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                        PIPELINE DE COMPILATION GCC                           │
└──────────────────────────────────────────────────────────────────────────────┘

  VOUS ÉCRIVEZ                                                    VOUS EXÉCUTEZ
       │                                                               │
       ▼                                                               ▼
  ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────────┐
  │ hello.c │───▶│ hello.i │───▶│ hello.s │───▶│ hello.o │───▶│   hello     │
  │         │    │         │    │         │    │         │    │ (exécutable)│
  └─────────┘    └─────────┘    └─────────┘    └─────────┘    └─────────────┘
       │              │              │              │               │
   Code C        Code C         Code          Code            Programme
   source       prétraité     assembleur     machine           prêt !
                (étendu)      (textuel)      (binaire)

       │              │              │              │               │
       └──────┬───────┴──────┬───────┴──────┬───────┴───────┬───────┘
              │              │              │               │
              ▼              ▼              ▼               ▼
         PRÉPROCESSEUR   COMPILATEUR   ASSEMBLEUR      LINKER
            (cpp)          (cc1)         (as)           (ld)
```

Chaque outil transforme le fichier d'entrée en un format différent, jusqu'à obtenir l'exécutable final.

---

## Anatomie des Fichiers à Chaque Étape

Prenons un programme simple et observons sa transformation à travers le pipeline.

### Le Code Source Initial (hello.c)

```c
#include <stdio.h>

#define AUTEUR "Alice"
#define ANNEE 2025

// Fonction principale
int main(void) {
    printf("Programme créé par %s en %d\n", AUTEUR, ANNEE);
    return 0;
}
```

Ce fichier fait environ **15 lignes**. Voyons comment il évolue.

---

### Étape 1 : Après le Préprocesseur (hello.i)

```bash
gcc -E hello.c -o hello.i
```

**Taille approximative : ~800 lignes** (oui, vraiment !)

Le fichier `hello.i` ressemble à ceci (version très simplifiée) :

```c
# 1 "hello.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "hello.c"
# 1 "/usr/include/stdio.h" 1 3 4
# 27 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/libc-header-start.h" 1 3 4
/* ... des centaines de lignes de stdio.h et ses dépendances ... */

typedef struct _IO_FILE FILE;
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

extern int printf (const char *__format, ...);
extern int scanf (const char *__format, ...);
/* ... encore beaucoup de déclarations ... */

# 2 "hello.c" 2

int main(void) {
    printf("Programme créé par %s en %d\n", "Alice", 2025);
    return 0;
}
```

**Ce qui a changé :**

| Avant | Après |
|-------|-------|
| `#include <stdio.h>` | ~700 lignes de déclarations |
| `AUTEUR` | `"Alice"` |
| `ANNEE` | `2025` |
| `// Fonction principale` | *(supprimé)* |

```
┌─────────────────────────────────────────────────────────────┐
│  VISUALISATION DE L'EXPANSION                               │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   hello.c (15 lignes)                                       │
│   ┌─────────────────┐                                       │
│   │ #include <stdio>│ ──────┐                               │
│   │ #define AUTEUR  │       │                               │
│   │ #define ANNEE   │       │  PRÉPROCESSEUR                │
│   │ int main() {    │       │                               │
│   │   printf(...)   │       ▼                               │
│   │ }               │  ┌─────────────────┐                  │
│   └─────────────────┘  │ /* stdio.h */   │                  │
│                        │ typedef ...     │                  │
│                        │ extern printf   │                  │
│                        │ /* ~700 lignes*/│                  │
│                        │ ─────────────── │                  │
│                        │ int main() {    │                  │
│                        │  printf("Alice",│                  │
│                        │         2025)   │                  │
│                        │ }               │                  │
│                        └─────────────────┘                  │
│                        hello.i (~800 lignes)                │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

### Étape 2 : Après la Compilation (hello.s)

```bash
gcc -S hello.c -o hello.s
```

**Taille approximative : ~30 lignes**

Le fichier `hello.s` contient du code assembleur :

```asm
	.file	"hello.c"
	.text
	.section	.rodata
.LC0:
	.string	"Programme cr\303\251\303\251 par %s en %d\n"
.LC1:
	.string	"Alice"
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$2025, %edx
	leaq	.LC1(%rip), %rax
	movq	%rax, %rsi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %eax
	popq	%rbp
	ret
	.size	main, .-main
```

**Décryptage pour débutants :**

```
┌─────────────────────────────────────────────────────────────┐
│  STRUCTURE DU CODE ASSEMBLEUR                               │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  .section .rodata          ◄── Section des données          │
│  .LC0:                         en lecture seule             │
│      .string "Programme..."    (nos chaînes de caractères)  │
│  .LC1:                                                      │
│      .string "Alice"                                        │
│                                                             │
│  ─────────────────────────────────────────────────────────  │
│                                                             │
│  .text                     ◄── Section du code exécutable   │
│  main:                                                      │
│      pushq  %rbp           ◄── Sauvegarde du contexte       │
│      movq   %rsp, %rbp                                      │
│                                                             │
│      movl   $2025, %edx    ◄── Prépare l'argument "2025"    │
│      leaq   .LC1, %rsi     ◄── Prépare l'argument "Alice"   │
│      leaq   .LC0, %rdi     ◄── Prépare le format string     │
│                                                             │
│      call   printf@PLT     ◄── Appelle printf               │
│                                                             │
│      movl   $0, %eax       ◄── Prépare return 0             │
│      ret                   ◄── Retourne                     │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

Le code C a été traduit en instructions que le processeur peut comprendre (après une dernière transformation).

---

### Étape 3 : Après l'Assemblage (hello.o)

```bash
gcc -c hello.c -o hello.o
```

**Taille approximative : ~1.5 Ko**

Le fichier `.o` est **binaire**. Vous ne pouvez pas le lire avec un éditeur de texte :

```bash
cat hello.o
# Affiche des caractères illisibles : ELF>�@@...
```

Mais vous pouvez l'inspecter avec des outils spécialisés :

```bash
# Voir la structure du fichier
file hello.o
# Résultat : hello.o: ELF 64-bit LSB relocatable, x86-64, ...

# Voir les symboles définis
nm hello.o
# Résultat :
#                  U printf
# 0000000000000000 T main

# Voir le désassemblage
objdump -d hello.o
```

**Signification des symboles :**

```
┌─────────────────────────────────────────────────────────────┐
│  TABLE DES SYMBOLES (nm hello.o)                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  SYMBOLE        TYPE    SIGNIFICATION                       │
│  ─────────────────────────────────────────────────────────  │
│                                                             │
│  main           T       Défini ici (T = Text/Code)          │
│                         C'est notre fonction main()         │
│                                                             │
│  printf         U       Non défini (U = Undefined)          │
│                         Utilisé mais pas encore trouvé      │
│                         → Sera résolu par le linker         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

Le fichier objet contient le code machine de `main`, mais l'adresse de `printf` est encore inconnue (marquée `U`). C'est le linker qui va résoudre ce mystère.

---

### Étape 4 : Après l'Édition de Liens (hello)

```bash
gcc hello.o -o hello
# ou directement : gcc hello.c -o hello
```

**Taille approximative : ~16 Ko**

L'exécutable final est prêt :

```bash
file hello
# Résultat : hello: ELF 64-bit LSB executable, x86-64, dynamically linked, ...

./hello
# Résultat : Programme créé par Alice en 2025
```

**Ce que le linker a fait :**

```
┌─────────────────────────────────────────────────────────────┐
│  RÉSOLUTION DES SYMBOLES PAR LE LINKER                      │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  hello.o                      libc.so (bibliothèque C)      │
│  ┌─────────────┐              ┌─────────────────────┐       │
│  │ main: T     │              │ printf: T           │       │
│  │ printf: U ──┼──────────────┼─▶ (code de printf)  │       │
│  └─────────────┘              │ scanf: T            │       │
│                               │ malloc: T           │       │
│        │                      │ ...                 │       │
│        │                      └─────────────────────┘       │
│        │                                                    │
│        ▼                                                    │
│  ┌─────────────────────────────────────────────┐            │
│  │              hello (exécutable)             │            │
│  │  ┌─────────────────────────────────────┐    │            │
│  │  │ main: appelle printf à l'adresse X  │    │            │
│  │  │ printf: référence vers libc.so      │    │            │
│  │  └─────────────────────────────────────┘    │            │
│  └─────────────────────────────────────────────┘            │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## Visualisation de l'Évolution des Tailles

Un aspect frappant du pipeline est l'évolution de la taille des fichiers :

```
┌─────────────────────────────────────────────────────────────┐
│  ÉVOLUTION DE LA TAILLE DES FICHIERS                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Fichier     Taille      Représentation                     │
│  ─────────────────────────────────────────────────────────  │
│                                                             │
│  hello.c     ~200 o      ██                                 │
│                          (votre code compact)               │
│                                                             │
│  hello.i     ~20 Ko      ████████████████████████████████   │
│                          (explosion due aux #include)       │
│                                                             │
│  hello.s     ~800 o      ████                               │
│                          (assembleur, plus compact)         │
│                                                             │
│  hello.o     ~1.5 Ko     ███████                            │
│                          (code machine + métadonnées)       │
│                                                             │
│  hello       ~16 Ko      ████████████████████████████████   │
│                          ████████████████████████           │
│                          (exécutable complet avec en-têtes) │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**Pourquoi ces variations ?**

- **hello.c → hello.i** : Explosion ! Le `#include <stdio.h>` inclut des milliers de lignes.
- **hello.i → hello.s** : Réduction. Seul le code utilisé est traduit en assembleur.
- **hello.s → hello.o** : Légère augmentation. Le binaire contient des métadonnées.
- **hello.o → hello** : Augmentation. L'exécutable contient des en-têtes ELF, des informations de liaison dynamique, etc.

---

## Pipeline avec Plusieurs Fichiers Sources

Dans un vrai projet, vous avez souvent plusieurs fichiers `.c`. Voici comment le pipeline s'organise :

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  PIPELINE MULTI-FICHIERS                                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   main.c ──▶ main.i ──▶ main.s ──▶ main.o ──┐                               │
│                                             │                               │
│   utils.c ─▶ utils.i ─▶ utils.s ─▶ utils.o ─┼──▶ LINKER ──▶ programme       │
│                                             │                               │
│   calc.c ──▶ calc.i ──▶ calc.s ──▶ calc.o ──┘                               │
│                                             │                               │
│                                  libc.so ───┘                               │
│                                (bibliothèque)                               │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

Chaque fichier `.c` traverse indépendamment les 3 premières étapes (préprocesseur, compilation, assemblage). C'est seulement à la fin que le linker les rassemble tous.

**Commandes correspondantes :**

```bash
# Compilation séparée de chaque fichier
gcc -c main.c -o main.o
gcc -c utils.c -o utils.o
gcc -c calc.c -o calc.o

# Édition de liens finale
gcc main.o utils.o calc.o -o programme
```

---

## Visualiser le Pipeline en Temps Réel

GCC propose une option pour voir les commandes exécutées à chaque étape :

```bash
gcc -v hello.c -o hello
```

L'option `-v` (verbose) affiche tout ce qui se passe :

```
Using built-in specs.
COLLECT_GCC=gcc
...
 /usr/lib/gcc/x86_64-linux-gnu/11/cc1 -quiet hello.c -o /tmp/ccXXXXXX.s
 as -v --64 -o /tmp/ccYYYYYY.o /tmp/ccXXXXXX.s
 /usr/lib/gcc/x86_64-linux-gnu/11/collect2 -o hello /tmp/ccYYYYYY.o -lc ...
```

Vous pouvez identifier :
- `cc1` : le compilateur (étapes 1-2)
- `as` : l'assembleur (étape 3)
- `collect2`/`ld` : le linker (étape 4)

---

## Schéma Récapitulatif Complet

Voici un schéma qui résume tout le pipeline avec les outils, fichiers et commandes :

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                     PIPELINE COMPLET DE COMPILATION                         │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  SOURCE          OUTIL           RÉSULTAT         COMMANDE GCC              │
│  ══════════════════════════════════════════════════════════════════════     │
│                                                                             │
│  ┌─────────┐                                                                │
│  │ hello.c │     Votre code C (texte lisible)                               │
│  └────┬────┘                                                                │
│       │                                                                     │
│       │         ┌─────────────┐                                             │
│       ├────────▶│ Préprocesseur│   cpp                                      │
│       │         │    (cpp)    │                     gcc -E hello.c -o hello.i
│       │         └──────┬──────┘                                             │
│       │                │                                                    │
│       │                ▼                                                    │
│       │         ┌─────────┐                                                 │
│       │         │ hello.i │     Code prétraité (texte, ~800 lignes)         │
│       │         └────┬────┘                                                 │
│       │              │                                                      │
│       │              │  ┌─────────────┐                                     │
│       │              └─▶│ Compilateur │   cc1                               │
│       │                 │    (cc1)    │             gcc -S hello.c -o hello.s
│       │                 └──────┬──────┘                                     │
│       │                        │                                            │
│       │                        ▼                                            │
│       │                 ┌─────────┐                                         │
│       │                 │ hello.s │     Code assembleur (texte, ~30 lignes) │
│       │                 └────┬────┘                                         │
│       │                      │                                              │
│       │                      │  ┌─────────────┐                             │
│       │                      └─▶│ Assembleur  │   as                        │
│       │                         │    (as)     │     gcc -c hello.c -o hello.o
│       │                         └──────┬──────┘                             │
│       │                                │                                    │
│       │                                ▼                                    │
│       │                         ┌─────────┐                                 │
│       │                         │ hello.o │     Fichier objet (binaire)     │
│       │                         └────┬────┘                                 │
│       │                              │                                      │
│       │                              │  ┌─────────────┐  ┌─────────┐        │
│       │                              └─▶│   Linker    │◀─┤ libc.so │        │
│       │                                 │  (ld/lld)   │  └─────────┘        │
│       │                                 └──────┬──────┘     gcc hello.o -o hello
│       │                                        │                            │
│       │                                        ▼                            │
│       │                                 ┌───────────┐                       │
│       │                                 │   hello   │   Exécutable final    │
│       │                                 └───────────┘                       │
│       │                                        │                            │
│       │                                        ▼                            │
│       │                                   ./hello                           │
│       │                             "Programme créé par                     │
│       │                              Alice en 2025"                         │
│       │                                                                     │
│  ─────┴─────────────────────────────────────────────────────────────────    │
│                                                                             │
│  RACCOURCI : gcc hello.c -o hello   (fait tout en une seule commande)       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Les Fichiers Temporaires

Quand vous utilisez `gcc hello.c -o hello` sans options intermédiaires, GCC crée quand même tous les fichiers intermédiaires, mais dans un répertoire temporaire (`/tmp/`). Ils sont automatiquement supprimés après la compilation.

Pour les conserver et les examiner :

```bash
gcc -save-temps hello.c -o hello
```

Cette commande garde tous les fichiers intermédiaires dans le répertoire courant :
- `hello.i` (préprocessé)
- `hello.s` (assembleur)
- `hello.o` (objet)
- `hello` (exécutable)

---

## Conclusion

Visualiser le pipeline de compilation vous permet de comprendre que la « compilation » n'est pas une opération magique, mais une série de transformations bien définies :

1. **Texte C** → **Texte C étendu** (préprocesseur)
2. **Texte C étendu** → **Texte assembleur** (compilateur)
3. **Texte assembleur** → **Binaire objet** (assembleur)
4. **Binaire(s) objet** → **Exécutable** (linker)

Chaque étape a ses propres outils, formats de fichiers et types d'erreurs. En comprenant ce pipeline, vous serez capable de diagnostiquer plus rapidement les problèmes et de mieux organiser vos projets C.

Dans la section suivante, nous découvrirons les options de compilation GCC essentielles pour contrôler ce pipeline selon vos besoins.

⏭️ [Options de compilation GCC essentielles](/03-chaine-de-compilation/03-options-gcc.md)
