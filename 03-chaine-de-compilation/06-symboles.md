🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 3.6 Comprendre les Symboles et la Table des Symboles

## Introduction

Quand vous compilez un programme C composé de plusieurs fichiers, comment le compilateur sait-il où se trouve la fonction `printf` ? Comment l'éditeur de liens connecte-t-il un appel de fonction dans un fichier à sa définition dans un autre ?

La réponse réside dans les **symboles** et la **table des symboles**.

Dans cette section, vous apprendrez :

- Ce qu'est un symbole et pourquoi il est essentiel
- Les différents types de symboles
- Comment fonctionne la table des symboles
- Le rôle des symboles dans l'édition de liens
- Comment contrôler la visibilité des symboles
- Comment diagnostiquer les erreurs liées aux symboles

---

## Qu'est-ce qu'un Symbole ?

### Définition Simple

Un **symbole** est un **nom** associé à une **adresse mémoire**. C'est la façon dont le compilateur et l'éditeur de liens identifient les éléments de votre programme.

En termes simples :
- Chaque **fonction** que vous définissez crée un symbole
- Chaque **variable globale** crée un symbole
- Ces symboles permettent de "retrouver" les éléments lors de l'édition de liens

### Exemple Concret

Prenons ce programme simple :

```c
// programme.c
#include <stdio.h>

int compteur = 0;           // Symbole : compteur (variable globale)

void incrementer(void) {    // Symbole : incrementer (fonction)
    compteur++;
}

int main(void) {            // Symbole : main (fonction)
    incrementer();
    printf("%d\n", compteur);
    return 0;
}
```

Ce programme crée **3 symboles** :
1. `compteur` — une variable globale
2. `incrementer` — une fonction
3. `main` — la fonction principale

Il **utilise** également un symbole externe :
- `printf` — défini dans la bibliothèque C

### Ce Qui Ne Crée PAS de Symbole

Les **variables locales** (automatiques) ne créent pas de symboles :

```c
void exemple(void) {
    int variable_locale = 42;    // PAS de symbole (variable locale)

    for (int i = 0; i < 10; i++) {  // PAS de symbole (i est local)
        // ...
    }
}
```

Les variables locales vivent sur la **pile** et n'ont pas besoin de symboles car elles ne sont pas accessibles depuis d'autres fichiers.

> **Attention :** Les variables `static` locales à une fonction (`static int compteur = 0;`) sont un cas particulier. Elles créent un **symbole local** (visible avec `nm` sous la forme `d compteur.1234`) car elles sont stockées dans `.data` ou `.bss`, pas sur la pile. Cependant, ce symbole n'est pas exporté et n'intervient pas dans l'édition de liens entre fichiers.

---

## La Table des Symboles

### Qu'est-ce que c'est ?

La **table des symboles** est une structure de données contenue dans les fichiers objets (`.o`) et les exécutables. Elle liste tous les symboles avec leurs informations :

- **Nom** du symbole
- **Adresse** (ou offset) en mémoire
- **Type** (fonction, variable, etc.)
- **Taille**
- **Visibilité** (global, local, externe)
- **Section** où il se trouve (.text, .data, etc.)

### Visualisation de la Table

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  TABLE DES SYMBOLES DE programme.o                                          │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Nom              Adresse     Type    Taille   Section   Visibilité         │
│  ─────────────────────────────────────────────────────────────────────────  │
│  compteur         0x00000000  OBJECT  4        .data     GLOBAL             │
│  incrementer      0x00000000  FUNC    23       .text     GLOBAL             │
│  main             0x00000017  FUNC    45       .text     GLOBAL             │
│  printf           (externe)   FUNC    ?        ?         UNDEFINED          │
│                                                                             │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                             │
│  Légende :                                                                  │
│  • OBJECT = variable (données)                                              │
│  • FUNC = fonction (code)                                                   │
│  • GLOBAL = visible depuis d'autres fichiers                                │
│  • UNDEFINED = utilisé mais non défini ici (sera résolu au linkage)         │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Voir la Table des Symboles

L'outil `nm` affiche la table des symboles :

```bash
gcc -c programme.c -o programme.o  
nm programme.o  
```

**Résultat :**

```
0000000000000000 D compteur
0000000000000000 T incrementer
0000000000000017 T main
                 U printf
```

L'outil `readelf` donne plus de détails :

```bash
readelf -s programme.o
```

**Résultat (simplifié) :**

```
Symbol table '.symtab' contains 12 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND
     1: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS programme.c
     ...
     8: 0000000000000000     4 OBJECT  GLOBAL DEFAULT    3 compteur
     9: 0000000000000000    23 FUNC    GLOBAL DEFAULT    1 incrementer
    10: 0000000000000017    45 FUNC    GLOBAL DEFAULT    1 main
    11: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND printf
```

---

## Types de Symboles

### Classification par `nm`

L'outil `nm` utilise des lettres pour identifier le type de chaque symbole :

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  TYPES DE SYMBOLES (nm)                                                     │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  LETTRE    SECTION         DESCRIPTION                                      │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                             │
│  ┌─── CODE (FONCTIONS) ───────────────────────────────────────────────┐     │
│  │                                                                    │     │
│  │  T        .text          Fonction définie, GLOBALE (visible)       │     │
│  │  t        .text          Fonction définie, locale (static)         │     │
│  │                                                                    │     │
│  └────────────────────────────────────────────────────────────────────┘     │
│                                                                             │
│  ┌─── DONNÉES INITIALISÉES ───────────────────────────────────────────┐     │
│  │                                                                    │     │
│  │  D        .data          Variable initialisée, GLOBALE             │     │
│  │  d        .data          Variable initialisée, locale (static)     │     │
│  │                                                                    │     │
│  └────────────────────────────────────────────────────────────────────┘     │
│                                                                             │
│  ┌─── DONNÉES NON INITIALISÉES ───────────────────────────────────────┐     │
│  │                                                                    │     │
│  │  B        .bss           Variable non initialisée, GLOBALE         │     │
│  │  b        .bss           Variable non initialisée, locale          │     │
│  │                                                                    │     │
│  └────────────────────────────────────────────────────────────────────┘     │
│                                                                             │
│  ┌─── DONNÉES EN LECTURE SEULE ───────────────────────────────────────┐     │
│  │                                                                    │     │
│  │  R        .rodata        Constante, GLOBALE                        │     │
│  │  r        .rodata        Constante, locale                         │     │
│  │                                                                    │     │
│  └────────────────────────────────────────────────────────────────────┘     │
│                                                                             │
│  ┌─── SYMBOLES SPÉCIAUX ──────────────────────────────────────────────┐     │
│  │                                                                    │     │
│  │  U        (aucune)       UNDEFINED - Non défini, à résoudre        │     │
│  │  C        (common)       Variable "commune" (GCC < 10, -fcommon)   │     │
│  │  W / w    (weak)         Symbole faible (peut être remplacé)       │     │
│  │  A        (absolute)     Adresse absolue (ne change pas)           │     │
│  │                                                                    │     │
│  └────────────────────────────────────────────────────────────────────┘     │
│                                                                             │
│  Note : Majuscule = GLOBAL (visible), Minuscule = local (non visible)       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Exemple avec Différents Types

```c
// types_symboles.c
#include <stdio.h>

// Variables globales
int var_init = 42;              // D - data, initialisée  
int var_non_init;               // B - bss, non initialisée  
const int constante = 100;      // R - rodata, lecture seule  

// Variables statiques (locales au fichier)
static int var_static = 10;     // d - data, locale  
static void helper(void) { }    // t - text, locale  

// Fonction globale
void publique(void) {           // T - text, globale
    printf("Hello\n");          // U - undefined (externe)
}

int main(void) {                // T - text, globale
    return 0;
}
```

```bash
gcc -c types_symboles.c -o types_symboles.o  
nm types_symboles.o  
```

**Résultat :**

```
0000000000000000 t helper
0000000000000000 T main
                 U printf
0000000000000000 T publique
0000000000000000 R constante
0000000000000000 D var_init
0000000000000004 B var_non_init
0000000000000004 d var_static
```

---

## Symboles Définis vs Non Définis

### Symboles Définis

Un symbole est **défini** quand le code ou les données correspondantes existent dans ce fichier :

```c
// Dans fichier1.c
int ma_variable = 42;        // Définition → symbole D

void ma_fonction(void) {     // Définition → symbole T
    // code
}
```

### Symboles Non Définis (Undefined)

Un symbole est **non défini** (U) quand il est utilisé mais pas défini dans ce fichier :

```c
// Dans fichier2.c
extern int ma_variable;      // Déclaration (pas définition) → U

void autre_fonction(void) {
    ma_fonction();           // Utilisation → U (sera lié plus tard)
    printf("%d", ma_variable);  // printf est U aussi
}
```

### Le Rôle de l'Éditeur de Liens

C'est l'éditeur de liens qui **résout** les symboles non définis :

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  RÉSOLUTION DES SYMBOLES                                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│                                                                             │
│  fichier1.o                          fichier2.o                             │
│  ┌─────────────────────┐             ┌─────────────────────┐                │
│  │ ma_variable    : D  │             │ ma_variable    : U ─┼───┐            │
│  │ ma_fonction    : T  │             │ ma_fonction    : U ─┼───┼──┐         │
│  └─────────────────────┘             │ autre_fonction : T  │   │  │         │
│            │                         │ printf         : U ─┼───┼──┼──┐      │
│            │                         └─────────────────────┘   │  │  │      │
│            │                                                   │  │  │      │
│            │              libc.so                              │  │  │      │
│            │              ┌─────────────────────┐              │  │  │      │
│            │              │ printf         : T ◄┼──────────────┼──┼──┘      │
│            │              │ malloc         : T  │              │  │         │
│            │              │ ...                 │              │  │         │
│            │              └─────────────────────┘              │  │         │
│            │                                                   │  │         │
│            │                        LINKER                     │  │         │
│            │                          │                        │  │         │
│            └──────────────────────────┼────────────────────────┘  │         │
│                                       │                           │         │
│                                       │ ◄─────────────────────────┘         │
│                                       │                                     │
│                                       ▼                                     │
│                              ┌─────────────────────┐                        │
│                              │    programme        │                        │
│                              │  (exécutable)       │                        │
│                              │                     │                        │
│                              │ Tous les symboles   │                        │
│                              │ sont résolus !      │                        │
│                              └─────────────────────┘                        │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Erreur : Symbole Non Résolu

Si l'éditeur de liens ne trouve pas un symbole, il affiche une erreur :

```bash
gcc fichier1.o fichier2.o -o programme
# undefined reference to `fonction_inexistante'
```

Cette erreur signifie qu'un symbole `U` n'a trouvé aucun symbole `T` ou `D` correspondant.

---

## Visibilité des Symboles

### Symboles Globaux (par défaut)

Par défaut, toutes les fonctions et variables globales sont **visibles** depuis les autres fichiers :

```c
// module.c
int variable_publique = 42;      // Visible partout

void fonction_publique(void) {   // Visible partout
    // ...
}
```

### Symboles Locaux avec `static`

Le mot-clé `static` rend un symbole **local** au fichier (non exporté) :

```c
// module.c
static int variable_privee = 42;     // Invisible depuis l'extérieur

static void fonction_privee(void) {  // Invisible depuis l'extérieur
    // ...
}

void fonction_publique(void) {       // Visible
    fonction_privee();               // Peut être appelée ici
}
```

### Comparaison

```c
// fichier_a.c
int globale = 1;                // nm: D globale (GLOBAL)  
static int locale = 2;          // nm: d locale  (local)  

void publique(void) { }         // nm: T publique (GLOBAL)  
static void privee(void) { }    // nm: t privee   (local)  
```

```bash
nm fichier_a.o
```

```
0000000000000000 D globale
0000000000000004 d locale
0000000000000011 t privee
0000000000000000 T publique
```

### Accès avec `extern`

Le mot-clé `extern` déclare qu'un symbole existe ailleurs :

```c
// fichier_b.c
extern int globale;             // Déclare (ne définit pas)
// extern int locale;           // ERREUR : locale n'est pas exportée !

void autre(void) {
    globale = 100;              // OK : globale est visible
}
```

### Schéma de Visibilité

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  VISIBILITÉ DES SYMBOLES                                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  fichier_a.c                         fichier_b.c                            │
│  ┌───────────────────────────┐       ┌───────────────────────────┐          │
│  │                           │       │                           │          │
│  │  int globale = 1;    ─────┼───────┼──▶ extern int globale; ✓  │          │
│  │       (GLOBAL)            │       │                           │          │
│  │                           │       │                           │          │
│  │  static int locale = 2;   │   ╳   │  extern int locale;  ✗    │          │
│  │       (local)             │       │  (ERREUR: invisible)      │          │
│  │                           │       │                           │          │
│  │  void publique() { }  ────┼───────┼──▶ publique();  ✓         │          │
│  │       (GLOBAL)            │       │                           │          │
│  │                           │       │                           │          │
│  │  static void privee() { } │   ╳   │  privee();  ✗             │          │
│  │       (local)             │       │  (ERREUR: invisible)      │          │
│  │                           │       │                           │          │
│  └───────────────────────────┘       └───────────────────────────┘          │
│                                                                             │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                             │
│  Règle : static = privé au fichier, visible uniquement dans ce fichier      │
│          (défaut) = public, visible par tous les fichiers                   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Symboles Forts et Faibles

### Symboles Forts (Strong)

Par défaut, les symboles sont **forts**. Un symbole fort ne peut être défini qu'une seule fois :

```c
// fichier1.c
int valeur = 10;    // Symbole fort

// fichier2.c
int valeur = 20;    // Symbole fort → ERREUR : définition multiple !
```

```bash
gcc fichier1.c fichier2.c -o programme
# error: multiple definition of `valeur'
```

### Symboles Faibles (Weak)

Un symbole **faible** peut être remplacé par un symbole fort :

```c
// defaut.c
__attribute__((weak)) int config = 100;    // Symbole faible

void afficher(void) {
    printf("config = %d\n", config);
}

// personnalise.c
int config = 200;    // Symbole fort, remplace le faible
```

```bash
gcc defaut.c personnalise.c -o programme
./programme
# config = 200   (le symbole fort a gagné)
```

### Utilité des Symboles Faibles

Les symboles faibles sont utiles pour :

1. **Valeurs par défaut** remplaçables
2. **Fonctions optionnelles** (hooks, callbacks)
3. **Bibliothèques extensibles**

```c
// bibliotheque.c
// Fonction par défaut (peut être remplacée)
__attribute__((weak)) void on_error(int code) {
    fprintf(stderr, "Erreur %d\n", code);
}

void faire_quelque_chose(void) {
    if (/* erreur */) {
        on_error(42);  // Appelle la version de l'utilisateur si elle existe
    }
}
```

```c
// application.c
// L'utilisateur fournit sa propre implémentation
void on_error(int code) {
    log_to_file("Erreur", code);
    send_alert(code);
}
```

### Voir les Symboles Faibles

```bash
nm fichier.o | grep -i weak
# w ou W indique un symbole faible
```

---

## Les Sections et les Symboles

### Où Sont Stockés les Symboles ?

Les symboles sont associés à des **sections** dans le fichier objet :

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  SECTIONS ET SYMBOLES                                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  SECTION        CONTENU                        SYMBOLES ASSOCIÉS            │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                             │
│  .text          Code exécutable                Fonctions (T/t)              │
│                 Instructions machine           main, printf, helper...      │
│                                                                             │
│  .data          Variables initialisées         Variables globales (D/d)     │
│                 int x = 42;                    var_init, var_static...      │
│                                                                             │
│  .bss           Variables non initialisées     Variables globales (B/b)     │
│                 int y;                         var_non_init...              │
│                 (rempli de zéros au runtime)                                │
│                                                                             │
│  .rodata        Données en lecture seule       Constantes (R/r)             │
│                 const int c = 10;              constante, chaînes...        │
│                 "Hello World"                                               │
│                                                                             │
│  .symtab        Table des symboles             (métadonnées)                │
│                 Noms, adresses, types                                       │
│                                                                             │
│  .strtab        Table des chaînes              (noms des symboles)          │
│                 "main", "printf", etc.                                      │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Voir les Sections

```bash
readelf -S programme.o | grep -E "\.text|\.data|\.bss|\.rodata"
```

### Relation Section-Symbole

```bash
# Voir les symboles avec leur section
nm -o programme.o

# Ou avec readelf
readelf -s programme.o
```

La colonne `Ndx` (index) indique la section :
- `1` = `.text`
- `3` = `.data`
- `UND` = non défini

---

## La Résolution des Symboles en Détail

### Étapes de la Résolution

Lors de l'édition de liens, le linker :

1. **Collecte** tous les symboles de tous les fichiers objets
2. **Identifie** les symboles définis (T, D, B, R...)
3. **Identifie** les symboles non définis (U)
4. **Associe** chaque U à un symbole défini correspondant
5. **Calcule** les adresses finales (relocation)
6. **Génère** l'exécutable

### Exemple Pas à Pas

```c
// main.c
extern void afficher(void);  
int compteur = 0;  

int main(void) {
    afficher();
    return 0;
}
```

```c
// affichage.c
#include <stdio.h>
extern int compteur;

void afficher(void) {
    printf("Compteur: %d\n", compteur);
}
```

**Symboles de main.o :**
```
0000000000000000 D compteur       (défini)
0000000000000000 T main           (défini)
                 U afficher       (non défini → cherche ailleurs)
```

**Symboles de affichage.o :**
```
0000000000000000 T afficher       (défini)
                 U compteur       (non défini → cherche ailleurs)
                 U printf         (non défini → cherche ailleurs)
```

**Résolution :**
```
┌─────────────────────────────────────────────────────────────────────────────┐
│  RÉSOLUTION                                                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Symbole      main.o        affichage.o     libc.so        Résultat         │
│  ─────────────────────────────────────────────────────────────────────────  │
│  compteur     D (définit)   U (cherche) ←───────────────── OK (main.o)      │
│  main         T (définit)   -               -              OK (main.o)      │
│  afficher     U (cherche) ──▶ T (définit)   -              OK (affichage.o) │
│  printf       -             U (cherche) ────▶ T (définit)  OK (libc.so)     │
│                                                                             │
│  Tous les U sont résolus → Édition de liens réussie !                       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Stripping : Supprimer les Symboles

### Pourquoi Supprimer les Symboles ?

Les symboles occupent de l'espace et révèlent des informations sur votre code :

- **Réduction de taille** : Un exécutable strippé est plus petit
- **Sécurité** : Cache les noms de fonctions (reverse engineering plus difficile)
- **Production** : Les symboles de debug ne sont pas nécessaires

### Comment Stripper

```bash
# Avec l'outil strip
strip programme

# Ou lors de la compilation
gcc -s programme.c -o programme
```

### Comparaison

```bash
# Compiler normalement
gcc programme.c -o programme_normal  
ls -l programme_normal  
# -rwxr-xr-x 1 user user 16696 ... programme_normal

# Compiler et stripper
gcc -s programme.c -o programme_stripped  
ls -l programme_stripped  
# -rwxr-xr-x 1 user user 14472 ... programme_stripped

# Voir les symboles
nm programme_normal | wc -l
# 35

nm programme_stripped
# nm: programme_stripped: no symbols
```

### Conserver les Symboles pour le Debug

Pour la production, vous pouvez :

1. **Garder une version avec symboles** pour le débogage
2. **Distribuer une version strippée** aux utilisateurs

```bash
# Compiler avec symboles de debug
gcc -g programme.c -o programme_debug

# Créer une version strippée pour la distribution
cp programme_debug programme_release  
strip programme_release  
```

### Symboles Externes (Debug Info Séparé)

Vous pouvez aussi extraire les symboles dans un fichier séparé :

```bash
# Extraire les symboles de debug
objcopy --only-keep-debug programme programme.debug

# Stripper l'exécutable
strip programme

# Lier les deux (pour GDB)
objcopy --add-gnu-debuglink=programme.debug programme
```

---

## Erreurs Courantes Liées aux Symboles

### 1. "undefined reference to..."

**Cause** : Un symbole utilisé (U) n'est défini nulle part.

```bash
gcc main.o -o programme
# undefined reference to `ma_fonction'
```

**Solutions** :
- Ajouter le fichier `.o` contenant la définition
- Ajouter la bibliothèque (`-lm`, `-pthread`, etc.)
- Vérifier l'orthographe du nom

```bash
# Trouver qui définit le symbole
nm *.o | grep "ma_fonction"
```

### 2. "multiple definition of..."

**Cause** : Un symbole est défini plusieurs fois.

```bash
gcc fichier1.o fichier2.o -o programme
# multiple definition of `variable'
```

**Solutions** :
- Utiliser `static` pour les symboles qui doivent être privés
- Utiliser `extern` pour déclarer (pas définir)
- Vérifier les inclusions dans les headers

```c
// MAUVAIS (dans un .h inclus plusieurs fois)
int variable = 0;  // Définition !

// BON (dans un .h)
extern int variable;  // Déclaration seulement

// BON (dans UN SEUL .c)
int variable = 0;  // Définition unique
```

### 3. "relocation truncated to fit..."

**Cause** : Une adresse est trop grande pour le format utilisé.

**Solutions** :
- Utiliser les bons flags de compilation (`-mcmodel=large`)
- Restructurer le code

### 4. Symbole Non Trouvé au Runtime

**Cause** : Bibliothèque dynamique manquante.

```bash
./programme
# error while loading shared libraries: libxxx.so: cannot open shared object file
```

**Solutions** :
```bash
# Vérifier les dépendances
ldd programme

# Ajouter le chemin de la bibliothèque
export LD_LIBRARY_PATH=/chemin/vers/lib:$LD_LIBRARY_PATH
```

---

## Outils de Diagnostic

### Tableau Récapitulatif

| Outil | Usage | Exemple |
|-------|-------|---------|
| `nm` | Lister les symboles | `nm fichier.o` |
| `nm -u` | Symboles non définis uniquement | `nm -u fichier.o` |
| `nm -g` | Symboles globaux uniquement | `nm -g fichier.o` |
| `nm -C` | Démangler les noms C++ | `nm -C fichier.o` |
| `readelf -s` | Table des symboles détaillée | `readelf -s fichier.o` |
| `objdump -t` | Table des symboles | `objdump -t fichier.o` |
| `objdump -T` | Symboles dynamiques | `objdump -T programme` |
| `ldd` | Bibliothèques dynamiques | `ldd programme` |

### Commandes Utiles

```bash
# Trouver tous les fichiers définissant un symbole
nm *.o | grep " T ma_fonction"

# Trouver tous les fichiers utilisant un symbole
nm *.o | grep " U ma_fonction"

# Voir les symboles exportés d'une bibliothèque
nm -D /lib/x86_64-linux-gnu/libc.so.6 | grep printf

# Comparer les symboles de deux fichiers
diff <(nm fichier1.o) <(nm fichier2.o)
```

---

## Bonnes Pratiques

### 1. Utilisez `static` pour les Symboles Internes

```c
// module.c
static int etat_interne = 0;           // Pas visible à l'extérieur  
static void fonction_interne(void) {}  // Pas visible à l'extérieur  

void api_publique(void) {              // Interface publique
    fonction_interne();
}
```

### 2. Déclarez dans les Headers, Définissez dans les Sources

```c
// module.h
extern int variable;              // Déclaration  
void fonction(void);              // Déclaration  

// module.c
#include "module.h"
int variable = 42;                // Définition  
void fonction(void) { /* ... */ } // Définition  
```

### 3. Utilisez les Gardes d'Inclusion

```c
// module.h
#ifndef MODULE_H
#define MODULE_H

extern int variable;  
void fonction(void);  

#endif // MODULE_H
```

### 4. Évitez les Variables Globales

```c
// MAUVAIS
int compteur_global = 0;

// MIEUX
static int compteur = 0;  
int get_compteur(void) { return compteur; }  
void increment_compteur(void) { compteur++; }  
```

---

## Conclusion

Les symboles sont le ciment qui lie les différentes parties de votre programme :

1. **Les symboles identifient** les fonctions et variables globales
2. **La table des symboles** stocke leurs noms, adresses et attributs
3. **L'éditeur de liens résout** les références entre fichiers
4. **`static`** contrôle la visibilité des symboles
5. **Les symboles faibles** permettent des valeurs par défaut

**Commandes essentielles à retenir :**

```bash
# Lister les symboles
nm fichier.o

# Symboles non définis (à résoudre)
nm -u fichier.o

# Table des symboles détaillée
readelf -s fichier.o

# Bibliothèques dynamiques
ldd programme

# Stripper les symboles
strip programme
```

Comprendre les symboles vous permet de diagnostiquer les erreurs de linkage, d'organiser votre code en modules, et de créer des bibliothèques propres. C'est une compétence fondamentale pour tout développeur C travaillant sur des projets multi-fichiers.

⏭️ [Types de données et Représentation mémoire](/04-types-et-memoire/README.md)
