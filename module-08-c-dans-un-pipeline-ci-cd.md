🔝 Retour au [Sommaire](/SOMMAIRE.md)

# Module 8 : C dans un Pipeline CI/CD

> **Formation : Maîtriser C sur Ubuntu - De la Syntaxe au Système**
> Niveau : **Bonus DevOps** ⭐ • Durée estimée : **30-40 heures de mise en pratique DevOps**

---

## 🎯 Objectifs du module

Vous maîtrisez le C, les outils de qualité, et la programmation système (Modules 1-7). Ce module **bonus** vous montre comment intégrer tout cela dans un **pipeline CI/CD moderne**.

En 2025, le C n'est plus développé de manière isolée. Il fait partie d'écosystèmes DevOps complexes avec tests automatisés, intégration continue, déploiement automatisé, et monitoring.

À l'issue de ce module, vous serez capable de :

1. ✅ **Écrire et automatiser des tests** : unitaires, intégration, TDD
2. ✅ **Configurer des pipelines CI/CD** : GitHub Actions, GitLab CI, Jenkins
3. ✅ **Automatiser la qualité** : sanitizers, Valgrind, analyse statique, coverage
4. ✅ **Cross-compiler** : ARM, RISC-V, architectures embarquées
5. ✅ **Packager** : .deb, .rpm, AppImage, Docker multi-stage
6. ✅ **Documenter** : Doxygen, README, changelog, versioning sémantique
7. ✅ **Maintenir du code legacy** : refactoring progressif, modernisation

---

## 🚀 Pourquoi ce module est crucial en 2025

### Le développement C a changé

**Année 2000** : Workflow C traditionnel
```
1. Écrire du code
2. Compiler localement
3. Tester manuellement
4. "Ça marche chez moi"
5. Tar.gz et envoyer par email
6. Le client compile et croise les doigts
```

**Année 2025** : Workflow C DevOps moderne
```
1. Écrire du code avec tests
2. Commit → Push
3. CI déclenché automatiquement :
   - Build sur 3 architectures (x86, ARM, RISC-V)
   - Tests unitaires (100+ tests)
   - Sanitizers (ASan, UBSan, TSan)
   - Valgrind memcheck
   - Analyse statique (cppcheck, clang-tidy)
   - Code coverage (>80% requis)
4. Si tout est vert → artifact généré automatiquement
5. CD : Déploiement automatique ou release GitHub
6. Monitoring et feedback en production
```

### Le C dans l'écosystème DevOps

**Le C n'est plus un langage "vieux" ou "legacy"** :

- 🐳 **Docker/Kubernetes** : Écrits en Go mais embarquent des composants C (runc, containerd)
- 📊 **Prometheus** : Les exporters système sont souvent en C
- 🔥 **eBPF** : Révolutionne le monitoring (Cilium, Pixie, Falco)
- ⚡ **Performance** : Les parties critiques de Python, Go, Rust sont en C
- 🔧 **Outils système** : CLI, agents, démons pour DevOps

**Le C s'intègre dans les stacks modernes**, il ne les remplace pas.

---

## 🧩 Place dans la formation globale

### Vous avez construit toute l'expertise

**Modules 1-7** vous ont donné :
- ✅ Maîtrise du langage C
- ✅ Gestion mémoire et structures de données
- ✅ Outils de qualité (CMake, sanitizers, Valgrind)
- ✅ Programmation système Linux
- ✅ Techniques avancées et optimisation

**Mais vous développez encore de manière isolée.**

### Ce module connecte le C au monde DevOps

```
Développement isolé          DevOps moderne
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Code sur votre machine       Code dans le CI/CD
Tests manuels                Tests automatisés
Build ad-hoc                 Build reproductible
Qualité "au feeling"         Qualité mesurée
Déploiement manuel           Déploiement automatisé
Pas de feedback              Feedback continu
```

### Pour qui est ce module ?

**Développeurs C** qui veulent :
- ✅ Travailler dans des équipes DevOps modernes
- ✅ Contribuer à des projets open-source (avec CI/CD)
- ✅ Automatiser leur workflow de qualité
- ✅ Créer des outils pour l'infrastructure

**Ingénieurs DevOps** qui ont du code C dans leur stack et veulent :
- ✅ Automatiser le build et les tests
- ✅ Intégrer du C dans leurs pipelines existants
- ✅ Assurer la qualité du code C
- ✅ Cross-compiler pour différentes architectures

**SRE/Platform Engineers** qui :
- ✅ Maintiennent des outils système en C
- ✅ Créent des agents de monitoring
- ✅ Doivent packager et distribuer des binaires

---

## 📚 Structure du module

Ce module est organisé en **quatre chapitres pratiques** qui couvrent l'ensemble du cycle DevOps pour le C :

### 🧪 [Chapitre 29 : Tests et Qualité](29-tests-et-qualite/README.md)
**Contenu** : 7 sections • **Niveau** : Pratique • **Durée** : 8-10 heures

**Vous apprendrez :**
- Philosophie du testing en C (pourquoi tester est différent en C)
- Frameworks de tests : Check, Unity, Criterion, cmocka
- Écriture de tests unitaires et assertions
- Mocking et stubbing (difficile en C, mais faisable)
- Tests d'intégration
- Mesure de couverture (gcov, lcov)
- Test-Driven Development (TDD) en C

**Concepts clés :**
```c
// Test unitaire avec Check
#include <check.h>

START_TEST(test_addition) {
    int result = add(2, 3);
    ck_assert_int_eq(result, 5);
}
END_TEST

Suite* math_suite(void) {
    Suite *s = suite_create("Math");
    TCase *tc = tcase_create("Core");
    tcase_add_test(tc, test_addition);
    suite_add_tcase(s, tc);
    return s;
}

// CMakeLists.txt pour tests
enable_testing()
add_executable(test_math test_math.c)
target_link_libraries(test_math check m pthread)
add_test(NAME MathTests COMMAND test_math)
```

**Pourquoi c'est crucial :**
- 🐛 **Confiance** : Refactoriser sans peur de casser
- 📊 **Qualité mesurable** : Coverage > 80% = standard industrie
- 🔄 **Régression** : Détecter les bugs introduits
- 📝 **Documentation** : Les tests documentent le comportement attendu

**Défi spécifique au C :**
- Pas de réflexion → pas de mocking automatique
- Gestion mémoire → tests doivent vérifier les fuites
- Pointeurs → tests plus complexes
- Mais faisable avec les bons outils !

---

### 🔄 [Chapitre 30 : Intégration Continue moderne](30-integration-continue/README.md) 🔥
**Contenu** : 5 sections majeures • **Niveau** : DevOps • **Durée** : 12-15 heures

**LE CŒUR DU MODULE**

**Section 30.1 : GitHub Actions pour projets C** 🔥
- Structure d'un workflow (.github/workflows/)
- Matrix build (Ubuntu 20.04/22.04/24.04, GCC 9/10/11/12)
- Build avec CMake dans le CI
- Exécution des tests unitaires
- Vérification avec sanitizers automatique
- Analyse Valgrind automatisée
- Code coverage et upload vers Codecov
- Analyse statique intégrée
- Publication d'artifacts (binaires, packages)

**Workflow GitHub Actions complet :**
```yaml
# .github/workflows/ci.yml
name: CI

on: [push, pull_request]

jobs:
  build-and-test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        compiler: [gcc-11, gcc-12, clang-14]
        build_type: [Debug, Release]

    steps:
    - uses: actions/checkout@v3

    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake valgrind libcheck-dev

    - name: Configure
      run: |
        cmake -B build \
              -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} \
              -DCMAKE_C_COMPILER=${{ matrix.compiler }} \
              -DENABLE_ASAN=ON \
              -DENABLE_COVERAGE=ON

    - name: Build
      run: cmake --build build -j$(nproc)

    - name: Test
      run: ctest --test-dir build --output-on-failure

    - name: Valgrind
      if: matrix.build_type == 'Debug'
      run: valgrind --leak-check=full --error-exitcode=1 ./build/myapp

    - name: Coverage
      if: matrix.build_type == 'Debug' && matrix.compiler == 'gcc-12'
      run: |
        lcov --capture --directory . --output-file coverage.info
        bash <(curl -s https://codecov.io/bash)

    - name: Static Analysis
      run: |
        cppcheck --enable=all --error-exitcode=1 src/
        clang-tidy src/*.c -- -Iinclude

    - name: Upload Artifacts
      uses: actions/upload-artifact@v3
      with:
        name: myapp-${{ matrix.compiler }}-${{ matrix.build_type }}
        path: build/myapp
```

**Badges pour README :**
```markdown
![CI](https://github.com/user/repo/workflows/CI/badge.svg)
[![codecov](https://codecov.io/gh/user/repo/branch/main/graph/badge.svg)](https://codecov.io/gh/user/repo)
```

**Section 30.2 : GitLab CI pour projets C**

**Section 30.3 : Jenkins - Pipelines déclaratifs**

**Section 30.4 : Gestion des caches** (ccache pour accélérer les builds)

**Section 30.5 : Notifications et reporting**

**Pourquoi c'est crucial :**
- 🚀 **Vitesse** : Feedback en 5 minutes au lieu de "ça marche chez moi"
- 🎯 **Qualité** : Impossible de merger si les tests échouent
- 🔒 **Sécurité** : Sanitizers et Valgrind sur chaque commit
- 📊 **Visibilité** : Toute l'équipe voit l'état du code
- 🌍 **Portabilité** : Tests sur plusieurs OS/architectures automatiquement

---

### 🌐 [Chapitre 31 : Cross-compilation et Déploiement](31-cross-compilation-deploiement/README.md)
**Contenu** : 9 sections • **Niveau** : Avancé DevOps • **Durée** : 10-12 heures

**Vous maîtriserez :**
- Principes de la cross-compilation
- Compiler pour ARM depuis x86 (Raspberry Pi, embarqué)
- CMake et cross-compilation (toolchain files)
- QEMU pour tester les binaires cross-compilés
- Packaging Linux : .deb (Debian/Ubuntu)
- Packaging Linux : .rpm (RedHat/CentOS)
- AppImage (distribution universelle)
- Docker multi-stage builds pour C
- Distribution de bibliothèques partagées

**Concepts clés :**
```cmake
# Toolchain file pour ARM
# arm-toolchain.cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)

# Build
cmake -B build -DCMAKE_TOOLCHAIN_FILE=arm-toolchain.cmake
cmake --build build
```

**Docker multi-stage pour C :**
```dockerfile
# Stage 1: Build
FROM ubuntu:22.04 AS builder
RUN apt-get update && apt-get install -y cmake gcc
COPY . /src
WORKDIR /src
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build

# Stage 2: Runtime (minimal)
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y libc6
COPY --from=builder /src/build/myapp /usr/local/bin/
ENTRYPOINT ["/usr/local/bin/myapp"]
```

**Packaging .deb :**
```bash
# Structure
myapp_1.0.0/
├── DEBIAN/
│   └── control
└── usr/
    └── bin/
        └── myapp

# control file
Package: myapp
Version: 1.0.0
Architecture: amd64
Maintainer: Your Name <you@example.com>
Description: My awesome C application

# Build
dpkg-deb --build myapp_1.0.0
```

**Pourquoi c'est crucial :**
- 🌍 **Multi-architecture** : x86, ARM, RISC-V
- 📦 **Distribution** : Utilisateurs peuvent installer facilement
- 🐳 **Containers** : Intégration Kubernetes/Docker
- 🚀 **Déploiement** : Automatiser la mise en production

---

### 📝 [Chapitre 32 : Documentation et Maintenance](32-documentation-maintenance/README.md)
**Contenu** : 5 sections • **Niveau** : Pratique • **Durée** : 6-8 heures

**Vous apprendrez :**
- Documentation du code avec Doxygen
  - Syntaxe des commentaires
  - Génération HTML/PDF automatique
  - Intégration dans le CI
- README et documentation utilisateur
- Gestion de versions avec Git
  - Commits atomiques
  - Branches et workflow (Git Flow, GitHub Flow)
  - Gestion des binaires (.gitignore, Git LFS)
- Changelog et versioning sémantique (SemVer)
- Maintenance du code legacy
  - Refactoring progressif
  - Ajout de tests sur code existant
  - Modernisation (C99 → C11 → C17)

**Concepts clés :**
```c
/**
 * @file database.h
 * @brief Database connection and query interface
 * @author Your Name
 * @date 2025-01-01
 */

/**
 * @brief Open a database connection
 *
 * Opens a connection to the specified database file. The connection
 * must be closed with db_close() when no longer needed.
 *
 * @param path Path to the database file
 * @return Database handle on success, NULL on failure
 * @note The caller is responsible for calling db_close()
 * @warning This function is not thread-safe
 *
 * @code
 * Database *db = db_open("/var/lib/myapp.db");
 * if (!db) {
 *     perror("db_open");
 *     return -1;
 * }
 * // Use database...
 * db_close(db);
 * @endcode
 */
Database* db_open(const char *path);
```

**Versioning sémantique (SemVer) :**
```
MAJOR.MINOR.PATCH (ex: 2.4.1)

MAJOR : Changements incompatibles (breaking changes)
MINOR : Nouvelles fonctionnalités (backward compatible)
PATCH : Bug fixes (backward compatible)

1.2.3 → 1.2.4  (bug fix)
1.2.4 → 1.3.0  (nouvelle feature)
1.3.0 → 2.0.0  (breaking change)
```

**README structure :**
```markdown
# Project Name

Brief description

## Features
- Feature 1
- Feature 2

## Installation
```bash
cmake -B build
cmake --build build
sudo cmake --install build
```

## Usage
```c
#include <mylib.h>
// Example
```

## Building from Source
Requirements:
- CMake 3.15+
- GCC 9+ or Clang 10+

## Running Tests
```bash
ctest --test-dir build
```

## License
MIT License
```

**Pourquoi c'est crucial :**
- 👥 **Collaboration** : Autres peuvent comprendre et contribuer
- 📚 **Maintenance** : Votre futur vous remerciera
- 🔄 **Onboarding** : Nouveaux développeurs productifs rapidement
- 📊 **Professionnalisme** : Documentation = projet sérieux

---

## 🎓 Prérequis

### Absolument nécessaire
- ✅ **Module 5 complété** : Maîtrise de CMake, sanitizers, Valgrind
- ✅ **Expérience Git** : Commits, branches, pull requests
- ✅ **Compréhension CI/CD** : Concepts de base
- ✅ **Ligne de commande** : Bash, scripting

### Fortement recommandé
- ✅ **Compte GitHub/GitLab** : Pour pratiquer les workflows
- ✅ **Notions Docker** : Comprendre les containers
- ✅ **Expérience d'un projet C complet** : Au moins un projet de 1000+ lignes

### Nice to have
- ☑️ Expérience Jenkins
- ☑️ Connaissance de Kubernetes
- ☑️ Utilisation de Codecov/Coveralls

---

## 🗺️ Approche d'apprentissage recommandée

### Ce module est PRATIQUE avant tout

**80% du temps = faire, 20% = lire**

### Workflow d'apprentissage

#### 1. **Créez un projet de référence**
```bash
mkdir ~/c-devops-template
cd ~/c-devops-template

# Structure complète
mkdir -p {src,include,tests,.github/workflows}

# CMakeLists.txt moderne (du Module 5)
# Tests unitaires (Chapitre 29)
# GitHub Actions (Chapitre 30)
# Dockerfile (Chapitre 31)
# README et Doxygen (Chapitre 32)
```

#### 2. **Itérez progressivement**
```
Jour 1: Tests unitaires avec Check
Jour 2: GitHub Actions basique (build + test)
Jour 3: Ajouter sanitizers dans le CI
Jour 4: Ajouter Valgrind
Jour 5: Code coverage + Codecov
Jour 6: Analyse statique
Jour 7: Artifacts et releases
```

#### 3. **Appliquez à vos projets existants**
Prenez un projet personnel et ajoutez progressivement :
- Tests (si pas déjà fait)
- CI avec GitHub Actions
- Coverage badges
- Documentation Doxygen

#### 4. **Contribuez à l'open-source**
Cherchez des projets C qui n'ont pas de CI/CD et proposez d'en ajouter un.

---

## 🛠️ Configuration initiale

### Setup GitHub Actions

```bash
# Créer le workflow
mkdir -p .github/workflows
cat > .github/workflows/ci.yml << 'EOF'
name: CI
on: [push, pull_request]
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3
    - name: Install
      run: sudo apt-get install -y cmake
    - name: Build
      run: |
        cmake -B build
        cmake --build build
    - name: Test
      run: ctest --test-dir build
EOF

git add .github/workflows/ci.yml
git commit -m "Add GitHub Actions CI"
git push
```

### Setup Codecov

```bash
# Dans GitHub Actions, ajouter :
- name: Coverage
  run: |
    cmake -B build -DCMAKE_BUILD_TYPE=Coverage
    cmake --build build
    ctest --test-dir build
    lcov --capture --directory . --output-file coverage.info
    bash <(curl -s https://codecov.io/bash)

# Badge dans README
[![codecov](https://codecov.io/gh/user/repo/branch/main/graph/badge.svg)](https://codecov.io/gh/user/repo)
```

---

## 🏆 Critères de maîtrise

### ✅ Niveau 1 : Tests
- [ ] Écrire des tests unitaires avec un framework
- [ ] Mesurer la couverture de code
- [ ] Utiliser TDD pour une nouvelle fonctionnalité
- [ ] Tester sans fuites mémoire (Valgrind dans les tests)

### ✅ Niveau 2 : CI/CD
- [ ] Configurer GitHub Actions pour un projet C
- [ ] Matrix build (plusieurs compilateurs/versions)
- [ ] Intégrer sanitizers dans le CI
- [ ] Automatiser l'analyse statique
- [ ] Publier des artifacts

### ✅ Niveau 3 : Packaging et distribution
- [ ] Cross-compiler pour ARM
- [ ] Créer un package .deb
- [ ] Créer un Docker multi-stage
- [ ] Automatiser les releases GitHub

### ✅ Niveau 4 : Projet complet DevOps
- [ ] Projet C avec 80%+ coverage
- [ ] CI/CD complet (build, test, analyse, deploy)
- [ ] Documentation Doxygen générée automatiquement
- [ ] Releases automatisées avec changelog
- [ ] Badges de qualité (CI, coverage, etc.)

---

## 📚 Ressources complémentaires

### Documentation CI/CD
- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [GitLab CI/CD Documentation](https://docs.gitlab.com/ee/ci/)
- [Jenkins Documentation](https://www.jenkins.io/doc/)

### Frameworks de tests
- [Check](https://libcheck.github.io/check/)
- [Unity](http://www.throwtheswitch.org/unity)
- [Criterion](https://github.com/Snaipe/Criterion)

### Outils
- [Codecov](https://codecov.io/) - Code coverage
- [Coveralls](https://coveralls.io/) - Alternative à Codecov
- [Doxygen](https://www.doxygen.nl/) - Documentation

### Exemples de projets avec CI/CD excellent
- [jq](https://github.com/stedolan/jq) - JSON processor
- [Redis](https://github.com/redis/redis) - In-memory database
- [curl](https://github.com/curl/curl) - Network tool

---

## 💡 Bonnes pratiques DevOps pour C

### 1. **Fail fast**
```yaml
# Dans GitHub Actions
- name: Build
  run: cmake --build build
  # Si le build échoue, le workflow s'arrête immédiatement
```

### 2. **Cache intelligemment**
```yaml
# Cache ccache pour accélérer les builds
- name: Cache
  uses: actions/cache@v3
  with:
    path: ~/.ccache
    key: ${{ runner.os }}-ccache-${{ hashFiles('**/*.c') }}
```

### 3. **Tests parallèles**
```bash
# Exécuter les tests en parallèle
ctest --test-dir build -j$(nproc)
```

### 4. **Artifacts significatifs**
```yaml
- name: Upload
  uses: actions/upload-artifact@v3
  with:
    name: myapp-ubuntu-22.04-amd64
    path: |
      build/myapp
      build/*.so
```

### 5. **Documentation vivante**
```bash
# Générer la doc à chaque push
doxygen Doxyfile
# Publier sur GitHub Pages automatiquement
```

---

## 🗓️ Planning suggéré

### Approche intensive (temps plein)
| Semaine | Contenu | Livrables |
|---------|---------|-----------|
| **Semaine 1** | Chapitre 29 : Tests | Tests unitaires fonctionnels |
| **Semaine 2-3** | Chapitre 30 : CI/CD | Pipeline GitHub Actions complet |
| **Semaine 4** | Chapitre 31 : Cross-compilation | Binaires ARM + packages |
| **Semaine 5** | Chapitre 32 : Documentation | Doc Doxygen + README pro |

### Approche à temps partiel (10h/semaine)
| Semaine | Contenu |
|---------|---------|
| **Semaines 1-2** | Chapitre 29 |
| **Semaines 3-6** | Chapitre 30 |
| **Semaines 7-9** | Chapitre 31 |
| **Semaines 10-11** | Chapitre 32 |

---

## 📑 Prochaines sections

**Automatisez tout votre workflow C :**

- **[Chapitre 29 : Tests et Qualité →](29-tests-et-qualite/README.md)**
- [Chapitre 30 : Intégration Continue moderne](30-integration-continue/README.md) 🔥
- [Chapitre 31 : Cross-compilation et Déploiement](31-cross-compilation-deploiement/README.md)
- [Chapitre 32 : Documentation et Maintenance](32-documentation-maintenance/README.md)

---

## 🎯 Mot de conclusion

Ce module transforme votre code C en **produit professionnel**.

**Avant ce module** : Code qui fonctionne sur votre machine
**Après ce module** : Code testé, automatisé, documenté, déployable

Ces compétences DevOps C sont **extrêmement rares** sur le marché. La plupart des développeurs C ne savent pas :
- Écrire des tests automatisés
- Configurer un pipeline CI/CD
- Cross-compiler pour l'embarqué
- Packager proprement

**Vous serez dans les 1% qui maîtrisent à la fois C et DevOps moderne.**

Les entreprises cherchent désespérément des profils comme le vôtre pour :
- Moderniser leur legacy C
- Créer des outils d'infrastructure
- Maintenir des systèmes critiques
- Construire des pipelines pour code C/C++

---

**Automatisez, testez, déployez ! 🚀**

---

## 📝 Note finale

Ce module est 80% pratique. **Ne vous contentez pas de lire** :

- **Créez** un template de projet avec CI/CD complet
- **Testez** chaque fonctionnalité (GitHub Actions, GitLab CI, etc.)
- **Appliquez** à vos projets existants
- **Partagez** vos templates (ils sont réutilisables)
- **Contribuez** à l'open-source avec vos compétences DevOps

Le **Module 9** (Projets de synthèse) vous permettra d'appliquer TOUTES les compétences de cette formation dans des projets complets et réalistes, avec CI/CD inclus.

Ce module est votre passeport pour le monde professionnel moderne du développement C.

⏭️ [Chapitre 29 : Tests et Qualité →](29-tests-et-qualite/README.md)
