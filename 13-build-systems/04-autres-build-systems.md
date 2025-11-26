🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 13.4 Autres build systems

## Introduction

Maintenant que vous maîtrisez CMake et comprenez Make, vous vous demandez peut-être : "Existe-t-il d'autres outils pour compiler mes projets C ?" La réponse est oui ! L'écosystème du build C/C++ est riche et varié, avec plusieurs alternatives modernes qui ont chacune leurs forces et leurs cas d'usage.

Dans cette section, nous allons explorer les principaux build systems alternatifs, comprendre pourquoi ils existent, et savoir quand les utiliser. **Spoiler** : CMake reste dominant en 2025, mais ces alternatives ont des niches intéressantes.

---

## Panorama de l'écosystème

### Les trois catégories de build systems

#### 1. Générateurs de build (comme CMake)

**Rôle** : Analyser le projet et générer des fichiers pour un autre outil.

**Exemples** : CMake, Premake, GN, Autotools

**Workflow** :
```
Fichier config → [Générateur] → Makefile/Ninja/MSBuild → Compilation
```

#### 2. Build systems directs (comme Ninja)

**Rôle** : Exécuter directement la compilation sans passer par un générateur.

**Exemples** : Ninja, Make, MSBuild

**Workflow** :
```
Fichier config → [Build system] → Compilation
```

#### 3. Build systems tout-en-un (comme Meson)

**Rôle** : Gérer la configuration ET l'exécution du build.

**Exemples** : Meson, Bazel, SCons, xmake

**Workflow** :
```
Fichier config → [Système intégré] → Compilation
```

---

## 1. Ninja - Le build system ultra-rapide

### Qu'est-ce que Ninja ?

**Ninja** est un build system minimaliste conçu pour la **vitesse pure**. Il ne gère pas la configuration du projet (comme CMake) mais excelle dans l'exécution rapide des builds.

**Créateur** : Evan Martin (Google)
**Année** : 2011
**Langage** : C++
**Utilisé par** : Chromium, Android, LLVM, Swift

### Caractéristiques principales

- ✅ **Extrêmement rapide** : 40-60% plus rapide que Make
- ✅ **Parallélisation optimale** : Utilise tous les cœurs CPU efficacement
- ✅ **Fichiers compacts** : Syntaxe minimale
- ✅ **Régénération intelligente** : Recompile le strict minimum

- ❌ **Non conçu pour être écrit à la main** : Fichiers générés par CMake/Meson
- ❌ **Pas de détection de dépendances** : Doit être configuré par un générateur

### Exemple de fichier build.ninja

```ninja
# build.ninja - Généré automatiquement, ne pas éditer à la main !

cc = gcc
cflags = -Wall -Wextra

rule compile
  command = $cc $cflags -c $in -o $out

rule link
  command = $cc $in -o $out

build main.o: compile main.c
build utils.o: compile utils.c
build mon_app: link main.o utils.o
```

**Note** : Vous n'écrirez JAMAIS ce fichier à la main. CMake ou Meson le génèrent pour vous.

### Utilisation avec CMake

```bash
# CMake génère des fichiers Ninja au lieu de Makefiles
cmake -B build -G Ninja

# Compilation ultra-rapide
cmake --build build

# Ou directement
ninja -C build
```

### Benchmark : Make vs Ninja

**Projet exemple** : LLVM (2000+ fichiers C++)

| Build system | Temps compilation complète | Recompilation 1 fichier |
|--------------|---------------------------|------------------------|
| GNU Make | 45 minutes | 3.2 secondes |
| Ninja | 28 minutes ⚡ | 1.8 secondes ⚡ |

**Conclusion** : Ninja est **40% plus rapide** sur gros projets !

### Quand utiliser Ninja ?

- ✅ **Gros projets** (100+ fichiers) où le temps de build compte
- ✅ **CI/CD** pour accélérer les pipelines
- ✅ **Développement quotidien** pour des itérations rapides

- ❌ **Projets simples** : Make suffit amplement
- ❌ **Seul** : Utilisez-le avec CMake ou Meson

---

## 2. Meson - Le concurrent moderne de CMake

### Qu'est-ce que Meson ?

**Meson** est un build system moderne qui vise à être plus simple et plus rapide que CMake. Il génère automatiquement des fichiers Ninja (ou MSBuild/Xcode).

**Créateur** : Jussi Pakkanen
**Année** : 2013
**Langage** : Python
**Utilisé par** : systemd, GNOME, Mesa, GStreamer, X.org

### Caractéristiques principales

- ✅ **Syntaxe claire** : Plus lisible que CMake
- ✅ **Rapide** : Utilise Ninja par défaut
- ✅ **Cross-compilation native** : Fichiers toolchain simples
- ✅ **Tests intégrés** : Support natif des tests unitaires
- ✅ **Détection automatique** : Trouve les dépendances facilement

- ❌ **Moins mature que CMake** : Moins de bibliothèques supportées
- ❌ **Nécessite Python** : Dépendance supplémentaire
- ❌ **Communauté plus petite** : Moins de ressources disponibles

### Exemple : Projet simple avec Meson

**meson.build** :
```meson
# meson.build - Configuration du projet
project('mon_app', 'c',
  version : '1.0.0',
  default_options : ['warning_level=3', 'c_std=c11']
)

# Sources
sources = [
  'src/main.c',
  'src/utils.c'
]

# Exécutable
executable('mon_app', sources,
  include_directories : include_directories('include'),
  dependencies : [dependency('threads')]
)

# Tests
test_exe = executable('test_utils', 'tests/test_utils.c', 'src/utils.c')
test('test_utils', test_exe)
```

**Compilation** :
```bash
# Configuration (génère des fichiers Ninja)
meson setup build

# Compilation
meson compile -C build

# Tests
meson test -C build

# Installation
meson install -C build
```

### Comparaison CMake vs Meson

| Critère | CMake | Meson |
|---------|-------|-------|
| **Syntaxe** | Verbeuse | ✅ Concise |
| **Vitesse** | Rapide | ✅ Plus rapide (Ninja natif) |
| **Écosystème** | ✅ Immense | Plus petit |
| **Maturité** | ✅ Très mature | Moins mature |
| **Courbe apprentissage** | Moyenne | ✅ Plus facile |
| **find_package** | ✅ Excellent | Bon |
| **Documentation** | ✅ Exhaustive | Bonne |
| **Adoption** | ✅ 78% projets | 4% projets |

### Exemple concret : même projet en CMake et Meson

**CMakeLists.txt** (14 lignes) :
```cmake
cmake_minimum_required(VERSION 3.15)
project(Calculator C)

set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra)

add_executable(calculator
  src/main.c
  src/operations.c
)

target_include_directories(calculator PRIVATE include)
target_link_libraries(calculator m)
```

**meson.build** (8 lignes) :
```meson
project('calculator', 'c', default_options : ['c_std=c11'])

executable('calculator',
  ['src/main.c', 'src/operations.c'],
  include_directories : include_directories('include'),
  dependencies : dependency('m')
)
```

**Verdict** : Meson est plus concis, mais CMake a un écosystème plus riche.

### Quand utiliser Meson ?

- ✅ **Nouveaux projets** sans legacy
- ✅ **Projets GNOME/Linux** (bonne intégration)
- ✅ **Simplicité** prioritaire sur l'écosystème
- ✅ **Équipes Python** (familiarité avec la syntaxe)

- ❌ **Portabilité Windows** critique (CMake meilleur)
- ❌ **Dépendances complexes** (vcpkg/Conan meilleurs avec CMake)
- ❌ **Legacy** : Migration depuis CMake coûteuse

---

## 3. Bazel - Le build system de Google

### Qu'est-ce que Bazel ?

**Bazel** est un build system open-source basé sur l'outil interne de Google (Blaze). Conçu pour les **monorepos géants** avec des milliers de développeurs.

**Créateur** : Google
**Année** : 2015
**Langage** : Java
**Utilisé par** : Google, Uber, Dropbox, TensorFlow

### Caractéristiques principales

- ✅ **Monorepos massifs** : Conçu pour des projets de millions de lignes
- ✅ **Reproductibilité parfaite** : Builds hermétiques
- ✅ **Cache distribué** : Partage les builds entre développeurs
- ✅ **Multi-langage** : C, C++, Java, Python, Go, etc.
- ✅ **Parallélisation extrême** : Utilise tous les cœurs/machines disponibles

- ❌ **Complexité élevée** : Courbe d'apprentissage très raide
- ❌ **Overkill pour petits projets** : Configuration lourde
- ❌ **Nécessite Java** : Dépendance JVM
- ❌ **Communauté limitée** : Principalement grandes entreprises

### Exemple : BUILD file

**BUILD** :
```python
# BUILD - Configuration Bazel
cc_binary(
    name = "mon_app",
    srcs = [
        "main.c",
        "utils.c",
    ],
    hdrs = [
        "utils.h",
    ],
    deps = [
        "@boost//:system",
        "@openssl//:ssl",
    ],
    copts = ["-Wall", "-Wextra"],
)

cc_test(
    name = "test_utils",
    srcs = ["test_utils.c"],
    deps = [":mon_app"],
)
```

**Compilation** :
```bash
# Build
bazel build //path/to:mon_app

# Tests
bazel test //path/to:test_utils

# Run
bazel run //path/to:mon_app
```

### Concept clé : Hermétique et reproductible

Bazel garantit que le **même code produit toujours le même binaire**, peu importe :
- La machine de compilation
- L'heure du build
- Les dépendances installées sur le système

**Avantage** : Debugging simplifié, cache distribué efficace

### Quand utiliser Bazel ?

- ✅ **Monorepos gigantesques** (Google-scale)
- ✅ **Équipes de 100+ développeurs**
- ✅ **Reproductibilité critique** (finance, défense)
- ✅ **Multi-langage** (C, Java, Python, Go dans un seul repo)

- ❌ **Projets < 100k lignes** : Trop complexe
- ❌ **Équipes < 10 personnes** : Overhead trop important
- ❌ **Débutants** : Courbe d'apprentissage trop raide

**Verdict** : Excellent outil, mais seulement pour **très** gros projets.

---

## 4. SCons - Build system en Python

### Qu'est-ce que SCons ?

**SCons** est un build system écrit en Python, où la configuration est également du code Python.

**Année** : 2000
**Langage** : Python
**Utilisé par** : MongoDB (historique), quelques projets embedded

### Caractéristiques principales

- ✅ **Configuration en Python** : Tout le pouvoir de Python
- ✅ **Détection automatique** : Trouve les dépendances
- ✅ **Cross-platform** : Linux, Windows, macOS

- ❌ **Lent** : Python est plus lent que les outils natifs
- ❌ **Communauté petite** : Moins populaire qu'avant
- ❌ **Syntaxe verbeuse** : Plus de code que CMake

### Exemple : SConstruct

**SConstruct** :
```python
# SConstruct - Configuration SCons en Python
env = Environment()

# Définir les options de compilation
env.Append(CCFLAGS=['-Wall', '-Wextra'])
env.Append(CPPPATH=['include'])

# Sources
sources = ['src/main.c', 'src/utils.c']

# Programme
program = env.Program('mon_app', sources)

# Installation
env.Install('/usr/local/bin', program)
env.Alias('install', '/usr/local/bin')
```

**Compilation** :
```bash
scons              # Build
scons -c           # Clean
scons install      # Install
```

### Quand utiliser SCons ?

- ✅ **Équipes Python** : Familiarité avec le langage
- ✅ **Builds complexes** : Logique personnalisée facile
- ✅ **Scripting avancé** : Tout le pouvoir de Python

- ❌ **Performance** : Préférer CMake+Ninja
- ❌ **Écosystème** : Moins de support que CMake
- ❌ **Nouveaux projets** : CMake ou Meson plus modernes

---

## 5. Autotools (Autoconf + Automake) - Le dinosaure

### Qu'est-ce qu'Autotools ?

**Autotools** est l'ancien standard pour la portabilité Unix/Linux. Vous l'avez vu dans les projets open-source classiques :

```bash
./configure
make
make install
```

**Année** : 1991
**Utilisé par** : GCC, Bash, Coreutils, projets GNU legacy

### Caractéristiques principales

- ✅ **Portabilité Unix maximale** : Supporte les Unix exotiques
- ✅ **Détection système** : `./configure` analyse l'environnement
- ✅ **Standard GNU** : Utilisé partout il y a 20 ans

- ❌ **Complexité extrême** : Génère des milliers de lignes
- ❌ **Lent** : `./configure` peut prendre plusieurs minutes
- ❌ **Windows non supporté** : Unix/Linux uniquement
- ❌ **Debugging impossible** : Scripts shell générés incompréhensibles
- ❌ **Obsolète** : Remplacé par CMake/Meson

### Exemple minimal

**configure.ac** :
```m4
AC_INIT([mon_app], [1.0])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
```

**Makefile.am** :
```makefile
bin_PROGRAMS = mon_app
mon_app_SOURCES = main.c utils.c
```

**Génération** :
```bash
autoreconf --install  # Génère configure
./configure           # Génère Makefile
make                  # Compile
```

### Pourquoi c'est obsolète

- ❌ **Syntaxe M4** : Langage de macros ésotérique
- ❌ **Génération de masse** : 10 000+ lignes pour un projet simple
- ❌ **Temps de configure** : 5-10 minutes sur gros projets
- ❌ **Windows** : Nécessite Cygwin ou MinGW

**Verdict** : N'utilisez Autotools que si vous **devez** maintenir du code legacy.

---

## 6. xmake - Build system Lua moderne

### Qu'est-ce que xmake ?

**xmake** est un build system moderne basé sur Lua, populaire en Chine et en Asie.

**Année** : 2015
**Langage** : C + Lua
**Utilisé par** : Projets Tencent, certains projets gaming

### Caractéristiques principales

- ✅ **Configuration simple** : Syntaxe Lua claire
- ✅ **Tout-en-un** : Gestion de paquets intégrée
- ✅ **Rapide** : Compilation parallèle optimisée
- ✅ **Cross-compilation** : Support natif

- ❌ **Communauté petite** : Principalement Asie
- ❌ **Documentation** : Parfois en chinois
- ❌ **Écosystème** : Moins mature que CMake

### Exemple : xmake.lua

**xmake.lua** :
```lua
-- xmake.lua
set_project("mon_app")
set_version("1.0.0")

add_rules("mode.debug", "mode.release")

target("mon_app")
    set_kind("binary")
    add_files("src/*.c")
    add_includedirs("include")
    add_links("pthread", "m")
```

**Compilation** :
```bash
xmake              # Build
xmake run          # Run
xmake install      # Install
```

### Quand utiliser xmake ?

- ✅ **Syntaxe Lua** appréciée
- ✅ **Projets gaming** (bonne intégration)
- ✅ **Marchés asiatiques**

- ❌ **Écosystème occidental** : CMake dominant
- ❌ **Support bibliothèques** : Moins que CMake

---

## 7. Premake - Générateur léger

### Qu'est-ce que Premake ?

**Premake** génère des projets pour Visual Studio, Makefiles, Xcode à partir de scripts Lua.

**Année** : 2002
**Langage** : C + Lua
**Utilisé par** : Quelques projets gaming indie

### Exemple : premake5.lua

**premake5.lua** :
```lua
workspace "MonApp"
   configurations { "Debug", "Release" }

project "mon_app"
   kind "ConsoleApp"
   language "C"
   files { "src/**.c" }
   includedirs { "include" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
```

**Génération** :
```bash
premake5 gmake2     # Génère Makefile
premake5 vs2022     # Génère projet Visual Studio
premake5 xcode      # Génère projet Xcode
```

### Quand utiliser Premake ?

- ✅ **Projets gaming** (syntaxe proche de Unity/Unreal)
- ✅ **Configuration Lua** appréciée
- ✅ **Génération multi-IDE**

- ❌ **Moins puissant** que CMake
- ❌ **Communauté petite**

---

## 8. Build2 - Le perfectionniste

### Qu'est-ce que Build2 ?

**Build2** est un build system moderne qui vise la **perfection technique** : builds reproductibles, modules C++20, gestion de paquets intégrée.

**Année** : 2014
**Utilisé par** : Quelques projets C++ modernes

### Caractéristiques principales

- ✅ **Modules C++20** : Support natif
- ✅ **Reproductibilité** : Builds hermétiques
- ✅ **Gestion de paquets** : Intégrée
- ✅ **Philosophie solide** : Très bien pensé

- ❌ **Communauté minuscule** : Très peu adopté
- ❌ **Documentation** : Dense et académique
- ❌ **Overkill** : Trop complexe pour la plupart des projets

### Quand utiliser Build2 ?

- ✅ **C++20 moderne** avec modules
- ✅ **Reproductibilité absolue**
- ✅ **Perfectionnisme technique**

- ❌ **Projets grand public** : Trop niche
- ❌ **Débutants** : Courbe d'apprentissage raide

---

## Comparaison globale

### Tableau récapitulatif

| Build System | Type | Difficulté | Popularité | Vitesse | Cas d'usage |
|--------------|------|------------|------------|---------|-------------|
| **CMake** | Générateur | Moyenne | ⭐⭐⭐⭐⭐ | ⚡⚡⚡ | Standard universel |
| **Make** | Direct | Moyenne | ⭐⭐⭐⭐ | ⚡⚡⚡ | Projets simples |
| **Ninja** | Direct | Facile* | ⭐⭐⭐⭐ | ⚡⚡⚡⚡⚡ | Backend rapide |
| **Meson** | Tout-en-un | Facile | ⭐⭐⭐ | ⚡⚡⚡⚡ | Alternative moderne |
| **Bazel** | Tout-en-un | Difficile | ⭐⭐ | ⚡⚡⚡⚡ | Monorepos géants |
| **SCons** | Tout-en-un | Moyenne | ⭐⭐ | ⚡⚡ | Équipes Python |
| **Autotools** | Générateur | Très difficile | ⭐⭐ | ⚡ | Legacy Unix |
| **xmake** | Tout-en-un | Facile | ⭐ | ⚡⚡⚡ | Asie, gaming |
| **Premake** | Générateur | Facile | ⭐ | ⚡⚡ | Gaming indie |
| **Build2** | Tout-en-un | Difficile | ⭐ | ⚡⚡⚡ | C++ moderne |

*Ninja est facile car généré automatiquement, pas écrit à la main

### Parts de marché (2025)

```
┌────────────────────────────────────────────────────┐
│  PROJETS OPEN-SOURCE C/C++ (GitHub)                │
├────────────────────────────────────────────────────┤
│  CMake:     78%  ████████████████████████████████  │
│  Make:      15%  ██████                            │
│  Meson:      4%  ██                                │
│  Bazel:      2%  █                                 │
│  Autres:     1%  ▌                                 │
└────────────────────────────────────────────────────┘
```

---

## Guide de décision : Quel build system choisir ?

### Pour les débutants

```
Vous débutez en C ?
  → Apprenez les bases avec Make (comprendre les concepts)
  → Passez rapidement à CMake (standard moderne)
  → Utilisez Ninja comme backend de CMake (vitesse)
```

### Selon la taille du projet

```
┌────────────────────────────────────────────────┐
│  TAILLE DU PROJET                              │
├────────────────────────────────────────────────┤
│  1-3 fichiers       → Make (simple)            │
│  4-50 fichiers      → CMake                    │
│  51-1000 fichiers   → CMake + Ninja            │
│  1000+ fichiers     → CMake + Ninja (ou Bazel) │
│  Monorepo géant     → Bazel                    │
└────────────────────────────────────────────────┘
```

### Selon l'équipe

```
Équipe habituée à Python ?  → Meson ou SCons
Équipe Google/Uber/etc ?    → Bazel
Équipe gaming ?             → Premake ou xmake
Équipe polyvalente ?        → CMake (standard)
```

### Selon la plateforme

```
Linux uniquement            → Make ou Meson (simples)
Linux + Windows + macOS     → CMake (meilleure portabilité)
Systèmes embarqués          → CMake + cross-compilation
```

### Selon les objectifs

```
Vitesse de build maximale      → CMake + Ninja
Simplicité de configuration    → Meson
Reproductibilité hermétique    → Bazel ou Build2
Contrôle total                 → Make
Standard industriel            → CMake
```

---

## Tendances et perspectives 2025

### Tendances actuelles

1. **CMake reste dominant** : 78% et croissant
2. **Ninja devient standard** : Backend préféré de CMake
3. **Meson gagne du terrain** : +2% par an, surtout Linux
4. **Bazel** : Adoption lente mais stable (Google, grandes entreprises)
5. **Make** : Déclin lent mais reste utilisé pour projets simples
6. **Autotools** : Quasi-extinction, migration vers CMake

### Projections 2026-2030

```
CMake:    78% → 82%    (consolidation)
Meson:     4% → 8%     (croissance forte)
Bazel:     2% → 3%     (croissance lente)
Make:     15% → 10%    (déclin)
Autres:    1% → 1%     (stagnation)
```

### Technologies émergentes

**BuildXL** (Microsoft) : Build system pour Windows, utilisé pour Windows et Office
**Please** : Build system multi-langage, alternative Bazel
**Buck2** (Meta) : Successeur de Buck, inspiré de Bazel

**Note** : Aucun ne menace vraiment la domination de CMake en 2025.

---

## Migration entre build systems

### De Make vers CMake

**Difficulté** : ⭐⭐ Facile
**Temps** : 1-3 jours selon taille
**Bénéfices** : Énormes (portabilité, IDE support)

### De CMake vers Meson

**Difficulté** : ⭐⭐⭐ Moyenne
**Temps** : 3-7 jours
**Bénéfices** : Syntaxe plus simple, vitesse légèrement meilleure

**Question** : Est-ce que ça vaut le coup ?
**Réponse** : Rarement, sauf si vous démarrez un nouveau projet.

### De Autotools vers CMake

**Difficulté** : ⭐⭐⭐⭐ Difficile
**Temps** : 1-4 semaines
**Bénéfices** : Énormes (portabilité Windows, vitesse, maintenabilité)

**Verdict** : Fortement recommandé si projet encore maintenu.

### Vers Bazel

**Difficulté** : ⭐⭐⭐⭐⭐ Très difficile
**Temps** : Plusieurs mois
**Bénéfices** : Seulement pour très gros projets

**Verdict** : Uniquement si vous avez vraiment besoin de ses capacités.

---

## Cas d'usage réels : Qui utilise quoi ?

### CMake (78%)

**Projets majeurs** :
- LLVM/Clang (compilateur)
- Qt (framework GUI)
- OpenCV (computer vision)
- Boost (bibliothèques C++)
- KDE (desktop Linux)
- MySQL/MariaDB (bases de données)
- Blender (3D)

### Meson (4%)

**Projets majeurs** :
- systemd (init system Linux)
- GNOME (desktop Linux)
- Mesa (drivers graphiques Linux)
- GStreamer (multimedia)
- X.org (serveur graphique)

### Bazel (2%)

**Projets majeurs** :
- TensorFlow (machine learning)
- Envoy (proxy réseau)
- Angular (framework JavaScript - oui, multi-langage !)
- Projets internes Google

### Make (15%)

**Projets majeurs** :
- Kernel Linux (Makefile custom très complexe)
- Redis (petit projet, Make suffit)
- Nginx (historique, reste sur Make)
- Nombreux petits projets legacy

---

## Recommandations finales

### Pour un nouveau projet en 2025

**Choix par défaut** : **CMake + Ninja**

Raisons :
- ✅ Standard industriel (78% des projets)
- ✅ Écosystème le plus riche
- ✅ Support IDE maximal
- ✅ Documentation exhaustive
- ✅ Communauté active
- ✅ Portabilité garantie

**Alternative viable** : **Meson**

Si :
- Vous démarrez from scratch (pas de legacy)
- Vous êtes sur Linux principalement
- Vous voulez une syntaxe plus simple
- Votre équipe connaît Python

### Pour maintenir du code existant

- **Projet avec Makefile** : Conserver si < 10 fichiers, sinon migrer vers CMake
- **Projet avec Autotools** : Migrer vers CMake dès que possible
- **Projet avec CMake** : Rester sur CMake, upgrader la version
- **Projet avec Bazel** : Rester sur Bazel (migration très coûteuse)

### Pour apprendre

1. **Commencez par Make** : Comprendre les concepts de base (1 semaine)
2. **Passez à CMake** : Standard moderne (2-3 semaines)
3. **Expérimentez Ninja** : Comme backend de CMake (1 jour)
4. **Optionnel : Testez Meson** : Voir l'alternative (quelques jours)
5. **Ignorez le reste** : Sauf besoin spécifique

---

## Ressources pour aller plus loin

### CMake
- Site officiel : https://cmake.org/
- Tutoriel moderne : https://cliutils.gitlab.io/modern-cmake/

### Ninja
- Site officiel : https://ninja-build.org/
- Intégration CMake : https://cmake.org/cmake/help/latest/generator/Ninja.html

### Meson
- Site officiel : https://mesonbuild.com/
- Comparaison CMake/Meson : https://mesonbuild.com/CMake-module.html

### Bazel
- Site officiel : https://bazel.build/
- Getting Started : https://bazel.build/start

### Autres
- SCons : https://scons.org/
- xmake : https://xmake.io/
- Build2 : https://build2.org/

---

## Conclusion

Le paysage des build systems en C/C++ est varié, mais **CMake domine largement** avec 78% de parts de marché en 2025. Cette domination n'est pas un hasard :
- Écosystème mature et riche
- Portabilité maximale
- Support IDE universel
- Communauté massive

Les alternatives ont leurs niches :
- **Ninja** : Backend ultra-rapide (utilisé avec CMake)
- **Meson** : Alternative moderne pour projets Linux
- **Bazel** : Monorepos géants (Google-scale)
- **Make** : Projets simples et legacy

**Recommandation pour 2025** : **Apprenez CMake, utilisez Ninja comme backend.** C'est le duo gagnant qui couvre 95% des cas d'usage.

Les autres outils ? Intéressants à connaître, utiles dans des contextes spécifiques, mais ne perdez pas trop de temps dessus sauf besoin particulier.

**L'essentiel** : Le choix du build system est important, mais **bien maîtriser un seul (CMake) vaut mieux que connaître superficiellement cinq outils.**

---


⏭️ [Les Bibliothèques](/14-bibliotheques/README.md)
