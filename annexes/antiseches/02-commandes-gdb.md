🔝 Retour au [Sommaire](/SOMMAIRE.md)

# B.2 Commandes GDB (Cheat Sheet)

## Introduction

**GDB** (GNU Debugger) est le débogueur standard pour les programmes C/C++ sur Linux. Il vous permet d'examiner ce qui se passe **à l'intérieur** de votre programme pendant son exécution ou au moment d'un crash.

💡 **Pour les débutants :** GDB peut sembler intimidant au début, mais c'est un outil **indispensable**. Apprendre quelques commandes de base vous fera gagner des heures de debugging.

### Pourquoi utiliser GDB ?

Sans GDB, débugger ressemble à ça :
```c
printf("A\n");
// ... code ...
printf("B\n");
// ... code ...
printf("C\n");  // Le programme crash ici
```

Avec GDB, vous pouvez :
- ✅ Mettre des points d'arrêt (breakpoints)
- ✅ Exécuter ligne par ligne (step)
- ✅ Inspecter les variables
- ✅ Voir la pile d'appels (backtrace)
- ✅ Comprendre exactement où et pourquoi ça crash

---

## Préparation : Compiler avec les symboles de debug

**IMPORTANT :** Pour utiliser GDB efficacement, compilez avec l'option `-g` :

```bash
gcc -g -Wall program.c -o program
```

Sans `-g`, GDB ne pourra pas afficher :
- Les noms de variables
- Les numéros de ligne
- Les noms de fonctions

---

## Démarrer GDB

### Lancer GDB avec un programme

```bash
gdb ./program
```

Vous entrez dans l'interface GDB :
```
GNU gdb (Ubuntu 12.1-0ubuntu1) 12.1
...
(gdb)
```

Le prompt `(gdb)` indique que GDB attend vos commandes.

---

### Autres façons de lancer GDB

```bash
# Avec arguments
gdb --args ./program arg1 arg2

# Attacher à un processus en cours
gdb -p <PID>

# Analyser un core dump
gdb ./program core

# Mode TUI (interface texte améliorée)
gdb -tui ./program
```

---

## 🎯 Commandes de base essentielles

### `run` (ou `r`) : Démarrer le programme

```gdb
(gdb) run
```

Ou avec des arguments :
```gdb
(gdb) run arg1 arg2
(gdb) r < input.txt
```

---

### `quit` (ou `q`) : Quitter GDB

```gdb
(gdb) quit
```

---

### `help` : Obtenir de l'aide

```gdb
(gdb) help
(gdb) help breakpoints
(gdb) help break
```

---

## 🔴 Breakpoints (Points d'arrêt)

Les breakpoints arrêtent l'exécution à un endroit précis.

### `break` (ou `b`) : Mettre un breakpoint

```gdb
# Par numéro de ligne
(gdb) break 42
(gdb) b 42

# Par fonction
(gdb) break main
(gdb) b fonction_name

# Par fichier:ligne
(gdb) break fichier.c:42
(gdb) b utils.c:100

# Par condition
(gdb) break 42 if x == 5
```

---

### `info breakpoints` (ou `info b`) : Lister les breakpoints

```gdb
(gdb) info breakpoints
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x0000000000401156 in main at program.c:10
2       breakpoint     keep y   0x000000000040117a in foo at program.c:20
```

---

### `delete` (ou `d`) : Supprimer un breakpoint

```gdb
# Supprimer un breakpoint spécifique
(gdb) delete 1

# Supprimer tous les breakpoints
(gdb) delete
```

---

### `disable` / `enable` : Désactiver/activer un breakpoint

```gdb
(gdb) disable 1
(gdb) enable 1
```

---

### `clear` : Supprimer breakpoint à une ligne

```gdb
(gdb) clear 42
(gdb) clear fonction_name
```

---

## 🚶 Contrôle de l'exécution (Stepping)

### `continue` (ou `c`) : Continuer jusqu'au prochain breakpoint

```gdb
(gdb) continue
```

---

### `next` (ou `n`) : Exécuter la ligne suivante (step over)

```gdb
(gdb) next
```

**Comportement :** N'entre **pas** dans les fonctions appelées.

```c
int result = calculate(x, y);  // next passe directement
printf("Result: %d\n", result);
```

---

### `step` (ou `s`) : Exécuter la ligne suivante (step into)

```gdb
(gdb) step
```

**Comportement :** Entre **dans** les fonctions appelées.

```c
int result = calculate(x, y);  // step entre dans calculate()
```

---

### `finish` : Terminer la fonction actuelle

```gdb
(gdb) finish
```

**Utilité :** Sortir d'une fonction sans stepper ligne par ligne.

---

### `until` : Continuer jusqu'à une ligne

```gdb
(gdb) until 50
```

**Utilité :** Sortir d'une boucle rapidement.

---

### `advance` : Continuer jusqu'à un point précis

```gdb
(gdb) advance fonction_name
(gdb) advance fichier.c:100
```

---

## 👁️ Inspection des variables

### `print` (ou `p`) : Afficher une variable

```gdb
(gdb) print x
(gdb) p x
```

**Sortie :**
```
$1 = 42
```

Le `$1` est une variable GDB que vous pouvez réutiliser :
```gdb
(gdb) print $1 + 10
$2 = 52
```

---

### Formats d'affichage avec `print`

```gdb
# Décimal (par défaut)
(gdb) print x

# Hexadécimal
(gdb) print/x x
(gdb) p/x 255
$1 = 0xff

# Binaire
(gdb) print/t x
(gdb) p/t 5
$2 = 101

# Octal
(gdb) print/o x

# Caractère
(gdb) print/c x

# Adresse
(gdb) print/a ptr
```

---

### `display` : Afficher automatiquement à chaque arrêt

```gdb
(gdb) display x
(gdb) display y
```

**Utilité :** Voir l'évolution d'une variable à chaque step.

**Lister :**
```gdb
(gdb) info display
```

**Supprimer :**
```gdb
(gdb) undisplay 1
```

---

### `info locals` : Afficher toutes les variables locales

```gdb
(gdb) info locals
```

---

### `info args` : Afficher les arguments de la fonction

```gdb
(gdb) info args
```

---

## 🧱 Pile d'appels (Call Stack)

### `backtrace` (ou `bt`) : Afficher la pile d'appels

```gdb
(gdb) backtrace
(gdb) bt
```

**Sortie typique :**
```
#0  crash_function () at program.c:45
#1  0x000000000040118a in intermediate () at program.c:30
#2  0x00000000004011b5 in main () at program.c:15
```

**Lecture :**
- `#0` : Fonction actuelle (où on est arrêté)
- `#1` : Fonction appelante
- `#2` : main (point de départ)

---

### `frame` (ou `f`) : Sélectionner un frame

```gdb
(gdb) frame 1
(gdb) f 1
```

**Utilité :** Se déplacer dans la pile pour inspecter les variables à différents niveaux.

```gdb
(gdb) bt
#0  foo () at program.c:20
#1  bar () at program.c:30
#2  main () at program.c:10

(gdb) frame 1
(gdb) info locals  # Variables de bar()
```

---

### `up` / `down` : Monter/descendre dans la pile

```gdb
(gdb) up      # Aller vers la fonction appelante
(gdb) down    # Revenir vers la fonction appelée
```

---

## 📍 Watchpoints (Surveiller une variable)

Un **watchpoint** arrête le programme quand une variable change.

### `watch` : Surveiller une variable

```gdb
(gdb) watch x
```

Le programme s'arrêtera dès que `x` change de valeur.

---

### `rwatch` : Surveiller la lecture

```gdb
(gdb) rwatch x
```

S'arrête quand `x` est **lue**.

---

### `awatch` : Surveiller lecture et écriture

```gdb
(gdb) awatch x
```

---

### `info watchpoints` : Lister les watchpoints

```gdb
(gdb) info watchpoints
```

---

## 🎯 Catchpoints (Attraper des événements)

### `catch` : Attraper un signal

```gdb
# Attraper un signal
(gdb) catch signal SIGSEGV

# Attraper un throw C++ (fonctionne aussi en C pour certains événements)
(gdb) catch throw

# Attraper un fork
(gdb) catch fork

# Attraper un syscall
(gdb) catch syscall write
```

---

## 🔍 Examiner la mémoire

### `x` : Examiner la mémoire (examine)

```gdb
(gdb) x/format adresse
```

**Format :** `nombre format taille`

**Exemples :**

```gdb
# Afficher 4 entiers en décimal à partir de &array
(gdb) x/4d &array

# Afficher 10 octets en hexadécimal
(gdb) x/10xb ptr

# Afficher une chaîne
(gdb) x/s string_ptr

# Afficher 20 instructions assembleur
(gdb) x/20i $pc
```

**Formats :**
- `d` : décimal
- `x` : hexadécimal
- `o` : octal
- `t` : binaire
- `c` : caractère
- `s` : chaîne
- `i` : instruction assembleur

**Tailles :**
- `b` : byte (1 octet)
- `h` : halfword (2 octets)
- `w` : word (4 octets)
- `g` : giant (8 octets)

---

### Examiner des pointeurs

```gdb
# Afficher l'adresse du pointeur
(gdb) print ptr
$1 = (int *) 0x7fffffffe410

# Afficher la valeur pointée
(gdb) print *ptr
$2 = 42

# Examiner la mémoire pointée
(gdb) x/4d ptr
```

---

## 📝 Lister le code source

### `list` (ou `l`) : Afficher le code source

```gdb
# Afficher autour de la position actuelle
(gdb) list

# Afficher autour d'une ligne
(gdb) list 42

# Afficher une fonction
(gdb) list main

# Afficher un fichier:ligne
(gdb) list fichier.c:100
```

---

### `list -` : Afficher les lignes précédentes

```gdb
(gdb) list -
```

---

## 🔧 Modifier des valeurs pendant le debug

### `set` : Modifier une variable

```gdb
# Modifier une variable
(gdb) set variable x = 100
(gdb) set var x = 100

# Modifier un pointeur
(gdb) set var *ptr = 42

# Modifier un tableau
(gdb) set var array[0] = 10
```

**Utilité :** Tester différents scénarios sans recompiler.

---

### `call` : Appeler une fonction

```gdb
(gdb) call printf("Debug: x = %d\n", x)
(gdb) call malloc(100)
```

**Utilité :** Appeler des fonctions pendant le debug.

---

## 🎨 Interface TUI (Text User Interface)

### Activer le mode TUI

```bash
# Au lancement
gdb -tui ./program

# Depuis GDB
(gdb) tui enable
(gdb) layout src
```

---

### Layouts TUI

```gdb
# Code source + commandes
(gdb) layout src

# Assembleur + commandes
(gdb) layout asm

# Code source + assembleur
(gdb) layout split

# Registres + code source
(gdb) layout regs

# Désactiver TUI
(gdb) tui disable
```

---

### Navigation TUI

**Raccourcis clavier :**
- `Ctrl-X A` : Activer/désactiver TUI
- `Ctrl-X O` : Changer de fenêtre active
- `Ctrl-X 1` : Layout source seul
- `Ctrl-X 2` : Layout split
- `Ctrl-L` : Rafraîchir l'écran

---

## 🧵 Debugging multi-threadé

### `info threads` : Lister les threads

```gdb
(gdb) info threads
  Id   Target Id                                    Frame
* 1    Thread 0x7ffff7fc3740 (LWP 1234) "program"  main () at program.c:10
  2    Thread 0x7ffff77c0700 (LWP 1235) "program"  worker () at program.c:45
```

L'astérisque `*` indique le thread actuel.

---

### `thread` : Sélectionner un thread

```gdb
(gdb) thread 2
```

---

### `thread apply all` : Exécuter une commande sur tous les threads

```gdb
# Backtrace de tous les threads
(gdb) thread apply all backtrace
(gdb) thread apply all bt

# Afficher info locals de tous les threads
(gdb) thread apply all info locals
```

---

## 📊 Informations système

### `info` : Obtenir diverses informations

```gdb
# Registres
(gdb) info registers
(gdb) info reg

# Variables locales
(gdb) info locals

# Variables globales et statiques
(gdb) info variables

# Fonctions
(gdb) info functions

# Types
(gdb) info types

# Fichiers sources
(gdb) info sources

# Frames
(gdb) info frame
```

---

## 💾 Core dumps

### Analyser un core dump

```bash
# Générer un core dump (si programme crash)
ulimit -c unlimited  # Autoriser les core dumps
./program

# Le fichier core est généré
gdb ./program core
```

---

### Dans GDB avec un core dump

```gdb
(gdb) backtrace
(gdb) frame 0
(gdb) info locals
(gdb) print variable
```

**Utilité :** Débugger un crash en production sans relancer le programme.

---

## 🎬 Enregistrement et replay (rr)

**Note :** `rr` est un outil séparé (Record and Replay) qui s'intègre avec GDB.

```bash
# Installer rr
sudo apt install rr

# Enregistrer une exécution
rr record ./program

# Rejouer dans GDB
rr replay
```

**Avantage :** Débugger en "remontant le temps" (reverse debugging).

---

## 📜 Scripts GDB

### Exécuter des commandes depuis un fichier

Créer un fichier `.gdbinit` ou un script :

```gdb
# fichier: debug.gdb
break main
run
next
next
print x
continue
```

Utiliser :
```bash
gdb -x debug.gdb ./program
```

---

### Automatisation avec `.gdbinit`

Créer `~/.gdbinit` pour des commandes exécutées à chaque lancement :

```gdb
# ~/.gdbinit
set pagination off
set print pretty on
set history save on
set history filename ~/.gdb_history
```

---

## 🔍 Commandes avancées

### `disassemble` (ou `disas`) : Afficher l'assembleur

```gdb
# Désassembler la fonction actuelle
(gdb) disassemble

# Désassembler une fonction
(gdb) disassemble main

# Désassembler une adresse
(gdb) disassemble 0x401156
```

---

### `info symbol` : Obtenir le symbole d'une adresse

```gdb
(gdb) info symbol 0x401156
main + 42 in section .text
```

---

### `ptype` : Afficher le type d'une variable

```gdb
(gdb) ptype x
type = int

(gdb) ptype ptr
type = int *

(gdb) ptype struct_var
type = struct person {
    char name[50];
    int age;
}
```

---

### `whatis` : Type simplifié

```gdb
(gdb) whatis x
type = int
```

---

## 🎯 Workflow typique de debugging

### Scénario 1 : Programme qui crash (Segmentation Fault)

```gdb
$ gdb ./program
(gdb) run
Program received signal SIGSEGV, Segmentation fault.
0x0000000000401167 in crash_function () at program.c:45
45          *ptr = 10;

(gdb) backtrace
#0  0x0000000000401167 in crash_function () at program.c:45
#1  0x0000000000401189 in main () at program.c:30

(gdb) info locals
ptr = 0x0

(gdb) print ptr
$1 = (int *) 0x0

# Conclusion : ptr est NULL !
```

---

### Scénario 2 : Résultat incorrect

```gdb
$ gdb ./program
(gdb) break main
(gdb) run

(gdb) break calculate
(gdb) continue

Breakpoint 2, calculate (x=5, y=10) at program.c:15
15      int result = x * y;

(gdb) next
16      return result + 1;

(gdb) print result
$1 = 50

(gdb) print x
$2 = 5

(gdb) print y
$3 = 10

# Vérifier chaque étape du calcul
```

---

### Scénario 3 : Boucle infinie

```gdb
$ gdb ./program
(gdb) run
^C  # Ctrl-C pour interrompre

Program received signal SIGINT, Interrupt.
0x000000000040118a in loop_function () at program.c:50
50      while (condition) {

(gdb) print condition
$1 = 1

(gdb) print i
$2 = 0

# i n'est jamais incrémenté !
```

---

### Scénario 4 : Fuite mémoire

```gdb
$ gdb ./program
(gdb) break malloc
(gdb) run

# À chaque malloc
(gdb) backtrace
(gdb) continue

# Vérifier qu'il y a bien des free() correspondants
```

**Mieux :** Utiliser Valgrind pour les fuites mémoire.

---

## 🎓 Exemples pratiques complets

### Exemple 1 : Débugger un pointeur NULL

**Code :**
```c
#include <stdio.h>

void crash() {
    int *ptr = NULL;
    *ptr = 42;  // CRASH !
}

int main() {
    crash();
    return 0;
}
```

**Session GDB :**
```bash
$ gcc -g test.c -o test
$ gdb ./test

(gdb) run
Program received signal SIGSEGV, Segmentation fault.
0x0000000000401141 in crash () at test.c:5
5           *ptr = 42;

(gdb) print ptr
$1 = (int *) 0x0

(gdb) backtrace
#0  0x0000000000401141 in crash () at test.c:5
#1  0x000000000040115a in main () at test.c:9

# Solution : ptr est NULL, il faut l'initialiser !
```

---

### Exemple 2 : Traquer une variable qui change

**Code :**
```c
#include <stdio.h>

int global_counter = 0;

void mysterious_function() {
    global_counter = 99;  // Qui fait ça ?
}

void another_function() {
    global_counter++;
}

int main() {
    global_counter = 0;
    another_function();
    mysterious_function();
    printf("Counter: %d\n", global_counter);
    return 0;
}
```

**Session GDB :**
```bash
$ gcc -g test.c -o test
$ gdb ./test

(gdb) watch global_counter
Hardware watchpoint 1: global_counter

(gdb) run
Hardware watchpoint 1: global_counter

Old value = 0
New value = 1
another_function () at test.c:10
10      }

(gdb) continue
Hardware watchpoint 1: global_counter

Old value = 1
New value = 99
mysterious_function () at test.c:5
5       }

# On a trouvé qui modifie la variable !
```

---

### Exemple 3 : Débugger un tableau

**Code :**
```c
#include <stdio.h>

int main() {
    int array[5] = {1, 2, 3, 4, 5};
    int sum = 0;

    for (int i = 0; i <= 5; i++) {  // Bug : i <= 5 au lieu de i < 5
        sum += array[i];
    }

    printf("Sum: %d\n", sum);
    return 0;
}
```

**Session GDB :**
```bash
$ gcc -g -Wall test.c -o test
$ gdb ./test

(gdb) break 8
(gdb) run

(gdb) display i
(gdb) display array[i]

# Step par step
(gdb) next
1: i = 0
2: array[i] = 1

(gdb) next
1: i = 1
2: array[i] = 2

# ... continue jusqu'à i = 5
(gdb) next
1: i = 5
2: array[i] = <valeur aléatoire>  # Hors limites !

# Bug trouvé : accès hors tableau
```

---

### Exemple 4 : Condition de breakpoint

**Code :**
```c
#include <stdio.h>

int main() {
    for (int i = 0; i < 100; i++) {
        int result = i * i;
        printf("%d\n", result);
    }
    return 0;
}
```

**Session GDB :**
```bash
$ gcc -g test.c -o test
$ gdb ./test

# On veut s'arrêter seulement quand i == 50
(gdb) break 5 if i == 50
Breakpoint 1 at 0x401156: file test.c, line 5.

(gdb) run
Breakpoint 1, main () at test.c:5
5               int result = i * i;

(gdb) print i
$1 = 50

# Parfait ! On s'arrête seulement à i == 50
```

---

## 📊 Tableau récapitulatif des commandes essentielles

| Commande | Raccourci | Description | Exemple |
|----------|-----------|-------------|---------|
| **Contrôle** |
| `run` | `r` | Démarrer le programme | `r arg1 arg2` |
| `continue` | `c` | Continuer l'exécution | `c` |
| `next` | `n` | Ligne suivante (over) | `n` |
| `step` | `s` | Ligne suivante (into) | `s` |
| `finish` | `fin` | Finir la fonction | `fin` |
| `quit` | `q` | Quitter GDB | `q` |
| **Breakpoints** |
| `break` | `b` | Mettre un breakpoint | `b main`, `b 42` |
| `delete` | `d` | Supprimer breakpoint | `d 1` |
| `info breakpoints` | `i b` | Lister breakpoints | `i b` |
| `disable` | - | Désactiver breakpoint | `disable 1` |
| `enable` | - | Activer breakpoint | `enable 1` |
| **Variables** |
| `print` | `p` | Afficher variable | `p x`, `p/x y` |
| `display` | - | Auto-afficher variable | `display x` |
| `info locals` | - | Variables locales | `i locals` |
| `set` | - | Modifier variable | `set var x=10` |
| **Stack** |
| `backtrace` | `bt` | Pile d'appels | `bt` |
| `frame` | `f` | Sélectionner frame | `f 1` |
| `up` / `down` | - | Monter/descendre | `up`, `down` |
| **Code** |
| `list` | `l` | Afficher code | `l`, `l main` |
| **Watchpoints** |
| `watch` | - | Surveiller variable | `watch x` |
| `info watchpoints` | - | Lister watchpoints | `i watch` |
| **Mémoire** |
| `x` | - | Examiner mémoire | `x/4d array` |
| **Info** |
| `info registers` | `i r` | Afficher registres | `i r` |
| `info threads` | `i threads` | Lister threads | `i threads` |
| `ptype` | - | Type d'une variable | `ptype x` |

---

## 💡 Astuces et raccourcis

### 1. Raccourcis clavier

```gdb
# Répéter la dernière commande
(gdb) <Entrée>

# Historique des commandes
(gdb) <Flèche Haut/Bas>

# Complétion automatique
(gdb) break ma<Tab>
break main
```

---

### 2. Abréviations

La plupart des commandes peuvent être abrégées :

```gdb
(gdb) b main          # break main
(gdb) r               # run
(gdb) n               # next
(gdb) s               # step
(gdb) c               # continue
(gdb) p x             # print x
(gdb) bt              # backtrace
(gdb) i b             # info breakpoints
(gdb) i locals        # info locals
```

---

### 3. Commandes combinées

```gdb
# Breakpoint avec commandes automatiques
(gdb) break main
(gdb) commands 1
> print x
> continue
> end

# À chaque fois qu'on arrive à main, affiche x et continue
```

---

### 4. Variables GDB (`$1`, `$2`, etc.)

```gdb
(gdb) print x
$1 = 42

(gdb) print $1 + 10
$2 = 52

(gdb) set $temp = x * 2
(gdb) print $temp
$3 = 84
```

---

### 5. Registres spéciaux

```gdb
# Program Counter
(gdb) print $pc
$1 = (void (*)()) 0x401156 <main+10>

# Stack Pointer
(gdb) print $sp

# Disassembler autour du PC
(gdb) x/10i $pc
```

---

### 6. Pretty printing

```gdb
# Activer le pretty printing
(gdb) set print pretty on

# Structures formatées
(gdb) print struct_var
$1 = {
  name = "John",
  age = 30,
  salary = 50000.00
}
```

---

## ⚠️ Problèmes courants

### 1. "No debugging symbols found"

**Cause :** Pas compilé avec `-g`

**Solution :**
```bash
gcc -g program.c -o program
```

---

### 2. "Optimized out"

**Cause :** Compilé avec optimisations (`-O2`, `-O3`)

**Solution :**
```bash
# Pour debug, compiler sans optimisations
gcc -g -O0 program.c -o program
```

---

### 3. Cannot access memory

**Cause :** Pointeur invalide (NULL, freed, hors limites)

**Solution :** Vérifier les pointeurs avec `print` avant de les déréférencer.

---

### 4. GDB ne s'arrête pas aux breakpoints

**Causes possibles :**
- Breakpoint sur une ligne sans code exécutable (déclaration, commentaire)
- Code optimisé (lignes fusionnées)
- Mauvais fichier source

**Solution :**
```gdb
(gdb) info line 42  # Vérifier si la ligne est exécutable
(gdb) list          # Vérifier le code autour
```

---

## 🎯 Checklist : Commandes à connaître absolument

### ✅ Niveau Débutant (5 commandes)
- [ ] `run` : Démarrer le programme
- [ ] `break` : Mettre un breakpoint
- [ ] `next` : Ligne suivante
- [ ] `print` : Afficher une variable
- [ ] `backtrace` : Voir la pile d'appels

**Avec ces 5 commandes, vous pouvez débugger 80% des bugs simples.**

---

### ✅ Niveau Intermédiaire (+5 commandes)
- [ ] `step` : Entrer dans les fonctions
- [ ] `continue` : Continuer l'exécution
- [ ] `finish` : Sortir de la fonction
- [ ] `watch` : Surveiller une variable
- [ ] `info locals` : Voir toutes les variables

---

### ✅ Niveau Avancé (+5 commandes)
- [ ] `frame` / `up` / `down` : Naviguer dans la pile
- [ ] `x` : Examiner la mémoire
- [ ] `set` : Modifier des valeurs
- [ ] `disassemble` : Voir l'assembleur
- [ ] `thread apply all` : Multi-threading

---

## 📝 Aide-mémoire rapide (copier-coller)

```bash
# Compiler avec debug
gcc -g -Wall program.c -o program

# Lancer GDB
gdb ./program

# Session typique
(gdb) break main              # Breakpoint sur main
(gdb) run                     # Lancer
(gdb) next                    # Ligne suivante (over)
(gdb) step                    # Ligne suivante (into)
(gdb) print x                 # Afficher x
(gdb) backtrace               # Pile d'appels
(gdb) info locals             # Variables locales
(gdb) continue                # Continuer
(gdb) quit                    # Quitter

# Debug d'un crash
(gdb) run
# ... crash ...
(gdb) backtrace               # Où ça a crashé ?
(gdb) frame 0                 # Aller au frame du crash
(gdb) info locals             # Variables locales
(gdb) print ptr               # Vérifier les pointeurs

# Surveiller une variable
(gdb) watch global_var
(gdb) run

# Breakpoint conditionnel
(gdb) break 42 if x == 10

# Mode TUI
(gdb) layout src
(gdb) layout split
```

---

## 🔗 Pour aller plus loin

### Documentation officielle

```bash
man gdb
info gdb
```

### En ligne

- **GDB Manual** : [https://sourceware.org/gdb/documentation/](https://sourceware.org/gdb/documentation/)
- **GDB Quick Reference** : [https://users.ece.utexas.edu/~adnan/gdb-refcard.pdf](https://users.ece.utexas.edu/~adnan/gdb-refcard.pdf)
- **GDB to LLDB Command Map** : Comparaison GDB/LLDB

### Alternatives et compléments

- **LLDB** : Débogueur LLVM (alternative à GDB)
- **rr** : Record and Replay debugging
- **Valgrind** : Détection de fuites mémoire
- **AddressSanitizer** : Détection rapide de bugs mémoire

---

## 📝 Conclusion

GDB est un outil **puissant** mais qui demande de la pratique. Voici comment progresser :

### Phase 1 : Débutant (Semaine 1)
**Objectif :** Débugger des crashes simples

**Commandes à maîtriser :**
- `run`, `break`, `next`, `print`, `backtrace`

**Pratique :**
- Créez un programme qui crash volontairement
- Utilisez GDB pour trouver où et pourquoi

---

### Phase 2 : Intermédiaire (Semaines 2-4)
**Objectif :** Comprendre le flux du programme

**Commandes à ajouter :**
- `step`, `finish`, `continue`, `list`, `info locals`

**Pratique :**
- Débugger un calcul incorrect
- Suivre le flux d'un algorithme complexe

---

### Phase 3 : Avancé (Mois 2+)
**Objectif :** Maîtriser tous les aspects

**Commandes à ajouter :**
- `watch`, `x`, `set`, `disassemble`, mode TUI

**Pratique :**
- Débugger du code multi-threadé
- Analyser des core dumps en production
- Contribuer à des projets open source

---

### Le conseil ultime

**N'ayez pas peur de GDB !**

Au début, ça semble complexe, mais :
1. Commencez avec 5 commandes de base
2. Pratiquez sur de petits programmes
3. Ajoutez progressivement de nouvelles commandes
4. Après 2-3 semaines, GDB deviendra naturel

**GDB vous fera gagner des heures (voire des jours) de debugging.** C'est un investissement qui en vaut la peine !

---


⏭️ [Appels système Linux](/annexes/antiseches/03-appels-systeme-linux.md)
