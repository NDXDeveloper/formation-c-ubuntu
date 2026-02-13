🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 3.1 Les 4 Étapes Détaillées de la Chaîne de Compilation

## Introduction

Quand vous écrivez un programme en C et que vous le « compilez », vous pensez peut-être qu'il s'agit d'une seule opération magique qui transforme votre code en programme exécutable. En réalité, cette transformation passe par **4 étapes distinctes**, chacune ayant un rôle bien précis.

Comprendre ces étapes vous aidera à :
- Mieux diagnostiquer les erreurs de compilation
- Comprendre les messages d'erreur du compilateur
- Optimiser vos temps de compilation sur de gros projets
- Déboguer plus efficacement

---

## Vue d'ensemble du processus

Voici le parcours complet de votre code source jusqu'à l'exécutable :

```
Code source (.c)
      │
      ▼
┌──────────────────┐
│ 1. PRÉPROCESSEUR │  ──►  Fichier prétraité (.i)
└──────────────────┘
      │
      ▼
┌──────────────────┐
│ 2. COMPILATION   │  ──►  Code assembleur (.s)
└──────────────────┘
      │
      ▼
┌──────────────────┐
│ 3. ASSEMBLAGE    │  ──►  Fichier objet (.o)
└──────────────────┘
      │
      ▼
┌──────────────────┐
│ 4. ÉDITION LIENS │  ──►  Exécutable (a.out ou nom choisi)
└──────────────────┘
```

Prenons un exemple concret avec ce programme simple que nous appellerons `hello.c` :

```c
#include <stdio.h>

#define MESSAGE "Bonjour le monde!"

int main(void) {
    printf("%s\n", MESSAGE);
    return 0;
}
```

Voyons maintenant ce qui se passe à chaque étape.

---

## Étape 1 : Le Préprocesseur

### Qu'est-ce que c'est ?

Le préprocesseur est un programme qui traite votre code source **avant** la compilation proprement dite. Il s'occupe de toutes les lignes qui commencent par `#` (les **directives de préprocesseur**).

### Que fait-il exactement ?

1. **Inclusion de fichiers** (`#include`) : Il remplace chaque directive `#include` par le contenu complet du fichier référencé. Par exemple, `#include <stdio.h>` est remplacé par des milliers de lignes contenant les déclarations de fonctions comme `printf`, `scanf`, etc.

2. **Expansion des macros** (`#define`) : Il remplace chaque utilisation d'une macro par sa valeur. Dans notre exemple, `MESSAGE` sera remplacé par `"Bonjour le monde!"` partout où il apparaît.

3. **Compilation conditionnelle** (`#ifdef`, `#ifndef`, `#endif`) : Il inclut ou exclut des portions de code selon certaines conditions.

4. **Suppression des commentaires** : Tous vos commentaires (`//` et `/* */`) sont retirés.

### Comment observer cette étape ?

Avec GCC, vous pouvez arrêter le processus après le préprocesseur en utilisant l'option `-E` :

```bash
gcc -E hello.c -o hello.i
```

Le fichier `hello.i` généré est un fichier texte que vous pouvez ouvrir. Vous y verrez :
- Le contenu de `stdio.h` (plusieurs milliers de lignes)
- Votre code avec `MESSAGE` remplacé par `"Bonjour le monde!"`
- Plus aucun commentaire

### Extension de fichier

Le fichier de sortie porte généralement l'extension `.i` (pour *intermediate* ou *preprocessed*).

### Erreurs typiques à cette étape

- `fichier.h: No such file or directory` → Le fichier d'en-tête n'est pas trouvé
- Erreurs dans les macros mal définies

---

## Étape 2 : La Compilation (au sens strict)

### Qu'est-ce que c'est ?

C'est l'étape où le code C prétraité est transformé en **code assembleur**, un langage de très bas niveau spécifique à l'architecture de votre processeur (x86, ARM, etc.).

### Que fait-elle exactement ?

1. **Analyse lexicale** : Le compilateur découpe votre code en « tokens » (mots-clés, identifiants, opérateurs, etc.)

2. **Analyse syntaxique** : Il vérifie que votre code respecte la grammaire du langage C. C'est ici que sont détectées les erreurs comme un point-virgule oublié ou une parenthèse manquante.

3. **Analyse sémantique** : Il vérifie le sens de votre code. Par exemple, il s'assure que vous n'additionnez pas un entier avec une chaîne de caractères.

4. **Génération de code** : Il produit le code assembleur équivalent à votre programme C.

5. **Optimisation** : Selon les options choisies, le compilateur peut optimiser le code pour le rendre plus rapide ou plus compact.

### Comment observer cette étape ?

Utilisez l'option `-S` pour obtenir le code assembleur :

```bash
gcc -S hello.c -o hello.s
```

Le fichier `hello.s` contient du code assembleur. Voici un extrait simplifié de ce que vous pourriez voir :

```asm
    .file   "hello.c"
    .section    .rodata
.LC0:
    .string "Bonjour le monde!"
    .text
    .globl  main
main:
    pushq   %rbp
    movq    %rsp, %rbp
    leaq    .LC0(%rip), %rdi
    call    puts@PLT
    movl    $0, %eax
    popq    %rbp
    ret
```

Ne vous inquiétez pas si ce code vous semble incompréhensible ! L'important est de comprendre que c'est une représentation textuelle des instructions que votre processeur exécutera.

### Extension de fichier

Le fichier de sortie porte l'extension `.s` (pour *assembly source*).

### Erreurs typiques à cette étape

- `expected ';' before...` → Point-virgule oublié
- `undeclared identifier` → Variable non déclarée
- `incompatible types` → Types incompatibles
- `implicit declaration of function` → Fonction utilisée sans déclaration

---

## Étape 3 : L'Assemblage

### Qu'est-ce que c'est ?

L'assembleur transforme le code assembleur (texte lisible par un humain) en **code machine** (instructions binaires compréhensibles par le processeur). Le résultat est appelé **fichier objet**.

### Que fait-il exactement ?

1. **Traduction** : Chaque instruction assembleur est convertie en son équivalent binaire (code machine).

2. **Création de la table des symboles** : L'assembleur crée une liste de tous les symboles (noms de fonctions, variables globales) définis ou utilisés dans le fichier.

3. **Génération des informations de relocation** : Il note les endroits où des adresses devront être ajustées lors de l'édition de liens.

### Comment observer cette étape ?

Utilisez l'option `-c` pour produire uniquement le fichier objet :

```bash
gcc -c hello.c -o hello.o
```

Le fichier `hello.o` est un fichier binaire. Vous ne pouvez pas le lire directement avec un éditeur de texte, mais vous pouvez examiner son contenu avec des outils comme `objdump` :

```bash
objdump -d hello.o    # Affiche le désassemblage  
objdump -t hello.o    # Affiche la table des symboles  
```

### Extension de fichier

Le fichier de sortie porte l'extension `.o` sous Linux/macOS (pour *object*) ou `.obj` sous Windows.

### Pourquoi cette étape est-elle importante ?

Un fichier objet n'est **pas encore exécutable**. Il contient le code machine de votre fichier source, mais :
- Les adresses des fonctions externes (comme `printf`) ne sont pas encore connues
- Si votre projet a plusieurs fichiers `.c`, chacun produit son propre `.o`

C'est l'étape suivante qui assemblera le tout.

### Erreurs typiques à cette étape

Les erreurs à cette étape sont rares car la plupart des problèmes sont détectés lors de la compilation. Vous pourriez rencontrer des erreurs si l'assembleur de votre système a des limitations spécifiques.

---

## Étape 4 : L'Édition de Liens (Linking)

### Qu'est-ce que c'est ?

L'éditeur de liens (ou *linker*) est le programme qui assemble tous les fichiers objets et les bibliothèques nécessaires pour créer l'**exécutable final**.

### Que fait-il exactement ?

1. **Résolution des symboles** : Il connecte les appels de fonctions à leurs définitions. Par exemple, quand votre code appelle `printf`, le linker trouve où se trouve réellement cette fonction (dans la bibliothèque C standard).

2. **Fusion des sections** : Il combine les différentes sections (code, données, etc.) de tous les fichiers objets.

3. **Relocation** : Il ajuste toutes les adresses pour qu'elles correspondent à leur position finale dans l'exécutable.

4. **Liaison avec les bibliothèques** : Il intègre le code des bibliothèques utilisées (statiquement) ou note les dépendances (dynamiquement).

### Comment observer cette étape ?

C'est l'étape finale, celle qui se produit quand vous compilez normalement :

```bash
gcc hello.o -o hello
```

Ou directement depuis le source :

```bash
gcc hello.c -o hello
```

L'exécutable `hello` est maintenant prêt à être lancé :

```bash
./hello
# Affiche : Bonjour le monde!
```

### Types de liaison

Il existe deux types de liaison avec les bibliothèques :

**Liaison statique** : Le code de la bibliothèque est copié dans votre exécutable. Celui-ci devient plus gros mais autonome.

**Liaison dynamique** : Votre exécutable contient juste une référence à la bibliothèque. Elle sera chargée en mémoire au moment de l'exécution. C'est le comportement par défaut sous Linux.

### Erreurs typiques à cette étape

- `undefined reference to 'fonction'` → Une fonction est appelée mais jamais définie. Soit vous avez oublié d'inclure un fichier `.o`, soit il manque une bibliothèque.
- `multiple definition of 'symbole'` → Un symbole (fonction ou variable) est défini plusieurs fois.

---

## Récapitulatif

| Étape | Entrée | Sortie | Option GCC | Rôle principal |
|-------|--------|--------|------------|----------------|
| **1. Préprocesseur** | `.c` | `.i` | `-E` | Traite les directives `#` |
| **2. Compilation** | `.i` | `.s` | `-S` | Traduit en assembleur |
| **3. Assemblage** | `.s` | `.o` | `-c` | Traduit en code machine |
| **4. Édition de liens** | `.o` | exécutable | (défaut) | Crée l'exécutable final |

### Commandes pour observer chaque étape

```bash
# Étape 1 : Préprocesseur uniquement
gcc -E hello.c -o hello.i

# Étape 2 : Jusqu'à la compilation (assembleur)
gcc -S hello.c -o hello.s

# Étape 3 : Jusqu'à l'assemblage (fichier objet)
gcc -c hello.c -o hello.o

# Étape 4 : Processus complet (exécutable)
gcc hello.c -o hello
```

---

## Pourquoi connaître ces étapes ?

### Pour le débogage

Quand une erreur survient, le message vous indique souvent à quelle étape elle s'est produite :
- Erreurs de préprocesseur → Problèmes avec `#include` ou `#define`
- Erreurs de compilation → Problèmes de syntaxe ou de sémantique C
- Erreurs de liaison → Problèmes de symboles manquants ou dupliqués

### Pour les projets multi-fichiers

Dans un vrai projet, vous avez généralement plusieurs fichiers `.c`. La compilation séparée permet de ne recompiler que les fichiers modifiés :

```bash
gcc -c fichier1.c -o fichier1.o  
gcc -c fichier2.c -o fichier2.o  
gcc -c fichier3.c -o fichier3.o  
gcc fichier1.o fichier2.o fichier3.o -o mon_programme  
```

Si vous modifiez uniquement `fichier2.c`, vous n'avez qu'à refaire :

```bash
gcc -c fichier2.c -o fichier2.o  
gcc fichier1.o fichier2.o fichier3.o -o mon_programme  
```

C'est exactement ce qu'automatisent les outils comme **Make** et **CMake** que vous découvrirez plus tard dans cette formation.

---

## Conclusion

La chaîne de compilation en C n'est pas une boîte noire mystérieuse. Elle se compose de quatre étapes bien distinctes, chacune avec son rôle :

1. **Le préprocesseur** prépare le code en traitant les directives `#`
2. **Le compilateur** transforme le C en assembleur
3. **L'assembleur** transforme l'assembleur en code machine
4. **L'éditeur de liens** assemble le tout en un exécutable

Comprendre ce processus est fondamental pour tout développeur C. Vous serez maintenant mieux armé pour comprendre les messages d'erreur et optimiser vos compilations !

⏭️ [Visualisation du pipeline](/03-chaine-de-compilation/02-visualisation-pipeline.md)
