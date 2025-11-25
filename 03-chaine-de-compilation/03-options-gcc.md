🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 3.3 Options de Compilation GCC Essentielles

## Introduction

GCC (GNU Compiler Collection) est le compilateur C le plus utilisé sous Linux. Par défaut, la commande `gcc fichier.c` compile votre programme avec des options par défaut, mais GCC offre des centaines d'options pour contrôler finement le processus de compilation.

Dans cette section, nous allons découvrir les options **essentielles** que tout développeur C doit connaître. Ces options vous permettront de :

- Contrôler les étapes de compilation
- Détecter plus d'erreurs grâce aux warnings
- Déboguer efficacement vos programmes
- Optimiser les performances
- Assurer la compatibilité avec les standards C

---

## Syntaxe Générale

La syntaxe de base de GCC est :

```bash
gcc [options] fichier(s)_source -o nom_executable
```

Les options commencent généralement par `-` (un tiret) ou `--` (deux tirets). Elles peuvent être combinées dans n'importe quel ordre.

```bash
# Ces deux commandes sont équivalentes
gcc -Wall -g hello.c -o hello
gcc hello.c -o hello -Wall -g
```

---

## 1. Options de Contrôle du Pipeline

Ces options permettent d'arrêter la compilation à une étape spécifique, comme nous l'avons vu dans les sections précédentes.

### Tableau Récapitulatif

| Option | Étape finale | Fichier produit | Description |
|--------|--------------|-----------------|-------------|
| `-E` | Préprocesseur | `.i` | Arrête après le prétraitement |
| `-S` | Compilation | `.s` | Produit le code assembleur |
| `-c` | Assemblage | `.o` | Produit le fichier objet |
| *(aucune)* | Édition de liens | exécutable | Produit l'exécutable final |

### L'Option `-o` : Nommer la Sortie

L'option `-o` (output) permet de spécifier le nom du fichier de sortie :

```bash
# Sans -o : l'exécutable s'appelle "a.out" par défaut
gcc hello.c
./a.out

# Avec -o : on choisit le nom
gcc hello.c -o hello
./hello

# Fonctionne à toutes les étapes
gcc -E hello.c -o hello.i
gcc -S hello.c -o hello.s
gcc -c hello.c -o hello.o
```

**Conseil :** Utilisez toujours `-o` pour donner un nom explicite à vos exécutables.

---

## 2. Options de Warnings (Avertissements)

Les warnings sont des messages du compilateur qui signalent des problèmes potentiels dans votre code. Contrairement aux erreurs, ils n'empêchent pas la compilation, mais **il ne faut jamais les ignorer**.

### `-Wall` : Activer les Warnings Courants

C'est l'option **la plus importante**. Elle active un ensemble de warnings très utiles :

```bash
gcc -Wall hello.c -o hello
```

**Ce que `-Wall` détecte :**
- Variables non utilisées
- Fonctions sans type de retour explicite
- Comparaisons toujours vraies ou fausses
- Parenthèses manquantes dans des expressions ambiguës
- Et bien d'autres...

**Exemple :**

```c
// programme.c
#include <stdio.h>

int main(void) {
    int x;           // Variable déclarée mais non utilisée
    printf("Hello");
    // Pas de return (problème !)
}
```

```bash
gcc -Wall programme.c -o programme
# Warnings affichés :
# warning: unused variable 'x'
# warning: control reaches end of non-void function
```

### `-Wextra` : Warnings Supplémentaires

Active des warnings additionnels non inclus dans `-Wall` :

```bash
gcc -Wall -Wextra hello.c -o hello
```

**Warnings supplémentaires :**
- Paramètres de fonction non utilisés
- Comparaisons entre signé et non signé
- Expressions vides dans certains contextes

### `-Werror` : Transformer les Warnings en Erreurs

Cette option **bloque la compilation** si un warning est émis :

```bash
gcc -Wall -Werror hello.c -o hello
```

**Pourquoi utiliser `-Werror` ?**
- Force à corriger tous les problèmes détectés
- Empêche de "laisser passer" des warnings
- Recommandé dans les environnements professionnels et CI/CD

### `-Wpedantic` : Conformité Stricte au Standard

Émet des warnings pour tout code non conforme au standard C choisi :

```bash
gcc -Wall -Wpedantic -std=c11 hello.c -o hello
```

### `-Wconversion` : Conversions Implicites

Avertit lors de conversions qui peuvent perdre des données :

```bash
gcc -Wall -Wconversion hello.c -o hello
```

```c
int x = 1000;
char c = x;  // Warning : perte potentielle de données
```

### Combinaison Recommandée pour Débutants

```bash
gcc -Wall -Wextra -Werror hello.c -o hello
```

Cette combinaison vous force à écrire du code propre dès le départ.

### Visualisation des Niveaux de Warnings

```
┌─────────────────────────────────────────────────────────────┐
│  NIVEAUX DE WARNINGS GCC                                    │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  (aucun)     Warnings minimaux (dangereux !)                │
│      │                                                      │
│      ▼                                                      │
│  -Wall       Warnings courants (minimum recommandé)         │
│      │       ├── Variables non utilisées                    │
│      │       ├── Fonctions implicites                       │
│      │       └── Problèmes de format printf/scanf           │
│      ▼                                                      │
│  -Wextra     Warnings supplémentaires                       │
│      │       ├── Paramètres non utilisés                    │
│      │       └── Comparaisons signé/non-signé               │
│      ▼                                                      │
│  -Wpedantic  Conformité stricte au standard                 │
│      │       └── Extensions GCC non standard                │
│      ▼                                                      │
│  -Werror     Tous les warnings deviennent des erreurs       │
│              └── Compilation bloquée au moindre warning     │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 3. Options de Débogage

Ces options ajoutent des informations dans l'exécutable pour faciliter le débogage avec des outils comme GDB.

### `-g` : Informations de Débogage

Ajoute les informations nécessaires pour déboguer avec GDB :

```bash
gcc -g hello.c -o hello
```

**Ce que `-g` ajoute :**
- Correspondance entre code machine et lignes de code source
- Noms des variables et des fonctions
- Informations sur les types

**Sans `-g`, GDB est quasi inutilisable :**

```bash
# Sans -g
gcc hello.c -o hello
gdb ./hello
# (gdb) break main
# No symbol table is loaded. Use the "file" command.

# Avec -g
gcc -g hello.c -o hello
gdb ./hello
# (gdb) break main
# Breakpoint 1 at 0x1149: file hello.c, line 5.
```

### `-ggdb` : Informations Optimisées pour GDB

Produit des informations de débogage au format le plus expressif pour GDB :

```bash
gcc -ggdb hello.c -o hello
```

### `-g3` : Informations de Débogage Maximales

Inclut les informations sur les macros (utile pour déboguer du code avec beaucoup de `#define`) :

```bash
gcc -g3 hello.c -o hello
```

### Niveaux de Débogage

| Option | Niveau | Description |
|--------|--------|-------------|
| `-g0` | Aucun | Pas d'info de débogage |
| `-g1` | Minimal | Info basique (backtrace) |
| `-g` ou `-g2` | Standard | Info complète (recommandé) |
| `-g3` | Maximum | Info + macros |

### Combinaison Débogage + Warnings

Pour le développement, utilisez :

```bash
gcc -g -Wall -Wextra hello.c -o hello
```

---

## 4. Options d'Optimisation

GCC peut optimiser votre code pour le rendre plus rapide ou plus compact. Les optimisations sont désactivées par défaut.

### Les Niveaux d'Optimisation

| Option | Niveau | Description |
|--------|--------|-------------|
| `-O0` | Aucun | Pas d'optimisation (défaut) |
| `-O1` | Basique | Optimisations rapides et sûres |
| `-O2` | Standard | Bon équilibre vitesse/taille |
| `-O3` | Agressif | Maximum de vitesse |
| `-Os` | Taille | Optimise pour la taille |
| `-Ofast` | Extrême | `-O3` + optimisations non conformes |

### `-O0` : Aucune Optimisation

```bash
gcc -O0 hello.c -o hello
```

- Compilation la plus rapide
- Code le plus facile à déboguer
- **Recommandé pendant le développement**

### `-O1` : Optimisation Basique

```bash
gcc -O1 hello.c -o hello
```

- Réduction de la taille du code
- Amélioration modeste des performances
- Temps de compilation légèrement plus long

### `-O2` : Optimisation Standard

```bash
gcc -O2 hello.c -o hello
```

- **Recommandé pour la production**
- Bon compromis entre vitesse et temps de compilation
- N'augmente pas excessivement la taille du code

### `-O3` : Optimisation Agressive

```bash
gcc -O3 hello.c -o hello
```

- Toutes les optimisations de `-O2`
- Déroulage de boucles (loop unrolling)
- Inlining plus agressif
- Peut augmenter la taille du code
- Temps de compilation plus long

### `-Os` : Optimisation pour la Taille

```bash
gcc -Os hello.c -o hello
```

- Comme `-O2` mais évite les optimisations qui augmentent la taille
- Utile pour les systèmes embarqués avec peu de mémoire

### `-Ofast` : Optimisation Extrême

```bash
gcc -Ofast hello.c -o hello
```

- Active `-O3` plus des optimisations non conformes au standard
- Peut changer le comportement des calculs flottants
- **À utiliser avec précaution**

### Visualisation des Niveaux d'Optimisation

```
┌─────────────────────────────────────────────────────────────┐
│  NIVEAUX D'OPTIMISATION                                     │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Performance                                                │
│      ▲                                                      │
│      │                                          -Ofast ●    │
│      │                                      (non standard)  │
│      │                                                      │
│      │                                    -O3 ●             │
│      │                                   (agressif)         │
│      │                                                      │
│      │                            -O2 ●                     │
│      │                           (production)               │
│      │                                                      │
│      │                    -O1 ●                             │
│      │                   (basique)                          │
│      │                                                      │
│      │            -O0 ●                                     │
│      │           (debug)                                    │
│      │                                                      │
│      └──────────────────────────────────────────────▶       │
│                    Temps de compilation                     │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Important : Optimisation et Débogage

Les optimisations peuvent réorganiser ou supprimer du code, ce qui rend le débogage difficile :

```bash
# Pour le développement (débogage facile)
gcc -g -O0 hello.c -o hello

# Pour la production (performance)
gcc -O2 hello.c -o hello

# Compromis (débogage possible avec optimisations)
gcc -g -Og hello.c -o hello  # -Og = optimisé pour le débogage
```

---

## 5. Options de Standard C

Le langage C a évolué au fil des années. GCC supporte plusieurs versions du standard.

### Les Standards Disponibles

| Option | Standard | Année | Description |
|--------|----------|-------|-------------|
| `-std=c89` ou `-ansi` | ANSI C | 1989 | Le C original standardisé |
| `-std=c99` | C99 | 1999 | Ajout de `//`, `bool`, VLA... |
| `-std=c11` | C11 | 2011 | Threads, atomiques, `_Generic` |
| `-std=c17` | C17 | 2017 | Corrections de C11 |
| `-std=c23` | C23 | 2023 | Dernière version |
| `-std=gnu11` | GNU C11 | - | C11 + extensions GNU |

### Choisir un Standard

```bash
# Compiler en C99
gcc -std=c99 hello.c -o hello

# Compiler en C11 (recommandé)
gcc -std=c11 hello.c -o hello

# Compiler en C11 avec extensions GNU (défaut de GCC)
gcc -std=gnu11 hello.c -o hello
```

### Pourquoi Spécifier le Standard ?

Certaines fonctionnalités n'existent que dans certains standards :

```c
// Commentaires // : C99+
// Ce commentaire ne compile pas en C89

// Déclaration de variable au milieu du code : C99+
for (int i = 0; i < 10; i++) {  // 'int i' dans le for : C99+
    // ...
}

// Booléens natifs : C99+ (avec stdbool.h)
#include <stdbool.h>
bool flag = true;
```

### Recommandation

Pour un nouveau projet en 2025 :

```bash
gcc -std=c11 -Wall -Wextra hello.c -o hello
```

C11 offre un bon équilibre entre fonctionnalités modernes et compatibilité.

---

## 6. Options d'Inclusion et de Liaison

Ces options indiquent à GCC où trouver les fichiers d'en-tête et les bibliothèques.

### `-I` : Chemin des Fichiers d'En-tête

Ajoute un répertoire où chercher les fichiers `.h` :

```bash
gcc -I./include hello.c -o hello
```

**Structure de projet typique :**

```
mon_projet/
├── include/
│   └── utils.h
├── src/
│   └── main.c
└── Makefile
```

```bash
gcc -I./include src/main.c -o programme
```

Vous pouvez spécifier plusieurs répertoires :

```bash
gcc -I./include -I./libs/include hello.c -o hello
```

### `-L` : Chemin des Bibliothèques

Ajoute un répertoire où chercher les bibliothèques (`.a`, `.so`) :

```bash
gcc -L./lib hello.c -o hello -lmalib
```

### `-l` : Lier une Bibliothèque

Lie votre programme avec une bibliothèque :

```bash
# Lier avec la bibliothèque mathématique (libm.so)
gcc hello.c -o hello -lm

# Lier avec pthread (libpthread.so)
gcc hello.c -o hello -lpthread
```

**Convention de nommage :**
- La bibliothèque `libXXX.so` se lie avec `-lXXX`
- `libm.so` → `-lm`
- `libpthread.so` → `-lpthread`

### Exemple Complet

```bash
gcc -I./include -L./lib src/main.c -o programme -lutils -lm
```

Cette commande :
1. Cherche les `.h` dans `./include`
2. Cherche les bibliothèques dans `./lib`
3. Compile `src/main.c`
4. Lie avec `libutils` et `libm`

---

## 7. Options du Préprocesseur

Ces options contrôlent le comportement du préprocesseur.

### `-D` : Définir une Macro

Définit une macro comme si vous aviez écrit `#define` dans le code :

```bash
# Équivalent à #define DEBUG
gcc -DDEBUG hello.c -o hello

# Équivalent à #define VERSION 2
gcc -DVERSION=2 hello.c -o hello
```

**Utilisation typique pour le débogage conditionnel :**

```c
#include <stdio.h>

int main(void) {
    #ifdef DEBUG
    printf("Mode debug activé\n");
    #endif

    printf("Programme principal\n");
    return 0;
}
```

```bash
# Sans DEBUG
gcc hello.c -o hello
./hello
# Affiche : Programme principal

# Avec DEBUG
gcc -DDEBUG hello.c -o hello
./hello
# Affiche : Mode debug activé
#           Programme principal
```

### `-U` : Supprimer une Macro

Supprime une macro prédéfinie :

```bash
gcc -UDEBUG hello.c -o hello
```

### `-include` : Inclure un Fichier

Force l'inclusion d'un fichier au début de chaque source :

```bash
gcc -include config.h hello.c -o hello
```

Équivalent à ajouter `#include "config.h"` au début de `hello.c`.

---

## 8. Options Diverses Utiles

### `-v` : Mode Verbeux

Affiche toutes les commandes exécutées par GCC :

```bash
gcc -v hello.c -o hello
```

Utile pour diagnostiquer des problèmes de compilation.

### `-save-temps` : Conserver les Fichiers Intermédiaires

Garde les fichiers `.i`, `.s` et `.o` :

```bash
gcc -save-temps hello.c -o hello
ls
# hello  hello.c  hello.i  hello.o  hello.s
```

### `-###` : Afficher sans Exécuter

Affiche les commandes qui seraient exécutées, sans les exécuter :

```bash
gcc -### hello.c -o hello
```

### `-M` et `-MM` : Générer les Dépendances

Affiche les dépendances du fichier (utile pour les Makefiles) :

```bash
gcc -M hello.c   # Toutes les dépendances
gcc -MM hello.c  # Sans les en-têtes système
```

### `--version` : Afficher la Version

```bash
gcc --version
# gcc (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0
```

### `--help` : Afficher l'Aide

```bash
gcc --help
gcc --help=warnings  # Aide sur les warnings
gcc --help=optimizers  # Aide sur les optimisations
```

---

## 9. Récapitulatif : Options par Usage

### Pour le Développement (Débogage)

```bash
gcc -g -O0 -Wall -Wextra -std=c11 source.c -o programme
```

- `-g` : Informations de débogage
- `-O0` : Pas d'optimisation (code prévisible)
- `-Wall -Wextra` : Maximum de warnings
- `-std=c11` : Standard moderne

### Pour la Production

```bash
gcc -O2 -Wall -Wextra -Werror -std=c11 source.c -o programme
```

- `-O2` : Optimisations standard
- `-Werror` : Aucun warning toléré
- Pas de `-g` (sauf si vous voulez déboguer en production)

### Pour les Projets Stricts

```bash
gcc -g -O0 -Wall -Wextra -Wpedantic -Werror -std=c11 source.c -o programme
```

- `-Wpedantic` : Conformité stricte au standard
- `-Werror` : Zéro tolérance aux problèmes

### Commande Complète Typique

```bash
gcc -g -Wall -Wextra -Werror -std=c11 \
    -I./include \
    -L./lib \
    src/main.c src/utils.c \
    -o programme \
    -lm -lpthread
```

---

## 10. Tableau de Référence Rapide

| Option | Catégorie | Description |
|--------|-----------|-------------|
| `-o` | Sortie | Nom du fichier de sortie |
| `-E` | Pipeline | Arrête après le préprocesseur |
| `-S` | Pipeline | Arrête après la compilation (assembleur) |
| `-c` | Pipeline | Arrête après l'assemblage (objet) |
| `-Wall` | Warnings | Warnings courants |
| `-Wextra` | Warnings | Warnings supplémentaires |
| `-Werror` | Warnings | Warnings → erreurs |
| `-Wpedantic` | Warnings | Conformité stricte |
| `-g` | Débogage | Infos de débogage |
| `-O0` | Optimisation | Aucune |
| `-O2` | Optimisation | Standard (production) |
| `-O3` | Optimisation | Agressive |
| `-Os` | Optimisation | Taille minimale |
| `-std=c11` | Standard | Utiliser C11 |
| `-I` | Chemins | Répertoire des headers |
| `-L` | Chemins | Répertoire des bibliothèques |
| `-l` | Liaison | Lier une bibliothèque |
| `-D` | Préprocesseur | Définir une macro |
| `-v` | Divers | Mode verbeux |
| `-save-temps` | Divers | Garder fichiers intermédiaires |

---

## Conclusion

Maîtriser les options de GCC est essentiel pour tout développeur C. En résumé :

1. **Toujours utiliser `-Wall`** (minimum absolu)
2. **Ajouter `-Wextra` et `-Werror`** pour du code propre
3. **Utiliser `-g`** pendant le développement
4. **Passer à `-O2`** pour la production
5. **Spécifier `-std=c11`** pour un standard moderne et stable

Commencez avec cette commande de base et ajustez selon vos besoins :

```bash
gcc -g -Wall -Wextra -std=c11 source.c -o programme
```

Dans les sections suivantes, nous verrons comment automatiser ces options avec des outils comme Make et CMake, pour ne plus avoir à les taper manuellement à chaque compilation.

⏭️ [Démystification du "Hello World"](/03-chaine-de-compilation/04-hello-world.md)
