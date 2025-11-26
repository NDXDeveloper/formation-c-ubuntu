🔝 Retour au [Sommaire](/SOMMAIRE.md)

# D.1 Terminologie C

## Glossaire des termes essentiels du langage C

Ce glossaire rassemble les termes fondamentaux que vous rencontrerez lors de votre apprentissage du langage C. Chaque définition est accompagnée d'explications accessibles pour faciliter votre compréhension.

---

## A

### **ABI (Application Binary Interface)**
Interface qui définit comment les programmes compilés interagissent au niveau binaire (passage de paramètres, organisation de la mémoire, appels de fonctions). Contrairement à l'API qui concerne le code source, l'ABI concerne le code compilé.

### **Adresse mémoire**
Numéro unique identifiant un emplacement précis dans la mémoire de l'ordinateur. Chaque variable, chaque fonction possède une adresse mémoire. En C, on manipule ces adresses via les pointeurs.

### **Affectation (Assignment)**
Action d'attribuer une valeur à une variable. En C, l'opérateur d'affectation est `=`.
```c
int x = 42;  // Affectation lors de la déclaration
x = 100;     // Nouvelle affectation
```

### **Alignement mémoire (Memory Alignment)**
Contrainte selon laquelle certains types de données doivent être stockés à des adresses multiples de leur taille. Par exemple, un `int` de 4 octets sera souvent aligné sur une adresse multiple de 4. L'alignement améliore les performances d'accès mémoire.

### **Allocation dynamique**
Réservation de mémoire pendant l'exécution du programme (au runtime), contrairement à l'allocation statique qui se fait à la compilation. En C, on utilise `malloc()`, `calloc()`, ou `realloc()` pour allouer dynamiquement de la mémoire sur le **heap**.

### **ANSI C**
Première standardisation du langage C en 1989 par l'American National Standards Institute. Aussi appelé C89 ou C90. Établit les fondations du langage portable.

### **API (Application Programming Interface)**
Ensemble de fonctions et de définitions permettant à un programme d'utiliser les services d'une bibliothèque ou d'un système. En C, une API est généralement décrite dans des fichiers d'en-tête (`.h`).

### **Appel de fonction (Function Call)**
Action d'exécuter une fonction en lui passant éventuellement des arguments. L'appel transfère temporairement le contrôle à la fonction appelée.
```c
int resultat = ma_fonction(10, 20);
```

### **Argument**
Valeur concrète passée à une fonction lors de son appel. À distinguer du **paramètre** qui est la variable déclarée dans la définition de la fonction.

### **Arithmétique de pointeurs**
Opérations mathématiques effectuées sur des pointeurs (addition, soustraction). Permet de parcourir des tableaux ou des zones mémoires.
```c
int tab[5] = {1, 2, 3, 4, 5};
int *ptr = tab;
ptr++;  // Pointe maintenant sur tab[1]
```

### **Assertion**
Vérification de condition qui doit absolument être vraie à un point donné du programme. Utilisée pour détecter les bugs. En C, via la macro `assert()` de `<assert.h>`.

### **Atomique (Atomic)**
Opération qui s'exécute complètement ou pas du tout, sans interruption possible. Essentiel en programmation concurrente. Le C11 introduit `<stdatomic.h>` pour les opérations atomiques.

### **Auto (storage class)**
Classe de stockage par défaut pour les variables locales. Le mot-clé `auto` est rarement utilisé car implicite. À ne pas confondre avec le `auto` du C++ moderne.

---

## B

### **Bibliothèque (Library)**
Collection de fonctions précompilées réutilisables. En C, on distingue :
- **Bibliothèques statiques** (`.a` sous Linux) : intégrées dans l'exécutable
- **Bibliothèques dynamiques** (`.so` sous Linux, `.dll` sous Windows) : chargées au runtime

### **Bibliothèque standard C (C Standard Library)**
Ensemble de fonctions définies par le standard C et disponibles dans toute implémentation conforme. Inclut `stdio.h`, `stdlib.h`, `string.h`, etc.

### **Binaire**
1. Système de numération en base 2 (0 et 1)
2. Fichier exécutable compilé contenant du code machine

### **Bit**
Plus petite unité d'information en informatique. Peut valoir 0 ou 1.

### **Bit-field**
Structure permettant de définir des membres occupant un nombre précis de bits plutôt que des octets entiers. Utile pour économiser la mémoire.
```c
struct flags {
    unsigned int actif : 1;
    unsigned int mode : 3;
};
```

### **Bloc (Block)**
Portion de code délimitée par des accolades `{}`. Définit une portée locale pour les variables.

### **Boolean (Booléen)**
Type de données représentant vrai ou faux. Historiquement absent du C, ajouté en C99 via `<stdbool.h>` avec le type `bool` (valeurs `true` et `false`).

### **Breakpoint**
Point d'arrêt placé dans un débogueur (comme GDB) pour suspendre l'exécution d'un programme à un endroit précis et inspecter son état.

### **Buffer**
Zone de mémoire temporaire utilisée pour stocker des données avant leur traitement ou leur transfert. Les chaînes de caractères en C sont des buffers.

### **Buffer overflow**
Erreur grave où on écrit au-delà de la fin d'un buffer, corrompant la mémoire adjacente. Source majeure de vulnérabilités de sécurité.

### **Byte (Octet)**
Unité de mesure de 8 bits. Taille du type `char` en C.

---

## C

### **Callback**
Fonction passée en argument à une autre fonction, qui sera appelée ultérieurement. Implémentée en C via des pointeurs de fonctions.

### **Cast (Transtypage)**
Conversion explicite d'une valeur d'un type vers un autre type.
```c
float x = 3.14;
int y = (int)x;  // Cast explicite : y vaut 3
```

### **Chaîne de caractères (String)**
Séquence de caractères terminée par un caractère nul `'\0'`. En C, les chaînes sont des tableaux de `char`.
```c
char nom[] = "Alice";  // Équivaut à {'A','l','i','c','e','\0'}
```

### **Chaîne de compilation (Compilation Pipeline)**
Processus en plusieurs étapes transformant le code source en exécutable :
1. **Préprocesseur** : expansion des macros et directives
2. **Compilation** : transformation en code assembleur
3. **Assemblage** : génération du code objet
4. **Édition de liens (linking)** : création de l'exécutable final

### **Caractère (Character)**
Symbole élémentaire (lettre, chiffre, symbole). Type `char` en C (1 octet).

### **Commentaire**
Texte ignoré par le compilateur, destiné aux humains pour documenter le code.
```c
// Commentaire sur une ligne (C99+)
/* Commentaire
   sur plusieurs lignes */
```

### **Compilation conditionnelle**
Mécanisme du préprocesseur permettant d'inclure ou exclure du code selon des conditions, via `#ifdef`, `#ifndef`, `#if`, etc.

### **Compilateur**
Programme traduisant le code source C en code machine exécutable. Exemples : GCC, Clang, MSVC.

### **Concaténation**
Action de coller bout à bout des éléments. En C :
- Chaînes : "Hello " "World" devient "Hello World"
- Macros : opérateur `##` pour concaténer des tokens

### **Constante**
Valeur qui ne peut pas être modifiée après son initialisation. Déclarée avec le mot-clé `const` ou via des macros `#define`.

### **Contrainte de type (Type Constraint)**
Restriction sur les types de données acceptables. Le C est un langage à typage statique strict (mais avec conversions implicites).

### **Conversion de type (Type Conversion)**
Transformation d'une valeur d'un type vers un autre. Peut être :
- **Implicite** : faite automatiquement par le compilateur
- **Explicite** : via un cast

### **Core dump**
Fichier généré lors d'un crash contenant l'image mémoire du processus. Utilisé pour le débogage post-mortem avec GDB.

---

## D

### **Dangling pointer**
Pointeur qui référence une zone mémoire qui a été libérée ou qui n'est plus valide. Provoque des comportements indéfinis si déréférencé.

### **Déclaration (Declaration)**
Annonce l'existence d'une variable, fonction, ou type sans nécessairement définir son contenu/implémentation.
```c
int fonction(int x);  // Déclaration (prototype)
extern int variable;  // Déclaration d'une variable externe
```

### **Définition (Definition)**
Déclaration qui alloue effectivement la mémoire ou fournit l'implémentation.
```c
int fonction(int x) {  // Définition complète
    return x * 2;
}
int variable = 42;  // Définition avec initialisation
```

### **Débogueur (Debugger)**
Outil permettant d'exécuter un programme pas à pas, d'inspecter les variables, et d'analyser les erreurs. GDB est le débogueur standard sous Linux.

### **Déréférencement (Dereferencing)**
Action d'accéder à la valeur pointée par un pointeur, via l'opérateur `*`.
```c
int x = 42;
int *ptr = &x;
int valeur = *ptr;  // Déréférencement : valeur vaut 42
```

### **Descripteur de fichier (File Descriptor)**
Entier identifiant un fichier ouvert au niveau du système d'exploitation. 0 = stdin, 1 = stdout, 2 = stderr.

### **Directive de préprocesseur**
Instruction commençant par `#` et traitée avant la compilation. Exemples : `#include`, `#define`, `#ifdef`.

### **Double free**
Erreur consistant à appeler `free()` deux fois sur le même pointeur. Provoque une corruption du tas et généralement un crash.

---

## E

### **Édition de liens (Linking)**
Étape finale de la compilation qui combine les fichiers objets et bibliothèques pour créer l'exécutable final. Résout les références entre modules.

### **Endianness (Boutisme)**
Ordre dans lequel les octets d'un type multi-octets sont stockés en mémoire :
- **Big-endian** : octet de poids fort en premier
- **Little-endian** : octet de poids faible en premier
Important pour la portabilité et la manipulation binaire.

### **En-tête (Header file)**
Fichier `.h` contenant des déclarations (fonctions, types, macros) destinées à être incluses via `#include`. Permet de partager des interfaces entre fichiers.

### **Énumération (enum)**
Type de données définissant un ensemble de constantes nommées avec des valeurs entières.
```c
enum Couleur { ROUGE, VERT, BLEU };
enum Couleur ma_couleur = ROUGE;
```

### **errno**
Variable globale définie dans `<errno.h>` contenant le code d'erreur de la dernière fonction système qui a échoué.

### **Espace de noms (Namespace)**
Concept inexistant en C (présent en C++). En C, on simule des espaces de noms via des préfixes de nommage.

### **Exécutable**
Fichier binaire contenant du code machine directement exécutable par le processeur.

### **Expression**
Combinaison de valeurs, variables et opérateurs produisant une valeur.
```c
int x = 5 + 3 * 2;  // "5 + 3 * 2" est une expression
```

### **extern**
Mot-clé indiquant qu'une variable ou fonction est définie dans un autre fichier. Utilisé pour partager des variables globales entre modules.

---

## F

### **Fichier objet (Object file)**
Fichier intermédiaire (`.o` sous Linux, `.obj` sous Windows) contenant du code machine mais pas encore lié. Produit de la compilation d'un fichier source.

### **Fonction**
Bloc de code réutilisable effectuant une tâche spécifique. Peut prendre des paramètres et retourner une valeur.

### **Fonction inline**
Fonction dont le code est inséré directement à l'endroit de l'appel plutôt que via un vrai appel de fonction. Mot-clé `inline` (C99).

### **Fonction variadique**
Fonction acceptant un nombre variable d'arguments. Exemple : `printf()`. Utilise `<stdarg.h>`.

### **Forward declaration**
Déclaration anticipée d'un type ou fonction avant sa définition complète. Résout les dépendances circulaires.

### **Fuite mémoire (Memory leak)**
Erreur où de la mémoire allouée dynamiquement n'est jamais libérée, causant une consommation croissante de RAM.

---

## G

### **Garde d'inclusion (Include guard)**
Mécanisme empêchant l'inclusion multiple d'un même fichier d'en-tête via des directives préprocesseur.
```c
#ifndef MON_HEADER_H
#define MON_HEADER_H
// Contenu du header
#endif
```
Alternative moderne : `#pragma once`

### **GCC (GNU Compiler Collection)**
Suite de compilateurs open-source incluant un compilateur C. Standard sous Linux.

### **GDB (GNU Debugger)**
Débogueur en ligne de commande standard sous Linux pour les programmes C/C++.

### **Gestion mémoire (Memory Management)**
Ensemble des techniques pour allouer, utiliser et libérer la mémoire de manière efficace et sûre.

### **Globale (Variable globale)**
Variable déclarée en dehors de toute fonction, accessible depuis n'importe où dans le fichier (ou le programme si `extern`).

---

## H

### **Heap (Tas)**
Zone de mémoire utilisée pour l'allocation dynamique (`malloc`, `calloc`, `realloc`). Gérée manuellement par le programmeur. Durée de vie : jusqu'à appel explicite à `free()`.

### **Header file**
Voir **En-tête**.

### **Hexadécimal**
Système de numération en base 16 (chiffres 0-9 et lettres A-F). Notation en C : `0x` suivi des chiffres (ex: `0xFF` = 255).

---

## I

### **Identifiant (Identifier)**
Nom donné à une variable, fonction, type, etc. Règles en C :
- Commence par une lettre ou `_`
- Contient lettres, chiffres, `_`
- Sensible à la casse
- Ne peut pas être un mot-clé réservé

### **Implémentation**
Réalisation concrète d'une interface ou d'une fonction (son code effectif).

### **Inclusion (Include)**
Action d'insérer le contenu d'un fichier dans un autre via `#include`.
```c
#include <stdio.h>   // Bibliothèque standard
#include "mon_header.h"  // Fichier local
```

### **Initialisation**
Affectation d'une valeur initiale à une variable lors de sa déclaration.
```c
int x = 10;  // Initialisée
int y;       // Non initialisée (valeur indéterminée)
```

### **Inline assembly**
Insertion de code assembleur directement dans du code C. Syntaxe dépendante du compilateur (GCC utilise `asm` ou `__asm__`).

### **Instruction**
Commande élémentaire du programme terminée par un point-virgule `;`.

### **Interopérabilité**
Capacité de faire communiquer du code C avec d'autres langages (Python, Go, Rust, etc.).

### **IPC (Inter-Process Communication)**
Mécanismes permettant à des processus de communiquer : pipes, sockets, mémoire partagée, files de messages, etc.

---

## J

### **Joinable (Thread)**
Thread qui doit être explicitement attendu avec `pthread_join()` pour récupérer sa valeur de retour et libérer ses ressources.

### **JIT (Just-In-Time compilation)**
Compilation à la volée pendant l'exécution. Peu utilisé en C natif, mais pertinent pour eBPF.

---

## K

### **Kernel (Noyau)**
Cœur du système d'exploitation gérant les ressources matérielles. Les programmes C utilisent ses services via des appels système.

### **Keyword (Mot-clé)**
Mot réservé du langage C ayant une signification spéciale. Exemples : `int`, `if`, `while`, `return`, `struct`, etc.

---

## L

### **Langage compilé**
Langage dont le code source est transformé en code machine avant exécution (contrairement aux langages interprétés). Le C est un langage compilé.

### **Langage de bas niveau**
Langage proche du matériel, offrant un contrôle fin sur la mémoire et le processeur. Le C est considéré comme un langage de "moyen-bas niveau".

### **Libc**
Abréviation de "C library", la bibliothèque standard C implémentant les fonctions définies par le standard. Exemples : glibc (GNU), musl.

### **Lien dynamique (Dynamic linking)**
Mécanisme où les bibliothèques sont chargées au moment de l'exécution plutôt qu'intégrées dans l'exécutable.

### **Lien statique (Static linking)**
Mécanisme où le code des bibliothèques est intégré dans l'exécutable final au moment de la compilation.

### **Littéral (Literal)**
Valeur écrite directement dans le code source.
```c
42          // Littéral entier
3.14f       // Littéral flottant
"Hello"     // Littéral chaîne
'A'         // Littéral caractère
```

### **Locale (Variable locale)**
Variable déclarée à l'intérieur d'une fonction ou d'un bloc. Accessible uniquement dans sa portée. Stockée sur la **stack**.

### **Lock (Verrou)**
Mécanisme de synchronisation empêchant plusieurs threads d'accéder simultanément à une ressource partagée. Implémenté via des mutex en C.

### **Longueur de chaîne (String length)**
Nombre de caractères avant le `'\0'` terminal. Obtenue avec `strlen()`.

### **Loop (Boucle)**
Structure de contrôle répétant un bloc de code : `while`, `for`, `do-while`.

### **Lvalue**
Expression qui peut apparaître à gauche d'une affectation (désigne un emplacement mémoire modifiable).
```c
int x = 10;
x = 20;     // 'x' est une lvalue
```

---

## M

### **Macro**
Substitution textuelle définie via `#define`. Traitée par le préprocesseur avant compilation.
```c
#define PI 3.14159
#define MAX(a,b) ((a) > (b) ? (a) : (b))
```

### **Makefile**
Fichier de configuration pour l'outil `make`, définissant les règles de compilation d'un projet.

### **malloc()**
Fonction allouant dynamiquement de la mémoire sur le heap. Retourne un pointeur `void*` vers la zone allouée ou `NULL` en cas d'échec.

### **Memory leak**
Voir **Fuite mémoire**.

### **Modularité**
Organisation du code en modules indépendants et réutilisables (fichiers séparés avec interfaces claires).

### **Mot-clé**
Voir **Keyword**.

### **Mutex (Mutual Exclusion)**
Mécanisme de synchronisation permettant à un seul thread à la fois d'accéder à une section critique du code.

---

## N

### **Named pipe (FIFO)**
Mécanisme IPC permettant la communication entre processus via un fichier spécial dans le système de fichiers.

### **NULL**
Valeur spéciale pour les pointeurs indiquant qu'ils ne pointent vers rien. Défini comme `((void*)0)` en C.

### **Norme C**
Spécification officielle du langage C. Versions majeures : C89/C90 (ANSI C), C99, C11, C17, C23.

---

## O

### **Obfuscation**
Rendre le code intentionnellement difficile à lire. Généralement déconseillé sauf pour la protection de propriété intellectuelle.

### **Objet (Object file)**
Voir **Fichier objet**.

### **Octal**
Système de numération en base 8. Notation en C : préfixe `0` (ex: `0755` = 493 en décimal).

### **Offset**
Décalage par rapport à une position de référence (adresse, indice de tableau, position dans un fichier).

### **Opérande**
Valeur ou variable sur laquelle opère un opérateur.
```c
int resultat = a + b;  // 'a' et 'b' sont des opérandes
```

### **Opérateur**
Symbole effectuant une opération sur un ou plusieurs opérandes : `+`, `-`, `*`, `/`, `%`, `&&`, `||`, `&`, `|`, etc.

### **Opérateur bit-à-bit**
Opérateurs manipulant les bits individuels : `&` (AND), `|` (OR), `^` (XOR), `~` (NOT), `<<` (décalage gauche), `>>` (décalage droite).

### **Opérateur ternaire**
Opérateur conditionnel compact : `condition ? valeur_si_vrai : valeur_si_faux`.

### **Optimisation**
Transformations appliquées par le compilateur pour améliorer les performances (vitesse, taille) du code généré. Niveaux GCC : `-O0`, `-O1`, `-O2`, `-O3`, `-Os`.

### **Overflow (Débordement)**
Dépassement de la capacité d'un type de données (integer overflow) ou d'un buffer (buffer overflow).

---

## P

### **Padding (Bourrage)**
Octets inutilisés insérés par le compilateur pour respecter l'alignement mémoire dans les structures.

### **Paramètre**
Variable déclarée dans la signature d'une fonction pour recevoir les valeurs passées lors de l'appel. À distinguer de l'**argument** (valeur concrète passée).

### **Passage par valeur (Pass by value)**
Mécanisme où la fonction reçoit une copie de la valeur de l'argument. La modification du paramètre n'affecte pas la variable originale.

### **Passage par référence**
En C, simulé en passant un pointeur, permettant à la fonction de modifier la variable originale.
```c
void increment(int *x) {
    (*x)++;
}
```

### **Pile (Stack)**
1. Structure de données LIFO (Last In, First Out)
2. Zone mémoire pour variables locales et appels de fonctions. Gestion automatique.

### **Pipe**
Mécanisme IPC permettant la communication unidirectionnelle entre processus via un canal de données.

### **Pointeur (Pointer)**
Variable contenant l'adresse mémoire d'une autre variable. Type déclaré avec `*`.
```c
int x = 42;
int *ptr = &x;  // ptr pointe vers x
```

### **Pointeur de fonction**
Pointeur vers l'adresse d'une fonction. Permet les callbacks et les tables de dispatch.
```c
int (*fptr)(int, int) = &ma_fonction;
int resultat = fptr(10, 20);
```

### **Pointeur opaque (Opaque pointer)**
Pointeur vers une structure dont la définition complète n'est pas exposée dans l'en-tête public. Encapsulation en C.

### **Pointeur void**
Pointeur générique (`void*`) pouvant pointer vers n'importe quel type. Nécessite un cast pour être déréférencé.

### **Portabilité**
Capacité d'un code à fonctionner sur différentes plateformes avec peu ou pas de modifications.

### **Portée (Scope)**
Zone du code où une variable ou fonction est accessible. Portées en C : bloc, fonction, fichier, globale.

### **POSIX**
Standard définissant l'API des systèmes Unix-like. Les threads POSIX (pthread) sont une implémentation standardisée du multithreading.

### **Pragma**
Directive préprocesseur permettant de donner des instructions spécifiques au compilateur.
```c
#pragma once  // Garde d'inclusion moderne
#pragma pack(1)  // Contrôle du padding
```

### **Préprocesseur**
Première phase de la compilation, traitant les directives `#` (includes, macros, compilation conditionnelle).

### **Priorité des opérateurs**
Ordre dans lequel les opérateurs sont évalués dans une expression. Exemple : `*` et `/` avant `+` et `-`.

### **Processus**
Instance d'un programme en cours d'exécution. Possède son propre espace d'adressage mémoire.

### **Profiling**
Analyse des performances d'un programme pour identifier les goulots d'étranglement (fonctions lentes, allocations excessives).

### **Prototype**
Déclaration d'une fonction spécifiant son type de retour, son nom et ses paramètres, sans fournir l'implémentation.
```c
int addition(int a, int b);  // Prototype
```

---

## Q

### **Qualificateur de type**
Mot-clé modifiant les propriétés d'un type : `const` (non modifiable), `volatile` (peut changer de manière imprévisible), `restrict` (optimisation pointeurs).

---

## R

### **Race condition**
Situation où le résultat d'un programme dépend de l'ordonnancement non déterministe de threads concurrents. Source de bugs critiques.

### **Récursivité**
Fonction qui s'appelle elle-même. Attention au risque de stack overflow si pas de condition d'arrêt.

### **Référence**
En C++, alias vers une variable. N'existe pas en C (on utilise des pointeurs).

### **Register**
Mot-clé suggérant au compilateur de stocker une variable dans un registre CPU pour un accès rapide. Obsolète et ignoré par les compilateurs modernes.

### **Ressource**
Élément limité que le programme utilise : mémoire, descripteur de fichier, socket, mutex, etc. Doit être correctement libéré.

### **Retour (Return)**
Instruction `return` terminant l'exécution d'une fonction et renvoyant optionnellement une valeur.

### **RTTI (Run-Time Type Information)**
Information de type disponible pendant l'exécution. Absent en C (présent en C++).

### **Runtime**
Période pendant laquelle le programme s'exécute, par opposition au temps de compilation.

### **Rvalue**
Expression ne pouvant pas apparaître à gauche d'une affectation (valeur temporaire, littéral).
```c
int x = 42;  // 42 est une rvalue
x = 10 + 5;  // 10 + 5 est une rvalue
```

---

## S

### **Sanitizer**
Outil d'analyse dynamique détectant les erreurs à l'exécution : AddressSanitizer (mémoire), UndefinedBehaviorSanitizer, ThreadSanitizer, etc.

### **Section critique (Critical section)**
Portion de code accédant à des ressources partagées et devant être protégée contre l'accès concurrent.

### **Segmentation fault (Segfault)**
Erreur survenant quand un programme tente d'accéder à une zone mémoire interdite. Cause fréquente : déréférencement de pointeur invalide.

### **Sémantique**
Signification et comportement d'un élément du langage, par opposition à la syntaxe (structure).

### **Sémaphore**
Mécanisme de synchronisation contrôlant l'accès à une ressource partagée via un compteur.

### **Sentinelle (Sentinel)**
Valeur spéciale marquant la fin d'une séquence. Exemple : `'\0'` pour les chaînes C.

### **Séquence d'échappement (Escape sequence)**
Caractères spéciaux représentés par `\` suivi d'un code : `\n` (newline), `\t` (tab), `\\` (backslash), `\"` (guillemet).

### **Shared memory**
Mémoire accessible par plusieurs processus simultanément. Mécanisme IPC performant mais nécessitant synchronisation.

### **Side effect**
Effet de bord : modification d'un état en dehors de la simple évaluation d'une expression (modification variable, I/O, etc.).

### **Signal**
Notification asynchrone envoyée à un processus pour indiquer un événement (SIGINT, SIGTERM, SIGSEGV, etc.).

### **Signed (Signé)**
Type entier pouvant représenter des valeurs négatives et positives. Par défaut pour `int`, `char` (dépend du compilateur).

### **sizeof**
Opérateur retournant la taille en octets d'un type ou d'une variable. Évalué à la compilation.
```c
size_t taille = sizeof(int);  // Généralement 4
```

### **Socket**
Point de terminaison pour la communication réseau. Interface de programmation pour TCP/UDP.

### **Source (code source)**
Texte du programme écrit dans un langage de programmation (fichiers `.c` en C).

### **Stack (Pile)**
Voir **Pile**.

### **Stack frame**
Zone de la pile dédiée à un appel de fonction, contenant paramètres, variables locales, adresse de retour.

### **Stack overflow**
Débordement de la pile, généralement causé par une récursion trop profonde ou des variables locales trop volumineuses.

### **Standard C**
Voir **Norme C**.

### **static (mot-clé)**
Modificateur avec deux usages :
1. **Variable statique locale** : conserve sa valeur entre les appels de fonction
2. **Variable/fonction statique globale** : visible uniquement dans le fichier où elle est définie (linkage interne)

### **stdint.h**
Header C99 définissant des types entiers de taille garantie : `int8_t`, `uint16_t`, `int32_t`, `uint64_t`, etc.

### **String**
Voir **Chaîne de caractères**.

### **struct (Structure)**
Type de données composite regroupant plusieurs variables (potentiellement de types différents) sous un même nom.
```c
struct Point {
    int x;
    int y;
};
```

### **Symbol table (Table des symboles)**
Table générée lors de la compilation contenant les noms et adresses des fonctions et variables. Utilisée pour le débogage et le linking.

### **Syntaxe**
Règles formelles définissant comment écrire correctement du code dans un langage.

### **Système de types (Type system)**
Ensemble de règles définissant les types disponibles et leurs interactions. Le C a un typage statique (vérifié à la compilation).

---

## T

### **Table de hachage (Hash table)**
Structure de données associant des clés à des valeurs via une fonction de hachage. Recherche en temps O(1) moyen.

### **Tas (Heap)**
Voir **Heap**.

### **Thread**
Fil d'exécution léger partageant l'espace mémoire d'un processus. Permet l'exécution concurrente.

### **Thread-safe**
Code pouvant être exécuté simultanément par plusieurs threads sans provoquer de race conditions.

### **Token**
Unité lexicale élémentaire du langage (mot-clé, identifiant, opérateur, littéral).

### **Toolchain**
Ensemble d'outils nécessaires au développement : compilateur, éditeur de liens, débogueur, etc.

### **Transtypage**
Voir **Cast**.

### **Type**
Catégorie de données définissant les valeurs possibles et les opérations applicables. Types de base en C : `char`, `int`, `float`, `double`, `void`.

### **Type dérivé**
Type construit à partir de types de base : tableaux, pointeurs, structures, unions, fonctions.

### **typedef**
Mot-clé créant un alias pour un type existant, améliorant la lisibilité.
```c
typedef unsigned long ulong;
typedef struct Point Point_t;
```

### **Type opaque**
Type dont l'implémentation est cachée (seul le pointeur est exposé). Encapsulation en C.

---

## U

### **UB (Undefined Behavior)**
Comportement indéfini : le standard C ne spécifie pas ce qui se passe. Peut conduire à des résultats imprévisibles, des crashes, ou sembler fonctionner. Exemples : déréférencement de pointeur NULL, dépassement de tableau, division par zéro.

### **union**
Type de données où tous les membres partagent le même emplacement mémoire. Seul un membre peut être actif à la fois. Économise de la mémoire.
```c
union Data {
    int i;
    float f;
    char str[20];
};
```

### **Unsigned (Non signé)**
Type entier ne représentant que des valeurs positives ou nulles. Plage deux fois plus grande que le type signé équivalent.

---

## V

### **Valeur de retour (Return value)**
Valeur renvoyée par une fonction via l'instruction `return`.

### **Valgrind**
Suite d'outils d'analyse dynamique, notamment Memcheck pour détecter les fuites mémoire et erreurs d'accès.

### **Variable**
Emplacement nommé en mémoire stockant une valeur qui peut changer pendant l'exécution.

### **Variable d'environnement**
Variable définie par le système ou l'utilisateur, accessible au programme via `getenv()`. Exemple : `PATH`, `HOME`.

### **Variadique**
Voir **Fonction variadique**.

### **Vectorisation**
Optimisation utilisant les instructions SIMD du processeur pour traiter plusieurs données simultanément.

### **Visibility (Visibilité des symboles)**
Contrôle quels symboles (fonctions, variables) d'une bibliothèque sont exposés publiquement.

### **void**
Type spécial indiquant :
1. Absence de valeur de retour : `void ma_fonction()`
2. Pointeur générique : `void*`
3. Liste de paramètres vide : `int fonction(void)`

### **volatile**
Qualificateur indiquant qu'une variable peut être modifiée de manière imprévisible (par du hardware, un signal, un autre thread). Empêche certaines optimisations.

---

## W

### **Warning (Avertissement)**
Message du compilateur signalant un problème potentiel sans empêcher la compilation. Il est fortement recommandé de les traiter (`-Wall -Werror`).

### **Watchpoint**
Point d'arrêt dans GDB qui suspend l'exécution quand une variable spécifique est modifiée.

### **Whitespace (Espace blanc)**
Caractères invisibles : espace, tabulation, newline. Généralement ignorés par le compilateur C (sauf dans les chaînes).

---

## X

### **X-Macro**
Technique de métaprogrammation utilisant le préprocesseur pour générer du code répétitif de manière DRY (Don't Repeat Yourself).

---

## Z

### **Zero-initialization**
Initialisation d'une variable à zéro. En C, les variables globales et statiques sont automatiquement initialisées à zéro. Les variables locales ne le sont pas (valeur indéterminée).

### **Zombie (processus)**
Processus terminé dont le parent n'a pas encore récupéré le code de sortie via `wait()`. Occupe toujours une entrée dans la table des processus.

---

## 📚 Conventions typographiques utilisées

- **Gras** : Terme principal défini
- `Code inline` : Code, commandes, noms de fichiers
- *Italique* : Emphase, termes étrangers

---

## 💡 Conseils d'utilisation de ce glossaire

Ce glossaire est conçu pour être une référence rapide pendant votre apprentissage du C. Voici comment l'utiliser efficacement :

1. **Lors de la lecture de tutoriels** : Consultez-le dès qu'un terme inconnu apparaît
2. **Avant un entretien technique** : Révisez les termes essentiels
3. **Pendant le débogage** : Vérifiez la signification précise d'un message d'erreur
4. **Pour approfondir** : Les termes en gras dans les définitions renvoient à d'autres entrées

**Note** : Ce glossaire couvre les termes standard du C. Pour la terminologie spécifique à Linux, consultez **D.2 Terminologie système Linux**.

---

*Ce glossaire est maintenu à jour avec les standards C modernes (C11, C17, C23) et les pratiques DevOps 2025.*

⏭️ [Terminologie système Linux](/annexes/glossaire/02-terminologie-linux.md)
