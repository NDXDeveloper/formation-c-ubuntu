🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 31.3 CMake et cross-compilation

## Introduction

**CMake** est un système de génération de build moderne qui simplifie considérablement la cross-compilation. Au lieu d'écrire des Makefiles complexes avec des règles spécifiques pour chaque architecture, CMake permet de :

- ✅ Décrire votre projet une seule fois
- ✅ Générer automatiquement les fichiers de build pour n'importe quelle plateforme
- ✅ Gérer facilement la cross-compilation via des fichiers **toolchain**
- ✅ Maintenir un code portable et propre

Dans cette section, nous verrons comment utiliser CMake pour cross-compiler vers ARM, et les concepts s'appliquent à n'importe quelle architecture cible.

## Pourquoi CMake pour la cross-compilation ?

### Comparaison Makefile vs CMake

**Avec Makefile classique :**

```makefile
# Makefile
CROSS_COMPILE ?= arm-linux-gnueabihf-  
CC = $(CROSS_COMPILE)gcc  
CFLAGS = -Wall -O2 -I/usr/arm-linux-gnueabihf/include  
LDFLAGS = -L/usr/arm-linux-gnueabihf/lib  

# Règles complexes à maintenir...
# Difficile à porter sur d'autres systèmes
```

**Avec CMake :**

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)  
project(MonProjet C)  

add_executable(programme main.c)

# CMake gère automatiquement la toolchain !
```

### Avantages de CMake

| Aspect | Sans CMake | Avec CMake |
|--------|------------|------------|
| **Portabilité** | Makefiles spécifiques par plateforme | Un seul CMakeLists.txt |
| **Cross-compilation** | Variables manuelles complexes | Fichier toolchain simple |
| **Dépendances** | Gestion manuelle | `find_package()` automatique |
| **IDE** | Support limité | VS Code, CLion, etc. |
| **Out-of-source builds** | Difficile | Natif |

## Installation de CMake

### Sur Ubuntu

```bash
# Installation via apt
sudo apt-get update  
sudo apt-get install cmake  

# Vérification
cmake --version

# Version minimale recommandée : 3.10+
```

### Vérifier l'installation

```bash
# Afficher la version
cmake --version

# Afficher l'aide
cmake --help

# Lister les générateurs disponibles
cmake --help | grep Generators -A 20
```

## Concepts de base CMake

### Structure d'un projet CMake

```
projet/
├── CMakeLists.txt          # Fichier principal
├── src/
│   ├── main.c
│   └── utils.c
├── include/
│   └── utils.h
├── cmake/
│   └── toolchain-arm.cmake # Fichier toolchain
└── build/                  # Répertoire de build (généré)
```

### Le fichier CMakeLists.txt minimal

```cmake
# Version minimale de CMake requise
cmake_minimum_required(VERSION 3.10)

# Nom du projet et langage
project(MonProjet C)

# Créer un exécutable
add_executable(programme main.c)
```

### Out-of-source builds

CMake recommande de compiler en dehors du répertoire source :

```bash
# Créer un répertoire de build
mkdir build  
cd build  

# Configurer le projet
cmake ..

# Compiler
make

# Le binaire est dans build/
./programme
```

**Avantages :**
- ✅ Code source non pollué par les fichiers générés
- ✅ Facile de nettoyer : `rm -rf build/`
- ✅ Possibilité de plusieurs builds (debug, release, cross-compile)

## Le fichier Toolchain

### Qu'est-ce qu'un fichier toolchain ?

Un **fichier toolchain** (ou toolchain file) est un fichier CMake qui indique :
- Quelle architecture cibler
- Quel compilateur utiliser
- Où trouver les bibliothèques système
- Options de compilation spécifiques

### Structure d'un fichier toolchain

**cmake/toolchain-arm32.cmake**

```cmake
# Système cible
set(CMAKE_SYSTEM_NAME Linux)  
set(CMAKE_SYSTEM_PROCESSOR arm)  

# Compilateurs
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)  
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)  

# Outils de build
set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)

# Configuration de recherche
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)  
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)  
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)  
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)  
```

### Explication des variables

#### CMAKE_SYSTEM_NAME
Définit le système d'exploitation cible :
- `Linux` : Pour Linux
- `Windows` : Pour Windows
- `Generic` : Pour systèmes embarqués sans OS

#### CMAKE_SYSTEM_PROCESSOR
Définit l'architecture cible :
- `arm` : ARM 32 bits
- `aarch64` : ARM 64 bits
- `mips` : MIPS
- etc.

#### CMAKE_C_COMPILER / CMAKE_CXX_COMPILER
Spécifie les compilateurs à utiliser :
```cmake
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)  
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)  
```

#### CMAKE_FIND_ROOT_PATH
Indique où chercher les bibliothèques et headers de la cible :
```cmake
set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)
```

#### CMAKE_FIND_ROOT_PATH_MODE_*
Contrôle où CMake cherche les ressources :
- `NEVER` : Ne jamais chercher dans CMAKE_FIND_ROOT_PATH
- `ONLY` : Chercher uniquement dans CMAKE_FIND_ROOT_PATH
- `BOTH` : Chercher partout

```cmake
# Les programmes (comme les compilateurs) : sur le système hôte
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Les bibliothèques : uniquement pour la cible
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)

# Les headers : uniquement pour la cible
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

## Exemples de fichiers toolchain

### Toolchain ARM 32 bits (Raspberry Pi 2/3)

**cmake/toolchain-arm32.cmake**

```cmake
set(CMAKE_SYSTEM_NAME Linux)  
set(CMAKE_SYSTEM_PROCESSOR arm)  

# Toolchain ARM 32 bits
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)  
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)  

# Sysroot
set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)

# Configuration de recherche
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)  
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)  
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)  

# Options de compilation spécifiques ARM
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=hard")  
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=hard")  
```

### Toolchain ARM 64 bits (Raspberry Pi 4)

**cmake/toolchain-arm64.cmake**

```cmake
set(CMAKE_SYSTEM_NAME Linux)  
set(CMAKE_SYSTEM_PROCESSOR aarch64)  

# Toolchain ARM 64 bits
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)  
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)  

# Sysroot
set(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)

# Configuration de recherche
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)  
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)  
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)  

# Options spécifiques ARM64
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a")  
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a")  
```

### Toolchain avec sysroot personnalisé

Si vous avez un sysroot personnalisé (par exemple, copié depuis votre Raspberry Pi) :

**cmake/toolchain-rpi-custom.cmake**

```cmake
set(CMAKE_SYSTEM_NAME Linux)  
set(CMAKE_SYSTEM_PROCESSOR aarch64)  

# Chemin vers votre sysroot personnalisé
set(RPI_SYSROOT /home/user/raspberry-sysroot)

# Compilateurs
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)  
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)  

# Utiliser le sysroot personnalisé
set(CMAKE_SYSROOT ${RPI_SYSROOT})  
set(CMAKE_FIND_ROOT_PATH ${RPI_SYSROOT})  

# Configuration de recherche
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)  
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)  
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)  

# Flags du compilateur avec sysroot
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=${RPI_SYSROOT}")  
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=${RPI_SYSROOT}")  
```

## Utilisation du fichier toolchain

### Méthode 1 : Option en ligne de commande

```bash
# Créer le répertoire de build
mkdir build-arm  
cd build-arm  

# Configurer avec le fichier toolchain
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-arm64.cmake ..

# Compiler
make

# Le binaire est cross-compilé pour ARM !
file programme
```

### Méthode 2 : Variable d'environnement

```bash
# Définir la variable
export CMAKE_TOOLCHAIN_FILE=/path/to/toolchain-arm64.cmake

# Configurer normalement
mkdir build-arm && cd build-arm  
cmake ..  
make  
```

### Méthode 3 : Script de build

**build-arm.sh**

```bash
#!/bin/bash

# Configuration
BUILD_DIR="build-arm64"  
TOOLCHAIN_FILE="cmake/toolchain-arm64.cmake"  

# Nettoyer si nécessaire
rm -rf $BUILD_DIR

# Créer et entrer dans le répertoire de build
mkdir -p $BUILD_DIR  
cd $BUILD_DIR  

# Configurer avec CMake
cmake -DCMAKE_TOOLCHAIN_FILE=../$TOOLCHAIN_FILE \
      -DCMAKE_BUILD_TYPE=Release \
      ..

# Compiler
make -j$(nproc)

# Afficher le résultat
echo "=== Binaire généré ==="  
file programme  
ls -lh programme  
```

**Utilisation :**
```bash
chmod +x build-arm.sh
./build-arm.sh
```

## Projet complet exemple

### Structure du projet

```
projet/
├── CMakeLists.txt
├── cmake/
│   ├── toolchain-arm32.cmake
│   └── toolchain-arm64.cmake
├── src/
│   ├── main.c
│   └── utils.c
├── include/
│   └── utils.h
├── build-arm.sh
└── README.md
```

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.10)  
project(MonProjet C)  

# Version du projet
set(PROJECT_VERSION "1.0.0")

# Standard C
set(CMAKE_C_STANDARD 17)  
set(CMAKE_C_STANDARD_REQUIRED ON)  

# Options de compilation
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

# Répertoires d'inclusion
include_directories(include)

# Sources
set(SOURCES
    src/main.c
    src/utils.c
)

# Créer l'exécutable
add_executable(programme ${SOURCES})

# Afficher des informations de configuration
message(STATUS "=== Configuration ===")  
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")  
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")  
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")  
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")  
message(STATUS "Project Version: ${PROJECT_VERSION}")  
```

### src/main.c

```c
#include <stdio.h>
#include "utils.h"

int main(void) {
    printf("=== Programme cross-compilé ===\n");
    afficher_info_systeme();
    return 0;
}
```

### include/utils.h

```c
#ifndef UTILS_H
#define UTILS_H

void afficher_info_systeme(void);

#endif
```

### src/utils.c

```c
#include <stdio.h>
#include "utils.h"

void afficher_info_systeme(void) {
    printf("Architecture: ");

#if defined(__aarch64__) || defined(__arm64__)
    printf("ARM 64 bits (aarch64)\n");
#elif defined(__arm__)
    printf("ARM 32 bits\n");
#elif defined(__x86_64__) || defined(__amd64__)
    printf("x86_64\n");
#elif defined(__i386__)
    printf("x86 32 bits\n");
#else
    printf("Inconnue\n");
#endif

    printf("Compilé avec: %s\n", __VERSION__);
}
```

### Compilation pour différentes cibles

```bash
# Compilation native (x86_64)
mkdir build-native && cd build-native  
cmake -DCMAKE_BUILD_TYPE=Release ..  
make  
cd ..  

# Compilation pour ARM 32 bits
mkdir build-arm32 && cd build-arm32  
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-arm32.cmake \  
      -DCMAKE_BUILD_TYPE=Release ..
make  
cd ..  

# Compilation pour ARM 64 bits
mkdir build-arm64 && cd build-arm64  
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-arm64.cmake \  
      -DCMAKE_BUILD_TYPE=Release ..
make  
cd ..  

# Vérifier les binaires
file build-native/programme  
file build-arm32/programme  
file build-arm64/programme  
```

## Gestion des bibliothèques externes

### Avec find_package()

CMake peut automatiquement trouver les bibliothèques, même en cross-compilation :

**CMakeLists.txt**

```cmake
cmake_minimum_required(VERSION 3.10)  
project(ProjetAvecLibs C)  

# Chercher la bibliothèque CURL
find_package(CURL REQUIRED)

# Afficher des informations
if(CURL_FOUND)
    message(STATUS "CURL trouvé: ${CURL_LIBRARIES}")
    message(STATUS "CURL include: ${CURL_INCLUDE_DIRS}")
endif()

# Sources
add_executable(programme main.c)

# Lier avec CURL
target_link_libraries(programme CURL::libcurl)  
target_include_directories(programme PRIVATE ${CURL_INCLUDE_DIRS})  
```

### Avec pkg-config

Pour les bibliothèques qui supportent pkg-config :

```cmake
cmake_minimum_required(VERSION 3.10)  
project(ProjetPkgConfig C)  

# Utiliser pkg-config
find_package(PkgConfig REQUIRED)

# Chercher une bibliothèque via pkg-config
pkg_check_modules(OPENSSL REQUIRED openssl)

# Créer l'exécutable
add_executable(programme main.c)

# Lier avec OpenSSL
target_link_libraries(programme ${OPENSSL_LIBRARIES})  
target_include_directories(programme PRIVATE ${OPENSSL_INCLUDE_DIRS})  
target_compile_options(programme PRIVATE ${OPENSSL_CFLAGS_OTHER})  
```

### Installation manuelle de dépendances ARM

Si CMake ne trouve pas les bibliothèques :

```bash
# Activer multiarch
sudo dpkg --add-architecture armhf

# Installer les dépendances pour ARM
sudo apt-get update  
sudo apt-get install libcurl4-openssl-dev:armhf  

# CMake devrait maintenant les trouver
```

## Build types et optimisation

### Les build types standards

CMake définit plusieurs build types :

| Build Type | Optimisation | Debug Info | Usage |
|------------|--------------|------------|-------|
| **Debug** | `-O0` | `-g` | Développement, debug |
| **Release** | `-O3` | Aucune | Production |
| **RelWithDebInfo** | `-O2` | `-g` | Production avec debug |
| **MinSizeRel** | `-Os` | Aucune | Systèmes contraints |

### Spécifier le build type

```bash
# Build Debug
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build Release
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build optimisé pour la taille (embarqué)
cmake -DCMAKE_BUILD_TYPE=MinSizeRel ..
```

### Personnaliser les flags de compilation

**CMakeLists.txt**

```cmake
# Flags de base
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

# Flags spécifiques Debug
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g3 -DDEBUG")

# Flags spécifiques Release
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG")

# Flags pour systèmes embarqués
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mthumb")
endif()
```

## Variables CMake utiles

### Variables d'information système

```cmake
# Afficher des informations sur le système
message(STATUS "System Name: ${CMAKE_SYSTEM_NAME}")  
message(STATUS "System Processor: ${CMAKE_SYSTEM_PROCESSOR}")  
message(STATUS "System Version: ${CMAKE_SYSTEM_VERSION}")  

# Chemin du compilateur
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")  
message(STATUS "C Compiler ID: ${CMAKE_C_COMPILER_ID}")  
message(STATUS "C Compiler Version: ${CMAKE_C_COMPILER_VERSION}")  

# Build type
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

# Répertoires
message(STATUS "Source Dir: ${CMAKE_SOURCE_DIR}")  
message(STATUS "Binary Dir: ${CMAKE_BINARY_DIR}")  
```

### Variables de contrôle

```cmake
# Contrôler la verbosité
set(CMAKE_VERBOSE_MAKEFILE ON)

# Position independent code (pour bibliothèques)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Exporter les commandes de compilation (pour clang-tidy, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

## Tests et validation

### Ajouter des tests

**CMakeLists.txt**

```cmake
cmake_minimum_required(VERSION 3.10)  
project(ProjetAvecTests C)  

# Activer les tests
enable_testing()

# Exécutable principal
add_executable(programme main.c utils.c)

# Tests
add_executable(test_utils test_utils.c utils.c)  
add_test(NAME TestUtils COMMAND test_utils)  

# Test qui doit échouer (pour validation)
add_test(NAME TestShouldPass COMMAND programme --test)  
set_tests_properties(TestShouldPass PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")  
```

**Exécution des tests :**

```bash
# Compiler
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-arm64.cmake ..  
make  

# Attention : les tests ne peuvent pas s'exécuter en cross-compilation !
# Il faut soit :
# 1. Tester sur la cible réelle
# 2. Utiliser QEMU

# Avec QEMU
ctest --verbose
```

## Intégration avec QEMU

Pour tester automatiquement les binaires cross-compilés :

**CMakeLists.txt**

```cmake
# Détecter si on est en cross-compilation
if(CMAKE_CROSSCOMPILING)
    message(STATUS "Cross-compilation détectée")

    # Configurer QEMU pour les tests
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
        set(QEMU_ARM qemu-arm)
        set(QEMU_ARM_SYSROOT /usr/arm-linux-gnueabihf)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
        set(QEMU_ARM qemu-aarch64)
        set(QEMU_ARM_SYSROOT /usr/aarch64-linux-gnu)
    endif()

    # Wrapper pour les tests
    set(CMAKE_CROSSCOMPILING_EMULATOR
        "${QEMU_ARM};-L;${QEMU_ARM_SYSROOT}")
endif()

enable_testing()  
add_test(NAME MonTest COMMAND programme)  
# Le test sera automatiquement exécuté via QEMU
```

## Bonnes pratiques CMake

### 1. Séparer configuration et code

```
projet/
├── CMakeLists.txt        # Configuration du projet
├── cmake/
│   ├── toolchain-*.cmake # Toolchains
│   └── modules/          # Modules CMake personnalisés
└── src/
```

### 2. Utiliser des variables pour les chemins

```cmake
# Définir des variables
set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)  
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)  

# Utiliser les variables
include_directories(${PROJECT_INCLUDE_DIR})
```

### 3. Créer des options configurables

```cmake
# Options configurables
option(BUILD_TESTS "Build tests" ON)  
option(ENABLE_OPTIMIZATIONS "Enable ARM optimizations" ON)  

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(ENABLE_OPTIMIZATIONS AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon")
endif()
```

**Utilisation :**
```bash
cmake -DBUILD_TESTS=OFF -DENABLE_OPTIMIZATIONS=ON ..
```

### 4. Utiliser add_subdirectory() pour les gros projets

**CMakeLists.txt (racine)**
```cmake
cmake_minimum_required(VERSION 3.10)  
project(GrosProjet C)  

add_subdirectory(src)  
add_subdirectory(tests)  
add_subdirectory(tools)  
```

**src/CMakeLists.txt**
```cmake
add_executable(programme main.c utils.c)
```

### 5. Documenter le processus de build

**README.md**
```markdown
## Cross-compilation pour ARM

### Prérequis
```bash
sudo apt-get install gcc-aarch64-linux-gnu cmake
```

### Build
```bash
./build-arm.sh
```

### Build manuel
```bash
mkdir build-arm64  
cd build-arm64  
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-arm64.cmake ..  
make  
```
```

## Debugging de la configuration CMake

### Afficher des variables

```cmake
# Afficher une variable
message(STATUS "Compilateur: ${CMAKE_C_COMPILER}")

# Afficher toutes les variables
get_cmake_property(_variableNames VARIABLES)  
foreach(_variableName ${_variableNames})  
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()
```

### Mode verbeux

```bash
# Configuration verbeuse
cmake --trace ..

# Build verbeux
make VERBOSE=1

# Ou
cmake --build . --verbose
```

### Exporter les commandes de compilation

```cmake
# Dans CMakeLists.txt
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

Génère `compile_commands.json` utilisable par les IDE et outils d'analyse.

## Exemple complet : Script automatisé

**build-all.sh** - Script pour compiler toutes les architectures

```bash
#!/bin/bash

set -e  # Arrêter en cas d'erreur

# Couleurs pour l'affichage
RED='\033[0;31m'  
GREEN='\033[0;32m'  
BLUE='\033[0;34m'  
NC='\033[0m' # No Color  

# Configuration
PROJECT_NAME="MonProjet"  
BUILD_TYPES=("Debug" "Release")  
ARCHITECTURES=("native" "arm32" "arm64")  

# Fonction de build
build() {
    local arch=$1
    local build_type=$2
    local build_dir="build-${arch}-${build_type}"

    echo -e "${BLUE}=== Building ${arch} (${build_type}) ===${NC}"

    # Créer le répertoire
    rm -rf "$build_dir"
    mkdir -p "$build_dir"
    cd "$build_dir"

    # Configurer
    if [ "$arch" = "native" ]; then
        cmake -DCMAKE_BUILD_TYPE="$build_type" ..
    else
        cmake -DCMAKE_TOOLCHAIN_FILE="../cmake/toolchain-${arch}.cmake" \
              -DCMAKE_BUILD_TYPE="$build_type" \
              ..
    fi

    # Compiler
    make -j$(nproc)

    # Vérifier le binaire
    echo -e "${GREEN}Binaire généré:${NC}"
    file "$PROJECT_NAME"
    ls -lh "$PROJECT_NAME"

    cd ..
    echo ""
}

# Nettoyer tous les builds
echo -e "${BLUE}=== Nettoyage ===${NC}"  
rm -rf build-*  

# Compiler toutes les combinaisons
for arch in "${ARCHITECTURES[@]}"; do
    for build_type in "${BUILD_TYPES[@]}"; do
        build "$arch" "$build_type"
    done
done

echo -e "${GREEN}=== Build terminé avec succès ! ===${NC}"  
echo "Binaires disponibles dans :"  
ls -d build-*  
```

**Utilisation :**
```bash
chmod +x build-all.sh
./build-all.sh
```

## Intégration CI/CD

### GitHub Actions

**.github/workflows/build-arm.yml**

```yaml
name: Cross-compilation ARM

on: [push, pull_request]

jobs:
  build-arm:
    runs-on: ubuntu-latest

    strategy:
      matrix:
        arch: [arm32, arm64]
        build_type: [Debug, Release]

    steps:
    - uses: actions/checkout@v4

    - name: Install ARM toolchains
      run: |
        sudo apt-get update
        sudo apt-get install -y \
          gcc-arm-linux-gnueabihf \
          gcc-aarch64-linux-gnu \
          qemu-user-static

    - name: Configure CMake
      run: |
        mkdir build
        cd build
        cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-${{ matrix.arch }}.cmake \
              -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} \
              ..

    - name: Build
      run: cmake --build build -j$(nproc)

    - name: Test with QEMU
      run: |
        cd build
        if [ "${{ matrix.arch }}" = "arm32" ]; then
          qemu-arm -L /usr/arm-linux-gnueabihf ./programme
        else
          qemu-aarch64 -L /usr/aarch64-linux-gnu ./programme
        fi

    - name: Upload artifacts
      uses: actions/upload-artifact@v4
      with:
        name: programme-${{ matrix.arch }}-${{ matrix.build_type }}
        path: build/programme
```

## Résumé des commandes

### Configuration et build

```bash
# Configuration avec toolchain
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-arm64.cmake \
      -DCMAKE_BUILD_TYPE=Release \
      ..

# Build
cmake --build . -j$(nproc)

# Build verbeux
cmake --build . --verbose

# Clean
cmake --build . --target clean

# Reconfigurer
cmake .
```

### Gestion des builds multiples

```bash
# Build native
mkdir build-native && cd build-native  
cmake .. && make  
cd ..  

# Build ARM
mkdir build-arm64 && cd build-arm64  
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-arm64.cmake .. && make  
cd ..  
```

## Avantages de CMake en cross-compilation

| Aspect | Avantage |
|--------|----------|
| **Portabilité** | Un seul CMakeLists.txt pour toutes les plateformes |
| **Maintenabilité** | Configuration centralisée et claire |
| **Reproductibilité** | Fichiers toolchain versionnés |
| **IDE** | Support natif dans VS Code, CLion, Qt Creator |
| **CI/CD** | Intégration facile dans les pipelines |
| **Dépendances** | Gestion automatique avec find_package() |
| **Out-of-source** | Builds propres et parallèles |

## Conclusion

CMake simplifie considérablement la cross-compilation en :

1. **Centralisant la configuration** dans des fichiers toolchain réutilisables
2. **Automatisant** la détection des bibliothèques et dépendances
3. **Permettant** des builds parallèles pour plusieurs architectures
4. **S'intégrant** facilement dans les workflows DevOps modernes

**Points clés à retenir :**

- ✅ Un fichier toolchain décrit l'architecture cible
- ✅ `CMAKE_SYSTEM_NAME` et `CMAKE_SYSTEM_PROCESSOR` définissent la cible
- ✅ `CMAKE_FIND_ROOT_PATH` pointe vers le sysroot
- ✅ Out-of-source builds permettent plusieurs configurations parallèles
- ✅ CMake peut tester avec QEMU automatiquement
- ✅ Intégration native dans les IDE et CI/CD

Dans la section suivante, nous verrons comment utiliser QEMU pour tester efficacement les binaires cross-compilés avant le déploiement.

---


⏭️ [QEMU pour tester les binaires](/31-cross-compilation-deploiement/04-qemu-tests.md)
