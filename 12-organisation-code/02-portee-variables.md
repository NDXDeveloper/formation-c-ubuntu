🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 12.2 La portée des variables

## Introduction

En C, toutes les variables n'ont pas la même **portée** (*scope*), c'est-à-dire la zone du programme où elles sont visibles et utilisables. De même, leur **durée de vie** varie selon l'endroit et la manière dont elles sont déclarées.

Comprendre ces concepts est essentiel pour :
- Éviter les conflits de noms
- Maîtriser la visibilité des données
- Organiser efficacement un projet multi-fichiers
- Prévenir les bugs liés aux variables non initialisées ou écrasées

Dans cette section, nous allons explorer :
- Les différents types de portée (locale, globale, fichier)
- La durée de vie des variables
- Les mots-clés `static` et `extern`
- Les bonnes pratiques d'organisation

---

## Les quatre niveaux de portée

En C, on distingue **quatre niveaux de portée** principaux :

| Portée | Où est déclarée la variable ? | Visible depuis... |
|--------|-------------------------------|-------------------|
| **Bloc** | À l'intérieur d'un bloc `{ }` | Le bloc uniquement |
| **Fonction** | En paramètre de fonction | La fonction uniquement |
| **Fichier** | Hors de toute fonction, avec `static` | Le fichier uniquement |
| **Globale** | Hors de toute fonction, sans `static` | Tout le programme |

Voyons chacun en détail.

---

## 1. Portée de bloc (Block Scope)

### Définition

Une variable déclarée **à l'intérieur d'un bloc** `{ }` n'est visible que dans ce bloc. Cela inclut :
- Les fonctions
- Les boucles (`for`, `while`)
- Les conditions (`if`, `else`)
- Tout bloc explicite

### Exemple simple

```c
#include <stdio.h>

int main(void) {
    int x = 10;  // Variable locale au bloc main

    printf("x = %d\n", x);  // ✅ Accessible

    return 0;
}

void autre_fonction(void) {
    printf("x = %d\n", x);  // ❌ ERREUR : x n'existe pas ici !
}
```

**Explication :** `x` est déclarée dans le bloc `main`, elle n'existe donc que dans `main`.

### Variables locales dans des sous-blocs

```c
#include <stdio.h>

int main(void) {
    int x = 5;

    if (x > 0) {
        int y = 20;  // y n'existe que dans ce bloc if
        printf("x = %d, y = %d\n", x, y);  // ✅ Les deux accessibles
    }

    printf("x = %d\n", x);  // ✅ x accessible
    printf("y = %d\n", y);  // ❌ ERREUR : y n'existe plus !

    return 0;
}
```

**Point clé :** `y` est **détruite** dès la sortie du bloc `if`.

### Variables dans les boucles

#### Déclaration C89/C90 (ancienne norme)

```c
int i;  
for (i = 0; i < 10; i++) {  
    printf("%d ", i);
}
printf("i après la boucle : %d\n", i);  // i vaut 10
```

**Comportement :** `i` existe avant et après la boucle.

#### Déclaration C99+ (norme moderne)

```c
for (int i = 0; i < 10; i++) {
    printf("%d ", i);
}
printf("i après la boucle : %d\n", i);  // ❌ ERREUR : i n'existe plus !
```

**Comportement :** `i` n'existe **que pendant la boucle**.

**Avantage :** Évite la pollution de l'espace de noms, réduit les bugs.

### Masquage de variables (Shadowing)

Que se passe-t-il si on redéclare une variable avec le même nom dans un sous-bloc ?

```c
#include <stdio.h>

int main(void) {
    int x = 10;
    printf("x externe : %d\n", x);  // Affiche 10

    {
        int x = 20;  // ⚠️ Nouvelle variable qui "masque" la première
        printf("x interne : %d\n", x);  // Affiche 20
    }

    printf("x externe après : %d\n", x);  // Affiche 10 (inchangé)

    return 0;
}
```

**Sortie :**
```
x externe : 10  
x interne : 20  
x externe après : 10  
```

**Explication :** Le `x` interne **masque** le `x` externe temporairement. Une fois le bloc fermé, l'ancien `x` redevient accessible.

**Recommandation :** Évitez le shadowing, source de confusion ! Utilisez des noms différents.

---

## 2. Portée de fonction (Function Scope)

### Paramètres de fonction

Les **paramètres** d'une fonction sont des variables locales à cette fonction :

```c
void calculer(int a, int b) {  // a et b sont locaux à calculer
    int somme = a + b;
    printf("Somme : %d\n", somme);
}

int main(void) {
    calculer(5, 3);
    printf("%d\n", a);  // ❌ ERREUR : a n'existe pas ici
    return 0;
}
```

### Labels (cas très particulier)

Les **labels** (utilisés avec `goto`) ont une portée de fonction :

```c
void exemple(void) {  
debut:  // Label visible dans toute la fonction  
    // ...
    if (condition) {
        goto debut;  // ✅ Accessible
    }
}

void autre(void) {
    goto debut;  // ❌ ERREUR : debut n'existe pas ici
}
```

**Note :** L'utilisation de `goto` est généralement déconseillée en programmation moderne.

---

## 3. Portée de fichier (File Scope) - Variables `static`

### Le mot-clé `static` pour les variables globales

Une variable déclarée **hors de toute fonction** avec `static` est visible **uniquement dans son fichier** :

#### Fichier `module1.c`

```c
#include <stdio.h>

static int compteur = 0;  // Variable globale MAIS visible uniquement dans module1.c

void incrementer(void) {
    compteur++;
    printf("Compteur dans module1 : %d\n", compteur);
}
```

#### Fichier `module2.c`

```c
#include <stdio.h>

static int compteur = 0;  // Autre variable, indépendante de celle de module1.c

void afficher(void) {
    printf("Compteur dans module2 : %d\n", compteur);  // Utilise SON compteur
}
```

#### Fichier `main.c`

```c
void incrementer(void);  // Prototype  
void afficher(void);  

int main(void) {
    incrementer();  // Affiche : Compteur dans module1 : 1
    incrementer();  // Affiche : Compteur dans module1 : 2
    afficher();     // Affiche : Compteur dans module2 : 0

    // compteur = 10;  // ❌ ERREUR : compteur n'est visible dans aucun des fichiers

    return 0;
}
```

**Avantages de `static` :**
- ✅ **Encapsulation** : Les données internes d'un module restent privées  
- ✅ **Pas de conflits de noms** : Deux fichiers peuvent avoir des variables `static` de même nom sans problème  
- ✅ **Sécurité** : Limite l'accès aux données

**Analogie :** C'est comme des variables "privées" dans un langage orienté objet.

### Le mot-clé `static` pour les fonctions

De même, une fonction déclarée `static` n'est visible que dans son fichier :

```c
// utils.c
#include <stdio.h>

static void helper_interne(void) {  // Fonction "privée"
    printf("Fonction interne\n");
}

void fonction_publique(void) {  // Fonction "publique"
    helper_interne();  // ✅ Accessible dans ce fichier
}
```

```c
// main.c
void fonction_publique(void);  // Déclaration externe

int main(void) {
    fonction_publique();  // ✅ OK
    helper_interne();     // ❌ ERREUR : fonction inconnue
    return 0;
}
```

**Utilité :** Permet de créer des "fonctions auxiliaires" qui ne polluent pas l'API publique du module.

---

## 4. Portée globale (Global Scope)

### Variables globales non-static

Une variable déclarée **hors de toute fonction** et **sans** `static` est accessible depuis **tous les fichiers** du programme (via `extern`).

#### Fichier `globals.c`

```c
#include <stdio.h>

int compteur_global = 0;  // Variable globale (sans static)

void incrementer_global(void) {
    compteur_global++;
}
```

#### Fichier `main.c`

```c
#include <stdio.h>

extern int compteur_global;  // Déclaration : "cette variable existe ailleurs"  
void incrementer_global(void);  

int main(void) {
    printf("Compteur initial : %d\n", compteur_global);  // 0
    incrementer_global();
    printf("Compteur après : %d\n", compteur_global);    // 1

    compteur_global = 100;  // ✅ Modification directe possible
    printf("Compteur modifié : %d\n", compteur_global);  // 100

    return 0;
}
```

**Compilation :**
```bash
gcc globals.c main.c -o programme
```

**Sortie :**
```
Compteur initial : 0  
Compteur après : 1  
Compteur modifié : 100  
```

### Le mot-clé `extern`

Le mot-clé `extern` indique au compilateur : *"Cette variable est définie dans un autre fichier, fais-moi confiance"*.

**Règle importante :** Une variable globale doit être **définie une seule fois** (dans un `.c`) mais peut être **déclarée plusieurs fois** (avec `extern`).

#### ✅ Bonne pratique

**Fichier `config.h`** (déclarations) :
```c
#ifndef CONFIG_H
#define CONFIG_H

extern int niveau_debug;  // Déclaration  
extern const char* nom_application;  

#endif
```

**Fichier `config.c`** (définitions) :
```c
#include "config.h"

int niveau_debug = 1;  // Définition (une seule fois)  
const char* nom_application = "MonApp";  
```

**Fichier `main.c`** :
```c
#include <stdio.h>
#include "config.h"

int main(void) {
    printf("Application : %s, Debug : %d\n", nom_application, niveau_debug);
    return 0;
}
```

#### ❌ Erreur courante : Définir dans le `.h`

```c
// config.h (MAUVAIS)
#ifndef CONFIG_H
#define CONFIG_H

int niveau_debug = 1;  // ❌ Définition dans le .h !

#endif
```

**Problème :** Si `config.h` est inclus dans plusieurs `.c`, la variable sera **définie plusieurs fois** → erreur de linkage (*multiple definition*).

---

## Durée de vie des variables

La **durée de vie** d'une variable détermine quand elle est créée et quand elle est détruite.

### Variables automatiques (locales)

Les variables locales (dans un bloc ou une fonction) ont une durée de vie **automatique** :
- **Création** : À l'entrée du bloc
- **Destruction** : À la sortie du bloc

```c
void fonction(void) {
    int x = 5;  // x est créée ici
    printf("%d\n", x);
}  // x est détruite ici (mémoire libérée)
```

**Conséquence :** À chaque appel de `fonction`, `x` est recréée et réinitialisée à 5.

### Variables statiques locales

Le mot-clé `static` **dans une fonction** donne une durée de vie **permanente** à la variable :

```c
#include <stdio.h>

void compter_appels(void) {
    static int compteur = 0;  // Initialisée UNE SEULE FOIS
    compteur++;
    printf("Appel numéro %d\n", compteur);
}

int main(void) {
    compter_appels();  // Affiche : Appel numéro 1
    compter_appels();  // Affiche : Appel numéro 2
    compter_appels();  // Affiche : Appel numéro 3
    return 0;
}
```

**Comportement :**
- `compteur` est initialisée **une seule fois** au premier appel
- Sa valeur **persiste** entre les appels
- Elle est **détruite** seulement à la fin du programme

**Différence avec une variable locale normale :**

```c
void compter_appels_sans_static(void) {
    int compteur = 0;  // Réinitialisée à CHAQUE appel
    compteur++;
    printf("Appel numéro %d\n", compteur);
}

int main(void) {
    compter_appels_sans_static();  // Affiche : Appel numéro 1
    compter_appels_sans_static();  // Affiche : Appel numéro 1 (pas 2 !)
    compter_appels_sans_static();  // Affiche : Appel numéro 1
    return 0;
}
```

### Variables globales et statiques (fichier)

Les variables globales et statiques de fichier existent **pendant toute l'exécution** du programme :
- **Création** : Au lancement du programme (avant `main`)
- **Destruction** : À la fin du programme (après `main`)

```c
int global = 42;  // Existe pendant toute l'exécution

int main(void) {
    // global est déjà créée et initialisée ici
    return 0;
}
// global est détruite après la sortie de main
```

---

## Initialisation des variables

### Variables locales (automatiques)

**Non initialisées par défaut** → contiennent des **valeurs indéterminées** (garbage) :

```c
void test(void) {
    int x;  // ⚠️ Valeur indéterminée (peut être 0, 42, -18374, n'importe quoi)
    printf("%d\n", x);  // Comportement indéfini !
}
```

**Bonne pratique :** **Toujours initialiser** les variables locales :

```c
void test(void) {
    int x = 0;  // ✅ Initialisée explicitement
    printf("%d\n", x);
}
```

### Variables globales et statiques

**Initialisées automatiquement à zéro** si non spécifié :

```c
int global;           // Vaut 0  
static int fichier;   // Vaut 0  

int main(void) {
    static int locale_statique;  // Vaut 0

    printf("%d %d %d\n", global, fichier, locale_statique);  // 0 0 0
    return 0;
}
```

**Avec initialisation explicite :**

```c
int global = 100;  
static int fichier = 200;  

int main(void) {
    static int locale_statique = 300;

    printf("%d %d %d\n", global, fichier, locale_statique);  // 100 200 300
    return 0;
}
```

---

## Tableaux récapitulatifs

### Portée et visibilité

| Type de variable | Déclaration | Visible depuis... | Durée de vie |
|------------------|-------------|-------------------|--------------|
| Locale (auto) | Dans un bloc | Le bloc uniquement | Bloc actif |
| Locale static | Dans un bloc avec `static` | Le bloc uniquement | Tout le programme |
| Globale static | Hors fonction avec `static` | Le fichier uniquement | Tout le programme |
| Globale | Hors fonction sans `static` | Tous les fichiers (via `extern`) | Tout le programme |

### Initialisation par défaut

| Type de variable | Initialisée par défaut ? | Valeur par défaut |
|------------------|--------------------------|-------------------|
| Locale (auto) | ❌ Non | Indéterminée (garbage) |
| Locale static | ✅ Oui | 0 (ou 0.0, NULL, etc.) |
| Globale static | ✅ Oui | 0 |
| Globale | ✅ Oui | 0 |

---

## Exemples pratiques

### Exemple 1 : Compteur de fonctions avec `static` local

```c
#include <stdio.h>

int calculer_moyenne(int a, int b) {
    static int nb_appels = 0;  // Persiste entre les appels
    nb_appels++;

    int moyenne = (a + b) / 2;
    printf("Appel #%d : moyenne de %d et %d = %d\n",
           nb_appels, a, b, moyenne);

    return moyenne;
}

int main(void) {
    calculer_moyenne(10, 20);  // Appel #1 : moyenne de 10 et 20 = 15
    calculer_moyenne(5, 15);   // Appel #2 : moyenne de 5 et 15 = 10
    calculer_moyenne(8, 12);   // Appel #3 : moyenne de 8 et 12 = 10

    return 0;
}
```

### Exemple 2 : Module avec état interne (`static` fichier)

**Fichier `compteur.c`** :
```c
#include <stdio.h>

// Variable privée au module (pas accessible depuis l'extérieur)
static int compteur_interne = 0;

void incrementer(void) {
    compteur_interne++;
}

void decrementer(void) {
    compteur_interne--;
}

int obtenir_valeur(void) {
    return compteur_interne;
}

void afficher(void) {
    printf("Compteur : %d\n", compteur_interne);
}
```

**Fichier `main.c`** :
```c
// Prototypes des fonctions publiques
void incrementer(void);  
void decrementer(void);  
int obtenir_valeur(void);  
void afficher(void);  

int main(void) {
    afficher();              // Compteur : 0
    incrementer();
    incrementer();
    afficher();              // Compteur : 2
    decrementer();
    afficher();              // Compteur : 1

    int val = obtenir_valeur();
    printf("Valeur récupérée : %d\n", val);  // 1

    // compteur_interne = 100;  // ❌ ERREUR : variable inaccessible !

    return 0;
}
```

**Avantage :** `compteur_interne` ne peut être modifié que via les fonctions publiques (encapsulation).

### Exemple 3 : Configuration globale partagée

**Fichier `config.h`** :
```c
#ifndef CONFIG_H
#define CONFIG_H

// Déclarations (extern)
extern int port_serveur;  
extern const char* nom_serveur;  
extern int mode_debug;  

#endif
```

**Fichier `config.c`** :
```c
#include "config.h"

// Définitions (une seule fois)
int port_serveur = 8080;  
const char* nom_serveur = "MonServeur";  
int mode_debug = 0;  
```

**Fichier `serveur.c`** :
```c
#include <stdio.h>
#include "config.h"

void demarrer_serveur(void) {
    printf("Démarrage de %s sur le port %d\n", nom_serveur, port_serveur);
    if (mode_debug) {
        printf("Mode debug activé\n");
    }
}
```

**Fichier `main.c`** :
```c
#include "config.h"

void demarrer_serveur(void);

int main(void) {
    mode_debug = 1;  // Modification de la config
    demarrer_serveur();
    return 0;
}
```

**Compilation :**
```bash
gcc config.c serveur.c main.c -o programme
```

**Sortie :**
```
Démarrage de MonServeur sur le port 8080  
Mode debug activé  
```

---

## Cas d'usage et recommandations

### Quand utiliser des variables locales ?

✅ **Par défaut**, privilégiez les variables locales :
- Portée limitée → moins de bugs
- Pas de pollution de l'espace de noms global
- Libération automatique de la mémoire

```c
void traiter_donnees(int* tableau, int taille) {
    int somme = 0;  // Locale : parfait pour un calcul temporaire
    for (int i = 0; i < taille; i++) {
        somme += tableau[i];
    }
    printf("Somme : %d\n", somme);
}
```

### Quand utiliser `static` local ?

✅ Lorsque vous avez besoin de **garder un état entre les appels** :
- Compteurs d'appels
- Caches/mémoïsation
- Génération de nombres uniques

```c
int generer_id(void) {
    static int dernier_id = 0;
    return ++dernier_id;  // 1, 2, 3, 4...
}
```

### Quand utiliser `static` fichier ?

✅ Pour créer des **modules avec état privé** :
- Variables internes à un module
- Fonctions auxiliaires non exportées
- Encapsulation de données

```c
// module.c
static int etat_interne;  // Privé au module

static void helper(void) {  // Fonction privée
    // ...
}

void api_publique(void) {  // Fonction publique
    helper();
}
```

### Quand utiliser des variables globales ?

⚠️ **À utiliser avec parcimonie** :
- Configuration globale de l'application
- États partagés entre modules (avec prudence)
- Ressources singleton (connexions, logs)

**Problèmes des variables globales :**
- ❌ Dépendances cachées entre modules  
- ❌ Difficiles à tester  
- ❌ Risque de modification accidentelle  
- ❌ Problèmes de concurrence (threads)

**Alternative recommandée :** Passer des pointeurs/structures explicitement plutôt que d'utiliser des globales.

```c
// ❌ Mauvais : variable globale
int compteur = 0;

void fonction1(void) { compteur++; }  
void fonction2(void) { compteur += 2; }  

// ✅ Meilleur : passage explicite
void fonction1(int* compteur) { (*compteur)++; }  
void fonction2(int* compteur) { (*compteur) += 2; }  

int main(void) {
    int compteur = 0;
    fonction1(&compteur);
    fonction2(&compteur);
    return 0;
}
```

---

## Pièges courants

### Piège 1 : Variable locale non initialisée

```c
int calculer(void) {
    int resultat;  // ⚠️ Non initialisée !

    if (condition_rare) {
        resultat = 42;
    }
    // Si condition_rare est fausse, resultat n'a jamais été assignée

    return resultat;  // ❌ Comportement indéfini !
}
```

**Solution :** Toujours initialiser.

```c
int calculer(void) {
    int resultat = 0;  // ✅ Valeur par défaut

    if (condition_rare) {
        resultat = 42;
    }

    return resultat;  // ✅ Toujours défini
}
```

### Piège 2 : Retourner un pointeur vers une variable locale

```c
int* fonction_dangereuse(void) {
    int x = 10;
    return &x;  // ❌ DANGER : x sera détruite à la sortie !
}

int main(void) {
    int* ptr = fonction_dangereuse();
    printf("%d\n", *ptr);  // ❌ Comportement indéfini (dangling pointer)
    return 0;
}
```

**Solution 1 :** Utiliser `static` (avec précaution) :

```c
int* fonction_ok(void) {
    static int x = 10;  // Persiste après la sortie de fonction
    return &x;  // ✅ OK
}
```

**Solution 2 :** Allocation dynamique :

```c
int* fonction_ok(void) {
    int* x = malloc(sizeof(int));
    *x = 10;
    return x;  // ✅ OK (mais penser à free !)
}
```

### Piège 3 : Oublier `extern` dans le `.h`

```c
// config.h (MAUVAIS)
int niveau_debug;  // Définition, pas déclaration !

// Si inclus dans 2+ fichiers → erreur de linkage
```

**Solution :**

```c
// config.h (BON)
extern int niveau_debug;  // Déclaration uniquement

// config.c
int niveau_debug = 1;  // Définition
```

### Piège 4 : Shadowing accidentel

```c
int x = 100;  // Globale

void fonction(void) {
    int x = 10;  // ⚠️ Masque la globale
    printf("%d\n", x);  // Affiche 10, pas 100 !
}
```

**Solution :** Évitez de réutiliser les mêmes noms à des portées différentes.

---

## Bonnes pratiques résumées

| ✅ À FAIRE | ❌ À ÉVITER |
|-----------|------------|
| Privilégier les variables locales | Abuser des variables globales |
| Initialiser toutes les variables locales | Laisser des variables non initialisées |
| Utiliser `static` pour l'encapsulation | Exposer toutes les fonctions/variables |
| Déclarer les globales avec `extern` dans les `.h` | Définir des variables dans les `.h` |
| Utiliser des noms de variables explicites | Réutiliser les mêmes noms (shadowing) |
| Limiter la portée au minimum nécessaire | Donner une portée globale par défaut |

---

## Schéma récapitulatif de la portée

```
┌─────────────────────────────────────────────────────┐
│ Programme entier                                    │
│                                                     │
│ ┌─────────────────────────────────────────────────┐ │
│ │ Fichier 1                                       │ │
│ │                                                 │ │
│ │ int global = 10;    ← Visible partout (extern)  │ │
│ │ static int priv=5;  ← Visible dans fichier1 seul│ │
│ │                                                 │ │
│ │ void fonction1() {                              │ │
│ │   int local = 3;  ← Visible dans fonction1      │ │
│ │   {                                             │ │
│ │     int bloc=1; ← Visible dans ce bloc          │ │
│ │   }                                             │ │
│ │ }                                               │ │
│ └─────────────────────────────────────────────────┘ │
│                                                     │
│ ┌─────────────────────────────────────────────────┐ │
│ │ Fichier 2                                       │ │
│ │                                                 │ │
│ │ extern int global;  ← Déclare la globale        │ │
│ │ static int priv=8;  ← Autre variable, indépend. │ │
│ │                                                 │ │
│ │ void fonction2() {                              │ │
│ │   static int compteur=0; ← Persiste entre appels│ │
│ │ }                                               │ │
│ └─────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────┘
```

---

## Conclusion

La **portée des variables** est un concept fondamental en C qui détermine où et quand vos variables sont accessibles. En comprenant les différences entre :
- Variables **locales** (bloc/fonction)
- Variables **statiques** (locales ou fichier)
- Variables **globales**

...vous pouvez écrire du code plus sûr, plus maintenable et mieux organisé.

**Points clés à retenir :**

1. **Par défaut, utilisez des variables locales** → portée minimale
2. **`static` dans une fonction** → état persistant entre appels
3. **`static` hors fonction** → variable/fonction privée au fichier
4. **Variables globales** → à utiliser avec parcimonie, déclarer avec `extern`
5. **Toujours initialiser** les variables locales
6. **Évitez le shadowing** → noms explicites et distincts

Dans la section suivante (12.3), nous verrons comment compiler séparément plusieurs fichiers sources pour créer des projets modulaires complets.

⏭️ [Compilation séparée](/12-organisation-code/03-compilation-separee.md)
