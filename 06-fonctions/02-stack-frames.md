🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 6.2 La Pile : Comprendre les Stack Frames

## Introduction

Lorsque vous appelez une fonction en C, quelque chose de fascinant se produit en coulisses : le processeur et le système d'exploitation collaborent pour gérer l'exécution de votre code de manière ordonnée. Au cœur de ce mécanisme se trouve la **pile d'exécution** (ou **stack** en anglais), une zone mémoire organisée de façon très particulière.

Comprendre la pile est essentiel pour :
- ✅ Savoir où vivent vos variables locales
- ✅ Comprendre comment les fonctions s'appellent et se terminent
- ✅ Déboguer les erreurs de segmentation et les dépassements de pile
- ✅ Optimiser votre code en connaissant les limites
- ✅ Préparer l'apprentissage des pointeurs et de l'allocation dynamique

---

## Qu'est-ce que la pile (Stack) ?

### Analogie : La pile d'assiettes

Imaginez une pile d'assiettes dans votre cuisine :
- Vous **ajoutez** une assiette sur le **dessus** de la pile
- Vous **retirez** une assiette depuis le **dessus** de la pile
- Vous ne pouvez pas retirer une assiette du milieu sans tout faire tomber

C'est le principe **LIFO** : **L**ast **I**n, **F**irst **O**ut (dernier entré, premier sorti).

La pile mémoire fonctionne exactement de la même manière !

```
    ┌─────────────┐
    │  Assiette 3 │  ← Dernière ajoutée (TOP)
    ├─────────────┤
    │  Assiette 2 │
    ├─────────────┤
    │  Assiette 1 │  ← Première ajoutée
    └─────────────┘
```

---

### La pile en mémoire

Votre programme dispose de plusieurs zones mémoire. La **pile** est l'une d'entre elles :

```
Adresses hautes (ex: 0xFFFFFFFF)
    ┌────────────────────┐
    │       Stack        │  ← La pile (croît vers le bas)
    │         ↓          │
    ├────────────────────┤
    │                    │
    │    (espace libre)  │
    │                    │
    ├────────────────────┤
    │         ↑          │
    │       Heap         │  ← Le tas (allocation dynamique)
    ├────────────────────┤
    │       BSS          │  ← Variables non initialisées
    ├────────────────────┤
    │       Data         │  ← Variables globales/statiques
    ├────────────────────┤
    │       Text         │  ← Code exécutable
    └────────────────────┘
Adresses basses (ex: 0x00000000)
```

**Caractéristique importante** : Sur la plupart des systèmes, la pile **croît vers les adresses basses**. Quand on "empile" des données, on descend en mémoire !

---

## Qu'est-ce qu'un Stack Frame ?

Un **stack frame** (ou **cadre de pile**) est une **zone de la pile dédiée à une fonction**. Chaque fois qu'une fonction est appelée, un nouveau stack frame est créé et empilé. Quand la fonction se termine, son stack frame est dépilé et détruit.

### Contenu d'un Stack Frame

Un stack frame typique contient :

1. **Paramètres de la fonction** : Les valeurs passées à la fonction
2. **Adresse de retour** : Où reprendre l'exécution après la fonction
3. **Variables locales** : Toutes les variables déclarées dans la fonction
4. **Registres sauvegardés** : État du processeur avant l'appel
5. **Pointeur de frame précédent** : Permet de revenir au frame appelant

```
     Stack Frame de la fonction
    ┌──────────────────────────┐
    │   Variables locales      │
    ├──────────────────────────┤
    │   Registres sauvegardés  │
    ├──────────────────────────┤
    │   Adresse de retour      │
    ├──────────────────────────┤
    │   Frame pointer ancien   │
    ├──────────────────────────┤
    │   Paramètres             │
    └──────────────────────────┘
```

---

## Visualisation : Appel de fonction simple

Prenons un exemple concret :

```c
#include <stdio.h>

int additionner(int a, int b) {
    int resultat = a + b;
    return resultat;
}

int main(void) {
    int x = 5;
    int y = 3;
    int somme = additionner(x, y);
    printf("Somme = %d\n", somme);
    return 0;
}
```

### Étape 1 : Début du programme

Le programme démarre. Le stack frame de `main` est créé :

```
    Stack (adresses décroissantes ↓)
    ┌─────────────────────────────┐
    │  main frame                 │
    │  ┌────────────────────────┐ │
    │  │ int somme = ?          │ │
    │  │ int y = 3              │ │
    │  │ int x = 5              │ │
    │  │ adresse retour (OS)    │ │
    │  └────────────────────────┘ │
    └─────────────────────────────┘
         ↑ Stack Pointer (SP)
```

---

### Étape 2 : Appel de additionner(x, y)

Quand `additionner(5, 3)` est appelé, plusieurs choses se passent :

1. Les paramètres `x` et `y` sont **copiés** (passage par valeur)
2. L'**adresse de retour** (instruction suivante dans `main`) est sauvegardée
3. Un **nouveau stack frame** est créé pour `additionner`
4. Les **variables locales** de `additionner` sont allouées

```
    Stack
    ┌─────────────────────────────┐
    │  additionner frame          │  ← Frame actif (TOP)
    │  ┌────────────────────────┐ │
    │  │ int resultat = 8       │ │  Variable locale
    │  │ adresse retour → main  │ │  Retour vers main
    │  │ int b = 3 (copie)      │ │  Paramètre
    │  │ int a = 5 (copie)      │ │  Paramètre
    │  └────────────────────────┘ │
    ├─────────────────────────────┤
    │  main frame                 │
    │  ┌────────────────────────┐ │
    │  │ int somme = ?          │ │
    │  │ int y = 3              │ │
    │  │ int x = 5              │ │
    │  │ adresse retour (OS)    │ │
    │  └────────────────────────┘ │
    └─────────────────────────────┘
         ↑ Stack Pointer
```

**Point important** : `a` et `b` dans `additionner` sont des **copies** de `x` et `y`. Modifier `a` ou `b` n'affecte pas `x` et `y` dans `main`.

---

### Étape 3 : Retour de la fonction

Quand `additionner` exécute `return resultat;` :

1. La valeur `8` est placée dans un **registre** du processeur (généralement `EAX` ou `RAX`)
2. Le stack frame de `additionner` est **détruit** (dépilé)
3. L'exécution reprend dans `main` à l'adresse de retour
4. La valeur retournée est assignée à `somme`

```
    Stack
    ┌─────────────────────────────┐
    │  main frame                 │  ← Frame actif à nouveau
    │  ┌────────────────────────┐ │
    │  │ int somme = 8          │ │  ← Valeur retournée
    │  │ int y = 3              │ │
    │  │ int x = 5              │ │
    │  │ adresse retour (OS)    │ │
    │  └────────────────────────┘ │
    └─────────────────────────────┘
         ↑ Stack Pointer

    Le frame de additionner a disparu !
```

**Conséquence importante** : Les variables locales de `additionner` (`resultat`, `a`, `b`) n'existent plus ! Leur mémoire est libérée automatiquement.

---

## Exemple avec appels imbriqués

Les fonctions peuvent s'appeler entre elles, créant une **chaîne de stack frames**.

```c
#include <stdio.h>

int multiplier(int a, int b) {
    int produit = a * b;
    return produit;
}

int calculer(int x, int y) {
    int somme = x + y;
    int triple = multiplier(somme, 3);
    return triple;
}

int main(void) {
    int resultat = calculer(4, 6);
    printf("Résultat = %d\n", resultat);
    return 0;
}
```

### Évolution de la pile

**Étape 1** : `main` appelle `calculer(4, 6)`

```
    ┌────────────────────────┐
    │  calculer frame        │  ← TOP
    │  somme = 10            │
    │  triple = ?            │
    │  y = 6, x = 4          │
    ├────────────────────────┤
    │  main frame            │
    │  resultat = ?          │
    └────────────────────────┘
```

---

**Étape 2** : `calculer` appelle `multiplier(10, 3)`

```
    ┌────────────────────────┐
    │  multiplier frame      │  ← TOP (plus profond)
    │  produit = 30          │
    │  b = 3, a = 10         │
    ├────────────────────────┤
    │  calculer frame        │
    │  somme = 10            │
    │  triple = ?            │
    │  y = 6, x = 4          │
    ├────────────────────────┤
    │  main frame            │
    │  resultat = ?          │
    └────────────────────────┘
```

**Profondeur d'appel** : Il y a maintenant **3 frames** empilés (main → calculer → multiplier).

---

**Étape 3** : `multiplier` retourne 30

```
    ┌────────────────────────┐
    │  calculer frame        │  ← TOP
    │  somme = 10            │
    │  triple = 30           │  ← Valeur récupérée
    │  y = 6, x = 4          │
    ├────────────────────────┤
    │  main frame            │
    │  resultat = ?          │
    └────────────────────────┘

    Frame de multiplier détruit
```

---

**Étape 4** : `calculer` retourne 30

```
    ┌────────────────────────┐
    │  main frame            │  ← TOP
    │  resultat = 30         │  ← Valeur récupérée
    └────────────────────────┘

    Frame de calculer détruit
```

---

**Étape 5** : `main` se termine

```
    Stack vide (ou presque)

    Le programme se termine et rend la main au système d'exploitation
```

---

## Durée de vie des variables locales

C'est **la pile qui détermine la durée de vie** des variables locales :

```c
void fonction_exemple(void) {
    int x = 42;        // x est créé sur la pile
    printf("%d\n", x); // x existe et vaut 42
}                      // x est DÉTRUIT ici (frame dépilé)

int main(void) {
    fonction_exemple();
    // x n'existe plus !
}
```

### ⚠️ Erreur classique : Retourner un pointeur vers une variable locale

```c
int* fonction_dangereuse(void) {
    int x = 100;
    return &x;  // ❌ DANGER ! On retourne l'adresse d'une variable locale
}

int main(void) {
    int* ptr = fonction_dangereuse();
    // ptr pointe vers une zone mémoire qui n'est plus valide !
    printf("%d\n", *ptr);  // ❌ Comportement indéfini (crash probable)
    return 0;
}
```

**Pourquoi c'est dangereux ?**

1. `x` vit dans le stack frame de `fonction_dangereuse`
2. Quand la fonction se termine, le frame est détruit
3. `ptr` pointe vers une zone mémoire qui n'appartient plus à `x`
4. Cette zone peut être réutilisée par d'autres fonctions
5. Déréférencer `ptr` cause un **comportement indéfini**

---

## Visualisation avec GDB

Vous pouvez observer la pile en action avec le débogueur GDB. Compilons et exécutons :

```bash
gcc -g -o programme programme.c
gdb programme
```

Commandes GDB utiles :

```gdb
(gdb) break main           # Point d'arrêt sur main
(gdb) run                  # Lancer le programme
(gdb) backtrace            # Afficher la pile d'appels (stack frames)
(gdb) info frame           # Détails du frame actuel
(gdb) info locals          # Variables locales du frame actuel
(gdb) up / down            # Navigator entre les frames
```

**Exemple de sortie `backtrace` :**

```
#0  multiplier (a=10, b=3) at programme.c:4
#1  0x0000555555555180 in calculer (x=4, y=6) at programme.c:10
#2  0x00005555555551a8 in main () at programme.c:15
```

Cela montre **3 frames** : `multiplier` (actuel) ← `calculer` ← `main`.

---

## Taille de la pile

### Limites de la pile

La pile n'est **pas infinie**. Sur Linux, la taille par défaut est généralement :
- **8 MB** (8 mégaoctets) sur les systèmes 64 bits
- Peut être configurée avec `ulimit -s`

Vérifier la taille actuelle :

```bash
ulimit -s
# Sortie : 8192 (en kilooctets, soit 8 MB)
```

---

### Stack Overflow (Dépassement de pile)

Si vous empilez trop de frames ou allouez trop de variables locales, vous provoquez un **stack overflow** :

**Exemple 1 : Récursion infinie**

```c
void boucle_infinie(void) {
    boucle_infinie();  // ❌ Appel récursif sans condition d'arrêt
}

int main(void) {
    boucle_infinie();  // Crash : Segmentation fault (stack overflow)
    return 0;
}
```

Chaque appel crée un nouveau frame, jusqu'à épuisement de la pile.

---

**Exemple 2 : Tableau local trop grand**

```c
int main(void) {
    int tableau[10000000];  // ❌ 40 MB sur la pile (dépasse 8 MB)
    // Crash probable : Segmentation fault
    return 0;
}
```

**Solution** : Utiliser l'allocation dynamique (heap) pour les grandes structures (voir Module 3).

---

## Stack vs Heap : Comparaison rapide

Nous approfondirons le **heap** dans le Module 3, mais voici une comparaison préliminaire :

| Aspect | Stack | Heap |
|--------|-------|------|
| **Gestion** | Automatique | Manuelle (`malloc`/`free`) |
| **Vitesse** | Très rapide | Plus lent |
| **Taille** | Limitée (8 MB typique) | Limitée par la RAM |
| **Durée de vie** | Scope de la fonction | Jusqu'à `free()` |
| **Organisation** | LIFO (structurée) | Non structurée |
| **Fragmentation** | Aucune | Possible |
| **Erreurs** | Stack overflow | Memory leaks, double free |

**Règle simple** :
- Variables locales, petits tableaux → **Stack**
- Grandes structures, données persistantes → **Heap** (avec `malloc`)

---

## Optimisations du compilateur

### Inline Functions

Le compilateur peut **éliminer** certains stack frames en "inlinant" (inline) les fonctions :

```c
inline int carre(int x) {
    return x * x;
}

int main(void) {
    int resultat = carre(5);  // Peut être remplacé par : int resultat = 5 * 5;
    return 0;
}
```

Si le compilateur décide d'inline `carre`, aucun stack frame n'est créé : le code de `carre` est directement inséré dans `main`.

---

### Optimisation de la récursion terminale

Une **récursion terminale** (tail recursion) peut être optimisée en boucle :

```c
// Version récursive (crée N frames)
int factorielle_recursive(int n, int acc) {
    if (n <= 1) return acc;
    return factorielle_recursive(n - 1, n * acc);
}

// Avec -O2, GCC peut optimiser en :
int factorielle_optimisee(int n, int acc) {
    while (n > 1) {
        acc = n * acc;
        n = n - 1;
    }
    return acc;
}
```

Avec l'optimisation, **un seul frame** est utilisé au lieu de N.

---

## Conventions d'appel (Calling Conventions)

Les **conventions d'appel** définissent comment les paramètres sont passés et où la valeur de retour est placée. Sur x86-64 Linux, la convention **System V AMD64 ABI** est utilisée :

### Passage des paramètres

- **6 premiers paramètres entiers** : Registres `RDI`, `RSI`, `RDX`, `RCX`, `R8`, `R9`
- **Paramètres suivants** : Sur la pile
- **Paramètres flottants** : Registres `XMM0` à `XMM7`

```c
void fonction(int a, int b, int c, int d, int e, int f, int g, int h) {
    // a → RDI
    // b → RSI
    // c → RDX
    // d → RCX
    // e → R8
    // f → R9
    // g → pile
    // h → pile
}
```

### Valeur de retour

- **Entiers/pointeurs** : Registre `RAX`
- **Flottants** : Registre `XMM0`
- **Grandes structures** : Adresse passée par l'appelant

---

## Inspection de la pile en assembleur

Regardons le code assembleur généré (simplifié) :

```c
int additionner(int a, int b) {
    int resultat = a + b;
    return resultat;
}
```

Assembleur x86-64 (Intel syntax) :

```asm
additionner:
    push    rbp                 ; Sauvegarder l'ancien frame pointer
    mov     rbp, rsp            ; Nouveau frame pointer
    sub     rsp, 16             ; Allouer espace pour variables locales

    mov     DWORD PTR [rbp-4], edi   ; Paramètre a (depuis RDI)
    mov     DWORD PTR [rbp-8], esi   ; Paramètre b (depuis RSI)

    mov     eax, DWORD PTR [rbp-4]   ; Charger a
    add     eax, DWORD PTR [rbp-8]   ; a + b
    mov     DWORD PTR [rbp-12], eax  ; Stocker dans resultat

    mov     eax, DWORD PTR [rbp-12]  ; Valeur de retour dans RAX

    leave                       ; Restaurer RSP et RBP (équivalent à mov rsp, rbp; pop rbp)
    ret                         ; Retour à l'appelant
```

**Explications** :
- `rbp` : **Base Pointer** (pointeur de base du frame)
- `rsp` : **Stack Pointer** (sommet de la pile)
- `push`/`pop` : Empiler/dépiler
- `call` : Appeler une fonction (empile adresse de retour)
- `ret` : Retourner (dépile adresse de retour et saute)

---

## Sécurité : Stack Canaries

Les **stack canaries** (ou stack cookies) protègent contre les **buffer overflow** :

```c
void fonction_vulnerable(void) {
    char buffer[16];
    // [canary] ← Valeur secrète insérée par le compilateur
    // ...
}
```

**Principe** :
1. Le compilateur insère une valeur aléatoire (canary) entre les variables locales et l'adresse de retour
2. Avant de retourner, on vérifie si le canary est intact
3. Si modifié → buffer overflow détecté → programme avorté

Compiler avec protection :

```bash
gcc -fstack-protector-all -o programme programme.c
```

---

## Récapitulatif

### Concepts clés

| Concept | Description |
|---------|-------------|
| **Pile (Stack)** | Zone mémoire LIFO pour variables locales et gestion des appels |
| **Stack Frame** | Bloc dédié à une fonction contenant paramètres, variables locales, adresse de retour |
| **Empiler (Push)** | Ajouter un frame (appel de fonction) |
| **Dépiler (Pop)** | Retirer un frame (retour de fonction) |
| **Stack Pointer (SP)** | Registre pointant vers le sommet de la pile |
| **Frame Pointer (BP)** | Registre pointant vers la base du frame actuel |
| **Stack Overflow** | Dépassement de la taille maximale de la pile |

---

### Points à retenir

1. ✅ Chaque appel de fonction crée un **nouveau stack frame**
2. ✅ Les variables locales vivent dans le **frame de leur fonction**
3. ✅ Les frames sont **automatiquement détruits** au retour de la fonction
4. ✅ La pile suit le principe **LIFO** (Last In, First Out)
5. ✅ La pile est **limitée en taille** (~8 MB sur Linux)
6. ✅ **Ne jamais retourner un pointeur** vers une variable locale
7. ✅ Les grandes allocations doivent se faire sur le **heap**, pas la stack
8. ✅ Les paramètres sont **copiés** (passage par valeur) dans le frame

---

## Exemple final : Tracer manuellement la pile

```c
#include <stdio.h>

int double_nombre(int n) {
    printf("  [double_nombre] n = %d\n", n);
    return n * 2;
}

int triple_nombre(int n) {
    printf(" [triple_nombre] n = %d\n", n);
    int result = double_nombre(n);
    printf(" [triple_nombre] result = %d\n", result);
    return result * 3;
}

int main(void) {
    printf("[main] Début\n");
    int x = 5;
    int y = triple_nombre(x);
    printf("[main] y = %d\n", y);
    return 0;
}
```

**Sortie :**
```
[main] Début
 [triple_nombre] n = 5
  [double_nombre] n = 5
 [triple_nombre] result = 10
[main] y = 30
```

**Évolution de la pile :**

```
Appel 1: main
┌──────────────┐
│ main         │
│ x = 5        │
│ y = ?        │
└──────────────┘

Appel 2: main → triple_nombre(5)
┌──────────────┐
│ triple_nombre│ ← TOP
│ n = 5        │
│ result = ?   │
├──────────────┤
│ main         │
│ x = 5        │
│ y = ?        │
└──────────────┘

Appel 3: main → triple_nombre → double_nombre(5)
┌──────────────┐
│ double_nombre│ ← TOP (profondeur 3)
│ n = 5        │
├──────────────┤
│ triple_nombre│
│ n = 5        │
│ result = ?   │
├──────────────┤
│ main         │
│ x = 5        │
│ y = ?        │
└──────────────┘

Retour 1: double_nombre retourne 10
┌──────────────┐
│ triple_nombre│ ← TOP
│ n = 5        │
│ result = 10  │ ← Mise à jour
├──────────────┤
│ main         │
│ x = 5        │
│ y = ?        │
└──────────────┘

Retour 2: triple_nombre retourne 30
┌──────────────┐
│ main         │ ← TOP
│ x = 5        │
│ y = 30       │ ← Mise à jour
└──────────────┘

Retour 3: main se termine (programme fini)
```

---

## Aller plus loin

Pour approfondir votre compréhension :

1. **Debugger avec GDB** : Observez les stack frames en temps réel
2. **Lire l'assembleur** : Compilez avec `gcc -S` pour voir le code assembleur
3. **Outils de visualisation** : Utilisez [Python Tutor](http://pythontutor.com/c.html) pour visualiser la pile
4. **Analyser les core dumps** : Examinez l'état de la pile lors d'un crash

Dans les sections suivantes, nous verrons :
- **6.3** : Le passage par valeur et ses implications
- **6.4** : Les valeurs de retour et la gestion des erreurs
- **Module 3** : La gestion avancée de la mémoire (heap, pointeurs, allocation dynamique)

Vous avez maintenant les bases pour comprendre comment votre programme gère la mémoire au niveau des fonctions ! 🚀

⏭️ [Passage par valeur](/06-fonctions/03-passage-par-valeur.md)
