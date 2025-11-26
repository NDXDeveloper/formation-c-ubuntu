🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 15.2 Utilisation de GDB

## Introduction

Vous avez écrit du code en C, vous l'avez compilé, mais votre programme ne fonctionne pas comme prévu. Il crash, il donne des résultats incorrects, ou pire, il semble fonctionner mais produit des bugs subtils et intermittents. Que faire ?

La réponse : **GDB** (GNU Debugger).

GDB est l'outil de débogage le plus puissant et le plus utilisé pour les programmes C et C++ sur Linux. C'est un **débogueur** - un programme qui vous permet d'observer ce qui se passe **à l'intérieur** de votre programme pendant qu'il s'exécute. Vous pouvez arrêter l'exécution, inspecter les variables, suivre le flux du programme ligne par ligne, et même modifier les valeurs en temps réel.

Dans cette section, nous allons apprendre les fondamentaux de GDB, de la compilation au débogage, en passant par les concepts essentiels. À la fin, vous serez capable de déboguer efficacement n'importe quel programme C.

---

## 1. Qu'est-ce que GDB ?

### Définition

**GDB** (GNU Debugger) est un débogueur en ligne de commande qui permet de :
- **Arrêter** l'exécution à des points précis (breakpoints)
- **Avancer** ligne par ligne (stepping)
- **Inspecter** les valeurs des variables
- **Examiner** la mémoire
- **Analyser** les crashs (segmentation faults)
- **Comprendre** le flux d'exécution (backtrace)
- **Modifier** les valeurs pendant l'exécution
- **Déboguer** à distance

### Pourquoi GDB et pas `printf()` ?

Beaucoup de débutants déboguent en ajoutant des `printf()` partout dans leur code :

```c
printf("DEBUG: x = %d\n", x);
printf("DEBUG: Entrée dans la fonction\n");
printf("DEBUG: Avant la boucle\n");
```

**Problèmes avec cette approche** :
- ❌ **Lent** : Recompiler à chaque fois
- ❌ **Pollue** le code avec du debug temporaire
- ❌ **Limité** : Ne peut pas voir l'état complet du programme
- ❌ **Intrusive** : Change le comportement du programme (timing, I/O)
- ❌ **Oublis** : On oublie souvent de retirer les `printf()` de debug

**Avantages de GDB** :
- ✅ **Aucune modification du code** : Pas de `printf()` à ajouter/retirer
- ✅ **Inspection complète** : Voir toutes les variables à tout moment
- ✅ **Contrôle total** : Arrêter, avancer, reculer dans l'exécution
- ✅ **Analyse post-mortem** : Déboguer les crashs avec les core dumps
- ✅ **Non-intrusif** : N'affecte pas le comportement du programme

### Analogie

Imaginez que vous essayez de comprendre pourquoi une voiture ne démarre pas :

**Méthode `printf()`** : Vous allumez les phares, klaxonnez, vérifiez le rétroviseur... Chaque test nécessite d'essayer de démarrer la voiture.

**Méthode GDB** : Vous ouvrez le capot, vous examinez le moteur, testez chaque composant individuellement, et identifiez exactement le problème sans avoir à démarrer la voiture à chaque fois.

---

## 2. Installation et Vérification

### Installation sur Ubuntu

GDB est généralement préinstallé avec le paquet `build-essential`. Si ce n'est pas le cas :

```bash
sudo apt update
sudo apt install gdb
```

### Vérification de l'Installation

```bash
gdb --version
```

Vous devriez voir quelque chose comme :

```
GNU gdb (Ubuntu 12.1-0ubuntu1~22.04) 12.1
Copyright (C) 2022 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
```

### Version Minimale

N'importe quelle version récente (7.0+) fonctionne bien. Les versions modernes (10.0+) ont des améliorations d'interface et de fonctionnalités.

---

## 3. Compilation pour le Débogage

### Le Flag `-g` : Indispensable

Pour que GDB puisse afficher le code source et les noms de variables, vous **devez** compiler avec le flag `-g` :

```bash
gcc -g -o programme programme.c
```

Le flag `-g` ajoute les **symboles de débogage** au binaire :
- Noms des variables
- Numéros de lignes
- Noms des fonctions
- Informations de types

### Exemple : Avec et Sans `-g`

**Programme simple** :

```c
// hello.c
#include <stdio.h>

int main() {
    int nombre = 42;
    printf("Le nombre est : %d\n", nombre);
    return 0;
}
```

**Compilation sans `-g`** :

```bash
gcc -o hello hello.c
gdb ./hello
```

Dans GDB :

```gdb
(gdb) break main
Breakpoint 1 at 0x1139
(gdb) run
Breakpoint 1, 0x0000555555555139 in main ()
(gdb) list
No symbol table is loaded.  Use the "file" command.
```

❌ GDB ne peut pas afficher le code source !

**Compilation avec `-g`** :

```bash
gcc -g -o hello hello.c
gdb ./hello
```

Dans GDB :

```gdb
(gdb) break main
Breakpoint 1 at 0x1149: file hello.c, line 4.
(gdb) run
Breakpoint 1, main () at hello.c:4
4           int nombre = 42;
(gdb) list
1       #include <stdio.h>
2
3       int main() {
4           int nombre = 42;
5           printf("Le nombre est : %d\n", nombre);
6           return 0;
7       }
```

✅ GDB affiche le code source avec les numéros de ligne !

### Optimisation vs Débogage

**Pendant le débogage**, compilez **sans optimisation** :

```bash
gcc -g -O0 -o programme programme.c
```

Le flag `-O0` désactive toutes les optimisations. Pourquoi ?

**Avec optimisation (`-O2` ou `-O3`)** :
- Le compilateur réorganise le code
- Les variables peuvent être éliminées
- Les fonctions peuvent être "inlinées" (insérées directement)
- Le flux d'exécution devient difficile à suivre

**Résultat** : GDB affiche souvent `<optimized out>` pour les variables.

**Recommandation** :
- **Debug builds** : `-g -O0 -Wall -Wextra`
- **Release builds** : `-O2` ou `-O3` (sans `-g`)

### Flags de Compilation Recommandés pour le Debug

```bash
gcc -g -O0 -Wall -Wextra -Werror -o programme programme.c
```

- **-g** : Symboles de débogage
- **-O0** : Pas d'optimisation
- **-Wall** : Tous les warnings courants
- **-Wextra** : Warnings supplémentaires
- **-Werror** : Traite les warnings comme des erreurs

---

## 4. Lancer GDB

### Méthode 1 : Lancer avec un Programme

```bash
gdb ./programme
```

Ceci lance GDB et charge le programme. Vous êtes dans l'**invite GDB** :

```
GNU gdb (Ubuntu 12.1-0ubuntu1~22.04) 12.1
...
Reading symbols from ./programme...
(gdb) _
```

Le curseur `(gdb)` indique que GDB attend vos commandes.

### Méthode 2 : Attacher à un Processus en Cours

Si votre programme est déjà en cours d'exécution, vous pouvez y attacher GDB :

```bash
# Trouver le PID du processus
ps aux | grep programme

# Attacher GDB au processus
gdb -p <PID>
```

Utile pour déboguer des programmes qui tournent en tâche de fond ou des serveurs.

### Méthode 3 : Analyser un Core Dump

Si votre programme a crashé et généré un core dump :

```bash
gdb ./programme core
```

Ceci vous permet d'analyser **pourquoi** le programme a crashé, même après coup.

### Quitter GDB

À tout moment, pour quitter GDB :

```gdb
(gdb) quit
```

Ou le raccourci : **Ctrl+D**

Si le programme est en cours d'exécution, GDB vous demandera confirmation :

```
A debugging session is active.
    Inferior 1 [process 12345] will be killed.
Quit anyway? (y or n) y
```

---

## 5. Commandes de Base

GDB utilise une **interface en ligne de commande**. Voici les commandes les plus fondamentales que vous utiliserez constamment.

### Obtenir de l'Aide

GDB a un système d'aide intégré :

```gdb
help           # Aide générale
help running   # Aide sur les commandes d'exécution
help breakpoints  # Aide sur les breakpoints
help <commande>   # Aide sur une commande spécifique
```

Exemple :

```gdb
(gdb) help run
Start debugged program.  You may specify arguments to give it.
Args may include "*", or "[...]"; they are expanded using "sh".
...
```

### Lister le Code Source

Pour voir le code source autour de la ligne actuelle :

```gdb
list
```

Ou raccourci : `l`

Pour voir une fonction spécifique :

```gdb
list main
list nom_fonction
```

Pour voir une ligne spécifique :

```gdb
list 42
```

Pour voir une plage de lignes :

```gdb
list 10,20
```

### Exécuter le Programme

Pour lancer le programme :

```gdb
run
```

Ou raccourci : `r`

Pour lancer avec des **arguments** :

```gdb
run arg1 arg2 arg3
```

Exemple, si votre programme prend des arguments en ligne de commande :

```bash
# Normalement, vous feriez :
./programme fichier.txt --verbose

# Dans GDB :
(gdb) run fichier.txt --verbose
```

### Répéter la Dernière Commande

Appuyer sur **Entrée** sans taper de commande répète la **dernière commande** exécutée.

C'est très pratique pour avancer ligne par ligne (`next`, `next`, `next`...) :

```gdb
(gdb) next
(gdb) [Entrée]
(gdb) [Entrée]
(gdb) [Entrée]
```

---

## 6. Concepts Fondamentaux

Avant de plonger dans les commandes avancées, comprenons quelques concepts clés.

### Le Programme est Contrôlé

Quand vous lancez un programme dans GDB, GDB **contrôle totalement** son exécution. Le programme ne s'exécute que quand GDB lui dit de le faire.

C'est comme avoir une **télécommande** pour votre programme :
- ▶️ **Play** : `continue` (laisse le programme s'exécuter)
- ⏸️ **Pause** : Breakpoint (arrête l'exécution)
- ⏭️ **Avance rapide** : `next`, `step` (une ligne à la fois)
- ⏹️ **Stop** : `kill` (tue le programme)

### L'Exécution est Déterministe

À chaque fois que vous lancez votre programme dans GDB avec les mêmes entrées, il s'exécutera **exactement de la même manière**. Ceci est crucial pour reproduire et corriger les bugs.

### GDB N'Affecte Pas le Comportement (Normalement)

GDB observe le programme sans le modifier (sauf si vous modifiez explicitement des variables). Le programme se comporte **comme si** GDB n'était pas là, sauf qu'il s'arrête aux points que vous avez définis.

### Le Contexte d'Exécution

À tout moment, le programme a un **contexte** :
- **Où suis-je ?** → Quelle ligne de code, quelle fonction
- **D'où je viens ?** → La pile d'appels (call stack)
- **Quelles sont les valeurs ?** → Variables locales, globales, registres

GDB vous permet d'inspecter tout ce contexte.

---

## 7. Workflow Typique de Débogage

Un cycle de débogage typique avec GDB ressemble à ceci :

### Étape 1 : Identifier le Problème

Votre programme :
- Crash avec un segmentation fault
- Donne un résultat incorrect
- Entre dans une boucle infinie
- A un comportement inattendu

### Étape 2 : Reproduire le Bug

Trouvez une **manière reproductible** de déclencher le bug. C'est crucial !

### Étape 3 : Compiler avec `-g -O0`

```bash
gcc -g -O0 -Wall -Wextra -o programme programme.c
```

### Étape 4 : Lancer GDB

```bash
gdb ./programme
```

### Étape 5 : Placer des Breakpoints

Mettez des points d'arrêt aux endroits stratégiques :

```gdb
(gdb) break main
(gdb) break fonction_suspecte
(gdb) break fichier.c:42
```

### Étape 6 : Exécuter

```gdb
(gdb) run
```

### Étape 7 : Observer et Avancer

Quand le programme s'arrête à un breakpoint :
- Inspectez les variables : `print variable`
- Avancez ligne par ligne : `next`, `step`
- Vérifiez la pile d'appels : `backtrace`

### Étape 8 : Identifier la Cause

Observez **où** et **quand** les valeurs deviennent incorrectes.

### Étape 9 : Corriger le Code

Quittez GDB, corrigez le code source, recompilez.

### Étape 10 : Vérifier

Relancez dans GDB pour confirmer que le bug est corrigé.

---

## 8. Premier Exemple Complet

Mettons tout cela en pratique avec un exemple simple.

### Le Programme (avec bug)

```c
// bug.c
#include <stdio.h>

int diviser(int a, int b) {
    return a / b;
}

int main() {
    int x = 10;
    int y = 0;  // Bug : division par zéro !
    int resultat = diviser(x, y);
    printf("Résultat : %d\n", resultat);
    return 0;
}
```

### Compilation

```bash
gcc -g -o bug bug.c
```

### Exécution Normale (sans GDB)

```bash
./bug
```

**Résultat** :

```
Floating point exception (core dumped)
```

Le programme crash ! Mais **où** exactement ? Et **pourquoi** ?

### Débogage avec GDB

```bash
gdb ./bug
```

```gdb
GNU gdb (Ubuntu 12.1-0ubuntu1~22.04) 12.1
...
Reading symbols from ./bug...
(gdb) _
```

**Étape 1** : Lancer le programme dans GDB

```gdb
(gdb) run
Starting program: /home/user/bug

Program received signal SIGFPE, Arithmetic exception.
0x0000555555555139 in diviser (a=10, b=0) at bug.c:4
4           return a / b;
```

✨ **GDB nous dit exactement où le crash se produit** : ligne 4, dans la fonction `diviser`, avec `a=10` et `b=0`.

**Étape 2** : Inspecter les variables

```gdb
(gdb) print a
$1 = 10

(gdb) print b
$2 = 0
```

Aha ! `b` est 0, ce qui cause la division par zéro.

**Étape 3** : Voir d'où vient l'appel

```gdb
(gdb) backtrace
#0  0x0000555555555139 in diviser (a=10, b=0) at bug.c:4
#1  0x0000555555555165 in main () at bug.c:10
```

Le backtrace montre que `diviser()` a été appelée depuis `main()` à la ligne 10.

**Étape 4** : Examiner l'appelant

```gdb
(gdb) frame 1
#1  0x0000555555555165 in main () at bug.c:10
10          int resultat = diviser(x, y);

(gdb) print y
$3 = 0
```

Dans `main()`, la variable `y` est initialisée à 0.

**Diagnostic** : Le bug est à la ligne 9 de `main()` où `y` est initialisé à 0. Il faut soit changer cette valeur, soit ajouter une vérification avant la division.

### Correction

```c
// bug_corrige.c
#include <stdio.h>

int diviser(int a, int b) {
    if (b == 0) {
        printf("Erreur : division par zéro\n");
        return 0;
    }
    return a / b;
}

int main() {
    int x = 10;
    int y = 0;
    int resultat = diviser(x, y);
    printf("Résultat : %d\n", resultat);
    return 0;
}
```

### Vérification

```bash
gcc -g -o bug_corrige bug_corrige.c
./bug_corrige
```

**Résultat** :

```
Erreur : division par zéro
Résultat : 0
```

Le programme ne crash plus ! ✅

---

## 9. Raccourcis et Abbréviations

GDB accepte des **abbréviations** pour la plupart des commandes. Cela rend l'utilisation beaucoup plus rapide.

### Abbréviations Courantes

| Commande Complète | Abréviation | Action |
|-------------------|-------------|--------|
| `run` | `r` | Lance le programme |
| `break` | `b` | Place un breakpoint |
| `continue` | `c` | Continue l'exécution |
| `next` | `n` | Ligne suivante (sans entrer dans les fonctions) |
| `step` | `s` | Ligne suivante (en entrant dans les fonctions) |
| `print` | `p` | Affiche une variable |
| `list` | `l` | Liste le code source |
| `backtrace` | `bt` | Affiche la pile d'appels |
| `quit` | `q` | Quitte GDB |
| `info` | `i` | Informations diverses |

### Exemples

Au lieu de :

```gdb
(gdb) break main
(gdb) run
(gdb) next
(gdb) print variable
(gdb) backtrace
```

Vous pouvez taper :

```gdb
(gdb) b main
(gdb) r
(gdb) n
(gdb) p variable
(gdb) bt
```

Beaucoup plus rapide !

### Complétion Automatique

GDB supporte la **complétion avec Tab** comme dans le shell :

```gdb
(gdb) bre[Tab]
(gdb) break

(gdb) print var[Tab]
variable1    variable2    variable_longue
(gdb) print variable_longue
```

Utilisez Tab généreusement pour gagner du temps.

---

## 10. Ressources et Documentation

### Man Page

```bash
man gdb
```

Affiche le manuel complet de GDB.

### Info Pages

GDB a une documentation très complète accessible via :

```bash
info gdb
```

C'est **la** référence officielle. Elle contient des centaines de pages d'explications détaillées.

### Aide Intégrée

Dans GDB :

```gdb
(gdb) help
(gdb) help <sujet>
```

### Documentation en Ligne

- **Site officiel** : https://www.gnu.org/software/gdb/documentation/
- **GDB Quick Reference** : Beaucoup de cheat sheets disponibles en ligne
- **Tutoriels** : De nombreux tutoriels et vidéos existent

### Livres Recommandés

- **"The Art of Debugging with GDB, DDD, and Eclipse"** par Matloff & Salzman
- **"GDB Pocket Reference"** par Arnold Robbins

---

## 11. Comparaison : GDB vs IDE Debuggers

Vous vous demandez peut-être : "Pourquoi apprendre GDB alors que mon IDE (VS Code, CLion, etc.) a un débogueur graphique ?"

### Avantages de GDB

- ✅ **Universel** : Fonctionne partout (serveurs, SSH, conteneurs)
- ✅ **Léger** : Pas besoin d'IDE lourd
- ✅ **Puissant** : Plus de fonctionnalités avancées
- ✅ **Scriptable** : Automatisable avec des scripts
- ✅ **Débogage distant** : Peut déboguer des programmes sur d'autres machines
- ✅ **Core dumps** : Analyse post-mortem

### Avantages des IDE Debuggers

- ✅ **Interface graphique** : Plus intuitif pour les débutants
- ✅ **Visualisation** : Graphiques de structures de données
- ✅ **Intégré** : Directement dans l'éditeur de code
- ✅ **Pas de commandes à mémoriser** : Boutons et menus

### Recommandation

**Apprenez GDB même si vous utilisez un IDE.**

Pourquoi ?
1. Les IDE utilisent **GDB en arrière-plan** - comprendre GDB vous aide à comprendre votre IDE
2. Sur les serveurs de production ou en SSH, vous n'aurez **que** GDB
3. GDB est plus **puissant** et **flexible** que la plupart des interfaces graphiques
4. C'est une **compétence professionnelle** attendue pour les postes système et embarqué

**Meilleure approche** : Utilisez les deux selon le contexte
- **Développement quotidien** : IDE avec interface graphique
- **Debugging complexe** : GDB en ligne de commande
- **Serveurs/Production** : GDB uniquement

---

## 12. Configuration de Base (Fichier `.gdbinit`)

Vous pouvez personnaliser GDB avec un fichier de configuration `~/.gdbinit`.

### Exemple de Configuration

```bash
# ~/.gdbinit

# Historique des commandes
set history save on
set history filename ~/.gdb_history
set history size 10000

# Affichage
set print pretty on          # Formatte les structures
set print array on           # Formatte les tableaux
set print array-indexes on   # Affiche les indices
set pagination off           # Pas de pause après une page

# Désassemblage en Intel syntax (optionnel)
set disassembly-flavor intel

# Afficher le contexte automatiquement
set disassemble-next-line on

# Messages plus verbeux
set verbose on
```

### Sauvegarder l'Historique

Avec l'historique activé, vous pouvez utiliser les **flèches haut/bas** pour naviguer dans vos commandes précédentes, comme dans le shell.

### Charger des Scripts au Démarrage

Si vous avez des commandes que vous tapez souvent, créez un fichier `.gdbinit` dans votre répertoire de projet :

```bash
# .gdbinit (dans le répertoire du projet)
break main
break fonction_importante
run arg1 arg2
```

GDB chargera automatiquement ce fichier au démarrage (dans le répertoire courant).

**Note de sécurité** : Pour des raisons de sécurité, GDB peut vous demander confirmation avant de charger un `.gdbinit` local. Vous pouvez ajouter dans votre `~/.gdbinit` :

```bash
set auto-load safe-path /
```

---

## 13. Messages et Sorties de GDB

### Comprendre les Messages

Quand vous lancez GDB, vous verrez beaucoup de texte. Voici comment le lire :

```
GNU gdb (Ubuntu 12.1-0ubuntu1~22.04) 12.1
Copyright (C) 2022 Free Software Foundation, Inc.
...
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from ./programme...
(gdb) _
```

- **GNU gdb...** : Version de GDB
- **Reading symbols...** : GDB charge les symboles de débogage
  - Si compilé avec `-g` : OK ✅
  - Si compilé sans `-g` : "(no debugging symbols found)" ❌

### Messages d'Erreur Courants

**"No symbol table is loaded"**
→ Le programme n'a pas été compilé avec `-g`
→ **Solution** : Recompilez avec `gcc -g`

**"No source file named X"**
→ GDB ne trouve pas le fichier source
→ **Solution** : Assurez-vous que le fichier source existe et que vous êtes dans le bon répertoire

**"Cannot access memory at address 0x..."**
→ Tentative d'accès à une adresse invalide (pointeur NULL, mémoire libérée, etc.)
→ **Solution** : Vérifiez vos pointeurs

---

## 14. Conventions et Terminologie

### Terminologie GDB

| Terme | Signification |
|-------|---------------|
| **Inferior** | Le programme en cours de débogage |
| **Frame** | Un niveau de la pile d'appels (call stack) |
| **Breakpoint** | Point d'arrêt dans le code |
| **Watchpoint** | Surveillance d'une variable |
| **Catchpoint** | Capture d'un événement (signal, exception) |
| **Symbol** | Nom de variable, fonction, ou type |
| **Core dump** | Image mémoire d'un programme crashé |

### Invite de Commande

L'invite `(gdb)` indique que GDB attend votre commande.

Si vous voyez `>`, c'est que GDB attend la **suite** d'une commande multi-lignes.

### Signaux

Quand un programme reçoit un signal (comme SIGSEGV pour un segfault), GDB intercepte le signal et affiche :

```
Program received signal SIGSEGV, Segmentation fault.
```

C'est le moment d'inspecter ce qui s'est passé !

---

## 15. Limites de GDB

GDB est puissant, mais il a des limites :

### Limites Techniques

- ❌ **Optimisation** : Le code optimisé est difficile à déboguer
- ❌ **Threading** : Déboguer les race conditions est très difficile
- ❌ **Temps réel** : GDB ralentit l'exécution, peut cacher des bugs de timing
- ❌ **GUI** : L'interface en ligne de commande peut être intimidante
- ❌ **Courbe d'apprentissage** : Beaucoup de commandes à apprendre

### Quand GDB N'Est Pas la Meilleure Solution

Pour certains problèmes, d'autres outils sont plus adaptés :

- **Fuites mémoire** → Valgrind (memcheck)
- **Race conditions** → Valgrind (helgrind) ou ThreadSanitizer
- **Performance** → perf, Valgrind (callgrind)
- **Problèmes de cache** → Valgrind (cachegrind)
- **Undefined behavior** → UndefinedBehaviorSanitizer (UBSan)
- **Corruption mémoire** → AddressSanitizer (ASan)

**Mais** : GDB reste l'outil universel pour comprendre le comportement d'un programme.

---

## 16. Bonnes Pratiques

### Compilez Toujours avec `-g` pour le Debug

Ne perdez pas de temps à déboguer sans symboles. Toujours :

```bash
gcc -g -O0 -Wall -Wextra -o programme programme.c
```

### Utilisez des Builds Séparés

Ayez deux versions de votre programme :
- **Debug build** : `-g -O0` pour le développement
- **Release build** : `-O2` ou `-O3` pour la production

Utilisez un système de build (Make, CMake) pour gérer cela facilement.

### Reproduisez le Bug

Avant de lancer GDB, assurez-vous de pouvoir **reproduire** le bug de manière fiable. Un bug non-reproductible est très difficile à corriger.

### Commencez Simple

Ne placez pas 50 breakpoints dès le début. Commencez par :
1. `break main`
2. `run`
3. Puis ajoutez des breakpoints stratégiques au fur et à mesure

### Documentez Vos Sessions

Si vous travaillez sur un bug complexe, documentez ce que vous découvrez :

```gdb
(gdb) set logging on
(gdb) set logging file debug_session.txt
```

Toutes vos commandes et sorties seront sauvegardées dans `debug_session.txt`.

### Apprenez Progressivement

Ne tentez pas d'apprendre toutes les commandes GDB d'un coup. Maîtrisez d'abord :
1. `break`, `run`, `continue`
2. `next`, `step`, `finish`
3. `print`, `backtrace`

Puis ajoutez progressivement d'autres commandes à votre arsenal.

---

## 17. Prochaines Étapes

Maintenant que vous comprenez les bases de GDB, vous êtes prêt à explorer les techniques spécifiques de débogage.

Les prochaines sections couvriront :

### 15.2.1 Breakpoints, Watchpoints et Catchpoints
- Comment arrêter l'exécution à des points précis
- Surveiller les changements de variables
- Capturer les événements système

### 15.2.2 Stepping (Avancer pas à pas)
- `next` : Avancer ligne par ligne
- `step` : Entrer dans les fonctions
- `finish` : Sortir d'une fonction
- `continue` : Reprendre l'exécution

### 15.2.3 Backtrace et Analyse de la Pile
- Comprendre la pile d'appels
- Naviguer entre les frames
- Inspecter les variables à différents niveaux

### 15.2.4 Inspection des Variables
- Afficher les valeurs
- Différents formats d'affichage
- Examiner la mémoire
- Modifier les variables

### 15.2.5 GDB TUI Mode
- Interface semi-graphique dans le terminal
- Voir le code source en temps réel
- Affichage des registres

Chacune de ces sections développera les compétences fondamentales que vous avez acquises ici.

---

## Conclusion

GDB est un outil **essentiel** pour tout développeur C sérieux. C'est comme avoir une **machine à rayons X** pour votre programme : vous pouvez voir exactement ce qui se passe à l'intérieur, ligne par ligne, variable par variable.

### Points Clés à Retenir

1. **Compilez avec `-g -O0`** pour le débogage
2. **GDB contrôle l'exécution** de votre programme
3. **Les commandes de base** : `break`, `run`, `next`, `print`, `backtrace`, `quit`
4. **GDB ne remplace pas la réflexion** : c'est un outil pour vous aider à comprendre, pas un outil magique qui trouve les bugs automatiquement
5. **Pratiquez** : Comme toute compétence, la maîtrise de GDB vient avec la pratique

### La Philosophie du Débogage

Déboguer, c'est **être un détective scientifique** :
1. **Observer** : Que fait le programme ?
2. **Hypothèse** : Pourquoi fait-il cela ?
3. **Tester** : Utiliser GDB pour vérifier l'hypothèse
4. **Itérer** : Affiner l'hypothèse jusqu'à trouver la cause

GDB vous donne les outils pour observer et tester. Votre cerveau fournit les hypothèses.

### Message de Motivation

Au début, GDB peut sembler compliqué avec toutes ses commandes. C'est normal ! Mais investir du temps pour apprendre GDB est l'un des **meilleurs investissements** que vous puissiez faire en tant que développeur C.

Une fois que vous serez à l'aise avec GDB, vous vous demanderez comment vous avez pu coder sans lui. Les bugs qui prenaient des heures à trouver avec des `printf()` prendront quelques minutes avec GDB.

Alors, prêt à devenir un maître du débogage ? Allons-y ! 🚀

---

**💡 Astuce pour commencer :** Pour votre première vraie session GDB, prenez un programme simple que vous avez écrit, placez un breakpoint dans `main`, lancez-le, et explorez avec `next`, `print`, et `backtrace`. L'apprentissage par la pratique est la meilleure approche. Ne vous inquiétez pas si vous faites des erreurs - c'est en faisant qu'on apprend !

⏭️ [Breakpoints, watchpoints, catchpoints](/15-debogage-et-qualite/02.1-breakpoints.md)
