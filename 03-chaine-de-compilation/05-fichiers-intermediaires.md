🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 3.5 Inspection des Fichiers Intermédiaires

## Introduction

Lors de la compilation d'un programme C, plusieurs fichiers intermédiaires sont créés avant d'obtenir l'exécutable final. Ces fichiers sont normalement temporaires et supprimés automatiquement, mais savoir les conserver et les analyser est une compétence précieuse.

Dans cette section, vous apprendrez à :

- Générer et conserver les fichiers intermédiaires
- Examiner le fichier prétraité (`.i`)
- Lire et comprendre le code assembleur (`.s`)
- Analyser les fichiers objets (`.o`)
- Utiliser les outils d'inspection appropriés

Cette compétence vous sera utile pour comprendre les erreurs de compilation, optimiser votre code et approfondir votre compréhension du langage C.

---

## Générer les Fichiers Intermédiaires

### Méthode 1 : Option `-save-temps`

La façon la plus simple est d'utiliser l'option `-save-temps` :

```bash
gcc -save-temps programme.c -o programme
```

Cette commande conserve tous les fichiers intermédiaires dans le répertoire courant :

```
programme.c      # Votre code source (original)  
programme.i      # Après le préprocesseur  
programme.s      # Code assembleur  
programme.o      # Fichier objet (binaire)  
programme        # Exécutable final
```

### Méthode 2 : Étape par Étape

Vous pouvez aussi générer chaque fichier individuellement :

```bash
# Étape 1 : Préprocesseur uniquement
gcc -E programme.c -o programme.i

# Étape 2 : Compilation (génère l'assembleur)
gcc -S programme.c -o programme.s

# Étape 3 : Assemblage (génère le fichier objet)
gcc -c programme.c -o programme.o

# Étape 4 : Édition de liens (génère l'exécutable)
gcc programme.o -o programme
```

### Notre Programme d'Exemple

Pour cette section, nous utiliserons ce programme légèrement plus élaboré que Hello World :

```c
// exemple.c
#include <stdio.h>
#include <stdlib.h>

#define TAILLE_MAX 100
#define CARRE(x) ((x) * (x))

// Variable globale
int compteur = 0;

// Fonction auxiliaire
int calculer_somme(int n) {
    int somme = 0;
    for (int i = 1; i <= n; i++) {
        somme += i;
        compteur++;
    }
    return somme;
}

int main(void) {
    int valeur = 5;
    int resultat = calculer_somme(valeur);
    int carre = CARRE(valeur);

    printf("Somme de 1 à %d : %d\n", valeur, resultat);
    printf("Carré de %d : %d\n", valeur, carre);
    printf("Nombre d'itérations : %d\n", compteur);

    return EXIT_SUCCESS;
}
```

Générons tous les fichiers intermédiaires :

```bash
gcc -save-temps exemple.c -o exemple  
ls -la exemple*
```

---

## 1. Le Fichier Prétraité (.i)

Le fichier `.i` contient le code source après le passage du préprocesseur. C'est encore du code C lisible, mais transformé.

### Caractéristiques du Fichier `.i`

- **Format** : Texte (code C)
- **Taille** : Généralement beaucoup plus grand que le `.c` original
- **Contenu** : Code C avec les `#include` expansés et les macros remplacées

### Comment l'Examiner

```bash
# Voir la taille du fichier
wc -l exemple.i
# Résultat : environ 800-2000 lignes (selon le système)

# Voir les premières lignes
head -50 exemple.i

# Voir les dernières lignes (votre code)
tail -50 exemple.i

# Ouvrir dans un éditeur
nano exemple.i
# ou
code exemple.i
```

### Structure du Fichier `.i`

```
┌─────────────────────────────────────────────────────────────┐
│  STRUCTURE D'UN FICHIER .i                                  │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────────────────────┐    │
│  │ # 1 "exemple.c"                                     │    │
│  │ # 1 "<built-in>"                                    │    │
│  │ # 1 "<command-line>"                                │    │
│  │                                                     │    │
│  │ Marqueurs de ligne (indiquent l'origine du code)    │    │
│  └─────────────────────────────────────────────────────┘    │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐    │
│  │ # 1 "/usr/include/stdio.h" 1 3 4                    │    │
│  │ ...                                                 │    │
│  │ extern int printf(const char *, ...);               │    │
│  │ extern int scanf(const char *, ...);                │    │
│  │ ...                                                 │    │
│  │                                                     │    │
│  │ Contenu de stdio.h (~500 lignes)                    │    │
│  └─────────────────────────────────────────────────────┘    │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐    │
│  │ # 1 "/usr/include/stdlib.h" 1 3 4                   │    │
│  │ ...                                                 │    │
│  │ extern void *malloc(size_t);                        │    │
│  │ extern void free(void *);                           │    │
│  │ ...                                                 │    │
│  │                                                     │    │
│  │ Contenu de stdlib.h (~300 lignes)                   │    │
│  └─────────────────────────────────────────────────────┘    │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐    │
│  │ # 10 "exemple.c"                                    │    │
│  │ int compteur = 0;                                   │    │
│  │                                                     │    │
│  │ int calculer_somme(int n) {                         │    │
│  │     int somme = 0;                                  │    │
│  │     ...                                             │    │
│  │ }                                                   │    │
│  │                                                     │    │
│  │ int main(void) {                                    │    │
│  │     int valeur = 5;                                 │    │
│  │     int carre = ((valeur) * (valeur));  ◄── CARRE   │    │
│  │     ...                                   expansé   │    │
│  │ }                                                   │    │
│  │                                                     │    │
│  │ Votre code (avec macros remplacées)                 │    │
│  └─────────────────────────────────────────────────────┘    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Ce Qui a Changé

**Les directives `#include` ont été remplacées :**

```c
// AVANT (exemple.c)
#include <stdio.h>

// APRÈS (exemple.i)
// ~500 lignes de déclarations de stdio.h
typedef struct _IO_FILE FILE;  
extern FILE *stdin;  
extern FILE *stdout;  
extern FILE *stderr;  
extern int printf(const char *__restrict __format, ...);
// ... etc
```

**Les macros `#define` ont été expansées :**

```c
// AVANT (exemple.c)
#define TAILLE_MAX 100
#define CARRE(x) ((x) * (x))
int carre = CARRE(valeur);

// APRÈS (exemple.i)
// TAILLE_MAX et CARRE n'existent plus comme macros
int carre = ((valeur) * (valeur));  // Macro expansée
```

**Les commentaires ont été supprimés :**

```c
// AVANT
// Variable globale
int compteur = 0;

// APRÈS
int compteur = 0;
```

### Les Marqueurs de Ligne

Les lignes commençant par `#` suivi d'un numéro sont des **marqueurs de ligne** (*line markers*). Ils indiquent au compilateur d'où vient le code pour les messages d'erreur :

```c
# 27 "/usr/include/stdio.h" 3 4
```

- `27` : Numéro de ligne dans le fichier original
- `"/usr/include/stdio.h"` : Fichier d'origine
- `3` : Flag indiquant un fichier système
- `4` : Flag indiquant du code extern "C" implicite

### Rechercher Votre Code

Pour trouver rapidement votre code dans le fichier `.i` :

```bash
# Chercher le début de votre code
grep -n "exemple.c" exemple.i

# Chercher une fonction spécifique
grep -n "calculer_somme" exemple.i

# Voir le contexte autour de main
grep -n -A 20 "int main" exemple.i
```

### Utilité du Fichier `.i`

L'inspection du `.i` est utile pour :

1. **Déboguer les macros** : Vérifier que vos macros s'expansent correctement
2. **Comprendre les erreurs** : Les erreurs de compilation proviennent de ce fichier
3. **Voir les déclarations** : Comprendre ce que contiennent réellement les headers
4. **Résoudre les conflits** : Identifier les définitions multiples

---

## 2. Le Fichier Assembleur (.s)

Le fichier `.s` contient le code assembleur généré par le compilateur. C'est une représentation textuelle des instructions machine.

### Caractéristiques du Fichier `.s`

- **Format** : Texte (syntaxe assembleur AT&T par défaut)
- **Taille** : Plus petit que le `.i`, variable selon les optimisations
- **Contenu** : Instructions assembleur, directives, données

### Comment l'Examiner

```bash
# Voir le fichier complet
cat exemple.s

# Avec numéros de ligne
cat -n exemple.s

# Ouvrir dans un éditeur avec coloration syntaxique
code exemple.s
```

### Générer avec les Optimisations

Comparez le code avec différents niveaux d'optimisation :

```bash
# Sans optimisation (plus lisible)
gcc -S -O0 exemple.c -o exemple_O0.s

# Avec optimisation standard
gcc -S -O2 exemple.c -o exemple_O2.s

# Comparer les tailles
wc -l exemple_O0.s exemple_O2.s
```

### Anatomie du Fichier Assembleur

Voici un exemple annoté de notre programme :

```asm
	.file	"exemple.c"                    # Nom du fichier source

	# ══════════════════════════════════════════════════════════
	# SECTION DES DONNÉES
	# ══════════════════════════════════════════════════════════

	.text                                  # Section du code exécutable

	.globl	compteur                       # Rend 'compteur' visible globalement
	.data                                  # Section des données initialisées
	.align 4                               # Alignement sur 4 octets
	.type	compteur, @object              # Type : objet (variable)
	.size	compteur, 4                    # Taille : 4 octets (int)
compteur:
	.long	0                              # Valeur initiale : 0

	# ══════════════════════════════════════════════════════════
	# SECTION DES CHAÎNES DE CARACTÈRES
	# ══════════════════════════════════════════════════════════

	.section	.rodata                    # Section lecture seule
.LC0:
	.string	"Somme de 1 à %d : %d\n"      # Première chaîne printf
.LC1:
	.string	"Carré de %d : %d\n"          # Deuxième chaîne printf
.LC2:
	.string	"Nombre d'itérations : %d\n"  # Troisième chaîne printf

	# ══════════════════════════════════════════════════════════
	# FONCTION calculer_somme
	# ══════════════════════════════════════════════════════════

	.text
	.globl	calculer_somme
	.type	calculer_somme, @function
calculer_somme:
	pushq	%rbp                           # Sauvegarde du frame pointer
	movq	%rsp, %rbp                     # Nouveau frame pointer
	movl	%edi, -20(%rbp)                # Paramètre n → pile
	movl	$0, -8(%rbp)                   # somme = 0
	movl	$1, -4(%rbp)                   # i = 1
	jmp	.L2                                # Saut vers la condition
.L3:                                       # Corps de la boucle
	movl	-4(%rbp), %eax                 # Charge i
	addl	%eax, -8(%rbp)                 # somme += i
	movl	compteur(%rip), %eax           # Charge compteur
	addl	$1, %eax                       # compteur + 1
	movl	%eax, compteur(%rip)           # Stocke compteur
	addl	$1, -4(%rbp)                   # i++
.L2:                                       # Condition de boucle
	movl	-4(%rbp), %eax                 # Charge i
	cmpl	-20(%rbp), %eax                # Compare i avec n
	jle	.L3                                # Si i <= n, continuer
	movl	-8(%rbp), %eax                 # Valeur de retour = somme
	popq	%rbp                           # Restaure frame pointer
	ret                                    # Retour

	# ══════════════════════════════════════════════════════════
	# FONCTION main
	# ══════════════════════════════════════════════════════════

	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp                      # Réserve espace sur la pile
	movl	$5, -12(%rbp)                  # valeur = 5
	movl	-12(%rbp), %eax
	movl	%eax, %edi                     # Argument pour calculer_somme
	call	calculer_somme                 # Appel de fonction
	movl	%eax, -8(%rbp)                 # resultat = retour
	movl	-12(%rbp), %eax
	imull	-12(%rbp), %eax                # valeur * valeur (CARRE)
	movl	%eax, -4(%rbp)                 # carre = résultat
	# ... suite avec les printf ...
	movl	$0, %eax                       # return 0
	leave
	ret
```

### Comprendre la Syntaxe AT&T

GCC utilise par défaut la syntaxe **AT&T** (différente de la syntaxe Intel) :

```
┌─────────────────────────────────────────────────────────────┐
│  SYNTAXE AT&T vs INTEL                                      │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Caractéristique      AT&T (GCC)         Intel              │
│  ─────────────────────────────────────────────────────────  │
│                                                             │
│  Ordre opérandes      source, dest       dest, source       │
│                       movl $5, %eax      mov eax, 5         │
│                                                             │
│  Préfixe registres    %                  (aucun)            │
│                       %eax, %rbp         eax, rbp           │
│                                                             │
│  Préfixe constantes   $                  (aucun)            │
│                       $42, $0xFF         42, 0xFF           │
│                                                             │
│  Suffixe taille       b/w/l/q            byte/word/dword    │
│                       movl (32-bit)      mov dword          │
│                       movq (64-bit)      mov qword          │
│                                                             │
│  Adressage mémoire    offset(base,idx,s) [base+idx*s+off]   │
│                       8(%rbp)            [rbp+8]            │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**Suffixes de taille :**

| Suffixe | Taille | Type C équivalent |
|---------|--------|-------------------|
| `b` | 1 octet (8 bits) | `char` |
| `w` | 2 octets (16 bits) | `short` |
| `l` | 4 octets (32 bits) | `int` |
| `q` | 8 octets (64 bits) | `long`, pointeur |

### Générer en Syntaxe Intel

Si vous préférez la syntaxe Intel (plus intuitive pour certains) :

```bash
gcc -S -masm=intel exemple.c -o exemple_intel.s
```

Exemple de différence :

```asm
# AT&T
movl    $5, -12(%rbp)

# Intel
mov     DWORD PTR [rbp-12], 5
```

### Les Registres x86-64

```
┌─────────────────────────────────────────────────────────────┐
│  REGISTRES x86-64 PRINCIPAUX                                │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  64-bit   32-bit   16-bit   8-bit    Usage courant          │
│  ───────────────────────────────────────────────────────    │
│  %rax     %eax     %ax      %al      Valeur de retour       │
│  %rbx     %ebx     %bx      %bl      Base (préservé)        │
│  %rcx     %ecx     %cx      %cl      Compteur, 4e argument  │
│  %rdx     %edx     %dx      %dl      3e argument, données   │
│  %rsi     %esi     %si      %sil     2e argument (source)   │
│  %rdi     %edi     %di      %dil     1er argument (dest)    │
│  %rbp     %ebp     %bp      %bpl     Frame pointer          │
│  %rsp     %esp     %sp      %spl     Stack pointer          │
│  %r8-%r15                            Registres additionnels │
│                                                             │
│  Convention d'appel Linux x86-64 (System V AMD64 ABI) :     │
│  ─────────────────────────────────────────────────────────  │
│  Arguments : %rdi, %rsi, %rdx, %rcx, %r8, %r9               │
│  Retour    : %rax (et %rdx pour 128 bits)                   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Ajouter les Informations de Débogage

Pour voir la correspondance avec le code C :

```bash
gcc -S -g -fverbose-asm exemple.c -o exemple_debug.s
```

L'option `-fverbose-asm` ajoute des commentaires utiles :

```asm
	movl	$5, -12(%rbp)	# valeur = 5
	movl	-12(%rbp), %eax	# tmp = valeur
	movl	%eax, %edi	    # premier argument
	call	calculer_somme	#
	movl	%eax, -8(%rbp)	# resultat = retour
```

### Utilité du Fichier `.s`

L'inspection du `.s` est utile pour :

1. **Comprendre les optimisations** : Voir ce que le compilateur fait réellement
2. **Analyser les performances** : Identifier le code critique
3. **Déboguer** : Comprendre les bugs bas niveau
4. **Apprendre** : Voir comment le C se traduit en machine

---

## 3. Le Fichier Objet (.o)

Le fichier `.o` est un fichier **binaire** contenant le code machine et des métadonnées. Il n'est pas lisible directement.

### Caractéristiques du Fichier `.o`

- **Format** : Binaire (ELF sous Linux)
- **Taille** : Variable, généralement quelques Ko
- **Contenu** : Code machine, table des symboles, informations de relocation

### Outils d'Inspection

Plusieurs outils permettent d'examiner les fichiers objets :

| Outil | Usage |
|-------|-------|
| `file` | Identifier le type de fichier |
| `size` | Voir la taille des sections |
| `nm` | Lister les symboles |
| `objdump` | Désassembler et inspecter |
| `readelf` | Analyser le format ELF |
| `hexdump` | Voir le contenu brut en hexadécimal |

### Identifier le Fichier

```bash
file exemple.o
# exemple.o: ELF 64-bit LSB relocatable, x86-64, version 1 (SYSV), not stripped
```

**Décryptage :**
- `ELF` : Format Executable and Linkable Format
- `64-bit` : Architecture 64 bits
- `LSB` : Little-endian (octet de poids faible en premier)
- `relocatable` : Fichier objet (pas encore exécutable)
- `not stripped` : Contient encore les symboles de débogage

### Voir la Taille des Sections

```bash
size exemple.o
#    text    data     bss     dec     hex filename
#     215       4       0     219      db exemple.o
```

| Section | Contenu |
|---------|---------|
| `text` | Code exécutable (215 octets) |
| `data` | Données initialisées (4 octets = notre `int compteur`) |
| `bss` | Données non initialisées (0 octet) |
| `dec` | Total en décimal |
| `hex` | Total en hexadécimal |

### Lister les Symboles avec `nm`

```bash
nm exemple.o
```

**Résultat :**

```
0000000000000000 T calculer_somme
0000000000000000 D compteur
0000000000000042 T main
                 U printf
```

**Signification des lettres :**

```
┌─────────────────────────────────────────────────────────────┐
│  TYPES DE SYMBOLES (nm)                                     │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Lettre   Signification                                     │
│  ─────────────────────────────────────────────────────────  │
│                                                             │
│  T / t    Text (code) - Fonction définie                    │
│           T = globale (visible), t = locale (static)        │
│                                                             │
│  D / d    Data - Variable initialisée                       │
│           D = globale, d = locale                           │
│                                                             │
│  B / b    BSS - Variable non initialisée                    │
│           B = globale, b = locale                           │
│                                                             │
│  U        Undefined - Symbole utilisé mais non défini       │
│           (sera résolu par le linker)                       │
│                                                             │
│  R / r    Read-only data - Données en lecture seule         │
│           (chaînes de caractères, constantes)               │
│                                                             │
│  C        Common - Variable commune non initialisée         │
│                                                             │
│  W / w    Weak - Symbole faible (peut être redéfini)        │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**Interprétation de notre sortie :**

| Adresse | Type | Nom | Signification |
|---------|------|-----|---------------|
| `0000...0000` | `T` | `calculer_somme` | Fonction définie, globale |
| `0000...0000` | `D` | `compteur` | Variable initialisée, globale |
| `0000...0042` | `T` | `main` | Fonction définie, globale |
| *(vide)* | `U` | `printf` | Non défini (dans la libc) |

Les adresses `0000...0000` sont des **adresses relatives** qui seront ajustées lors de l'édition de liens.

### Options Utiles de `nm`

```bash
# Trier par adresse
nm -n exemple.o

# Afficher la taille des symboles
nm -S exemple.o

# Afficher les symboles non définis uniquement
nm -u exemple.o
# Résultat : U printf

# Format détaillé
nm -f sysv exemple.o
```

### Désassembler avec `objdump`

L'outil `objdump` permet de "désassembler" le code machine en assembleur :

```bash
objdump -d exemple.o
```

**Résultat (extrait) :**

```
exemple.o:     file format elf64-x86-64

Disassembly of section .text:

0000000000000000 <calculer_somme>:
   0:   55                      push   %rbp
   1:   48 89 e5                mov    %rsp,%rbp
   4:   89 7d ec                mov    %edi,-0x14(%rbp)
   7:   c7 45 f8 00 00 00 00    movl   $0x0,-0x8(%rbp)
   e:   c7 45 fc 01 00 00 00    movl   $0x1,-0x4(%rbp)
  15:   eb 1b                   jmp    32 <calculer_somme+0x32>
  ...

0000000000000042 <main>:
  42:   55                      push   %rbp
  43:   48 89 e5                mov    %rsp,%rbp
  ...
```

**Colonnes :**
1. Adresse relative dans la section
2. Octets du code machine (hexadécimal)
3. Instruction assembleur équivalente

### Options Utiles de `objdump`

```bash
# Désassembler avec le code source (si compilé avec -g)
objdump -d -S exemple.o

# Afficher toutes les sections
objdump -h exemple.o

# Afficher le contenu des sections de données
objdump -s -j .rodata exemple.o

# Syntaxe Intel
objdump -d -M intel exemple.o
```

### Voir les Sections avec `readelf`

```bash
readelf -S exemple.o
```

**Résultat (simplifié) :**

```
Section Headers:
  [Nr] Name              Type             Address           Offset
       Size              EntSize          Flags  Link  Info  Align
  [ 0]                   NULL             0000000000000000  00000000
  [ 1] .text             PROGBITS         0000000000000000  00000040
       00000000000000a3  0000000000000000  AX       0     0     1
  [ 2] .rela.text        RELA             0000000000000000  00000340
       00000000000000c0  0000000000000018   I      11     1     8
  [ 3] .data             PROGBITS         0000000000000000  000000e4
       0000000000000004  0000000000000000  WA       0     0     4
  [ 4] .rodata           PROGBITS         0000000000000000  000000e8
       0000000000000054  0000000000000000   A       0     0     1
  ...
```

**Sections importantes :**

| Section | Description |
|---------|-------------|
| `.text` | Code exécutable |
| `.data` | Variables globales initialisées |
| `.rodata` | Données en lecture seule (chaînes) |
| `.bss` | Variables non initialisées |
| `.rela.text` | Informations de relocation |
| `.symtab` | Table des symboles |
| `.strtab` | Table des chaînes de caractères |

### Voir le Contenu des Chaînes

```bash
# Extraire les chaînes lisibles
strings exemple.o
```

**Résultat :**

```
Somme de 1 à %d : %d  
Carré de %d : %d  
Nombre d'itérations : %d  
GCC: (Ubuntu 11.4.0) 11.4.0  
exemple.c  
calculer_somme  
compteur  
main  
printf
```

### Voir le Contenu Brut

```bash
# Afficher en hexadécimal
hexdump -C exemple.o | head -20
```

**Résultat :**

```
00000000  7f 45 4c 46 02 01 01 00  00 00 00 00 00 00 00 00  |.ELF............|
00000010  01 00 3e 00 01 00 00 00  00 00 00 00 00 00 00 00  |..>.............|
00000020  00 00 00 00 00 00 00 00  18 04 00 00 00 00 00 00  |................|
...
```

Les premiers octets `7f 45 4c 46` correspondent à la signature ELF (`.ELF`).

---

## 4. L'Exécutable Final

Bien que ce ne soit pas un fichier "intermédiaire", l'exécutable final mérite aussi une inspection.

### Identifier l'Exécutable

```bash
file exemple
# exemple: ELF 64-bit LSB pie executable, x86-64, dynamically linked,
#          interpreter /lib64/ld-linux-x86-64.so.2, for GNU/Linux 3.2.0,
#          not stripped
```

**Nouvelles informations :**
- `pie executable` : Position Independent Executable (sécurité)
- `dynamically linked` : Utilise des bibliothèques partagées
- `interpreter` : Chemin du chargeur dynamique

### Comparer les Tailles

```bash
ls -l exemple.o exemple
# -rw-r--r-- 1 user user  1760 ... exemple.o
# -rwxr-xr-x 1 user user 16696 ... exemple
```

L'exécutable est beaucoup plus gros car il contient :
- Les en-têtes ELF complets
- Les informations de liaison dynamique
- Le code de démarrage (crt*.o)

### Voir les Bibliothèques Liées

```bash
ldd exemple
```

**Résultat :**

```
    linux-vdso.so.1 (0x00007fff3e5fe000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f8b3e200000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f8b3e432000)
```

| Bibliothèque | Rôle |
|--------------|------|
| `linux-vdso.so.1` | Virtual Dynamic Shared Object (optimisations noyau) |
| `libc.so.6` | Bibliothèque C standard (printf, etc.) |
| `ld-linux-x86-64.so.2` | Chargeur dynamique |

### Symboles de l'Exécutable

```bash
nm exemple | head -20
```

Vous verrez beaucoup plus de symboles que dans le `.o`, car l'exécutable contient aussi le code de démarrage.

### Désassembler l'Exécutable

```bash
objdump -d exemple | less
```

Le désassemblage est plus long car il inclut :
- Votre code (`main`, `calculer_somme`)
- Le code de démarrage (`_start`, `__libc_start_main`, etc.)
- Les stubs PLT pour les fonctions dynamiques

---

## 5. Tableau Récapitulatif des Outils

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  OUTILS D'INSPECTION PAR TYPE DE FICHIER                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Fichier    Format      Outils recommandés                                  │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                             │
│  .i         Texte       cat, less, grep, head, tail, nano/vim/code          │
│                         (C'est du code C, lisible directement)              │
│                                                                             │
│  .s         Texte       cat, less, grep                                     │
│                         (Code assembleur, lisible mais technique)           │
│                                                                             │
│  .o         Binaire     file      - Identifier le format                    │
│             (ELF)       size      - Taille des sections                     │
│                         nm        - Lister les symboles                     │
│                         objdump   - Désassembler                            │
│                         readelf   - Analyser le format ELF                  │
│                         strings   - Extraire les chaînes                    │
│                         hexdump   - Voir le contenu brut                    │
│                                                                             │
│  exécutable Binaire     Mêmes outils que .o, plus :                         │
│             (ELF)       ldd       - Bibliothèques dynamiques                │
│                         strace    - Tracer les appels système               │
│                         ltrace    - Tracer les appels de bibliothèque       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 6. Cas Pratiques d'Inspection

### Cas 1 : Vérifier l'Expansion d'une Macro

Vous avez une macro complexe et voulez vérifier qu'elle s'expanse correctement :

```bash
gcc -E programme.c | grep -A 5 "ma_fonction"
```

### Cas 2 : Voir si une Fonction est Inlinée

Comparez le code assembleur avec et sans optimisation :

```bash
gcc -S -O0 programme.c -o sans_inline.s  
gcc -S -O2 programme.c -o avec_inline.s  
diff sans_inline.s avec_inline.s
```

### Cas 3 : Vérifier qu'un Symbole est Exporté

```bash
nm programme.o | grep "ma_fonction"
# T ma_fonction  → Définie et exportée
# t ma_fonction  → Définie mais locale (static)
# U ma_fonction  → Non définie (externe)
```

### Cas 4 : Trouver des Symboles Non Résolus

```bash
nm -u programme.o
# Liste tous les symboles U (undefined)
```

### Cas 5 : Identifier une Fonction Manquante lors du Linkage

Si vous avez une erreur "undefined reference" :

```bash
nm *.o | grep "fonction_manquante"
# Devrait montrer U dans un fichier et T dans un autre
```

---

## Conclusion

L'inspection des fichiers intermédiaires est une compétence précieuse qui vous permet de :

1. **Comprendre** ce que fait réellement le compilateur
2. **Déboguer** des problèmes complexes de compilation et de linkage
3. **Optimiser** votre code en comprenant son impact au niveau machine
4. **Apprendre** les concepts bas niveau de la programmation système

**Commandes essentielles à retenir :**

```bash
# Générer tous les fichiers intermédiaires
gcc -save-temps programme.c -o programme

# Inspecter le fichier prétraité
tail -50 programme.i

# Voir l'assembleur avec commentaires
gcc -S -fverbose-asm programme.c -o programme.s

# Lister les symboles d'un fichier objet
nm programme.o

# Désassembler un fichier objet
objdump -d programme.o

# Voir les bibliothèques d'un exécutable
ldd programme
```

Dans la section suivante, nous approfondirons la compréhension des symboles et de la table des symboles, éléments essentiels pour maîtriser l'édition de liens.

⏭️ [Comprendre les symboles et la table des symboles](/03-chaine-de-compilation/06-symboles.md)
