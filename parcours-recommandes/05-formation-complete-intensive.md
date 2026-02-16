🔝 Retour au [Sommaire](/SOMMAIRE.md)

# Parcours Recommandé : Formation Complète (Intensive)

## 🎯 Profil Cible

Vous êtes soit en reconversion professionnelle, soit en formation initiale intensive (bootcamp, école d'ingénieurs), ou vous avez du temps à consacrer pleinement à l'apprentissage. Vous voulez maîtriser **tous les aspects** de la programmation C et du développement système Linux en un minimum de temps, avec un objectif professionnel clair : devenir ingénieur système, DevOps senior, ou développeur C expert.

**Prérequis :**
- Notions de programmation (Python, Java, ou autre langage) OU forte motivation
- Disponibilité : 30-40h/semaine minimum
- Ordinateur sous Ubuntu (ou Linux)
- Connaissance de base de la ligne de commande (cd, ls, mkdir...)
- Capacité à apprendre rapidement et de manière autonome

**Niveau de départ :** Débutant à Intermédiaire en C, motivé pour apprendre intensivement

---

## 🚀 Objectifs de la Formation Intensive

À l'issue de cette formation de **16-20 semaines à temps plein**, vous serez capable de :

### Compétences Techniques

✅ **Maîtrise complète du C**
- Syntaxe, types, structures de contrôle, fonctions
- Gestion avancée de la mémoire (pointeurs, allocation dynamique)
- Structures de données (listes, arbres, tables de hachage)
- Programmation système (fichiers, processus, threads, sockets)

✅ **Expertise Système Linux**
- Architecture Linux et kernel
- Programmation bas niveau (appels système, I/O avancé)
- eBPF pour observabilité et tracing
- Performance et optimisation

✅ **Outillage DevOps Moderne**
- Build systems (CMake, Make)
- Débogage professionnel (GDB, Valgrind, Sanitizers)
- CI/CD pour projets C (GitHub Actions, GitLab CI)
- Conteneurisation et DevContainers
- Cross-compilation et déploiement

✅ **Pratiques Professionnelles**
- Tests automatisés et qualité de code
- Analyse statique et code coverage
- Sécurité et code défensif
- Documentation et maintenance
- Contribution open-source

---

## 📊 Vue d'Ensemble du Parcours

### Structure de la Formation (16-20 semaines)

**Phase 1 (Semaines 1-2) : Fondations Express**
- Installation et configuration complète
- Bases du C et compilation
- Premier contact avec les outils

**Phase 2 (Semaines 3-5) : Maîtrise du Langage**
- Types, opérateurs, structures de contrôle
- Fonctions et organisation du code
- Pointeurs et gestion mémoire de base

**Phase 3 (Semaines 6-8) : Gestion Mémoire Avancée**
- Pointeurs avancés
- Allocation dynamique
- Structures de données dynamiques

**Phase 4 (Semaines 9-11) : Outillage et Automatisation**
- Build systems (Make, CMake)
- Débogage professionnel
- CI/CD et DevContainers

**Phase 5 (Semaines 12-15) : Programmation Système**
- Fichiers et I/O
- Processus, threads, signaux
- Networking et IPC
- eBPF et observabilité

**Phase 6 (Semaines 16-18) : Expertise et Optimisation**
- Sécurité et code défensif
- Performance et optimisation
- Cross-compilation
- Distributions embarquées (optionnel)

**Phase 7 (Semaines 19-20) : Projet Final et Portfolio**
- Projet de synthèse complet
- Documentation professionnelle
- Préparation portfolio

---

## 📚 Parcours Détaillé par Semaine

### 🔥 Semaines 1-2 : Fondations Express (80h)

**Objectif :** Environnement opérationnel, bases du C, premiers programmes

#### Module 1 : L'Écosystème C sous Linux *(Jour 1-2)*

**Lecture et compréhension (8h) :**
- 1.1 Pourquoi le C en 2025 ?
- 1.2 Relation C et UNIX/Linux
- 1.3 Normes du langage (focus C17)
- 1.4 C vs C++ vs Rust

**Configuration environnement (8h) :**
- 2.1 Installation toolchain complète
- 2.2 Versions GCC
- 2.3 VS Code avec extensions C/C++
- 2.5 DevContainers (configuration de base)

**Actions concrètes :**
```bash
# Installation complète
sudo apt update && sudo apt upgrade  
sudo apt install build-essential git cmake gdb valgrind \  
                 clang-format clang-tidy cppcheck lcov

# Vérification
gcc --version  
git --version  
cmake --version  
```

---

#### Module 1 - Chapitre 3 : Chaîne de Compilation *(Jour 3)*

**Compréhension approfondie (8h) :**
- 3.1 Les 4 étapes détaillées
- 3.2 Visualisation du pipeline
- 3.3 Options GCC essentielles
- 3.4 Démystification "Hello World"
- 3.5 Inspection fichiers intermédiaires

**Pratique :**
```bash
# Voir toutes les étapes
gcc -E hello.c -o hello.i          # Préprocesseur  
gcc -S hello.i -o hello.s          # Compilation  
gcc -c hello.s -o hello.o          # Assemblage  
gcc hello.o -o hello               # Linking  

# Compilation avec options
gcc -Wall -Wextra -Werror -std=c17 -O2 -g hello.c -o hello
```

---

#### Module 2 : Fondamentaux du Langage *(Jour 4-10)*

**Chapitre 4 : Types et Mémoire (1 jour)**
- 4.1 Types primitifs et taille
- 4.2 Signed vs Unsigned
- 4.3 Constantes et littéraux
- 4.4 Conversion de types
- 4.5 Endianness

**Mini-projet :** Programme affichant les tailles et limites de tous les types

**Chapitre 5 : Opérateurs et Contrôle (1.5 jours)**
- 5.1 Opérateurs arithmétiques
- 5.2 Opérateurs logiques
- 5.3 Opérateurs bit-à-bit ⭐
- 5.4 Conditions
- 5.5 Boucles
- 5.7 clang-format (configuration)

**Mini-projet :** Calculatrice avec opérations bit-à-bit

**Chapitre 6 : Fonctions (1.5 jours)**
- 6.1 Déclaration et définition
- 6.2 Stack frames
- 6.3 Passage par valeur
- 6.4 Valeurs de retour
- 6.5 main() et ses paramètres

**Mini-projet :** Bibliothèque de fonctions mathématiques

**Premier Projet : Convertisseur Multi-Usage (2 jours)**
- Températures (C, F, K)
- Bases numériques (bin, dec, hex)
- Unités (km/miles, kg/lbs)
- Interface CLI propre
- Gestion d'erreurs
- Makefile

---

### 🔥 Semaines 3-5 : Maîtrise du Langage (120h)

#### Module 3 : Gestion de la Mémoire *(Semaine 3)*

**Chapitre 7 : Pointeurs (3 jours)** 🔥
- 7.1 Concept d'adresse mémoire
- 7.2 Déclaration, déréférencement
- 7.3 Arithmétique des pointeurs
- 7.4 Pointeurs void*
- 7.5 NULL et dangling pointers
- 7.6 Pointeurs const

**Exercices intensifs :**
- Manipulation de pointeurs
- Debugging avec GDB
- Détection d'erreurs avec ASan

**Chapitre 8 : Tableaux et Chaînes (2 jours)**
- 8.1 Relation tableaux/pointeurs
- 8.2 Tableaux multidimensionnels
- 8.3 Chaînes en C
- 8.4 Manipulation de chaînes
- 8.5 Buffer overflows ⭐
- 8.6 Fonctions sécurisées

**Mini-projet :** Bibliothèque de manipulation de chaînes sécurisée

---

#### Module 3 : Allocation Dynamique *(Semaine 4, jours 1-3)*

**Chapitre 9 : (3 jours)** 🔥
- 9.1 Stack vs Heap
- 9.2 Durée de vie et performance
- 9.3 malloc, calloc, realloc, free
- 9.4 Gestion d'erreurs
- 9.5 Fuites mémoire
- 9.6 Double free
- 9.7 Stratégies d'allocation

**Pratique intensive :**
- Valgrind sur tous les programmes
- Détection de fuites systématique
- Patterns d'allocation/libération

---

#### Module 4 : Structures de Données *(Semaine 4-5)*

**Chapitre 10 : Types Utilisateur (2 jours)**
- 10.1 struct
- 10.2 Alignement mémoire
- 10.4 union
- 10.5 enum
- 10.6 typedef

**Chapitre 11 : Structures Dynamiques (2 jours)** ⭐
- 11.1 Listes chaînées simples
- 11.2 Listes doublement chaînées
- 11.3 Piles et files
- 11.4 Tables de hachage
- 11.5 Arbres binaires (introduction)

**Chapitre 12 : Organisation du Code (1 jour)**
- 12.1 Fichiers d'en-tête
- 12.2 Portée des variables
- 12.3 Compilation séparée
- 12.4 Organisation projet
- 12.5 Conventions de nommage

**Projet Semaine 5 : Base de Données Simple en Mémoire (2 jours)**
- Structures pour représenter les données
- Table de hachage pour l'indexation
- CRUD complet (Create, Read, Update, Delete)
- Sauvegarde/chargement depuis fichier
- Organisation en modules
- Makefile complet

---

### 🔥 Semaines 6-8 : Structures de Données Avancées (120h)

#### Approfondissement Module 4 et 11 *(Semaine 6)*

**Structures de données complexes (5 jours) :**
- Implémentation complète de liste chaînée (avec tous les cas limites)
- Table de hachage avec gestion des collisions
- Arbre binaire de recherche (BST)
- File de priorité (heap)
- Graphes (représentation et parcours de base)

**Projet : Gestionnaire de Tâches Avancé (2 jours)**
- Structures complexes imbriquées
- Plusieurs structures de données combinées
- Algorithmes de tri et recherche
- Persistance sur disque
- Tests unitaires de base

---

#### Module 5 - Chapitre 14 : Les Bibliothèques *(Semaine 7)*

**Chapitre 14 complet (5 jours) :**
- 14.1 Bibliothèques statiques (.a)
- 14.2 Bibliothèques dynamiques (.so)
- 14.3 Différences statiques vs dynamiques
- 14.4 LD_LIBRARY_PATH
- 14.5 ldd et ldconfig
- 14.6 Création d'API propre
- 14.7 Symbol visibility

**Pratique :**
```bash
# Créer bibliothèque statique
gcc -c utils.c -o utils.o  
ar rcs libutils.a utils.o  

# Créer bibliothèque dynamique
gcc -fPIC -c utils.c -o utils.o  
gcc -shared utils.o -o libutils.so  

# Utiliser
gcc main.c -L. -lutils -o programme
```

**Projet : Bibliothèque Réutilisable (2 jours)**
- API complète pour structures de données
- Version statique et dynamique
- Documentation (headers bien commentés)
- Exemples d'utilisation
- Tests

---

#### Module 7 - Chapitre 22 : Pointeurs Avancés *(Semaine 8)*

**Chapitre 22 complet (3 jours) :**
- 22.1 Pointeurs de fonctions (callbacks)
- 22.2 Tableaux de pointeurs de fonctions
- 22.3 Pointeurs multi-niveaux
- 22.4 Pointeurs opaques
- 22.5 Fonctions variadiques
- 22.6 va_list

**Chapitre 23 : Macros et Préprocesseur (2 jours)**
- 23.1 Macros paramétrées
- 23.2 Stringification
- 23.3 Compilation conditionnelle
- 23.5 Dangers des macros
- 23.7 X-Macros

**Projet : Framework de Plugins (2 jours)**
- Système de plugins avec pointeurs de fonctions
- Chargement dynamique
- Interface générique
- Callbacks et événements

---

### 🔥 Semaines 9-11 : Outillage DevOps (120h)

#### Module 5 - Chapitre 13 : Build Systems *(Semaine 9)* 🔥

**CMake complet (3 jours) :**
- 13.1.1 Pourquoi CMake ?
- 13.1.2 Installation et premiers pas
- 13.1.3 Structure CMakeLists.txt
- 13.1.4 Targets, properties, generators
- 13.1.5 Gestion des dépendances
- 13.1.6 Build types
- 13.1.7 Out-of-source builds
- 13.1.8 Intégration IDE

**Exemple CMakeLists.txt complet :**
```cmake
cmake_minimum_required(VERSION 3.15)  
project(MonProjet VERSION 1.0.0 LANGUAGES C)  

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)  

# Options
option(BUILD_TESTS "Build tests" ON)  
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)  

# Flags de compilation
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

if(ENABLE_ASAN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
endif()

# Bibliothèque
add_library(mylib SHARED src/mylib.c)  
target_include_directories(mylib PUBLIC include)  

# Exécutable
add_executable(app src/main.c)  
target_link_libraries(app mylib)  

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
```

**Make avancé (2 jours) :**
- 13.2 Makefiles complexes
- Variables automatiques
- Pattern rules
- Makefiles récursifs

---

#### Module 5 - Chapitre 15 : Débogage et Qualité *(Semaine 10)* 🔥🔥🔥

**15.1 Sanitizers (1 jour) :**
- AddressSanitizer (ASan)
- UndefinedBehaviorSanitizer (UBSan)
- ThreadSanitizer (TSan)
- LeakSanitizer (LSan)
- Intégration dans le workflow

**15.2-15.3 GDB (2 jours) :**
- Utilisation de base (breakpoints, stepping)
- Backtrace et analyse
- Inspection de variables
- GDB TUI mode
- Core dumps
- Debugging distant
- Scripts GDB
- rr (Time Travel Debugging)

**15.4-15.5 Valgrind (1 jour) :**
- Memcheck
- Helgrind (race conditions)
- Cachegrind (cache profiling)
- Callgrind (call graph)
- Massif (heap profiling)

**15.6 Analyse Statique (1 jour) :**
- cppcheck
- clang-tidy
- Configuration et intégration

**15.7 Code Coverage (1 jour) :**
- gcov
- lcov
- Génération de rapports HTML
- Intégration CI

**Pratique intensive :**
- Déboguer des programmes intentionnellement buggés
- Trouver toutes les fuites mémoire
- Optimiser avec cachegrind

---

#### Module 8 - Chapitre 29 : Tests *(Semaine 11, jours 1-2)*

**29.1-29.7 Tests complets :**
- Philosophie du testing
- Frameworks (Unity, CMocka, Check)
- Écriture de tests
- Mocking et stubbing
- Mesure de couverture
- TDD (introduction)

**Exemple avec Unity :**
```c
#include "unity.h"
#include "mylib.h"

void setUp(void) {
    // Avant chaque test
}

void tearDown(void) {
    // Après chaque test
}

void test_addition(void) {
    TEST_ASSERT_EQUAL(5, add(2, 3));
}

void test_allocation(void) {
    int *ptr = allocate_array(10);
    TEST_ASSERT_NOT_NULL(ptr);
    free(ptr);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_addition);
    RUN_TEST(test_allocation);
    return UNITY_END();
}
```

---

#### Module 8 - Chapitre 30 : CI/CD *(Semaine 11, jours 3-5)* 🔥

**30.1 GitHub Actions complet :**
- Structure de workflow
- Matrix builds (Ubuntu 20.04, 22.04, 24.04)
- Build avec CMake
- Tests unitaires
- Sanitizers
- Valgrind
- Code coverage + Codecov
- Static analysis
- Publication d'artifacts

**Workflow complet `.github/workflows/ci.yml` :**
```yaml
name: CI

on: [push, pull_request]

jobs:
  build-and-test:
    strategy:
      matrix:
        os: [ubuntu-22.04, ubuntu-24.04]
        compiler: [gcc, clang]
    runs-on: ${{ matrix.os }}

    steps:
    - uses: actions/checkout@v4

    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake valgrind lcov cppcheck

    - name: Configure
      run: |
        mkdir build && cd build
        cmake -DCMAKE_BUILD_TYPE=Debug \
              -DENABLE_ASAN=ON \
              -DBUILD_TESTS=ON ..

    - name: Build
      run: cmake --build build

    - name: Run tests
      run: cd build && ctest --output-on-failure

    - name: Run Valgrind
      run: valgrind --leak-check=full --error-exitcode=1 ./build/app

    - name: Code coverage
      run: |
        cd build
        make coverage
        bash <(curl -s https://codecov.io/bash)

    - name: Static analysis
      run: cppcheck --enable=all --error-exitcode=1 src/
```

**30.2 GitLab CI (1 jour)**  
**30.3 Jenkins (introduction)**  

---

### 🔥 Semaines 12-15 : Programmation Système (160h)

#### Module 6 - Chapitre 16 : Fichiers et I/O *(Semaine 12)* 🔥

**Tous les chapitres 16 (5 jours) :**
- 16.1 Descripteurs de fichiers
- 16.2 Appels système (open, read, write, close, stat)
- 16.3 Système vs bibliothèque
- 16.4 Permissions et modes
- 16.5 I/O bufferisé vs non bufferisé
- 16.6 dup, dup2, redirection
- 16.7 I/O multiplexing (select, poll, epoll)
- 16.8 I/O asynchrone (AIO)

**Projets :**
- Réimplémentation de cat, cp, wc
- Programme avec epoll (serveur de fichiers)
- Moniteur de fichiers (inotify)

---

#### Module 6 - Chapitre 17 : Processus et Signaux *(Semaine 13)* 🔥🔥🔥

**Tout le chapitre 17 (5 jours) :**
- 17.1 fork, exec, wait
- 17.2 PIDs et hiérarchie
- 17.3 Processus orphelins et zombies
- 17.4 Variables d'environnement
- 17.5-17.7 Signaux complets
- 17.8 Pipes anonymes
- 17.9 Named pipes (FIFO)

**Projets pratiques :**
```c
// Mini-shell simplifié
// - Exécution de commandes (fork/exec)
// - Pipes (cmd1 | cmd2)
// - Redirections (>, <)
// - Background (&)
// - Gestion de SIGINT, SIGCHLD
```

**Daemon complet :**
```c
// Daemon système
// - Détachement du terminal
// - PID file
// - Gestion SIGHUP (reload config)
// - Gestion SIGTERM (shutdown propre)
// - Logging vers syslog
```

---

#### Module 6 - Chapitre 18 : Threads et Concurrence *(Semaine 14)* 🔥🔥

**Tout le chapitre 18 (5 jours) :**
- 18.1-18.4 pthreads de base
- 18.5 Race conditions
- 18.6 Mutex
- 18.7 Deadlocks
- 18.8 Variables de condition
- 18.9 Sémaphores
- 18.10 Atomics (C11) ⭐
- 18.11 Read-write locks
- 18.12 Thread-local storage

**Projet : Pool de Threads (2 jours)**
```c
// Thread pool générique
// - N worker threads
// - Queue de tâches
// - Synchronisation propre
// - Shutdown gracieux
// - Tests avec ThreadSanitizer
```

---

#### Module 6 - Chapitre 20 : Networking *(Semaine 15, jours 1-3)* 🔥

**Tout le chapitre 20 :**
- 20.1-20.4 Sockets TCP/UDP
- 20.5 Gestion d'erreurs réseau
- 20.6 Options de sockets
- 20.8 Serveur concurrent
- 20.9 Non-blocking I/O et epoll
- 20.10 Mini-serveur HTTP

**Projet : Serveur Web Minimal (3 jours)**
```c
// Serveur HTTP basique
// - Écoute sur port 8080
// - Multithreading (pool)
// - Parsing HTTP/1.1
// - Static file serving
// - Gestion propre des erreurs
// - Logging
// - Graceful shutdown
```

---

#### Module 6 - Chapitre 21 : eBPF *(Semaine 15, jours 4-5)* 🔥🔥🔥

**Introduction eBPF complète :**
- 21.1 Qu'est-ce qu'eBPF ? Pourquoi révolutionnaire ?
- 21.2 Architecture (programs, maps, verifier, JIT)
- 21.3 Use cases DevOps
- 21.4 Toolchain (libbpf, bpftool)
- 21.5 Premier programme eBPF
- 21.6 Écosystème moderne (Cilium, Pixie)

**Pratique :**
```c
// Programme eBPF simple
// - Tracer les appels open()
// - Compter les syscalls par processus
// - Latences I/O
// - Communication userspace via maps
```

---

### 🔥 Semaines 16-18 : Expertise et Optimisation (120h)

#### Module 7 - Chapitre 26 : Sécurité *(Semaine 16)* 🔥

**Tout le chapitre 26 (5 jours) :**
- 26.1 CERT C Standards
- 26.2 Validation des entrées
- 26.3 Prévention buffer overflows
- 26.4 Format string vulnerabilities
- 26.5 Integer overflow
- 26.6 Principe du moindre privilège
- 26.7 Static analysis et fuzzing
- 26.8 Hardening flags

**Compilation hardened :**
```cmake
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    -D_FORTIFY_SOURCE=2 \
    -fstack-protector-strong \
    -fPIE \
    -Wformat \
    -Wformat-security \
    -Werror=format-security")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
    -pie \
    -Wl,-z,relro \
    -Wl,-z,now")
```

**Pratique : Audit de sécurité**
- Analyser du code vulnérable
- Corriger les failles
- Fuzzing avec AFL/libFuzzer

---

#### Module 7 - Chapitre 27 : Performance *(Semaine 17)* 🔥

**Tout le chapitre 27 (5 jours) :**
- 27.1 Flags d'optimisation (-O2, -O3, -Os)
- 27.2 Comprendre l'optimiseur
- 27.3 Profiling (gprof, perf)
- 27.4 Cache awareness
- 27.5 Branch prediction
- 27.6 Optimisations algorithmiques
- 27.7 Vectorisation et SIMD
- 27.8 LTO (Link-Time Optimization)
- 27.9 PGO (Profile-Guided Optimization)
- 27.10 Benchmarking rigoureux

**Pratique intensive :**
```bash
# Profiling avec perf
perf record -g ./programme  
perf report  

# Analyse de cache
perf stat -e cache-misses,cache-references ./programme

# Flamegraphs
perf record -F 99 -a -g -- ./programme  
perf script | stackcollapse-perf.pl | flamegraph.pl > flame.svg  
```

**Projet : Optimisation d'Algorithme**
- Prendre un algorithme naïf
- Profiler et identifier les bottlenecks
- Optimiser (cache, SIMD, algorithme)
- Mesurer les gains (10x, 100x ?)

---

#### Module 8 - Chapitre 31 : Cross-Compilation *(Semaine 18)* 🔥

**Tout le chapitre 31 (4 jours) :**
- 31.1 Principes cross-compilation
- 31.2 Compiler pour ARM
- 31.3 CMake et cross-compilation
- 31.4 QEMU pour tests
- 31.5-31.6 Packaging (.deb, .rpm)
- 31.7 AppImage
- 31.8 Docker multi-stage
- 31.9 Distribution de bibliothèques

**Pratique :**
```cmake
# Toolchain file pour ARM
set(CMAKE_SYSTEM_NAME Linux)  
set(CMAKE_SYSTEM_PROCESSOR arm)  

set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)  
set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)  

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)  
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)  
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)  
```

**Projet : Application Multi-Architecture (1 jour)**
- Build pour x86, ARM32, ARM64
- Tests dans QEMU
- Packaging .deb
- Image Docker multi-arch

---

#### Module 7 - Chapitres 24-25 : Gestion Mémoire et Erreurs Avancées *(Semaine 18, jour 5)*

**Survol rapide :**
- 24.1 RAII-like avec __attribute__((cleanup))
- 24.2 Custom allocators
- 25.1-25.3 Patterns de gestion d'erreurs
- 25.6 Logging professionnel

---

### 🔥 Semaines 19-20 : Projet Final et Portfolio (80h)

#### Projet de Synthèse Complet 🎯

**Option 1 : Système de Monitoring Distribué**

**Description :**
Un système complet de monitoring pour clusters de serveurs.

**Composants :**
1. **Agent (C)** sur chaque serveur :
   - Collecte métriques (CPU, RAM, disk, network)
   - Utilise eBPF pour tracing avancé
   - Daemon robuste (SIGHUP, SIGTERM)
   - Envoi vers collecteur via sockets

2. **Collecteur (C)** :
   - Serveur TCP multi-threadé
   - Stockage time-series simple
   - API HTTP pour requêtes
   - Gestion de milliers de connexions (epoll)

3. **CLI (C)** :
   - Interface de requête
   - Parsing d'arguments
   - Formatage de sortie

**Stack technique complète :**
- CMake avec multiple targets
- Tests unitaires (Unity/CMocka)
- CI/CD GitHub Actions
  - Matrix build (Ubuntu versions)
  - Sanitizers
  - Valgrind
  - Code coverage (Codecov)
  - Static analysis (cppcheck, clang-tidy)
- Cross-compilation ARM
- Docker multi-stage
- Documentation (Doxygen)

**Livrables :**
- Code source organisé
- Tests (>80% coverage)
- Pipeline CI/CD fonctionnel
- Documentation complète
- README professionnel
- Binaires pour x86 et ARM
- Image Docker

---

**Option 2 : Serveur Web avec Load Balancer**

**Description :**
Serveur HTTP/1.1 complet avec reverse proxy et load balancing.

**Composants :**
1. **Serveur HTTP** :
   - Parsing HTTP/1.1 complet
   - Static file serving
   - Keep-alive
   - Chunked transfer
   - Multi-threadé (pool)

2. **Load Balancer** :
   - Reverse proxy
   - Round-robin / least-connections
   - Health checks
   - Sticky sessions

3. **Admin Interface** :
   - API pour statistiques
   - Rechargement config sans downtime

**Stack similaire à Option 1.**

---

**Option 3 : Émulateur de Microprocesseur Simple**

**Description :**
Émulateur de CPU 8-bit avec debugger intégré.

**Fonctionnalités :**
- Set d'instructions complet (ADD, SUB, LOAD, STORE, JMP...)
- Mémoire virtuelle
- Debugger interactif (style GDB)
- Assembleur intégré
- Tests de programmes exemples

**Défi technique :** Gestion d'état complexe, optimisation performance.

---

#### Documentation et Portfolio *(Derniers jours)*

**32.1 Doxygen complet**
- Tous les headers documentés
- Génération HTML professionnelle

**32.2 README exemplaire**
```markdown
# Projet Name

[![Build Status](badge)](link)
[![Coverage](badge)](link)

## Description
...

## Installation
```bash
# Clone
git clone ...

# Build
mkdir build && cd build  
cmake ..  
make  

# Run tests
ctest
```

## Usage
...

## Architecture
...

## Contributing
...

## License
MIT
```

**Portfolio GitHub :**
- README de profil attractif
- Repositories bien organisés
- Issues/PRs sur projets open-source
- Contributions à documenter

---

## 📊 Évaluation et Certification

### Critères de Réussite

**Compétences Techniques (70%) :**
- [ ] Maîtrise du C (syntaxe, mémoire, structures)
- [ ] Programmation système (processus, threads, réseau)
- [ ] Outillage moderne (CMake, GDB, Sanitizers)
- [ ] CI/CD opérationnel
- [ ] Projet final fonctionnel et de qualité

**Pratiques Professionnelles (20%) :**
- [ ] Code propre et bien organisé
- [ ] Tests automatisés (>70% coverage)
- [ ] Documentation complète
- [ ] Git utilisé proprement

**Projet Final (10%) :**
- [ ] Complexité technique
- [ ] Innovation
- [ ] Qualité globale

### Auto-Évaluation Hebdomadaire

**Chaque vendredi :**
1. Qu'ai-je appris cette semaine ?
2. Quels concepts restent flous ?
3. Ai-je atteint mes objectifs ?
4. Plan pour la semaine suivante ?

---

## 💼 Débouchés Professionnels

**Avec cette formation complète, vous êtes qualifié pour :**

**Postes Directement Accessibles :**
- Ingénieur Logiciel C/C++
- Développeur Système Linux
- DevOps Engineer (focus système)
- Site Reliability Engineer (SRE)
- Ingénieur Performance
- Développeur Backend systèmes critiques

**Avec Spécialisation Additionnelle :**
- Ingénieur Linux Embarqué (+ hardware)
- Développeur Kernel Linux (+ approfondissement)
- Ingénieur Sécurité (+ focus cyber)
- Architecte Systèmes Distribués

**Secteurs :**
- Cloud providers (AWS, GCP, Azure)
- Éditeurs de logiciels infrastructure
- Bases de données (PostgreSQL, MongoDB...)
- Networking (Cisco, F5...)
- Systèmes embarqués critiques
- Finance haute fréquence
- Jeux vidéo (engine)

**Salaires (France, 2025) :**
- Junior (0-2 ans) : 40-50k€
- Confirmé (2-5 ans) : 50-70k€
- Senior (5+ ans) : 70-90k€+
- Expert / Architecte : 90-120k€+

**À l'international :** x1.5 à x2.5 (Silicon Valley, Suisse, Londres...)

---

## 🎯 Conseils pour Réussir le Parcours Intensif

### 1. **Organisation et Discipline**

**Planning strict :**
- 8h-12h : Théorie et lecture
- 12h-13h : Pause déjeuner
- 13h-17h : Pratique et projets
- 17h-18h : Révision et planification lendemain

**Un jour de repos par semaine minimum.**

### 2. **Apprentissage Actif**

❌ **Ne pas faire :**
- Lire passivement
- Copier-coller du code sans comprendre
- Sauter les exercices

✅ **Faire :**
- Coder en même temps que vous lisez
- Expérimenter et modifier les exemples
- Créer vos propres variations
- Expliquer à voix haute (rubber duck debugging)

### 3. **Pomodoro Technique**

- 25 min de focus intense
- 5 min de pause
- Après 4 pomodoros : 15-30 min de pause

**Outils :** Brain.fm pour la concentration, Forest app pour éviter distractions.

### 4. **Communauté et Mentorat**

**Rejoignez :**
- Discord de programmation
- Reddit r/C_Programming
- Stack Overflow
- Slack de développeurs locaux

**Trouvez un mentor** (développeur senior) pour :
- Code reviews
- Questions bloquantes
- Conseils de carrière

### 5. **Gestion du Syndrome de l'Imposteur**

**C'est normal de se sentir dépassé.**

- Tout le monde a commencé débutant
- Même les experts cherchent sur Google
- L'erreur fait partie de l'apprentissage

**Astuce :** Tenez un journal de vos progrès. Relisez-le quand vous doutez.

### 6. **Équilibre Vie/Formation**

**Prenez soin de vous :**
- Sommeil : 7-8h minimum
- Sport : 30 min par jour
- Alimentation équilibrée
- Contacts sociaux maintenus

**Un cerveau fatigué n'apprend pas bien.**

### 7. **Debugging Mindset**

Quand vous êtes bloqué :
1. Lire l'erreur attentivement
2. Google l'erreur exacte
3. Simplifier le problème (diviser pour régner)
4. Debugger avec GDB
5. Expliquer le problème à quelqu'un (ou à un canard)
6. Faire une pause (solution vient souvent après)

### 8. **Portfolio dès le Début**

**Semaine 1 :**
- Créer compte GitHub
- README de profil
- Commencer à commit régulièrement

**Chaque projet devient une ligne sur votre CV.**

### 9. **Veille Technologique**

**Suivez :**
- Hacker News
- Lobste.rs
- r/programming
- Blogs de développeurs kernel Linux
- Twitter #linux #c #ebpf

**15 min/jour de veille.**

### 10. **Préparation Entretiens**

**Dès semaine 12 :**
- LeetCode (algorithmes en C)
- Préparer récit de vos projets
- Préparer questions techniques fréquentes
- Mock interviews

---

## 📚 Ressources Essentielles

### Livres (par ordre de priorité)

**1. "The C Programming Language" (K&R)** ⭐⭐⭐
- La bible du C
- Concis, précis, complet
- À garder comme référence

**2. "The Linux Programming Interface" par Michael Kerrisk** ⭐⭐⭐
- LA référence programmation système
- 1500 pages, exhaustif
- Mainteneur des man pages

**3. "Expert C Programming" par Peter van der Linden** ⭐⭐
- Approfondit les subtilités du C
- Lecture agréable
- Plein d'anecdotes

**4. "Computer Systems: A Programmer's Perspective" (CS:APP)** ⭐⭐⭐
- Comprendre l'architecture
- Assembly, mémoire, optimisations
- Utilisé dans les meilleures universités

**5. "Linux Device Drivers" (LDD3)** ⭐
- Pour aller plus loin sur les drivers
- Un peu daté mais toujours pertinent

### Cours en Ligne

**CS50 (Harvard)** - Gratuit sur edX ⭐⭐⭐
- Introduction Computer Science en C
- Excellente qualité pédagogique

**Systems Programming (Angrave)** - UIUC ⭐⭐
- Notes de cours gratuites
- Focus programmation système

### Documentation

**man pages Linux** ⭐⭐⭐
```bash
man 2 open    # Appel système  
man 3 printf  # Fonction bibliothèque  
man 7 signal  # Concepts  
```

**GNU C Library Manual**
- https://www.gnu.org/software/libc/manual/

**Linux Kernel Documentation**
- https://www.kernel.org/doc/html/latest/

### Outils

**Essential :**
- GCC / Clang
- GDB
- Valgrind
- CMake
- Git

**Recommandés :**
- tmux (multiplexeur terminal)
- Vim / Neovim (éditeur efficace)
- htop (monitoring)
- strace (trace syscalls)
- ltrace (trace library calls)

---

## ✅ Checklist Globale de Progression

### Semaines 1-2 : Fondations
- [ ] Environnement complètement configuré
- [ ] Premier programme compilé et exécuté
- [ ] Compréhension chaîne de compilation
- [ ] Bases du C maîtrisées

### Semaines 3-5 : Langage
- [ ] Pointeurs maîtrisés
- [ ] Allocation dynamique sans fuites
- [ ] Structures de données implémentées
- [ ] Projet base de données complété

### Semaines 6-8 : Avancé
- [ ] Table de hachage fonctionnelle
- [ ] Bibliothèque statique/dynamique créée
- [ ] Pointeurs de fonctions utilisés
- [ ] Macros avancées comprises

### Semaines 9-11 : Outillage
- [ ] CMake maîtrisé
- [ ] GDB utilisé efficacement
- [ ] Valgrind sans erreurs
- [ ] Pipeline CI/CD fonctionnel
- [ ] Tests unitaires >80% coverage

### Semaines 12-15 : Système
- [ ] Programme avec I/O avancé
- [ ] Daemon fonctionnel créé
- [ ] Serveur multi-threadé opérationnel
- [ ] Programme eBPF écrit et testé

### Semaines 16-18 : Expertise
- [ ] Code sécurisé (aucune vulnérabilité)
- [ ] Programme optimisé (10x+ speedup)
- [ ] Cross-compilation ARM réussie
- [ ] Package .deb créé

### Semaines 19-20 : Projet Final
- [ ] Projet complexe complété
- [ ] Documentation professionnelle
- [ ] Portfolio GitHub attractif
- [ ] Prêt pour entretiens

---

## 🎉 Conclusion

**Félicitations d'avoir choisi ce parcours intensif !**

En 16-20 semaines, vous passerez de débutant (ou intermédiaire) à **ingénieur C compétent**, capable de :
- Développer des applications système complexes
- Contribuer à des projets open-source majeurs
- Postuler à des postes d'ingénieur logiciel senior
- Comprendre les systèmes au niveau le plus bas

**Ce parcours est exigeant.** Vous aurez des moments de doute, de frustration, où les bugs sembleront insurmontables. **C'est normal.**

**Mais rappelez-vous pourquoi vous avez commencé.**

Chaque concept maîtrisé, chaque bug résolu, chaque projet complété vous rapproche de votre objectif : devenir un ingénieur système expert, capable de travailler sur l'infrastructure qui fait tourner le monde moderne.

**Le C n'est pas juste un langage, c'est une compréhension profonde de l'informatique.**

**Votre voyage commence maintenant. Bon courage ! 🚀**

---

**Premiers pas (À FAIRE CETTE SEMAINE) :**

1. ✅ Installer Ubuntu (ou WSL2 sur Windows)
2. ✅ Installer la toolchain complète
3. ✅ Compiler "Hello World"
4. ✅ Configurer Git et créer compte GitHub
5. ✅ Lire Module 1 complet
6. ✅ Planifier vos 20 prochaines semaines
7. ✅ Rejoindre une communauté de développeurs

**Dans 20 semaines, vous serez un autre développeur. Let's go! 💪**

---


⏭️ [Ressources complémentaires](/ressources-complementaires/README.md)
