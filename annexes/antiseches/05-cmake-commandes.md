🔝 Retour au [Sommaire](/SOMMAIRE.md)

# B.5 CMake - Commandes Principales - Antisèche

## 📌 Introduction

**CMake** (Cross-platform Make) est un système de build moderne qui génère des fichiers de build natifs (Makefile, projet Visual Studio, Ninja, etc.) à partir d'un fichier de configuration `CMakeLists.txt`.

### Pourquoi CMake ?
- ✅ Multi-plateforme (Linux, Windows, macOS)
- ✅ Gestion automatique des dépendances
- ✅ Support des IDE modernes (VS Code, CLion)
- ✅ Standard de facto pour les projets C/C++

### Workflow CMake de base
```bash
# 1. Créer un répertoire de build (out-of-source)
mkdir build && cd build

# 2. Configurer le projet
cmake ..

# 3. Compiler
cmake --build .

# 4. (Optionnel) Installer
cmake --install .
```

---

## 📄 Structure minimale d'un CMakeLists.txt

```cmake
# Version minimale de CMake requise
cmake_minimum_required(VERSION 3.10)

# Nom du projet
project(MonProjet)

# Créer un exécutable
add_executable(mon_programme main.c)
```

---

## 🏗️ Commandes de Base

### `cmake_minimum_required()`
Spécifie la version minimale de CMake requise.

```cmake
cmake_minimum_required(VERSION 3.10)
```

**Versions courantes :**
- `3.10` : Ubuntu 18.04
- `3.16` : Ubuntu 20.04
- `3.22` : Ubuntu 22.04
- `3.30` : Version moderne (2024)

💡 **Astuce :** Utilisez la version disponible sur votre système cible le plus ancien.

---

### `project()`
Définit le nom du projet et optionnellement sa version et ses langages.

```cmake
# Simple
project(MonProjet)

# Avec version
project(MonProjet VERSION 1.2.3)

# Avec langages spécifiques
project(MonProjet LANGUAGES C CXX)

# Avec description
project(MonProjet
    VERSION 1.0.0
    DESCRIPTION "Mon super projet en C"
    LANGUAGES C
)
```

**Variables créées automatiquement :**
- `PROJECT_NAME` : Nom du projet
- `PROJECT_VERSION` : Version du projet
- `PROJECT_SOURCE_DIR` : Répertoire source racine

---

## 🎯 Création de Cibles (Targets)

### `add_executable()`
Crée un exécutable à partir de fichiers sources.

```cmake
# Un seul fichier
add_executable(mon_programme main.c)

# Plusieurs fichiers
add_executable(mon_programme
    main.c
    utils.c
    math_ops.c
)

# Avec variable
set(SOURCES main.c utils.c math_ops.c)
add_executable(mon_programme ${SOURCES})
```

---

### `add_library()`
Crée une bibliothèque (statique ou dynamique).

```cmake
# Bibliothèque statique (.a)
add_library(ma_lib STATIC
    lib.c
    helper.c
)

# Bibliothèque partagée (.so)
add_library(ma_lib SHARED
    lib.c
    helper.c
)

# CMake choisit automatiquement
add_library(ma_lib
    lib.c
    helper.c
)
```

**Types de bibliothèques :**
- `STATIC` : Bibliothèque statique (`.a` sous Linux)
- `SHARED` : Bibliothèque dynamique (`.so` sous Linux)
- `MODULE` : Plugin chargeable dynamiquement
- `INTERFACE` : Bibliothèque header-only

---

### `target_link_libraries()`
Lie une cible avec des bibliothèques.

```cmake
# Lier mon_programme avec ma_lib
target_link_libraries(mon_programme ma_lib)

# Lier avec plusieurs bibliothèques
target_link_libraries(mon_programme
    ma_lib
    pthread
    m  # bibliothèque mathématique
)

# Avec spécificateurs de visibilité
target_link_libraries(mon_programme
    PUBLIC ma_lib       # Propagé aux dépendants
    PRIVATE pthread     # Usage interne seulement
)
```

**Spécificateurs de visibilité :**
- `PUBLIC` : La dépendance est propagée aux cibles qui dépendent de celle-ci
- `PRIVATE` : La dépendance est interne uniquement
- `INTERFACE` : Uniquement pour les utilisateurs de cette cible

---

## 📁 Gestion des Fichiers

### `file()`
Opérations sur les fichiers.

```cmake
# Lister tous les fichiers .c
file(GLOB SOURCES "*.c")

# Récursif (inclut sous-répertoires)
file(GLOB_RECURSE SOURCES "src/*.c")

# Lire un fichier
file(READ "version.txt" VERSION_CONTENT)

# Écrire dans un fichier
file(WRITE "output.txt" "Contenu")

# Copier un fichier
file(COPY "config.ini" DESTINATION ${CMAKE_BINARY_DIR})
```

⚠️ **Attention :** `GLOB` est déconseillé pour les sources (CMake ne détecte pas automatiquement les nouveaux fichiers). Préférez lister explicitement les fichiers.

---

### `aux_source_directory()`
Collecte tous les fichiers sources d'un répertoire.

```cmake
# Ajoute tous les .c du répertoire src/
aux_source_directory(src/ SOURCES)
add_executable(mon_programme ${SOURCES})
```

⚠️ **Note :** Moins flexible que `file(GLOB)`, usage limité.

---

## 🔧 Configuration du Compilateur

### `target_compile_options()`
Ajoute des options de compilation à une cible spécifique.

```cmake
# Warnings stricts
target_compile_options(mon_programme PRIVATE
    -Wall
    -Wextra
    -Werror
)

# Optimisation
target_compile_options(mon_programme PRIVATE
    -O3
    -march=native
)
```

---

### `target_compile_definitions()`
Définit des macros de préprocesseur pour une cible.

```cmake
# Définir DEBUG
target_compile_definitions(mon_programme PRIVATE DEBUG)

# Avec valeur
target_compile_definitions(mon_programme PRIVATE
    VERSION="1.0"
    MAX_USERS=100
)
```

Équivalent de `gcc -DDEBUG -DVERSION=\"1.0\"`.

---

### `target_include_directories()`
Spécifie les répertoires d'en-têtes pour une cible.

```cmake
# Ajouter le répertoire include/
target_include_directories(mon_programme PRIVATE include/)

# Plusieurs répertoires
target_include_directories(mon_programme
    PUBLIC include/        # Exporté aux dépendants
    PRIVATE src/internal/  # Usage interne seulement
)
```

---

### `set()`
Définit une variable.

```cmake
# Variable simple
set(MA_VARIABLE "valeur")

# Liste
set(SOURCES main.c utils.c)

# Ajouter à une liste existante
list(APPEND SOURCES helper.c)

# Standard C
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
```

**Variables importantes :**
```cmake
# Standard C (11, 17, 23)
set(CMAKE_C_STANDARD 11)

# Standard C++ (11, 14, 17, 20, 23)
set(CMAKE_CXX_STANDARD 17)

# Type de build (Debug, Release, RelWithDebInfo, MinSizeRel)
set(CMAKE_BUILD_TYPE Debug)
```

---

## 📦 Gestion des Dépendances

### `find_package()`
Trouve et charge une bibliothèque externe.

```cmake
# Cherche la bibliothèque Threads
find_package(Threads REQUIRED)

# Utilisation
target_link_libraries(mon_programme Threads::Threads)
```

**Exemples courants :**
```cmake
# OpenSSL
find_package(OpenSSL REQUIRED)
target_link_libraries(mon_programme OpenSSL::SSL OpenSSL::Crypto)

# CURL
find_package(CURL REQUIRED)
target_link_libraries(mon_programme CURL::libcurl)

# Optionnel (ne provoque pas d'erreur si absent)
find_package(MyOptionalLib)
if(MyOptionalLib_FOUND)
    target_link_libraries(mon_programme MyOptionalLib::MyOptionalLib)
endif()
```

---

### `find_library()`
Cherche une bibliothèque spécifique.

```cmake
# Chercher libmath.so ou libmath.a
find_library(MATH_LIB math)

# Utilisation
if(MATH_LIB)
    target_link_libraries(mon_programme ${MATH_LIB})
endif()
```

---

### `pkg_check_modules()`
Utilise pkg-config pour trouver des bibliothèques.

```cmake
# Charger PkgConfig
find_package(PkgConfig REQUIRED)

# Chercher GTK
pkg_check_modules(GTK REQUIRED gtk+-3.0)

# Utilisation
target_include_directories(mon_programme PRIVATE ${GTK_INCLUDE_DIRS})
target_link_libraries(mon_programme ${GTK_LIBRARIES})
```

---

## 🏗️ Organisation Multi-Répertoires

### `add_subdirectory()`
Inclut un sous-répertoire contenant un autre CMakeLists.txt.

```cmake
# Structure du projet :
# projet/
# ├── CMakeLists.txt
# ├── src/
# │   └── CMakeLists.txt
# └── lib/
#     └── CMakeLists.txt

# Dans projet/CMakeLists.txt
add_subdirectory(src)
add_subdirectory(lib)
```

**Exemple complet :**

`projet/CMakeLists.txt` :
```cmake
cmake_minimum_required(VERSION 3.10)
project(MonProjet)

add_subdirectory(lib)
add_subdirectory(src)
```

`projet/lib/CMakeLists.txt` :
```cmake
add_library(ma_lib STATIC
    lib.c
    helper.c
)
target_include_directories(ma_lib PUBLIC .)
```

`projet/src/CMakeLists.txt` :
```cmake
add_executable(mon_programme main.c)
target_link_libraries(mon_programme ma_lib)
```

---

### `include()`
Inclut un fichier CMake.

```cmake
# Inclure un fichier de configuration
include(cmake/compiler_flags.cmake)

# Module CMake standard
include(GNUInstallDirs)
```

---

## 🧪 Tests

### `enable_testing()`
Active le support des tests.

```cmake
enable_testing()
```

---

### `add_test()`
Ajoute un test.

```cmake
# Test simple
add_test(NAME mon_test COMMAND mon_programme --test)

# Test avec exécutable dédié
add_executable(test_unitaire test.c)
add_test(NAME test_unitaire COMMAND test_unitaire)
```

**Exécution des tests :**
```bash
# Depuis le répertoire build/
ctest

# Avec sortie détaillée
ctest --verbose

# Test spécifique
ctest -R mon_test
```

---

## 📦 Installation

### `install()`
Définit les règles d'installation.

```cmake
# Installer un exécutable
install(TARGETS mon_programme
    DESTINATION bin
)

# Installer une bibliothèque
install(TARGETS ma_lib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Installer des headers
install(FILES lib.h
    DESTINATION include
)

# Installer un répertoire entier
install(DIRECTORY include/
    DESTINATION include
)
```

**Installation :**
```bash
# Depuis build/
cmake --install .

# Avec préfixe personnalisé
cmake --install . --prefix /usr/local
```

---

## 🔀 Logique Conditionnelle

### `if()` / `elseif()` / `else()` / `endif()`
Conditions.

```cmake
# Condition simple
if(UNIX)
    message("Système Unix/Linux")
endif()

# Avec else
if(WIN32)
    message("Windows")
else()
    message("Pas Windows")
endif()

# Conditions complexes
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(mon_programme PRIVATE DEBUG_MODE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(mon_programme PRIVATE -O3)
endif()

# Vérifier l'existence d'une variable
if(DEFINED MA_VARIABLE)
    message("Variable définie : ${MA_VARIABLE}")
endif()

# Vérifier si variable est vraie
if(ENABLE_FEATURE)
    add_definitions(-DFEATURE_ENABLED)
endif()
```

**Opérateurs courants :**
- `STREQUAL` : Comparaison de chaînes
- `EQUAL` : Comparaison numérique
- `LESS` / `GREATER` : Comparaisons numériques
- `AND` / `OR` / `NOT` : Opérateurs logiques
- `EXISTS` : Vérifie l'existence d'un fichier

---

### `option()`
Crée une option ON/OFF pour l'utilisateur.

```cmake
# Option par défaut OFF
option(ENABLE_TESTS "Compiler les tests" OFF)

# Utilisation
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
```

**Configuration de l'option :**
```bash
# Activer l'option
cmake -DENABLE_TESTS=ON ..

# Désactiver
cmake -DENABLE_TESTS=OFF ..
```

---

## 🔁 Boucles

### `foreach()`
Itère sur une liste.

```cmake
# Liste simple
foreach(fichier main.c utils.c helper.c)
    message("Fichier : ${fichier}")
endforeach()

# Avec variable
set(SOURCES main.c utils.c helper.c)
foreach(src ${SOURCES})
    message("Source : ${src}")
endforeach()

# Range numérique
foreach(i RANGE 5)
    message("Itération ${i}")
endforeach()
```

---

### `while()`
Boucle conditionnelle.

```cmake
set(COUNT 0)
while(COUNT LESS 5)
    message("Count = ${COUNT}")
    math(EXPR COUNT "${COUNT} + 1")
endwhile()
```

---

## 📝 Messages et Affichage

### `message()`
Affiche un message.

```cmake
# Information
message("Configuration du projet...")

# Statut
message(STATUS "Compilateur : ${CMAKE_C_COMPILER}")

# Warning
message(WARNING "Cette fonctionnalité est dépréciée")

# Erreur fatale (arrête la configuration)
message(FATAL_ERROR "Erreur critique !")

# Variables
message("Version du projet : ${PROJECT_VERSION}")
```

**Niveaux de message :**
- *(aucun)* : Message normal
- `STATUS` : Information (préfixe `--`)
- `WARNING` : Avertissement (continue)
- `SEND_ERROR` : Erreur (continue mais échoue à la fin)
- `FATAL_ERROR` : Erreur fatale (arrête immédiatement)

---

## 📊 Variables Importantes de CMake

### Variables système

```cmake
# Répertoires
${CMAKE_SOURCE_DIR}        # Répertoire source racine
${CMAKE_BINARY_DIR}        # Répertoire de build
${CMAKE_CURRENT_SOURCE_DIR}  # Répertoire source actuel
${CMAKE_CURRENT_BINARY_DIR}  # Répertoire build actuel

# Informations système
${CMAKE_SYSTEM_NAME}       # Linux, Windows, Darwin, etc.
${CMAKE_SYSTEM_PROCESSOR}  # x86_64, arm, etc.
${CMAKE_HOST_SYSTEM}       # Système hôte

# Compilateur
${CMAKE_C_COMPILER}        # Chemin du compilateur C
${CMAKE_CXX_COMPILER}      # Chemin du compilateur C++
${CMAKE_C_COMPILER_ID}     # GNU, Clang, MSVC, etc.

# Configuration
${CMAKE_BUILD_TYPE}        # Debug, Release, RelWithDebInfo, MinSizeRel
```

---

### Variables de configuration

```cmake
# Standards
${CMAKE_C_STANDARD}        # Standard C (90, 99, 11, 17, 23)
${CMAKE_CXX_STANDARD}      # Standard C++ (98, 11, 14, 17, 20, 23)

# Flags de compilation
${CMAKE_C_FLAGS}           # Flags C globaux
${CMAKE_C_FLAGS_DEBUG}     # Flags pour Debug
${CMAKE_C_FLAGS_RELEASE}   # Flags pour Release

# Préfixes et suffixes
${CMAKE_STATIC_LIBRARY_PREFIX}  # "lib" sous Linux
${CMAKE_STATIC_LIBRARY_SUFFIX}  # ".a" sous Linux
${CMAKE_SHARED_LIBRARY_PREFIX}  # "lib" sous Linux
${CMAKE_SHARED_LIBRARY_SUFFIX}  # ".so" sous Linux
```

---

## 🎨 Exemple Complet : Projet Structuré

```
mon_projet/
├── CMakeLists.txt          # CMake principal
├── include/
│   └── mon_projet.h
├── src/
│   ├── CMakeLists.txt
│   ├── main.c
│   └── utils.c
├── lib/
│   ├── CMakeLists.txt
│   ├── ma_lib.c
│   └── ma_lib.h
└── tests/
    ├── CMakeLists.txt
    └── test_main.c
```

**`CMakeLists.txt` (racine) :**
```cmake
cmake_minimum_required(VERSION 3.10)
project(MonProjet VERSION 1.0.0 LANGUAGES C)

# Standard C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Option pour les tests
option(BUILD_TESTS "Compiler les tests" ON)

# Sous-répertoires
add_subdirectory(lib)
add_subdirectory(src)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
install(FILES include/mon_projet.h
    DESTINATION include
)
```

**`lib/CMakeLists.txt` :**
```cmake
add_library(ma_lib STATIC
    ma_lib.c
)

target_include_directories(ma_lib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

install(TARGETS ma_lib
    ARCHIVE DESTINATION lib
)
```

**`src/CMakeLists.txt` :**
```cmake
add_executable(mon_programme
    main.c
    utils.c
)

target_link_libraries(mon_programme
    PRIVATE ma_lib
    PRIVATE m  # libm (math)
)

target_compile_options(mon_programme PRIVATE
    -Wall
    -Wextra
    -Werror
)

install(TARGETS mon_programme
    DESTINATION bin
)
```

**`tests/CMakeLists.txt` :**
```cmake
add_executable(test_main test_main.c)
target_link_libraries(test_main PRIVATE ma_lib)

add_test(NAME test_main COMMAND test_main)
```

---

## 🚀 Commandes en Ligne de Commande

### Configuration
```bash
# Configuration basique
cmake ..

# Spécifier le type de build
cmake -DCMAKE_BUILD_TYPE=Release ..

# Spécifier le compilateur
cmake -DCMAKE_C_COMPILER=clang ..

# Options personnalisées
cmake -DBUILD_TESTS=ON ..

# Générateur spécifique
cmake -G Ninja ..
```

---

### Compilation
```bash
# Compiler (méthode moderne)
cmake --build .

# Avec parallélisme
cmake --build . -j 8

# Cible spécifique
cmake --build . --target mon_programme

# Verbose
cmake --build . --verbose
```

---

### Installation
```bash
# Installer
cmake --install .

# Avec préfixe
cmake --install . --prefix /opt/mon_projet

# Composant spécifique
cmake --install . --component runtime
```

---

### Tests
```bash
# Lancer tous les tests
ctest

# Verbose
ctest --verbose

# Tests spécifiques
ctest -R mon_test

# Parallèle
ctest -j 4
```

---

### Nettoyage
```bash
# Nettoyer les fichiers de build
cmake --build . --target clean

# Supprimer complètement le répertoire build
rm -rf build/
```

---

## 📋 Build Types (Types de Build)

CMake offre 4 types de build prédéfinis :

| Type | Description | Flags typiques |
|------|-------------|----------------|
| `Debug` | Débogage | `-g -O0` |
| `Release` | Production optimisée | `-O3 -DNDEBUG` |
| `RelWithDebInfo` | Release + symboles debug | `-O2 -g -DNDEBUG` |
| `MinSizeRel` | Taille minimale | `-Os -DNDEBUG` |

**Utilisation :**
```cmake
# Dans CMakeLists.txt
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# En ligne de commande
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

---

## 🎯 Bonnes Pratiques

### 1. Toujours utiliser out-of-source builds
```bash
# ❌ MAUVAIS
cd mon_projet
cmake .

# ✅ BON
mkdir build && cd build
cmake ..
```

---

### 2. Utiliser des cibles modernes (target_*)
```cmake
# ❌ MAUVAIS (ancien style)
include_directories(include/)
link_libraries(ma_lib)
add_definitions(-DDEBUG)

# ✅ BON (moderne)
target_include_directories(mon_programme PRIVATE include/)
target_link_libraries(mon_programme PRIVATE ma_lib)
target_compile_definitions(mon_programme PRIVATE DEBUG)
```

---

### 3. Spécifier les visibilités (PUBLIC/PRIVATE/INTERFACE)
```cmake
# ✅ BON
target_include_directories(ma_lib
    PUBLIC include/    # Exporté aux utilisateurs
    PRIVATE src/       # Usage interne seulement
)
```

---

### 4. Éviter les variables globales
```cmake
# ❌ MAUVAIS
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# ✅ BON
target_compile_options(mon_programme PRIVATE -Wall)
```

---

### 5. Vérifier les dépendances
```cmake
find_package(Threads REQUIRED)
if(NOT Threads_FOUND)
    message(FATAL_ERROR "Pthread non trouvé !")
endif()
```

---

## ⚠️ Pièges Courants à Éviter

### 1. Utiliser GLOB pour les sources
```cmake
# ❌ ÉVITER (CMake ne détecte pas les nouveaux fichiers)
file(GLOB SOURCES "*.c")

# ✅ PRÉFÉRER (liste explicite)
set(SOURCES
    main.c
    utils.c
    helper.c
)
```

---

### 2. Oublier de spécifier CMAKE_BUILD_TYPE
```bash
# Sans BUILD_TYPE, le code n'est ni optimisé ni débogable !
cmake -DCMAKE_BUILD_TYPE=Release ..
```

---

### 3. Modifier CMAKE_C_FLAGS au lieu de target_compile_options
```cmake
# ❌ MAUVAIS (affecte tout le projet)
set(CMAKE_C_FLAGS "-Wall")

# ✅ BON (cible spécifique)
target_compile_options(mon_programme PRIVATE -Wall)
```

---

## 📚 Ressources Complémentaires

### Documentation officielle
```bash
# Aide sur une commande
cmake --help-command add_executable

# Liste des variables
cmake --help-variable-list

# Modules disponibles
cmake --help-module-list
```

### Sites utiles
- 📖 Documentation : https://cmake.org/documentation/
- 🎓 Tutoriel officiel : https://cmake.org/cmake/help/latest/guide/tutorial/
- 📦 Modern CMake : https://cliutils.gitlab.io/modern-cmake/

---

## 🎓 Récapitulatif des Commandes Essentielles

| Commande | Usage |
|----------|-------|
| `cmake_minimum_required()` | Version minimale de CMake |
| `project()` | Déclarer le projet |
| `add_executable()` | Créer un exécutable |
| `add_library()` | Créer une bibliothèque |
| `target_link_libraries()` | Lier des bibliothèques |
| `target_include_directories()` | Ajouter répertoires d'en-têtes |
| `target_compile_options()` | Options de compilation |
| `target_compile_definitions()` | Définir des macros |
| `find_package()` | Trouver dépendance externe |
| `add_subdirectory()` | Inclure sous-répertoire |
| `install()` | Règles d'installation |
| `option()` | Option utilisateur ON/OFF |
| `message()` | Afficher message |
| `if()`/`endif()` | Condition |
| `foreach()`/`endforeach()` | Boucle |

---

**💡 Conseil final :** Commencez simple avec un projet minimal, puis ajoutez progressivement les fonctionnalités. CMake est puissant mais peut être complexe. Les cibles modernes (`target_*`) sont la clé d'un CMakeLists.txt maintenable !

⏭️ [Flags de sanitizers et Valgrind](/annexes/antiseches/06-flags-sanitizers-valgrind.md)
