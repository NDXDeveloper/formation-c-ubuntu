🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 13.3 Comparaison Make vs CMake

## Introduction

Vous avez maintenant découvert CMake en profondeur, et vous vous demandez peut-être : "Dois-je complètement abandonner Make ?" ou "Quand devrais-je utiliser l'un ou l'autre ?". Cette section va répondre à ces questions en comparant objectivement Make et CMake, leurs forces, leurs faiblesses, et leurs cas d'usage respectifs.

**Spoiler** : CMake et Make ne sont pas vraiment des concurrents directs. CMake **génère** des Makefiles ! Mais il y a des situations où écrire un Makefile manuellement reste pertinent.

---

## Différence fondamentale : Générateur vs Exécuteur

### Make : Un exécuteur de build

**Make** lit un `Makefile` et exécute les commandes qu'il contient selon les dépendances définies.

```makefile
# Makefile - Instructions directes
mon_app: main.o utils.o
	gcc main.o utils.o -o mon_app

main.o: main.c
	gcc -c main.c -o main.o

utils.o: utils.c
	gcc -c utils.c -o utils.o
```

**Rôle** : Exécuter les bonnes commandes dans le bon ordre.

### CMake : Un générateur de build system

**CMake** lit un `CMakeLists.txt` et **génère** un système de build (souvent un Makefile, mais pas seulement).

```cmake
# CMakeLists.txt - Description abstraite
add_executable(mon_app main.c utils.c)
```

CMake génère ensuite automatiquement un Makefile adapté à votre système.

**Rôle** : Analyser votre projet et créer les bons fichiers de build pour votre plateforme.

### Le workflow complet

```
┌─────────────────────────────────────────────────────────────┐
│                    AVEC MAKE SEUL                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Vous écrivez : Makefile (instructions gcc exactes)         │
│                      ↓                                      │
│  make lit le Makefile                                       │
│                      ↓                                      │
│  make exécute : gcc -c main.c, gcc main.o -o app, etc.      │
│                      ↓                                      │
│  Résultat : Binaire compilé                                 │
│                                                             │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│                    AVEC CMAKE                               │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Vous écrivez : CMakeLists.txt (description abstraite)      │
│                      ↓                                      │
│  cmake génère : Makefile (ou build.ninja, .sln, etc.)       │
│                      ↓                                      │
│  make/ninja lit le fichier généré                           │
│                      ↓                                      │
│  make/ninja exécute : gcc -c main.c, etc.                   │
│                      ↓                                      │
│  Résultat : Binaire compilé                                 │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**Point clé** : CMake **utilise** Make (ou Ninja) comme backend !

---

## Comparaison détaillée

### Tableau récapitulatif

| Critère | Make | CMake |
|---------|------|-------|
| **Type** | Exécuteur de build | Générateur de build |
| **Fichier config** | `Makefile` | `CMakeLists.txt` |
| **Syntaxe** | Impérative (COMMENT faire) | Déclarative (QUOI faire) |
| **Portabilité** | Linux principalement | Multi-plateforme natif |
| **Courbe apprentissage** | Moyenne (syntaxe obscure) | Facile → Moyenne |
| **Projet simple** | ✅ Excellent | ⚠️ Overkill |
| **Projet complexe** | ❌ Ingérable | ✅ Excellent |
| **Détection dépendances** | Manuelle | Automatique |
| **IDE Support** | Limité | Excellent |
| **Vitesse** | ⚡ Rapide | ⚡ Rapide (génère pour Make/Ninja) |
| **Écosystème** | Mature mais vieillissant | Moderne et actif |

---

## Syntaxe : Impérative vs Déclarative

### Projet exemple : Calculatrice avec 3 fichiers

Structure :
```
calculator/
├── main.c
├── operations.c
└── operations.h
```

### Avec Make (Impératif)

```makefile
# Makefile - Vous dites COMMENT compiler

# Variables
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
OBJS = main.o operations.o
TARGET = calculator

# Règle par défaut
all: $(TARGET)

# Comment créer l'exécutable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Comment créer main.o
main.o: main.c operations.h
	$(CC) $(CFLAGS) -c main.c

# Comment créer operations.o
operations.o: operations.c operations.h
	$(CC) $(CFLAGS) -c operations.c

# Comment nettoyer
clean:
	rm -f $(OBJS) $(TARGET)

# Règles .PHONY
.PHONY: all clean
```

**Observations** :
- 📝 Vous spécifiez chaque étape de compilation
- 🔧 Vous devez lister toutes les dépendances manuellement
- ⚙️ Vous écrivez les commandes gcc explicites

### Avec CMake (Déclaratif)

```cmake
# CMakeLists.txt - Vous dites QUOI compiler

cmake_minimum_required(VERSION 3.15)
project(Calculator C)

# Standard C
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Warnings
add_compile_options(-Wall -Wextra)

# Créer l'exécutable
add_executable(calculator main.c operations.c)
```

**Observations** :
- 📝 Vous déclarez simplement ce que vous voulez construire
- 🔧 CMake détecte automatiquement les dépendances entre fichiers
- ⚙️ CMake génère les commandes gcc appropriées

**Différence de taille** : 25 lignes (Make) vs 10 lignes (CMake)

---

## Portabilité : Le grand avantage de CMake

### Problème avec Make : Portabilité limitée

#### Makefile pour Linux

```makefile
CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lm -lpthread
TARGET = mon_app

all: $(TARGET)

$(TARGET): main.o utils.o
	$(CC) main.o utils.o -o $(TARGET) $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f *.o $(TARGET)
```

#### Même Makefile sur Windows

**Problèmes** :
- ❌ `rm` n'existe pas (il faut `del` ou `erase`)
- ❌ Extension `.exe` nécessaire pour les exécutables
- ❌ Bibliothèques système différentes (`-lpthread` → Win32 API)
- ❌ Séparateurs de chemins différents (`/` vs `\`)

#### Solution avec Make : Plusieurs Makefiles ou logique conditionnelle

```makefile
# Makefile multi-plateforme (complexe !)
ifeq ($(OS),Windows_NT)
    CC = cl
    RM = del /Q
    TARGET = mon_app.exe
    LDFLAGS = /link ws2_32.lib
else
    CC = gcc
    RM = rm -f
    TARGET = mon_app
    LDFLAGS = -lm -lpthread
endif

# ... reste du Makefile
```

**Problème** : Le Makefile devient rapidement illisible et difficile à maintenir.

### Solution CMake : Portabilité native

```cmake
# CMakeLists.txt - Fonctionne partout !
cmake_minimum_required(VERSION 3.15)
project(MonApp C)

add_executable(mon_app main.c utils.c)

# Lier avec les bibliothèques (CMake s'adapte automatiquement)
find_package(Threads REQUIRED)
target_link_libraries(mon_app Threads::Threads m)
```

**Compilation** :
```bash
# Linux
cmake -B build
cmake --build build

# Windows (génère un projet Visual Studio)
cmake -B build
cmake --build build

# macOS (génère un projet Xcode)
cmake -B build
cmake --build build
```

**Le même CMakeLists.txt fonctionne partout !**

---

## Gestion des dépendances

### Avec Make : Tout est manuel

#### Trouver OpenSSL manuellement

```makefile
# Makefile - Dépendances manuelles

# Où est OpenSSL ? (peut varier selon le système)
OPENSSL_INCLUDE = /usr/include/openssl
OPENSSL_LIB = /usr/lib/x86_64-linux-gnu

# Ou peut-être ici sur un autre système ?
# OPENSSL_INCLUDE = /usr/local/opt/openssl/include
# OPENSSL_LIB = /usr/local/opt/openssl/lib

CC = gcc
CFLAGS = -I$(OPENSSL_INCLUDE)
LDFLAGS = -L$(OPENSSL_LIB) -lssl -lcrypto

secure_app: main.o
	$(CC) main.o -o secure_app $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c
```

**Problèmes** :
- ❌ Chemins codés en dur
- ❌ Varie entre Ubuntu, Fedora, macOS, Windows
- ❌ Varie selon la méthode d'installation (paquet, compilation manuelle)
- ❌ L'utilisateur doit souvent éditer le Makefile

### Avec CMake : Détection automatique

```cmake
# CMakeLists.txt - Détection automatique

cmake_minimum_required(VERSION 3.15)
project(SecureApp C)

# CMake trouve OpenSSL automatiquement
find_package(OpenSSL REQUIRED)

add_executable(secure_app main.c)

# Lien automatique avec les bons chemins
target_link_libraries(secure_app OpenSSL::SSL OpenSSL::Crypto)
```

**Avantages** :
- ✅ CMake cherche dans les emplacements standards du système
- ✅ Fonctionne sur Ubuntu, Fedora, macOS, Windows
- ✅ Gère automatiquement les chemins d'include et de lib
- ✅ Message d'erreur clair si la bibliothèque est absente

---

## Projet de taille croissante

### Projet avec 3 fichiers : Make est OK

**Makefile simple** :
```makefile
calculator: main.o ops.o
	gcc main.o ops.o -o calculator

%.o: %.c
	gcc -c $< -o $@

clean:
	rm -f *.o calculator
```

**Verdict** : 6 lignes, c'est raisonnable.

### Projet avec 10 fichiers : Make devient verbeux

**Makefile** :
```makefile
OBJS = main.o parser.o lexer.o ast.o codegen.o \
       utils.o error.o memory.o optimizer.o

compiler: $(OBJS)
	gcc $(OBJS) -o compiler

main.o: main.c parser.h lexer.h
parser.o: parser.c parser.h ast.h
lexer.o: lexer.c lexer.h utils.h
# ... 7 autres règles de dépendances à maintenir manuellement !
```

**CMakeLists.txt** :
```cmake
add_executable(compiler
    main.c parser.c lexer.c ast.c codegen.c
    utils.c error.c memory.c optimizer.c
)
```

**Verdict** : 15+ lignes (Make) vs 5 lignes (CMake)

### Projet avec 50+ fichiers : Make devient ingérable

**Makefile** : 100+ lignes avec des dépendances impossibles à maintenir manuellement.

**CMakeLists.txt** :
```cmake
# Organiser par modules
add_subdirectory(frontend)
add_subdirectory(backend)
add_subdirectory(optimizer)

add_executable(compiler main.c)
target_link_libraries(compiler frontend backend optimizer)
```

**Verdict** : Make = cauchemar, CMake = gérable.

---

## Builds multiples (Debug/Release)

### Avec Make : Makefiles séparés ou logique conditionnelle

**Approche 1 : Makefiles séparés**
```bash
# Structure
Makefile.debug
Makefile.release

# Utilisation
make -f Makefile.debug
make -f Makefile.release
```

**Approche 2 : Variable d'environnement**
```makefile
# Makefile avec logique conditionnelle
ifdef DEBUG
    CFLAGS = -g -O0 -DDEBUG
else
    CFLAGS = -O3 -DNDEBUG
endif

all: mon_app

mon_app: main.o
	$(CC) $(CFLAGS) main.o -o mon_app
```

**Utilisation** :
```bash
make DEBUG=1          # Build debug
make                  # Build release
```

**Problèmes** :
- ❌ Duplication de code
- ❌ Nettoyage nécessaire entre les builds
- ❌ Risque de mélanger les fichiers objets

### Avec CMake : Native et élégant

```cmake
# CMakeLists.txt - Unique
cmake_minimum_required(VERSION 3.15)
project(MonApp C)

add_executable(mon_app main.c)
```

**Utilisation** :
```bash
# Build Debug
cmake -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug

# Build Release
cmake -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release

# Les deux coexistent sans conflit !
./build-debug/mon_app
./build-release/mon_app
```

**Avantages** :
- ✅ Un seul CMakeLists.txt
- ✅ Builds séparés automatiquement
- ✅ Pas de nettoyage nécessaire
- ✅ Pas de risque de mélange

---

## Support des IDEs

### Make : Support limité

**IDEs avec support Make** :
- Code::Blocks (basique)
- Vim/Emacs (avec plugins)
- NetBeans (limité)

**Limitations** :
- ❌ Pas d'autocomplétion intelligente
- ❌ Pas de navigation dans le code basée sur les dépendances
- ❌ Debugging moins intégré

### CMake : Excellent support IDE

**IDEs avec support CMake natif** :
- ✅ Visual Studio Code (extension CMake Tools)
- ✅ CLion (JetBrains)
- ✅ Visual Studio 2017+
- ✅ Qt Creator
- ✅ Xcode
- ✅ Eclipse CDT

**Avantages** :
- ✅ Configuration automatique du projet
- ✅ IntelliSense basé sur compile_commands.json
- ✅ Debugging intégré avec breakpoints visuels
- ✅ Refactoring avancé
- ✅ Navigation dans les dépendances

---

## Vitesse de compilation

### Make : Très rapide... pour ce qu'il fait

Make excelle dans l'incrémental : il ne recompile que les fichiers modifiés.

**Benchmark** (projet de 100 fichiers) :
```
Compilation complète : 45 secondes
Modification d'1 fichier : 2 secondes
```

### CMake + Make : Équivalent

CMake **génère** un Makefile optimisé. La vitesse de build est la même !

**Benchmark** (même projet) :
```
Configuration initiale : 3 secondes (une fois)
Compilation complète : 45 secondes (identique)
Modification d'1 fichier : 2 secondes (identique)
```

### CMake + Ninja : Plus rapide

Ninja est un build system plus moderne et rapide que Make.

**Benchmark** (même projet) :
```
Configuration initiale : 2 secondes
Compilation complète : 28 secondes (37% plus rapide !)
Modification d'1 fichier : 1.5 secondes
```

**Commande** :
```bash
cmake -B build -G Ninja
cmake --build build
```

**Conclusion** : CMake n'est pas plus lent, et peut même être plus rapide avec Ninja.

---

## Quand utiliser Make seul ?

### Cas 1 : Projets ultra-simples (1-3 fichiers)

```c
// hello.c - Programme trivial
#include <stdio.h>
int main(void) {
    printf("Hello, World!\n");
    return 0;
}
```

**Makefile** :
```makefile
hello: hello.c
	gcc hello.c -o hello
```

**Verdict** : CMake serait overkill. Make suffit amplement.

### Cas 2 : Scripts de build très spécifiques

**Exemple** : Build qui nécessite des étapes non-standard :
```makefile
# Générer du code, compiler du Asm, puis lier
all: generated
	nasm -f elf64 startup.asm -o startup.o
	gcc main.c startup.o -nostdlib -o kernel
	objcopy -O binary kernel kernel.bin

generated:
	./generate_tables.sh
```

**Verdict** : Make donne un contrôle total sur chaque commande.

### Cas 3 : Intégration dans un environnement existant

Si votre projet fait partie d'un écosystème qui utilise Make (exemple : kernel Linux), restez avec Make pour la cohérence.

### Cas 4 : Apprentissage des concepts de build

Make est excellent pédagogiquement pour comprendre :
- Les dépendances entre fichiers
- La compilation séparée
- Le linking

**Après avoir compris Make, CMake devient plus clair.**

---

## Quand utiliser CMake ?

### Cas 1 : Projets de plus de 5 fichiers

Dès que vous avez :
- Plusieurs fichiers sources
- Des headers partagés
- Des dépendances entre modules

→ **CMake simplifie drastiquement la gestion**

### Cas 2 : Projets multi-plateformes

Si votre code doit fonctionner sur :
- Linux + Windows
- Linux + macOS
- Ou les trois

→ **CMake est indispensable** (portabilité native)

### Cas 3 : Utilisation de bibliothèques externes

Si vous utilisez :
- OpenSSL, libcurl, SQLite, Boost, etc.

→ **find_package() de CMake économise des heures de configuration**

### Cas 4 : Travail en équipe

CMake garantit que :
- Chaque développeur a la même configuration
- Les nouveaux arrivants peuvent build immédiatement
- Le CI/CD fonctionne automatiquement

### Cas 5 : Intégration IDE

Si vous utilisez :
- VS Code, CLion, Visual Studio, Qt Creator

→ **CMake offre la meilleure expérience**

---

## Migration de Make vers CMake

### Stratégie progressive

Vous avez un projet existant avec un Makefile ? Voici comment migrer :

#### Étape 1 : Analyser le Makefile actuel

```makefile
# Makefile existant
CC = gcc
CFLAGS = -Wall -Wextra -I include
LDFLAGS = -lm -lpthread

SRCS = main.c utils.c parser.c
OBJS = $(SRCS:.c=.o)
TARGET = mon_app

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
```

#### Étape 2 : Créer un CMakeLists.txt équivalent

```cmake
cmake_minimum_required(VERSION 3.15)
project(MonApp C)

# Équivalent de CFLAGS
add_compile_options(-Wall -Wextra)
include_directories(include)

# Équivalent de la définition du target
add_executable(mon_app main.c utils.c parser.c)

# Équivalent de LDFLAGS
target_link_libraries(mon_app m pthread)
```

#### Étape 3 : Tester en parallèle

```bash
# Garder le Makefile pour l'instant
make

# Tester CMake
cmake -B build
cmake --build build

# Comparer les binaires
ls -lh mon_app build/mon_app
```

#### Étape 4 : Valider puis supprimer le Makefile

Une fois que CMake fonctionne parfaitement :
```bash
git rm Makefile
git add CMakeLists.txt
git commit -m "Migration vers CMake"
```

### Tableau de correspondance Make → CMake

| Makefile | CMakeLists.txt |
|----------|----------------|
| `CC = gcc` | CMake détecte automatiquement |
| `CFLAGS = -Wall` | `add_compile_options(-Wall)` |
| `CFLAGS += -I include` | `include_directories(include)` ou `target_include_directories()` |
| `LDFLAGS = -lm` | `target_link_libraries(... m)` |
| `TARGET = app` | `add_executable(app ...)` |
| `$(TARGET): $(OBJS)` | CMake gère automatiquement |
| `%.o: %.c` | CMake gère automatiquement |
| `ifdef DEBUG` | `if(CMAKE_BUILD_TYPE STREQUAL "Debug")` |
| `clean:` | `cmake --build build --target clean` |

---

## Coexistence Make + CMake

### CMake peut générer des Makefiles

Le workflow le plus courant :

```bash
# CMake génère un Makefile
cmake -B build -G "Unix Makefiles"

# Puis vous utilisez make normalement
cd build
make
make clean
make install
```

**Vous bénéficiez des deux mondes** :
- ✅ Configuration abstraite avec CMake
- ✅ Build rapide avec Make

### Make peut appeler CMake

Certains projets utilisent un "Makefile wrapper" :

```makefile
# Makefile (wrapper pour CMake)
.PHONY: all clean test

all:
	cmake -B build -DCMAKE_BUILD_TYPE=Release
	cmake --build build

debug:
	cmake -B build -DCMAKE_BUILD_TYPE=Debug
	cmake --build build

clean:
	rm -rf build

test: all
	cd build && ctest
```

**Utilisation** :
```bash
make          # Build release avec CMake
make debug    # Build debug avec CMake
make test     # Tests
make clean    # Nettoyage
```

**Avantages** :
- ✅ Interface familière (make)
- ✅ Puissance de CMake en arrière-plan
- ✅ Transition douce pour les équipes habituées à Make

---

## Exemple comparatif complet : Projet réaliste

### Projet : Serveur HTTP simple

**Structure** :
```
http_server/
├── src/
│   ├── main.c
│   ├── server.c
│   ├── parser.c
│   └── utils.c
├── include/
│   ├── server.h
│   ├── parser.h
│   └── utils.h
└── tests/
    └── test_parser.c
```

### Solution 1 : Makefile (50 lignes)

```makefile
# Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I include
LDFLAGS = -lpthread

SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = tests
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

TARGET = http_server
TEST_TARGET = test_parser

.PHONY: all clean test

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(TEST_DIR)/test_parser.c src/parser.c -o $(BUILD_DIR)/$(TEST_TARGET)
	./$(BUILD_DIR)/$(TEST_TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Dépendances manuelles
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c $(INCLUDE_DIR)/server.h
$(BUILD_DIR)/server.o: $(SRC_DIR)/server.c $(INCLUDE_DIR)/server.h $(INCLUDE_DIR)/utils.h
$(BUILD_DIR)/parser.o: $(SRC_DIR)/parser.c $(INCLUDE_DIR)/parser.h
$(BUILD_DIR)/utils.o: $(SRC_DIR)/utils.c $(INCLUDE_DIR)/utils.h
```

### Solution 2 : CMakeLists.txt (20 lignes)

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
project(HttpServer VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra)

# Répertoires d'include
include_directories(include)

# Exécutable principal
add_executable(http_server
    src/main.c
    src/server.c
    src/parser.c
    src/utils.c
)

target_link_libraries(http_server pthread)

# Tests
enable_testing()
add_executable(test_parser tests/test_parser.c src/parser.c)
add_test(NAME TestParser COMMAND test_parser)
```

### Comparaison

| Aspect | Makefile | CMakeLists.txt |
|--------|----------|----------------|
| **Lignes de code** | 50 | 20 |
| **Dépendances** | Manuelles (4 lignes) | Automatiques |
| **Gestion build dir** | Manuelle (`mkdir -p`) | Automatique |
| **Tests** | Compilation manuelle | `ctest` intégré |
| **Portabilité** | Linux seulement | Multi-plateforme |
| **Maintenance** | ⚠️ Fragile | ✅ Robuste |

**Compilation** :

```bash
# Avec Make
make
./http_server

# Avec CMake
cmake -B build
cmake --build build
./build/http_server
```

**Tests** :

```bash
# Avec Make
make test

# Avec CMake
cd build && ctest
```

---

## Récapitulatif : Make vs CMake en 2025

### Utilisez Make quand...

- ✅ **Projet ultra-simple** (1-3 fichiers)
- ✅ **Script de build très spécifique** (étapes non-standard)
- ✅ **Apprentissage** des concepts de build
- ✅ **Contraintes legacy** (environnement Make obligatoire)
- ✅ **Contrôle absolu** des commandes nécessaires

### Utilisez CMake quand...

- ✅ **Projet de taille moyenne à grande** (5+ fichiers)
- ✅ **Multi-plateforme** (Linux + Windows/macOS)
- ✅ **Bibliothèques externes** (OpenSSL, etc.)
- ✅ **Travail en équipe** (reproducibilité)
- ✅ **Intégration IDE** (VS Code, CLion, etc.)
- ✅ **CI/CD** (automatisation)
- ✅ **Projets professionnels** (standard industriel)

### La réalité en 2025

```
┌──────────────────────────────────────────────────┐
│  PROJETS OPEN-SOURCE C/C++ (GitHub)              │
├──────────────────────────────────────────────────┤
│  CMake:        78%   ████████████████████████    │
│  Make:         15%   ████                        │
│  Meson:         4%   █                           │
│  Autres:        3%   █                           │
└──────────────────────────────────────────────────┘
```

**Tendance** : Migration progressive de Make vers CMake.

---

## Conclusion

**Make** reste un outil valide et performant, excellent pour :
- Comprendre les fondamentaux du build
- Projets très simples
- Situations où un contrôle total est nécessaire

**CMake** est devenu le standard pour :
- Projets professionnels et industriels
- Travail en équipe et open-source
- Portabilité multi-plateforme
- Intégration avec l'écosystème moderne (IDEs, CI/CD, gestionnaires de paquets)

**Recommandation** :
1. 🟢 **Débutants** : Apprenez Make pour comprendre les concepts, puis passez à CMake
2. 🟡 **Intermédiaire** : Utilisez CMake pour tous vos projets de plus de 5 fichiers
3. 🔴 **Avancé** : Maîtrisez les deux, utilisez le bon outil selon le contexte

**La bonne nouvelle** : CMake et Make ne sont pas ennemis. CMake **utilise** Make (ou Ninja) en backend, donc vous bénéficiez du meilleur des deux mondes !

---


⏭️ [Autres build systems](/13-build-systems/04-autres-build-systems.md)
