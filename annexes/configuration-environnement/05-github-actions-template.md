🔝 Retour au [Sommaire](/SOMMAIRE.md)

# C.5 - GitHub Actions workflow template

## Table des matières
- [Introduction](#introduction)
- [Qu'est-ce que GitHub Actions ?](#quest-ce-que-github-actions-)
- [Concepts fondamentaux](#concepts-fondamentaux)
- [Structure d'un workflow](#structure-dun-workflow)
- [Création de votre premier workflow](#cr%C3%A9ation-de-votre-premier-workflow)
- [Template de base commenté](#template-de-base-comment%C3%A9)
- [Workflows par cas d'usage](#workflows-par-cas-dusage)
- [Matrix builds](#matrix-builds)
- [Caching et optimisation](#caching-et-optimisation)
- [Artifacts](#artifacts)
- [Intégration avec les outils C](#int%C3%A9gration-avec-les-outils-c)
- [Secrets et variables](#secrets-et-variables)
- [Badges de statut](#badges-de-statut)
- [Bonnes pratiques](#bonnes-pratiques)
- [Dépannage](#d%C3%A9pannage)
- [Exemples complets](#exemples-complets)

---

## Introduction

GitHub Actions est un système d'intégration continue (CI) et de déploiement continu (CD) intégré directement dans GitHub. Il permet d'automatiser vos workflows de développement : compilation, tests, analyse de code, déploiement, etc.

**Ce que vous allez apprendre :**
- Créer des workflows GitHub Actions pour vos projets C
- Automatiser la compilation et les tests
- Intégrer des outils d'analyse (Valgrind, sanitizers, cppcheck)
- Tester sur plusieurs versions de GCC et plusieurs OS
- Générer des rapports de couverture de code
- Publier des artifacts (binaires, documentation)

---

## Qu'est-ce que GitHub Actions ?

### **Définition**

**GitHub Actions** est une plateforme d'automatisation qui exécute des tâches (workflows) en réponse à des événements GitHub (push, pull request, release, etc.).

**Analogie simple :** Imaginez un assistant qui, à chaque fois que vous poussez du code sur GitHub, compile automatiquement votre projet, exécute tous les tests, vérifie qu'il n'y a pas de fuites mémoire avec Valgrind, et vous envoie un rapport. C'est ça, GitHub Actions !

### **Avantages**

- ✅ **Gratuit pour les projets open-source** (2000 minutes/mois pour les privés)
- ✅ **Intégré à GitHub** : Pas de service externe à configurer
- ✅ **Marketplace d'actions** : Milliers d'actions réutilisables
- ✅ **Multi-plateforme** : Linux, Windows, macOS
- ✅ **Parallélisation** : Exécution simultanée de plusieurs jobs
- ✅ **Caching intelligent** : Accélération des builds

### **Cas d'usage pour projets C**

- 🎯 **Compilation automatique** : Vérifier que le code compile après chaque commit
- 🎯 **Tests unitaires** : Exécuter automatiquement les tests
- 🎯 **Analyse statique** : Détecter les bugs avec cppcheck, clang-tidy
- 🎯 **Analyse mémoire** : Vérifier les fuites avec Valgrind
- 🎯 **Formatage** : Vérifier que le code respecte le style (clang-format)
- 🎯 **Cross-compilation** : Compiler pour différentes architectures
- 🎯 **Release automatique** : Publier des binaires lors d'un tag

---

## Concepts fondamentaux

### **Workflow**

Un **workflow** est un processus automatisé configurable composé d'un ou plusieurs jobs. Il est défini dans un fichier YAML.

**Emplacement :**
```
mon_projet/
└── .github/
    └── workflows/
        ├── ci.yml           ← Workflow d'intégration continue
        ├── release.yml      ← Workflow de release
        └── coverage.yml     ← Workflow de couverture
```

### **Event (Déclencheur)**

Un **event** est un événement GitHub qui déclenche un workflow.

**Événements courants :**
- `push` : À chaque push sur une branche
- `pull_request` : À chaque PR
- `release` : Lors d'une release
- `schedule` : À intervalles réguliers (cron)
- `workflow_dispatch` : Déclenchement manuel

**Exemple :**
```yaml
on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]
```

### **Job**

Un **job** est un ensemble d'étapes (steps) qui s'exécutent sur le même runner.

**Caractéristiques :**
- Les jobs s'exécutent en parallèle par défaut
- Peuvent être configurés pour s'exécuter séquentiellement
- Chaque job tourne sur une machine virtuelle fraîche

**Exemple :**
```yaml
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - run: echo "Building..."

  test:
    runs-on: ubuntu-latest
    needs: build  # Attend que 'build' soit terminé
    steps:
      - run: echo "Testing..."
```

### **Step**

Un **step** est une tâche individuelle dans un job.

**Types de steps :**
- `uses` : Utilise une action existante
- `run` : Exécute une commande shell

**Exemple :**
```yaml
steps:
  - uses: actions/checkout@v4        # Action : clone le repo
  - run: gcc main.c -o main          # Commande : compile
  - run: ./main                      # Commande : exécute
```

### **Runner**

Un **runner** est une machine virtuelle qui exécute les jobs.

**Runners disponibles :**
- `ubuntu-latest` (Ubuntu 22.04)
- `ubuntu-20.04`
- `windows-latest`
- `macos-latest`

**Pour C, on utilise généralement `ubuntu-latest`.**

### **Action**

Une **action** est un composant réutilisable (comme une fonction).

**Actions populaires :**
- `actions/checkout@v4` : Clone le repository
- `actions/upload-artifact@v4` : Upload un fichier
- `actions/cache@v4` : Cache des dépendances
- `codecov/codecov-action@v4` : Upload vers Codecov

**Marketplace :** https://github.com/marketplace?type=actions

---

## Structure d'un workflow

### **Fichier YAML**

Les workflows utilisent le format **YAML** (Yet Another Markup Language).

**Syntaxe de base :**
```yaml
# Commentaire
cle: valeur
liste:
  - element1
  - element2
objet:
  sous_cle: sous_valeur
```

**⚠️ Important :** L'indentation compte ! Utilisez 2 espaces (pas de tabs).

### **Structure minimale**

```yaml
name: Nom du workflow

on: [push, pull_request]

jobs:
  nom_du_job:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - run: echo "Hello World"
```

### **Structure complète**

```yaml
name: Nom du workflow

# Déclencheurs
on:
  push:
    branches: [ main ]
  pull_request:

# Variables d'environnement globales
env:
  CC: gcc
  CFLAGS: -Wall -Wextra

# Jobs
jobs:
  build:
    runs-on: ubuntu-latest

    # Variables d'environnement du job
    env:
      BUILD_TYPE: Debug

    # Étapes
    steps:
      # Cloner le repo
      - name: Checkout code
        uses: actions/checkout@v4

      # Installer des dépendances
      - name: Install dependencies
        run: sudo apt-get install -y gcc make

      # Compiler
      - name: Build
        run: make

      # Tester
      - name: Test
        run: make test
```

---

## Création de votre premier workflow

### **Étape 1 : Créer le fichier**

```bash
# Dans votre projet
mkdir -p .github/workflows
touch .github/workflows/ci.yml
```

### **Étape 2 : Écrire le workflow minimal**

`.github/workflows/ci.yml` :

```yaml
name: CI

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Install GCC
        run: sudo apt-get update && sudo apt-get install -y gcc

      - name: Compile
        run: gcc main.c -o main

      - name: Run
        run: ./main
```

### **Étape 3 : Commiter et pousser**

```bash
git add .github/workflows/ci.yml
git commit -m "Add CI workflow"
git push
```

### **Étape 4 : Voir le résultat**

1. Allez sur GitHub → Votre repository
2. Cliquez sur l'onglet **Actions**
3. Vous voyez votre workflow en cours d'exécution !

**Interface GitHub Actions :**
```
Actions
├── CI (votre workflow)
│   └── build (votre job)
│       ├── ✓ Checkout code
│       ├── ✓ Install GCC
│       ├── ✓ Compile
│       └── ✓ Run
└── Status: ✓ Success
```

---

## Template de base commenté

Voici un template complet et commenté pour un projet C simple :

`.github/workflows/ci.yml` :

```yaml
# ============================================
# Workflow CI pour projet C
# Déclenché sur push et pull request
# ============================================

name: CI

# -------------------- DÉCLENCHEURS --------------------

on:
  # Déclencher sur push vers certaines branches
  push:
    branches:
      - main
      - develop
      - 'feature/**'  # Toutes les branches feature/*

  # Déclencher sur pull request vers main
  pull_request:
    branches:
      - main

  # Permettre le déclenchement manuel
  workflow_dispatch:

# -------------------- VARIABLES GLOBALES --------------------

env:
  # Compilateur par défaut
  CC: gcc
  # Flags de compilation
  CFLAGS: -Wall -Wextra -Werror -std=c11
  # Type de build
  BUILD_TYPE: Debug

# -------------------- JOBS --------------------

jobs:
  # Job de compilation et tests
  build-and-test:
    # Nom affiché dans l'interface
    name: Build and Test

    # Runner Ubuntu 22.04
    runs-on: ubuntu-latest

    # Étapes du job
    steps:
      # ===== Checkout =====
      - name: 📥 Checkout code
        uses: actions/checkout@v4

      # ===== Installation des dépendances =====
      - name: 📦 Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y \
            gcc \
            g++ \
            make \
            cmake \
            valgrind \
            cppcheck

      # ===== Affichage des versions =====
      - name: 🔍 Display tool versions
        run: |
          echo "=== GCC Version ==="
          gcc --version
          echo "=== Make Version ==="
          make --version
          echo "=== CMake Version ==="
          cmake --version

      # ===== Compilation =====
      - name: 🔨 Build
        run: |
          mkdir -p build
          cd build
          cmake -DCMAKE_BUILD_TYPE=${{ env.BUILD_TYPE }} ..
          make -j$(nproc)

      # ===== Tests =====
      - name: 🧪 Run tests
        run: |
          cd build
          ctest --output-on-failure

      # ===== Upload du binaire =====
      - name: 📤 Upload binary
        uses: actions/upload-artifact@v4
        with:
          name: program-binary
          path: build/main
          retention-days: 7
```

---

## Workflows par cas d'usage

### **1. Compilation simple (GCC + Make)**

`.github/workflows/build.yml` :

```yaml
name: Build

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - name: Install GCC and Make
        run: sudo apt-get update && sudo apt-get install -y gcc make

      - name: Compile
        run: make

      - name: Upload binary
        uses: actions/upload-artifact@v4
        with:
          name: main
          path: main
```

### **2. Compilation avec CMake**

`.github/workflows/cmake.yml` :

```yaml
name: CMake Build

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y cmake build-essential

      - name: Configure CMake
        run: |
          mkdir build
          cd build
          cmake -DCMAKE_BUILD_TYPE=Release ..

      - name: Build
        run: cmake --build build --config Release

      - name: Test
        run: |
          cd build
          ctest --output-on-failure
```

### **3. Tests avec Valgrind**

`.github/workflows/valgrind.yml` :

```yaml
name: Valgrind Check

on: [push, pull_request]

jobs:
  valgrind:
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y gcc make valgrind

      - name: Build
        run: make

      - name: Run Valgrind
        run: |
          valgrind \
            --leak-check=full \
            --show-leak-kinds=all \
            --track-origins=yes \
            --error-exitcode=1 \
            ./main

      - name: Check Valgrind results
        if: failure()
        run: echo "❌ Memory leaks detected!"
```

### **4. Tests avec Sanitizers**

`.github/workflows/sanitizers.yml` :

```yaml
name: Sanitizers

on: [push, pull_request]

jobs:
  address-sanitizer:
    name: AddressSanitizer
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - name: Install dependencies
        run: sudo apt-get update && sudo apt-get install -y gcc make

      - name: Build with ASan
        run: gcc -fsanitize=address -fno-omit-frame-pointer -g main.c -o main

      - name: Run with ASan
        run: ./main

  undefined-sanitizer:
    name: UndefinedBehaviorSanitizer
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - name: Install dependencies
        run: sudo apt-get update && sudo apt-get install -y gcc make

      - name: Build with UBSan
        run: gcc -fsanitize=undefined -fno-omit-frame-pointer -g main.c -o main

      - name: Run with UBSan
        run: ./main
```

### **5. Analyse statique (cppcheck, clang-tidy)**

`.github/workflows/static-analysis.yml` :

```yaml
name: Static Analysis

on: [push, pull_request]

jobs:
  cppcheck:
    name: cppcheck
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - name: Install cppcheck
        run: sudo apt-get update && sudo apt-get install -y cppcheck

      - name: Run cppcheck
        run: |
          cppcheck \
            --enable=all \
            --inconclusive \
            --std=c11 \
            --error-exitcode=1 \
            --suppress=missingIncludeSystem \
            src/

  clang-tidy:
    name: clang-tidy
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - name: Install clang-tidy
        run: sudo apt-get update && sudo apt-get install -y clang-tidy

      - name: Generate compile_commands.json
        run: |
          mkdir build
          cd build
          cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..

      - name: Run clang-tidy
        run: |
          clang-tidy \
            -p build \
            src/*.c
```

### **6. Vérification du formatage (clang-format)**

`.github/workflows/format-check.yml` :

```yaml
name: Format Check

on: [push, pull_request]

jobs:
  format:
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - name: Install clang-format
        run: sudo apt-get update && sudo apt-get install -y clang-format

      - name: Check formatting
        run: |
          files=$(find . -name "*.c" -o -name "*.h")
          for file in $files; do
            clang-format --dry-run --Werror "$file"
          done

      - name: Show diff (if failed)
        if: failure()
        run: |
          echo "❌ Code non formaté. Exécutez :"
          echo "   clang-format -i \$(find . -name '*.c' -o -name '*.h')"
          echo ""
          echo "Différences :"
          files=$(find . -name "*.c" -o -name "*.h")
          for file in $files; do
            diff -u "$file" <(clang-format "$file") || true
          done
```

### **7. Couverture de code (gcov + lcov)**

`.github/workflows/coverage.yml` :

```yaml
name: Code Coverage

on: [push, pull_request]

jobs:
  coverage:
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y gcc make lcov

      - name: Build with coverage
        run: |
          gcc -fprofile-arcs -ftest-coverage main.c -o main

      - name: Run tests
        run: ./main

      - name: Generate coverage report
        run: |
          lcov --capture --directory . --output-file coverage.info
          lcov --remove coverage.info '/usr/*' --output-file coverage.info
          lcov --list coverage.info

      - name: Upload to Codecov
        uses: codecov/codecov-action@v4
        with:
          files: coverage.info
          token: ${{ secrets.CODECOV_TOKEN }}
```

---

## Matrix builds

Les **matrix builds** permettent de tester sur plusieurs configurations simultanément.

### **Tester plusieurs versions de GCC**

`.github/workflows/matrix-gcc.yml` :

```yaml
name: Matrix Build (GCC)

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest

    strategy:
      matrix:
        gcc-version: [9, 10, 11, 12, 13]

    steps:
      - uses: actions/checkout@v4

      - name: Install GCC ${{ matrix.gcc-version }}
        run: |
          sudo apt-get update
          sudo apt-get install -y gcc-${{ matrix.gcc-version }}
          sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${{ matrix.gcc-version }} 100

      - name: Display GCC version
        run: gcc --version

      - name: Build
        run: gcc -Wall -Wextra main.c -o main

      - name: Run
        run: ./main
```

### **Tester sur plusieurs OS**

`.github/workflows/matrix-os.yml` :

```yaml
name: Matrix Build (OS)

on: [push, pull_request]

jobs:
  build:
    strategy:
      matrix:
        os: [ubuntu-latest, ubuntu-22.04, ubuntu-20.04]

    runs-on: ${{ matrix.os }}

    steps:
      - uses: actions/checkout@v4

      - name: Install dependencies
        run: sudo apt-get update && sudo apt-get install -y gcc make

      - name: Build
        run: make

      - name: Test
        run: make test
```

### **Matrix complexe**

```yaml
name: Matrix Build (Complex)

on: [push, pull_request]

jobs:
  build:
    runs-on: ${{ matrix.os }}

    strategy:
      fail-fast: false  # Continue même si un job échoue
      matrix:
        os: [ubuntu-latest, macos-latest]
        compiler: [gcc, clang]
        build-type: [Debug, Release]
        exclude:
          # Exclure certaines combinaisons
          - os: macos-latest
            compiler: gcc

    steps:
      - uses: actions/checkout@v4

      - name: Install dependencies
        run: |
          if [ "${{ matrix.os }}" = "ubuntu-latest" ]; then
            sudo apt-get update
            sudo apt-get install -y ${{ matrix.compiler }} cmake
          else
            brew install cmake
          fi

      - name: Build
        run: |
          mkdir build
          cd build
          cmake -DCMAKE_BUILD_TYPE=${{ matrix.build-type }} \
                -DCMAKE_C_COMPILER=${{ matrix.compiler }} ..
          make
```

---

## Caching et optimisation

Le **caching** permet d'accélérer les builds en réutilisant des dépendances déjà téléchargées.

### **Cache des paquets apt**

```yaml
- name: Cache apt packages
  uses: actions/cache@v4
  with:
    path: /var/cache/apt/archives
    key: ${{ runner.os }}-apt-${{ hashFiles('**/apt-packages.txt') }}
    restore-keys: |
      ${{ runner.os }}-apt-
```

### **Cache CMake**

```yaml
- name: Cache CMake build
  uses: actions/cache@v4
  with:
    path: build
    key: ${{ runner.os }}-cmake-${{ hashFiles('**/CMakeLists.txt') }}
    restore-keys: |
      ${{ runner.os }}-cmake-
```

### **Cache des dépendances compilées**

```yaml
- name: Cache compiled dependencies
  uses: actions/cache@v4
  with:
    path: |
      ~/.cache
      vendor/
    key: ${{ runner.os }}-deps-${{ hashFiles('**/deps.txt') }}
```

### **Workflow avec cache complet**

```yaml
name: Build with Cache

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      # Cache des paquets apt
      - name: Cache apt packages
        uses: actions/cache@v4
        id: apt-cache
        with:
          path: /var/cache/apt/archives
          key: apt-${{ runner.os }}

      # Installation (seulement si pas en cache)
      - name: Install dependencies
        if: steps.apt-cache.outputs.cache-hit != 'true'
        run: |
          sudo apt-get update
          sudo apt-get install -y gcc make cmake

      # Cache du build CMake
      - name: Cache CMake build
        uses: actions/cache@v4
        with:
          path: build
          key: cmake-${{ hashFiles('**/CMakeLists.txt') }}

      # Build
      - name: Build
        run: |
          mkdir -p build
          cd build
          cmake ..
          make
```

---

## Artifacts

Les **artifacts** permettent de sauvegarder et partager des fichiers générés par le workflow.

### **Upload d'un binaire**

```yaml
- name: Upload binary
  uses: actions/upload-artifact@v4
  with:
    name: program
    path: build/main
    retention-days: 30  # Conserver 30 jours
```

### **Upload de plusieurs fichiers**

```yaml
- name: Upload artifacts
  uses: actions/upload-artifact@v4
  with:
    name: build-artifacts
    path: |
      build/main
      build/libutils.a
      docs/*.pdf
```

### **Download d'artifacts entre jobs**

```yaml
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - run: make
      - uses: actions/upload-artifact@v4
        with:
          name: binary
          path: main

  test:
    needs: build  # Attend que build soit terminé
    runs-on: ubuntu-latest
    steps:
      - uses: actions/download-artifact@v4
        with:
          name: binary
      - run: chmod +x main && ./main
```

### **Workflow complet avec artifacts**

```yaml
name: Build and Release

on:
  push:
    tags:
      - 'v*'  # Déclenché sur tags v1.0, v2.0, etc.

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - name: Install dependencies
        run: sudo apt-get update && sudo apt-get install -y gcc make

      - name: Build
        run: make release

      - name: Upload binary
        uses: actions/upload-artifact@v4
        with:
          name: program-${{ github.ref_name }}
          path: build/main

      - name: Create Release
        uses: softprops/action-gh-release@v1
        if: startsWith(github.ref, 'refs/tags/')
        with:
          files: build/main
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

---

## Intégration avec les outils C

### **Compilation avec différents flags**

```yaml
jobs:
  build-debug:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - run: gcc -Wall -Wextra -g -O0 main.c -o main-debug

  build-release:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - run: gcc -Wall -Wextra -O3 main.c -o main-release
```

### **Tests avec Criterion**

```yaml
jobs:
  test:
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - name: Install Criterion
        run: |
          sudo apt-get update
          sudo apt-get install -y meson ninja-build libffi-dev
          git clone --recursive https://github.com/Snaipe/Criterion
          cd Criterion
          meson build
          ninja -C build
          sudo ninja -C build install

      - name: Build tests
        run: gcc -lcriterion tests/*.c -o test_suite

      - name: Run tests
        run: ./test_suite --verbose
```

### **Documentation avec Doxygen**

```yaml
jobs:
  docs:
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - name: Install Doxygen
        run: sudo apt-get update && sudo apt-get install -y doxygen graphviz

      - name: Generate documentation
        run: doxygen Doxyfile

      - name: Upload docs
        uses: actions/upload-artifact@v4
        with:
          name: documentation
          path: docs/html/

      # Optionnel : Publier sur GitHub Pages
      - name: Deploy to GitHub Pages
        uses: peaceiris/actions-gh-pages@v3
        if: github.ref == 'refs/heads/main'
        with:
          github_token: ${{ secrets.GITHUB_TOKEN }}
          publish_dir: ./docs/html
```

---

## Secrets et variables

### **Qu'est-ce qu'un secret ?**

Un **secret** est une information sensible (token, mot de passe, clé API) stockée de manière sécurisée dans GitHub.

**Créer un secret :**
1. Repository → Settings → Secrets and variables → Actions
2. Cliquez sur "New repository secret"
3. Nom : `CODECOV_TOKEN`
4. Valeur : Votre token Codecov
5. Enregistrez

### **Utiliser un secret**

```yaml
- name: Upload to Codecov
  uses: codecov/codecov-action@v4
  with:
    token: ${{ secrets.CODECOV_TOKEN }}
```

### **Variables d'environnement**

```yaml
env:
  # Globales (tout le workflow)
  CC: gcc
  CFLAGS: -Wall -Wextra

jobs:
  build:
    env:
      # Spécifiques au job
      BUILD_TYPE: Release

    steps:
      - name: Build
        env:
          # Spécifiques au step
          EXTRA_FLAGS: -O3
        run: |
          echo "Compiler: $CC"
          echo "Flags: $CFLAGS $EXTRA_FLAGS"
          $CC $CFLAGS $EXTRA_FLAGS main.c -o main
```

### **Variables GitHub Actions**

GitHub fournit des variables prédéfinies :

| Variable | Description | Exemple |
|----------|-------------|---------|
| `${{ github.repository }}` | Nom du repo | `username/project` |
| `${{ github.ref }}` | Référence Git | `refs/heads/main` |
| `${{ github.ref_name }}` | Nom de la branche | `main` |
| `${{ github.sha }}` | SHA du commit | `abc123...` |
| `${{ github.actor }}` | Utilisateur qui a déclenché | `username` |
| `${{ runner.os }}` | OS du runner | `Linux` |

**Utilisation :**
```yaml
- name: Display info
  run: |
    echo "Repository: ${{ github.repository }}"
    echo "Branch: ${{ github.ref_name }}"
    echo "Commit: ${{ github.sha }}"
    echo "Actor: ${{ github.actor }}"
```

---

## Badges de statut

Les **badges** affichent le statut de vos workflows dans votre README.

### **Badge de base**

```markdown
![CI](https://github.com/username/repo/workflows/CI/badge.svg)
```

### **Badge avec branche spécifique**

```markdown
![CI](https://github.com/username/repo/workflows/CI/badge.svg?branch=main)
```

### **Badge de couverture (Codecov)**

```markdown
[![codecov](https://codecov.io/gh/username/repo/branch/main/graph/badge.svg)](https://codecov.io/gh/username/repo)
```

### **Plusieurs badges**

```markdown
# Mon Projet C

![CI](https://github.com/username/repo/workflows/CI/badge.svg)
![Valgrind](https://github.com/username/repo/workflows/Valgrind/badge.svg)
[![codecov](https://codecov.io/gh/username/repo/branch/main/graph/badge.svg)](https://codecov.io/gh/username/repo)
![License](https://img.shields.io/badge/license-MIT-blue.svg)
```

**Rendu :**

![CI](https://img.shields.io/badge/CI-passing-brightgreen) ![Valgrind](https://img.shields.io/badge/Valgrind-passing-brightgreen) ![Coverage](https://img.shields.io/badge/coverage-95%25-brightgreen) ![License](https://img.shields.io/badge/license-MIT-blue)

---

## Bonnes pratiques

### **✅ À faire**

1. **Nommer clairement les workflows et jobs**
   ```yaml
   name: CI - Build and Test
   jobs:
     build-gcc-11:
       name: Build with GCC 11
   ```

2. **Utiliser des actions officielles**
   ```yaml
   - uses: actions/checkout@v4       # ✅ Officiel Microsoft
   - uses: actions/cache@v4          # ✅ Officiel
   ```

3. **Épingler les versions des actions**
   ```yaml
   - uses: actions/checkout@v4       # ✅ Version majeure
   # Ou encore mieux :
   - uses: actions/checkout@8e5e7e5 # ✅ SHA spécifique
   ```

4. **Utiliser le caching**
   ```yaml
   - uses: actions/cache@v4
     with:
       path: ~/.cache
       key: ${{ runner.os }}-cache
   ```

5. **Paralléliser quand possible**
   ```yaml
   jobs:
     build:     # S'exécute en parallèle
     test:      # S'exécute en parallèle
     lint:      # S'exécute en parallèle
   ```

6. **Tester sur plusieurs configurations (matrix)**
   ```yaml
   strategy:
     matrix:
       gcc: [9, 10, 11, 12]
   ```

7. **Définir des timeouts**
   ```yaml
   jobs:
     build:
       timeout-minutes: 10  # Évite les jobs bloqués
   ```

8. **Utiliser `fail-fast: false` pour les matrix**
   ```yaml
   strategy:
     fail-fast: false  # Continue même si un job échoue
     matrix:
       os: [ubuntu, macos]
   ```

### **❌ À éviter**

1. **Ne pas commiter de secrets**
   ```yaml
   # ❌ JAMAIS
   env:
     API_KEY: sk-1234567890abcdef

   # ✅ À la place
   env:
     API_KEY: ${{ secrets.API_KEY }}
   ```

2. **Ne pas utiliser `sudo` inutilement**
   ```yaml
   # ❌ Éviter
   - run: sudo apt-get install gcc

   # ✅ Préférer (runners Ubuntu ont déjà sudo)
   - run: |
       sudo apt-get update
       sudo apt-get install -y gcc
   ```

3. **Ne pas ignorer les erreurs**
   ```yaml
   # ❌ Mauvais
   - run: make || true  # Continue même si échec

   # ✅ Bon
   - run: make
   ```

4. **Ne pas dupliquer du code**
   ```yaml
   # ❌ Duplication
   jobs:
     build-ubuntu:
       steps:
         - run: gcc main.c
     build-macos:
       steps:
         - run: gcc main.c

   # ✅ Utiliser une matrix
   jobs:
     build:
       strategy:
         matrix:
           os: [ubuntu-latest, macos-latest]
       runs-on: ${{ matrix.os }}
       steps:
         - run: gcc main.c
   ```

---

## Dépannage

### **Problème 1 : Workflow ne se déclenche pas**

**Vérifications :**

1. **Le fichier est bien dans `.github/workflows/` ?**
   ```bash
   ls .github/workflows/
   ```

2. **Le YAML est valide ?**
   - Utilisez un validateur : https://www.yamllint.com/
   - Vérifiez l'indentation (2 espaces, pas de tabs)

3. **Le déclencheur est correct ?**
   ```yaml
   on:
     push:
       branches: [ main ]  # Vérifier le nom de la branche
   ```

4. **Le workflow est activé ?**
   - Repository → Actions → Vérifier qu'Actions est activé

### **Problème 2 : Job échoue avec "command not found"**

**Erreur :**
```
gcc: command not found
```

**Solution :** Installer les dépendances
```yaml
- name: Install dependencies
  run: sudo apt-get update && sudo apt-get install -y gcc
```

### **Problème 3 : Permissions refusées**

**Erreur :**
```
Permission denied: ./main
```

**Solution :** Rendre le fichier exécutable
```yaml
- name: Make executable
  run: chmod +x ./main

- name: Run
  run: ./main
```

### **Problème 4 : Cache ne fonctionne pas**

**Vérifier :**
1. La clé de cache change à chaque fois ?
2. Le chemin est correct ?

```yaml
- uses: actions/cache@v4
  with:
    path: build/              # Vérifier que ce dossier existe
    key: ${{ runner.os }}-${{ hashFiles('**/Makefile') }}
```

### **Problème 5 : Workflow trop lent**

**Optimisations :**

1. **Utiliser le cache**
   ```yaml
   - uses: actions/cache@v4
   ```

2. **Paralléliser**
   ```yaml
   jobs:
     job1:  # Parallèle
     job2:  # Parallèle
   ```

3. **Limiter les installations**
   ```yaml
   # ❌ Lent
   - run: sudo apt-get install -y build-essential

   # ✅ Plus rapide (installer seulement ce qui est nécessaire)
   - run: sudo apt-get install -y gcc make
   ```

4. **Utiliser des images Docker pré-configurées**
   ```yaml
   jobs:
     build:
       container:
         image: gcc:11  # Image Docker avec GCC déjà installé
   ```

### **Problème 6 : Artefacts non uploadés**

**Vérifications :**

1. **Le chemin est correct ?**
   ```yaml
   - uses: actions/upload-artifact@v4
     with:
       name: binary
       path: build/main  # Vérifier que ce fichier existe
   ```

2. **Le fichier existe vraiment ?**
   ```yaml
   - name: Verify binary exists
     run: ls -lh build/main

   - name: Upload
     uses: actions/upload-artifact@v4
     with:
       name: binary
       path: build/main
   ```

---

## Exemples complets

### **Exemple 1 : Projet simple (GCC + Make)**

`.github/workflows/ci.yml` :

```yaml
name: CI

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  build-and-test:
    runs-on: ubuntu-latest

    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y gcc make

      - name: Build
        run: make

      - name: Test
        run: make test

      - name: Upload binary
        uses: actions/upload-artifact@v4
        with:
          name: program
          path: main
```

### **Exemple 2 : Projet complet (CMake + Tests + Valgrind + Coverage)**

`.github/workflows/complete-ci.yml` :

```yaml
name: Complete CI

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

env:
  BUILD_TYPE: Debug

jobs:
  build:
    name: Build
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y \
            gcc \
            cmake \
            ninja-build

      - name: Configure CMake
        run: |
          cmake -B build -G Ninja \
            -DCMAKE_BUILD_TYPE=${{ env.BUILD_TYPE }} \
            -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

      - name: Build
        run: cmake --build build

      - name: Upload binary
        uses: actions/upload-artifact@v4
        with:
          name: program
          path: build/main

  test:
    name: Test
    needs: build
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4
      - uses: actions/download-artifact@v4
        with:
          name: program

      - name: Run tests
        run: |
          chmod +x main
          ./main

  valgrind:
    name: Valgrind Check
    needs: build
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4
      - uses: actions/download-artifact@v4
        with:
          name: program

      - name: Install Valgrind
        run: sudo apt-get update && sudo apt-get install -y valgrind

      - name: Run Valgrind
        run: |
          chmod +x main
          valgrind --leak-check=full --error-exitcode=1 ./main

  coverage:
    name: Code Coverage
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y gcc lcov

      - name: Build with coverage
        run: |
          gcc -fprofile-arcs -ftest-coverage main.c -o main

      - name: Run tests
        run: ./main

      - name: Generate coverage
        run: |
          lcov --capture --directory . --output-file coverage.info
          lcov --remove coverage.info '/usr/*' --output-file coverage.info

      - name: Upload to Codecov
        uses: codecov/codecov-action@v4
        with:
          files: coverage.info
          token: ${{ secrets.CODECOV_TOKEN }}
```

### **Exemple 3 : Matrix build complet**

`.github/workflows/matrix.yml` :

```yaml
name: Matrix Build

on: [push, pull_request]

jobs:
  build:
    runs-on: ${{ matrix.os }}

    strategy:
      fail-fast: false
      matrix:
        os: [ubuntu-22.04, ubuntu-20.04]
        compiler: [gcc, clang]
        version: [9, 10, 11, 12]
        build-type: [Debug, Release]
        exclude:
          # Exclure combinaisons non supportées
          - os: ubuntu-20.04
            version: 12

    steps:
      - uses: actions/checkout@v4

      - name: Install compiler
        run: |
          sudo apt-get update
          if [ "${{ matrix.compiler }}" = "gcc" ]; then
            sudo apt-get install -y gcc-${{ matrix.version }}
            echo "CC=gcc-${{ matrix.version }}" >> $GITHUB_ENV
          else
            sudo apt-get install -y clang-${{ matrix.version }}
            echo "CC=clang-${{ matrix.version }}" >> $GITHUB_ENV
          fi

      - name: Build
        run: |
          $CC -Wall -Wextra \
            $([ "${{ matrix.build-type }}" = "Debug" ] && echo "-g -O0" || echo "-O3") \
            main.c -o main

      - name: Test
        run: ./main
```

---

## Ressources complémentaires

### **Documentation officielle**

- 📚 [GitHub Actions Documentation](https://docs.github.com/en/actions)
- 📚 [Workflow syntax](https://docs.github.com/en/actions/reference/workflow-syntax-for-github-actions)
- 📚 [Actions Marketplace](https://github.com/marketplace?type=actions)

### **Actions utiles pour C**

- 🔧 [actions/checkout](https://github.com/actions/checkout) - Clone le repository
- 🔧 [actions/cache](https://github.com/actions/cache) - Cache des dépendances
- 🔧 [actions/upload-artifact](https://github.com/actions/upload-artifact) - Upload d'artefacts
- 🔧 [codecov/codecov-action](https://github.com/codecov/codecov-action) - Upload vers Codecov
- 🔧 [softprops/action-gh-release](https://github.com/softprops/action-gh-release) - Créer des releases

### **Exemples de projets C avec CI**

- 🔗 [Redis](https://github.com/redis/redis/tree/unstable/.github/workflows)
- 🔗 [Git](https://github.com/git/git/tree/master/.github/workflows)
- 🔗 [Curl](https://github.com/curl/curl/tree/master/.github/workflows)

### **Outils**

- 🛠️ [act](https://github.com/nektos/act) - Tester GitHub Actions localement
- 🛠️ [actionlint](https://github.com/rhysd/actionlint) - Linter pour workflows

---

## Conclusion

GitHub Actions est un outil puissant pour automatiser votre workflow de développement C. Avec les templates fournis, vous pouvez :

- ✨ **Compiler automatiquement** après chaque commit
- ✨ **Exécuter les tests** pour détecter les régressions
- ✨ **Analyser le code** avec Valgrind, sanitizers, cppcheck
- ✨ **Mesurer la couverture** et l'afficher avec des badges
- ✨ **Tester sur plusieurs configurations** (OS, versions GCC)
- ✨ **Publier des releases** automatiquement

**Prochaines étapes :**

1. ✅ Créez un fichier `.github/workflows/ci.yml` dans votre projet
2. ✅ Commencez avec un template simple (compilation de base)
3. ✅ Ajoutez progressivement des fonctionnalités (tests, Valgrind, etc.)
4. ✅ Ajoutez des badges dans votre README
5. ✅ Explorez les actions du Marketplace
6. ✅ Partagez vos workflows avec la communauté

**Astuce finale :** Ne construisez pas un workflow parfait dès le début. Commencez simple, et ajoutez des étapes au fur et à mesure que vous en avez besoin.

---

🚀 **Bon CI/CD avec GitHub Actions !**

⏭️ [Glossaire](/annexes/glossaire/README.md)
