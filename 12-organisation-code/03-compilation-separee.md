🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 12.3 Compilation séparée

## Introduction

Lorsqu'un projet C grandit au-delà de quelques centaines de lignes, **compiler tout le code en une seule fois** devient inefficace et difficile à maintenir. La **compilation séparée** est la solution qui permet de :

- Diviser le code en **modules** logiques
- Compiler chaque module **indépendamment**
- **Réutiliser** des modules dans différents projets
- **Accélérer** la compilation (seuls les fichiers modifiés sont recompilés)
- Faciliter le **travail en équipe** (chacun travaille sur son module)

Dans cette section, nous allons découvrir :
- Le processus de compilation en détail
- Les fichiers objets (`.o`)
- Le linkage (édition de liens)
- Comment organiser un projet multi-fichiers
- Les commandes GCC pour la compilation séparée

---

## Rappel : La chaîne de compilation complète

Avant de plonger dans la compilation séparée, rappelons les **4 étapes** que GCC effectue lors de la compilation d'un programme C :

```
Source.c → Préprocesseur → Compilateur → Assembleur → Éditeur de liens → Exécutable
  (.c)         (.i)           (.s)          (.o)              (a.out)
```

### Les 4 étapes en détail

1. **Préprocessing** (prétraitement)
   - Gère les `#include`, `#define`, `#ifdef`, etc.
   - Résultat : fichier `.i` (rarement visible)

2. **Compilation** (compilation proprement dite)
   - Traduit le code C en assembleur
   - Résultat : fichier `.s` (assembleur)

3. **Assemblage**
   - Convertit l'assembleur en code machine
   - Résultat : fichier `.o` (**fichier objet**)

4. **Linkage** (édition de liens)
   - Combine tous les fichiers objets (`.o`)
   - Résout les références externes
   - Résultat : **exécutable** final

### Compilation monolithique vs séparée

**Compilation monolithique** (tout en une fois) :
```bash
gcc main.c utils.c math.c -o programme
```
→ GCC fait les 4 étapes pour tous les fichiers d'un coup

**Compilation séparée** :
```bash
gcc -c main.c   # Produit main.o (étapes 1-3)  
gcc -c utils.c  # Produit utils.o  
gcc -c math.c   # Produit math.o  
gcc main.o utils.o math.o -o programme  # Linkage (étape 4)  
```
→ Chaque fichier est compilé séparément, puis lié

**Avantage :** Si on modifie uniquement `utils.c`, on recompile **seulement** `utils.c`, pas les autres !

---

## Qu'est-ce qu'un fichier objet (`.o`) ?

### Définition

Un **fichier objet** (extension `.o` sur Linux/Unix, `.obj` sur Windows) est un fichier binaire qui contient :
- Du **code machine** (instructions CPU)
- Des **données** (variables globales/statiques)
- Une **table des symboles** (noms de fonctions/variables)
- Des **informations de relocation** (adresses à ajuster lors du linkage)

Il n'est **pas exécutable** seul, car il peut contenir des références à des fonctions/variables définies dans d'autres fichiers.

### Visualiser le contenu d'un fichier objet

**Commande `nm`** (affiche les symboles) :
```bash
gcc -c math.c   # Crée math.o  
nm math.o       # Liste les symboles  
```

**Exemple de sortie :**
```
0000000000000000 T addition
0000000000000015 T multiplication
                 U printf
```

**Légende :**
- `T` : Symbole défini dans ce fichier (Text/code)
- `U` : Symbole non défini (Undefined) → sera résolu au linkage
- `D` : Donnée initialisée
- `B` : Donnée non initialisée (BSS)

---

## Exemple simple de compilation séparée

### Structure du projet

```
projet/
├── math.h         # Déclarations
├── math.c         # Implémentation des fonctions mathématiques
└── main.c         # Programme principal
```

### Fichier `math.h`

```c
#ifndef MATH_H
#define MATH_H

// Déclarations (prototypes)
int addition(int a, int b);  
int multiplication(int a, int b);  

#endif // MATH_H
```

### Fichier `math.c`

```c
#include "math.h"

// Implémentations
int addition(int a, int b) {
    return a + b;
}

int multiplication(int a, int b) {
    return a * b;
}
```

### Fichier `main.c`

```c
#include <stdio.h>
#include "math.h"  // Inclut les déclarations

int main(void) {
    int x = 10, y = 5;

    printf("%d + %d = %d\n", x, y, addition(x, y));
    printf("%d * %d = %d\n", x, y, multiplication(x, y));

    return 0;
}
```

### Compilation séparée étape par étape

**Étape 1 : Compiler `math.c` en fichier objet**

```bash
gcc -c math.c
```

Cette commande :
- Effectue les étapes 1-3 (préprocessing, compilation, assemblage)
- Produit `math.o`
- Ne fait **pas** le linkage (option `-c`)

**Étape 2 : Compiler `main.c` en fichier objet**

```bash
gcc -c main.c
```

Produit `main.o`.

**Étape 3 : Lier les fichiers objets**

```bash
gcc main.o math.o -o programme
```

Cette commande :
- Prend les fichiers objets `main.o` et `math.o`
- Effectue le linkage (étape 4)
- Résout les références (par ex. `addition` dans `main.o` est trouvée dans `math.o`)
- Produit l'exécutable `programme`

**Étape 4 : Exécuter**

```bash
./programme
```

**Sortie :**
```
10 + 5 = 15
10 * 5 = 50
```

### Compilation en une commande

On peut tout faire en une fois avec :

```bash
gcc math.c main.c -o programme
```

Mais c'est équivalent à :

```bash
gcc -c math.c  
gcc -c main.c  
gcc math.o main.o -o programme  
```

**Avantage de la compilation séparée :** Si on modifie `main.c`, on ne recompile **que** `main.c` :

```bash
# Modification de main.c
gcc -c main.c              # Recompile main.c uniquement  
gcc main.o math.o -o programme  # Re-linkage  
```

Pas besoin de recompiler `math.c` !

---

## Options de compilation GCC utiles

### Option `-c` : Compiler sans linker

```bash
gcc -c fichier.c
```

Produit `fichier.o` (fichier objet).

### Option `-o` : Spécifier le nom de sortie

```bash
gcc -c math.c -o math_operations.o  # Fichier objet personnalisé  
gcc main.o math_operations.o -o mon_programme  # Exécutable personnalisé  
```

### Option `-I` : Ajouter un répertoire d'include

Si les fichiers `.h` sont dans un sous-dossier :

```
projet/
├── include/
│   └── math.h
├── src/
│   ├── math.c
│   └── main.c
```

```bash
gcc -c src/math.c -I./include   # Cherche math.h dans include/  
gcc -c src/main.c -I./include  
gcc math.o main.o -o programme  
```

### Option `-Wall` et `-Wextra` : Warnings

**Toujours recommandé :**

```bash
gcc -c -Wall -Wextra math.c
```

Active tous les warnings courants pour détecter les erreurs potentielles.

### Option `-g` : Informations de débogage

Pour utiliser GDB (debugger) :

```bash
gcc -c -g math.c  
gcc -c -g main.c  
gcc -g main.o math.o -o programme  
```

### Option `-O` : Optimisation

```bash
gcc -c -O2 math.c  # Optimisation niveau 2 (recommandé pour production)
```

Niveaux : `-O0` (aucune), `-O1`, `-O2`, `-O3` (maximum), `-Os` (taille)

---

## Projet avec plusieurs modules

### Structure plus complexe

```
projet/
├── include/
│   ├── math_utils.h
│   ├── string_utils.h
│   └── config.h
├── src/
│   ├── math_utils.c
│   ├── string_utils.c
│   ├── config.c
│   └── main.c
└── build/          # Répertoire pour les .o et l'exécutable
```

### Fichier `include/math_utils.h`

```c
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

int addition(int a, int b);  
int multiplication(int a, int b);  
int puissance(int base, int exposant);  

#endif
```

### Fichier `src/math_utils.c`

```c
#include "math_utils.h"

int addition(int a, int b) {
    return a + b;
}

int multiplication(int a, int b) {
    return a * b;
}

int puissance(int base, int exposant) {
    int resultat = 1;
    for (int i = 0; i < exposant; i++) {
        resultat *= base;
    }
    return resultat;
}
```

### Fichier `include/string_utils.h`

```c
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

void inverser_chaine(char* str);  
int longueur_chaine(const char* str);  

#endif
```

### Fichier `src/string_utils.c`

```c
#include "string_utils.h"
#include <string.h>

void inverser_chaine(char* str) {
    int n = strlen(str);
    for (int i = 0; i < n / 2; i++) {
        char temp = str[i];
        str[i] = str[n - i - 1];
        str[n - i - 1] = temp;
    }
}

int longueur_chaine(const char* str) {
    return strlen(str);
}
```

### Fichier `include/config.h`

```c
#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "1.0.0"
#define APP_NAME "MonApplication"

extern int debug_mode;  // Déclaration

#endif
```

### Fichier `src/config.c`

```c
#include "config.h"

int debug_mode = 0;  // Définition
```

### Fichier `src/main.c`

```c
#include <stdio.h>
#include <string.h>
#include "math_utils.h"
#include "string_utils.h"
#include "config.h"

int main(void) {
    printf("=== %s v%s ===\n", APP_NAME, VERSION);

    // Tests mathématiques
    printf("\nTests mathématiques :\n");
    printf("5 + 3 = %d\n", addition(5, 3));
    printf("5 * 3 = %d\n", multiplication(5, 3));
    printf("2^10 = %d\n", puissance(2, 10));

    // Tests de chaînes
    printf("\nTests de chaînes :\n");
    char texte[] = "Bonjour";
    printf("Texte original : %s\n", texte);
    printf("Longueur : %d\n", longueur_chaine(texte));

    inverser_chaine(texte);
    printf("Texte inversé : %s\n", texte);

    // Configuration
    if (debug_mode) {
        printf("\nMode debug activé\n");
    }

    return 0;
}
```

### Compilation du projet complet

**Méthode manuelle :**

```bash
# Créer le dossier build
mkdir -p build

# Compiler chaque .c en .o
gcc -c src/math_utils.c -Iinclude -o build/math_utils.o  
gcc -c src/string_utils.c -Iinclude -o build/string_utils.o  
gcc -c src/config.c -Iinclude -o build/config.o  
gcc -c src/main.c -Iinclude -o build/main.o  

# Lier tous les .o
gcc build/main.o build/math_utils.o build/string_utils.o build/config.o -o build/programme

# Exécuter
./build/programme
```

**Méthode simplifiée (en une fois) :**

```bash
gcc src/*.c -Iinclude -o build/programme
```

Mais on perd l'avantage de la compilation séparée !

---

## Le processus de linkage en détail

### Qu'est-ce que le linkage ?

Le **linkage** (édition de liens) est l'étape qui :
1. Combine plusieurs fichiers objets (`.o`)
2. Résout les **références externes** (symboles utilisés mais non définis dans un fichier)
3. Inclut le code de démarrage et la bibliothèque standard
4. Produit un exécutable final

### Exemple de résolution de symboles

**Fichier `main.c`** :
```c
extern int addition(int, int);  // Déclaration externe

int main(void) {
    int resultat = addition(5, 3);  // Utilisation
    return 0;
}
```

**Après compilation (`gcc -c main.c`)** :
- `main.o` contient une **référence non résolue** à `addition`
- La table des symboles de `main.o` marque `addition` comme `U` (Undefined)

**Fichier `math.c`** :
```c
int addition(int a, int b) {
    return a + b;
}
```

**Après compilation (`gcc -c math.c`)** :
- `math.o` contient la **définition** de `addition`
- La table des symboles de `math.o` marque `addition` comme `T` (définie)

**Lors du linkage (`gcc main.o math.o -o programme`)** :
1. Le linker lit `main.o` et voit `addition` est utilisée mais non définie
2. Le linker lit `math.o` et trouve la définition de `addition`
3. Le linker **relie** la référence dans `main.o` à la définition dans `math.o`
4. Résultat : exécutable fonctionnel

### Visualisation avec `nm`

```bash
# Compiler
gcc -c main.c  
gcc -c math.c  

# Voir les symboles de main.o
nm main.o
# Sortie : ... U addition ...  (U = Undefined)

# Voir les symboles de math.o
nm math.o
# Sortie : ... T addition ...  (T = défini)

# Après linkage
gcc main.o math.o -o programme  
nm programme  
# Sortie : ... T addition ...  (résolu dans l'exécutable)
```

---

## Erreurs de linkage courantes

### Erreur 1 : Symbole non défini (Undefined reference)

**Code :**

```c
// main.c
#include <stdio.h>

void fonction_externe(void);  // Déclarée mais jamais définie !

int main(void) {
    fonction_externe();
    return 0;
}
```

**Compilation :**

```bash
gcc -c main.c  # ✅ Compilation OK (aucune vérification de définition)  
gcc main.o -o programme  # ❌ ERREUR de linkage  
```

**Message d'erreur :**

```
undefined reference to `fonction_externe'
```

**Cause :** `fonction_externe` est déclarée (prototype) mais jamais implémentée (définie).

**Solution :** Implémenter la fonction ou linker avec le fichier qui la contient.

### Erreur 2 : Définition multiple (Multiple definition)

**Code :**

```c
// math.c
int compteur = 0;  // Définition

// utils.c
int compteur = 0;  // Même définition !
```

**Compilation :**

```bash
gcc -c math.c  
gcc -c utils.c  
gcc math.o utils.o -o programme  # ❌ ERREUR  
```

**Message d'erreur :**

```
multiple definition of `compteur'
```

**Cause :** La variable `compteur` est définie dans deux fichiers différents.

**Solution :** Déclarer avec `extern` dans un `.h`, définir dans un seul `.c`.

```c
// globals.h
extern int compteur;  // Déclaration

// globals.c
int compteur = 0;  // Définition (une seule fois)
```

### Erreur 3 : Oublier de linker un fichier objet

**Code :**

```c
// main.c
void fonction_utile(void);

int main(void) {
    fonction_utile();
    return 0;
}

// utils.c
#include <stdio.h>

void fonction_utile(void) {
    printf("Fonction utile\n");
}
```

**Compilation incorrecte :**

```bash
gcc -c main.c  
gcc -c utils.c  
gcc main.o -o programme  # ❌ Oubli de utils.o  
```

**Erreur :**

```
undefined reference to `fonction_utile'
```

**Solution :**

```bash
gcc main.o utils.o -o programme  # ✅ Correct
```

---

## Bibliothèques statiques : Regrouper des fichiers objets

### Qu'est-ce qu'une bibliothèque statique ?

Une **bibliothèque statique** (extension `.a` sur Linux) est une **archive** de plusieurs fichiers objets (`.o`). Elle permet de regrouper du code réutilisable.

### Créer une bibliothèque statique

**Exemple :** Créer `libmath.a` contenant `addition` et `multiplication`.

**Fichiers :**

```c
// addition.c
int addition(int a, int b) {
    return a + b;
}

// multiplication.c
int multiplication(int a, int b) {
    return a * b;
}
```

**Étape 1 : Compiler en fichiers objets**

```bash
gcc -c addition.c       # Produit addition.o  
gcc -c multiplication.c # Produit multiplication.o  
```

**Étape 2 : Créer l'archive avec `ar`**

```bash
ar rcs libmath.a addition.o multiplication.o
```

**Options :**
- `r` : Remplace/ajoute les fichiers à l'archive
- `c` : Crée l'archive si elle n'existe pas
- `s` : Crée un index des symboles (équivalent de `ranlib`)

**Étape 3 : Utiliser la bibliothèque**

```c
// main.c
#include <stdio.h>

int addition(int, int);  
int multiplication(int, int);  

int main(void) {
    printf("5 + 3 = %d\n", addition(5, 3));
    printf("5 * 3 = %d\n", multiplication(5, 3));
    return 0;
}
```

**Compilation avec la bibliothèque :**

```bash
gcc main.c libmath.a -o programme
# OU
gcc main.c -L. -lmath -o programme  # -L. cherche dans le dossier actuel, -lmath charge libmath.a
```

**Exécution :**

```bash
./programme
```

**Sortie :**
```
5 + 3 = 8
5 * 3 = 15
```

### Avantages des bibliothèques statiques

- ✅ Tout le code est inclus dans l'exécutable (pas de dépendance externe)  
- ✅ Performance (pas de résolution de symboles au runtime)  
- ✅ Portabilité (un seul fichier exécutable)

**Inconvénient :** Taille de l'exécutable plus grande.

---

## Automatisation avec un script

Pour éviter de retaper toutes les commandes, on peut créer un script Bash :

### Fichier `build.sh`

```bash
#!/bin/bash

# Nettoyer les anciens fichiers
rm -rf build  
mkdir -p build  

# Compiler les fichiers sources
echo "Compilation des modules..."  
gcc -c src/math_utils.c -Iinclude -o build/math_utils.o -Wall -Wextra  
gcc -c src/string_utils.c -Iinclude -o build/string_utils.o -Wall -Wextra  
gcc -c src/config.c -Iinclude -o build/config.o -Wall -Wextra  
gcc -c src/main.c -Iinclude -o build/main.o -Wall -Wextra  

# Linkage
echo "Linkage..."  
gcc build/*.o -o build/programme  

echo "✅ Compilation terminée : build/programme"
```

**Utilisation :**

```bash
chmod +x build.sh  # Rendre exécutable (une seule fois)
./build.sh         # Compiler
./build/programme  # Exécuter
```

---

## Compilation conditionnelle : Ne recompiler que ce qui a changé

### Problème

Avec un gros projet, recompiler **tous** les fichiers à chaque modification est lent.

**Solution :** Ne recompiler que les fichiers modifiés.

### Script amélioré avec détection de modification

```bash
#!/bin/bash

mkdir -p build

# Fonction pour compiler un fichier si nécessaire
compile_if_needed() {
    SOURCE=$1
    OBJECT=$2

    # Si l'objet n'existe pas ou si la source est plus récente
    if [ ! -f "$OBJECT" ] || [ "$SOURCE" -nt "$OBJECT" ]; then
        echo "Compilation de $SOURCE..."
        gcc -c "$SOURCE" -Iinclude -o "$OBJECT" -Wall -Wextra
    else
        echo "✓ $OBJECT à jour"
    fi
}

# Compiler chaque fichier
compile_if_needed "src/math_utils.c" "build/math_utils.o"  
compile_if_needed "src/string_utils.c" "build/string_utils.o"  
compile_if_needed "src/config.c" "build/config.o"  
compile_if_needed "src/main.c" "build/main.o"  

# Linkage
echo "Linkage..."  
gcc build/*.o -o build/programme  

echo "✅ Build terminé"
```

**Comportement :**
- Si `math_utils.c` n'a pas changé → `math_utils.o` n'est pas recompilé
- Si `main.c` a été modifié → seul `main.o` est recompilé

**Note :** Pour de vrais projets, utilisez **Make** ou **CMake** (voir Module 5).

---

## Dépendances entre fichiers

### Le problème des headers

Si on modifie un fichier `.h`, tous les `.c` qui l'incluent doivent être **recompilés**.

**Exemple :**

```
math_utils.h  (modifié)
    ↓ inclus par
main.c        (doit être recompilé)  
utils.c       (doit être recompilé)  
```

### Gestion manuelle des dépendances

```bash
# Si math_utils.h est modifié, recompiler tous les fichiers qui l'incluent
if [ "include/math_utils.h" -nt "build/main.o" ]; then
    gcc -c src/main.c -Iinclude -o build/main.o
fi
```

**Problème :** Difficile à maintenir manuellement pour de gros projets.

**Solution :** Utiliser **Make** ou **CMake** qui gèrent automatiquement les dépendances.

---

## Avantages de la compilation séparée (récapitulatif)

| Avantage | Explication |
|----------|-------------|
| **Vitesse** | Seuls les fichiers modifiés sont recompilés |
| **Modularité** | Code organisé en modules logiques |
| **Réutilisabilité** | Modules compilés une fois, réutilisables dans plusieurs projets |
| **Travail en équipe** | Plusieurs développeurs travaillent sur différents modules |
| **Debugging** | Plus facile de localiser les erreurs par module |
| **Maintenance** | Changements localisés, moins de risque de casser le code |

---

## Exemple complet : Flux de travail typique

### Situation initiale

```
projet/
├── src/
│   ├── module_a.c
│   ├── module_b.c
│   └── main.c
└── include/
    ├── module_a.h
    └── module_b.h
```

### Première compilation (tout est neuf)

```bash
gcc -c src/module_a.c -Iinclude -o build/module_a.o  
gcc -c src/module_b.c -Iinclude -o build/module_b.o  
gcc -c src/main.c -Iinclude -o build/main.o  
gcc build/*.o -o build/programme  
```

**Temps :** 5 secondes (exemple)

### Modification de `module_b.c`

```bash
# Seul module_b.c est modifié
gcc -c src/module_b.c -Iinclude -o build/module_b.o  # Recompilation  
gcc build/*.o -o build/programme  # Re-linkage  
```

**Temps :** 1 seconde (80% plus rapide !)

### Modification de `module_b.h`

```bash
# module_b.h a changé → recompiler tous les fichiers qui l'incluent
gcc -c src/module_b.c -Iinclude -o build/module_b.o  # module_b.c inclut module_b.h  
gcc -c src/main.c -Iinclude -o build/main.o          # main.c inclut module_b.h  
# Pas besoin de recompiler module_a.c (n'inclut pas module_b.h)
gcc build/*.o -o build/programme
```

**Temps :** 3 secondes

---

## Commandes GCC résumées

| Commande | Description |
|----------|-------------|
| `gcc -c fichier.c` | Compile en fichier objet (`.o`) |
| `gcc -c fichier.c -o output.o` | Spécifie le nom du `.o` |
| `gcc fichier.o -o programme` | Linke un fichier objet |
| `gcc *.o -o programme` | Linke tous les `.o` |
| `gcc -Ichemin fichier.c` | Ajoute un répertoire d'include |
| `gcc -Wall -Wextra fichier.c` | Active les warnings |
| `gcc -g fichier.c` | Ajoute les infos de debug |
| `gcc -O2 fichier.c` | Optimise le code |
| `ar rcs libnom.a *.o` | Crée une bibliothèque statique |
| `gcc main.c -L. -lnom` | Linke avec `libnom.a` |

---

## Outils pour visualiser les dépendances

### `nm` : Lister les symboles

```bash
nm fichier.o
```

Affiche les symboles (fonctions/variables) définis (`T`, `D`) ou non définis (`U`).

### `objdump` : Désassembler

```bash
objdump -d fichier.o
```

Affiche le code assembleur du fichier objet.

### `ldd` : Dépendances dynamiques

```bash
ldd programme
```

Liste les bibliothèques partagées (`.so`) dont dépend l'exécutable.

**Note :** Fonctionne uniquement sur les exécutables, pas les `.o`.

### `file` : Type de fichier

```bash
file fichier.o  
file programme  
```

Affiche le type du fichier (objet, exécutable, etc.).

---

## Bonnes pratiques

| ✅ À FAIRE | ❌ À ÉVITER |
|-----------|------------|
| Compiler avec `-Wall -Wextra` | Ignorer les warnings |
| Séparer déclarations (`.h`) et définitions (`.c`) | Tout mettre dans les `.h` |
| Organiser en modules logiques | Fichiers géants (1000+ lignes) |
| Utiliser un système de build (Make/CMake) | Scripts de compilation manuels complexes |
| Garder les `.o` dans un dossier `build/` | Mélanger `.o` et sources |
| Recompiler seulement ce qui a changé | Toujours tout recompiler |

---

## Vers l'automatisation : Aperçu de Make

Pour de vrais projets, écrire manuellement toutes les commandes de compilation devient vite ingérable. C'est pourquoi on utilise des **systèmes de build** comme **Make** ou **CMake**.

### Exemple de `Makefile` simple

```makefile
# Variables
CC = gcc  
CFLAGS = -Wall -Wextra -Iinclude  
OBJDIR = build  
SOURCES = src/main.c src/math_utils.c src/string_utils.c src/config.c  
OBJECTS = $(OBJDIR)/main.o $(OBJDIR)/math_utils.o $(OBJDIR)/string_utils.o $(OBJDIR)/config.o  
TARGET = $(OBJDIR)/programme  

# Règle par défaut
all: $(TARGET)

# Lier l'exécutable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# Compiler les fichiers sources
$(OBJDIR)/%.o: src/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyer
clean:
	rm -rf $(OBJDIR)

.PHONY: all clean
```

**Utilisation :**

```bash
make        # Compile le projet  
make clean  # Supprime les fichiers générés  
```

**Avantage :** Make détecte automatiquement les fichiers modifiés et ne recompile que le nécessaire !

**Note :** Les Makefiles et CMake seront détaillés dans le Module 5.

---

## Conclusion

La **compilation séparée** est une technique fondamentale pour gérer des projets C de taille moyenne à grande. Elle permet de :

- Organiser le code en modules indépendants
- Accélérer considérablement les temps de compilation
- Faciliter la maintenance et le travail en équipe
- Créer des bibliothèques réutilisables

**Points clés à retenir :**

1. **Option `-c`** → Compile sans linker, produit des fichiers `.o`
2. **Fichiers objets** → Contiennent du code machine et des symboles
3. **Linkage** → Résout les références externes et combine les `.o`
4. **Erreurs de linkage** → "undefined reference" (fonction manquante) ou "multiple definition" (conflit)
5. **Bibliothèques statiques** → Archives (`.a`) de fichiers objets
6. **Automatisation** → Make/CMake pour gérer les dépendances

Dans les sections suivantes, nous verrons comment organiser efficacement les projets et comment utiliser des outils de build modernes comme CMake pour automatiser complètement le processus de compilation.

⏭️ [Organisation d'un projet](/12-organisation-code/04-organisation-projet.md)
