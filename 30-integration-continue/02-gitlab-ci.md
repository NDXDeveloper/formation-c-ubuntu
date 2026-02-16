🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 30.2 GitLab CI pour projets C

## Introduction

**GitLab CI/CD** est le système d'intégration continue et de déploiement continu intégré à GitLab. Comme GitHub Actions, c'est une plateforme d'automatisation puissante, mais avec une philosophie et des fonctionnalités légèrement différentes qui peuvent s'avérer particulièrement adaptées aux projets C, notamment dans les environnements d'entreprise.

GitLab CI/CD se distingue par sa **maturité** (lancé en 2012, avant GitHub Actions), sa **flexibilité** (runners auto-hébergés faciles à configurer) et son **intégration complète** dans l'écosystème DevOps de GitLab (issues, merge requests, registry, kubernetes).

---

## Pourquoi GitLab CI pour les projets C ?

### Les avantages spécifiques à GitLab

1. **Runners auto-hébergés simplifiés** : Parfait pour compiler sur du matériel spécifique (architecture ARM, serveurs puissants)
2. **Pipeline visualisé** : Interface graphique claire montrant les dépendances entre stages
3. **Cache distribué** : Système de cache plus avancé que GitHub Actions
4. **Container Registry intégré** : Stockage d'images Docker directement dans GitLab
5. **Environnements multiples** : Gestion native de dev, staging, production
6. **Gratuit pour self-hosted** : Pas de limite de minutes si vous hébergez vos runners

### Cas d'usage idéaux pour GitLab CI

- **Projets d'entreprise** : Contrôle total sur l'infrastructure
- **Embedded/Cross-compilation** : Runners avec toolchains spécifiques
- **Projets nécessitant beaucoup de ressources** : Compilation longue, builds ARM
- **Intégration DevOps complète** : De l'issue au déploiement
- **Sécurité stricte** : Code et CI/CD restent en interne

---

## Architecture de GitLab CI/CD

### Les concepts clés

#### 1. Pipeline

Un **pipeline** est l'ensemble du processus d'automatisation. Il contient plusieurs stages exécutés séquentiellement.

**Exemple de pipeline pour C** :
```
build → test → analyze → deploy
```

#### 2. Stage (Étape du pipeline)

Un **stage** regroupe des jobs qui s'exécutent en parallèle. Les stages s'exécutent dans l'ordre défini.

**Stages typiques** :
- `build` : Compilation
- `test` : Tests unitaires
- `analyze` : Analyse statique, Valgrind
- `deploy` : Publication d'artifacts

#### 3. Job

Un **job** est une tâche individuelle dans un stage. Les jobs d'un même stage s'exécutent en parallèle.

**Exemple** : `build:gcc-12`, `build:clang-15` dans le stage `build`

#### 4. Runner

Un **runner** est une machine (physique ou virtuelle) qui exécute les jobs.

**Types** :
- **Shared runners** : Fournis par GitLab.com (minutes limitées)
- **Specific runners** : Vos propres machines (illimité)
- **Group runners** : Partagés entre plusieurs projets

#### 5. Artifact

Un **artifact** est un fichier généré par un job et passé aux jobs suivants ou disponible au téléchargement.

**Exemples** : Binaires compilés, rapports de tests, couverture

#### 6. Cache

Le **cache** accélère les builds en réutilisant des fichiers entre pipelines (mais pas entre jobs du même pipeline, utilisez les artifacts pour ça).

**Exemples** : Objets compilés, dépendances

---

## Le fichier .gitlab-ci.yml

Contrairement à GitHub Actions (`.github/workflows/*.yml`), GitLab utilise **un seul fichier** à la racine : `.gitlab-ci.yml`

### Structure de base

```yaml
# Définir les stages du pipeline
stages:
  - build
  - test
  - deploy

# Définir les jobs
build:gcc:
  stage: build
  script:
    - gcc -o app main.c
  artifacts:
    paths:
      - app

test:unit:
  stage: test
  script:
    - ./app --test
```

---

## Exemple complet : Premier pipeline pour un projet C

Voici un pipeline GitLab CI complet pour un projet C :

**.gitlab-ci.yml**

```yaml
# Image Docker par défaut pour tous les jobs
image: ubuntu:22.04

# Variables globales
variables:
  GIT_SUBMODULE_STRATEGY: recursive

# Stages du pipeline
stages:
  - build
  - test
  - analyze
  - deploy

# Installation des dépendances (avant chaque job)
before_script:
  - apt-get update -qq
  - apt-get install -y -qq gcc cmake make

# ===== STAGE BUILD =====

build:gcc:
  stage: build
  script:
    - cmake -B build -DCMAKE_BUILD_TYPE=Release
    - cmake --build build -j$(nproc)
  artifacts:
    paths:
      - build/mon_app
    expire_in: 1 week

# ===== STAGE TEST =====

test:unit:
  stage: test
  script:
    - cd build
    - ctest --output-on-failure
  dependencies:
    - build:gcc

# ===== STAGE ANALYZE =====

analyze:static:
  stage: analyze
  before_script:
    - apt-get update -qq
    - apt-get install -y -qq clang-tidy cmake
  script:
    - cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    - clang-tidy -p build src/*.c
  allow_failure: true

# ===== STAGE DEPLOY =====

deploy:artifacts:
  stage: deploy
  script:
    - echo "Déploiement des artifacts"
  artifacts:
    paths:
      - build/mon_app
    expire_in: 30 days
  only:
    - main
```

---

## Différences clés avec GitHub Actions

| Aspect | GitLab CI | GitHub Actions |
|--------|-----------|----------------|
| **Fichier config** | `.gitlab-ci.yml` (1 fichier) | `.github/workflows/*.yml` (multiple) |
| **Organisation** | Stages → Jobs | Workflows → Jobs → Steps |
| **Exécution** | Stages séquentiels, jobs parallèles | Jobs parallèles par défaut |
| **Runners** | Shared ou self-hosted facile | Hosted par GitHub (ou self-hosted) |
| **Gratuit** | 400 minutes/mois (SaaS) ou illimité (self-hosted) | Illimité open-source, 2000 min/mois privé |
| **Cache** | Système de cache avancé | Actions cache moins flexible |
| **Container Registry** | ✅ Intégré | Via ghcr.io |
| **Artifacts** | Entre stages et téléchargeables | Entre jobs et téléchargeables |

---

## Runners GitLab : Shared vs Self-Hosted

### Shared Runners (GitLab.com)

**Caractéristiques** :
- Fournis par GitLab sur GitLab.com
- Linux (Ubuntu), macOS (limited), Windows
- Gratuit : 400 minutes/mois (Free tier)

**Limitations** :
- Quotas de temps
- Pas de customisation de l'environnement
- Peut être plus lent aux heures de pointe

**Pour projets C** : Suffisant pour petits projets, mais peut devenir limitant.

### Self-Hosted Runners (Recommandé pour C)

**Avantages** :
- ✅ **Illimité** : Pas de quota de minutes
- ✅ **Personnalisable** : Installez ce que vous voulez
- ✅ **Rapide** : Réseau local, pas de téléchargement
- ✅ **Spécialisé** : Toolchains embarquées, cross-compilation
- ✅ **Sécurité** : Code reste en interne

**Installation d'un runner (Ubuntu)** :

```bash
# 1. Télécharger le runner
curl -L "https://packages.gitlab.com/install/repositories/runner/gitlab-runner/script.deb.sh" | sudo bash  
sudo apt-get install gitlab-runner  

# 2. Enregistrer le runner
sudo gitlab-runner register

# 3. Démarrer le runner
sudo gitlab-runner start
```

**Configuration lors de l'enregistrement** :
- URL GitLab : `https://gitlab.com` ou votre instance
- Token : Récupéré dans Settings → CI/CD → Runners
- Tags : `c-builder`, `linux` (pour cibler des jobs spécifiques)
- Executor : `shell` ou `docker`

---

## Pipeline avec stages multiples

### Définition des stages

```yaml
stages:
  - prepare
  - build
  - test
  - analyze
  - package
  - deploy
```

**Ordre d'exécution** :
1. Tous les jobs `prepare` s'exécutent en parallèle
2. Quand tous réussissent → jobs `build` en parallèle
3. Puis `test`, `analyze`, `package`, `deploy` séquentiellement

### Jobs avec dépendances explicites

```yaml
test:integration:
  stage: test
  script:
    - ./run_integration_tests.sh
  dependencies:
    - build:gcc    # Attend que build:gcc soit terminé
  needs:           # Alternative : dépendances explicites
    - build:gcc
```

**Différence `dependencies` vs `needs`** :
- `dependencies` : Récupère les artifacts
- `needs` : Définit l'ordre d'exécution (peut être plus rapide)

---

## Artifacts : Partage de fichiers entre jobs

### Publier des artifacts

```yaml
build:release:
  stage: build
  script:
    - cmake -B build -DCMAKE_BUILD_TYPE=Release
    - cmake --build build
  artifacts:
    name: "binary-$CI_COMMIT_SHORT_SHA"
    paths:
      - build/mon_app
      - build/lib/*.so
    expire_in: 30 days
    when: on_success  # Publier seulement si succès
```

**Options importantes** :

- `name` : Nom de l'archive (utilise des variables CI)
- `paths` : Fichiers/dossiers à inclure
- `expire_in` : Durée de conservation (1 day, 1 week, 30 days, never)
- `when` : Quand publier (on_success, on_failure, always)

### Télécharger des artifacts

Les artifacts sont automatiquement disponibles dans les jobs suivants si vous utilisez `dependencies` :

```yaml
test:binary:
  stage: test
  dependencies:
    - build:release
  script:
    - ./build/mon_app --test  # L'artifact est disponible
```

### Télécharger via l'interface

Dans GitLab : `Pipelines → Job → Browse → Download artifacts`

---

## Cache : Accélérer les builds

### Configuration du cache

```yaml
variables:
  CACHE_KEY: "$CI_COMMIT_REF_SLUG"

cache:
  key: $CACHE_KEY
  paths:
    - build/CMakeCache.txt
    - build/CMakeFiles/
    - build/**/*.o

build:
  script:
    - cmake -B build
    - cmake --build build
```

**Différence cache vs artifacts** :

| Cache | Artifacts |
|-------|-----------|
| Réutilisé entre pipelines | Passés entre jobs du même pipeline |
| Optimisation (best effort) | Garantis |
| Objets compilés, dépendances | Binaires finaux, rapports |

### Cache par branche

```yaml
cache:
  key: "$CI_COMMIT_REF_SLUG"  # Une clé par branche
  paths:
    - build/
```

### Cache global

```yaml
cache:
  key: "global-cache"
  paths:
    - .ccache/  # Compilateur cache
```

---

## Variables d'environnement

### Variables prédéfinies

GitLab fournit de nombreuses variables automatiquement :

```yaml
test:
  script:
    - echo "Pipeline ID: $CI_PIPELINE_ID"
    - echo "Commit SHA: $CI_COMMIT_SHA"
    - echo "Branch: $CI_COMMIT_REF_NAME"
    - echo "Runner: $CI_RUNNER_DESCRIPTION"
```

**Variables utiles pour C** :

| Variable | Description |
|----------|-------------|
| `CI_COMMIT_SHA` | Hash du commit |
| `CI_COMMIT_SHORT_SHA` | Hash court (8 chars) |
| `CI_COMMIT_REF_NAME` | Nom de la branche/tag |
| `CI_PIPELINE_ID` | ID du pipeline |
| `CI_PROJECT_NAME` | Nom du projet |
| `CI_RUNNER_EXECUTABLE_ARCH` | Architecture du runner |

### Variables personnalisées

Dans `.gitlab-ci.yml` :

```yaml
variables:
  CC: gcc-12
  CFLAGS: "-Wall -Wextra -O2"
  BUILD_TYPE: Release

build:
  script:
    - $CC $CFLAGS -DCMAKE_BUILD_TYPE=$BUILD_TYPE main.c
```

Dans l'interface GitLab : `Settings → CI/CD → Variables`

**Exemple** : Stocker des secrets comme `CODECOV_TOKEN`

---

## Matrix builds avec parallel

Pour tester plusieurs configurations :

```yaml
build:matrix:
  stage: build
  parallel:
    matrix:
      - GCC_VERSION: ["11", "12", "13"]
        BUILD_TYPE: ["Debug", "Release"]
  script:
    - apt-get install -y gcc-$GCC_VERSION
    - gcc-$GCC_VERSION --version
    - cmake -B build -DCMAKE_BUILD_TYPE=$BUILD_TYPE
    - cmake --build build
  artifacts:
    name: "build-gcc$GCC_VERSION-$BUILD_TYPE"
    paths:
      - build/
```

**Résultat** : 6 jobs en parallèle (3 versions GCC × 2 build types)

---

## Intégration CMake complète

```yaml
image: ubuntu:22.04

variables:
  BUILD_DIR: "build"
  GIT_SUBMODULE_STRATEGY: recursive

stages:
  - configure
  - build
  - test

before_script:
  - apt-get update -qq
  - apt-get install -y -qq cmake gcc g++ make

configure:
  stage: configure
  script:
    - cmake -B $BUILD_DIR -DCMAKE_BUILD_TYPE=Release
  artifacts:
    paths:
      - $BUILD_DIR/
    expire_in: 1 hour

build:
  stage: build
  dependencies:
    - configure
  script:
    - cmake --build $BUILD_DIR -j$(nproc)
  artifacts:
    paths:
      - $BUILD_DIR/mon_app
      - $BUILD_DIR/lib/
    expire_in: 1 week

test:
  stage: test
  dependencies:
    - build
  script:
    - cd $BUILD_DIR
    - ctest --output-on-failure --verbose
```

---

## Tests unitaires automatisés

### Avec CTest

```yaml
test:unit:
  stage: test
  script:
    - cd build
    - ctest --output-on-failure --verbose
  artifacts:
    when: always
    reports:
      junit: build/test-results.xml  # Rapport JUnit
    paths:
      - build/Testing/
```

### Avec coverage (gcov/lcov)

```yaml
test:coverage:
  stage: test
  before_script:
    - apt-get update -qq
    - apt-get install -y -qq gcc cmake lcov
  script:
    - cmake -B build -DCMAKE_C_FLAGS="--coverage"
    - cmake --build build
    - cd build && ctest
    - lcov --capture --directory . --output-file coverage.info
    - lcov --list coverage.info
  coverage: '/lines\.*: \d+\.\d+%/'  # Extraction du %
  artifacts:
    paths:
      - build/coverage.info
```

**Note** : `coverage:` permet d'afficher le % dans l'interface GitLab.

---

## Sanitizers en CI

```yaml
test:sanitizers:
  stage: test
  parallel:
    matrix:
      - SANITIZER: ["address", "undefined", "thread"]
  script:
    - cmake -B build -DCMAKE_C_FLAGS="-fsanitize=$SANITIZER -g"
    - cmake --build build
    - cd build && ctest
  allow_failure: true  # Ne bloque pas le pipeline si échec
```

---

## Valgrind automatisé

```yaml
analyze:valgrind:
  stage: analyze
  before_script:
    - apt-get update -qq
    - apt-get install -y -qq valgrind cmake gcc
  script:
    - cmake -B build -DCMAKE_BUILD_TYPE=Debug
    - cmake --build build
    - valgrind --leak-check=full --error-exitcode=1 ./build/mon_app
  artifacts:
    when: always
    paths:
      - valgrind-report.txt
```

---

## Analyse statique

### clang-tidy

```yaml
analyze:clang-tidy:
  stage: analyze
  before_script:
    - apt-get update -qq
    - apt-get install -y -qq clang-tidy cmake
  script:
    - cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    - clang-tidy -p build src/*.c 2>&1 | tee clang-tidy-report.txt
  artifacts:
    paths:
      - clang-tidy-report.txt
  allow_failure: true
```

### cppcheck

```yaml
analyze:cppcheck:
  stage: analyze
  before_script:
    - apt-get update -qq
    - apt-get install -y -qq cppcheck
  script:
    - cppcheck --enable=all --suppress=missingIncludeSystem src/ 2>&1 | tee cppcheck-report.txt
  artifacts:
    paths:
      - cppcheck-report.txt
  allow_failure: true
```

---

## Multi-plateformes avec tags

Pour cibler des runners spécifiques :

```yaml
build:linux:
  stage: build
  tags:
    - linux
  script:
    - gcc -o app-linux main.c
  artifacts:
    paths:
      - app-linux

build:windows:
  stage: build
  tags:
    - windows
  script:
    - gcc -o app-windows.exe main.c
  artifacts:
    paths:
      - app-windows.exe

build:macos:
  stage: build
  tags:
    - macos
  script:
    - gcc -o app-macos main.c
  artifacts:
    paths:
      - app-macos
```

**Note** : Nécessite des runners avec les tags correspondants.

---

## Images Docker personnalisées

Pour éviter d'installer les dépendances à chaque fois :

### Créer une image Docker

**Dockerfile**
```dockerfile
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    gcc-12 gcc-13 \
    clang-14 clang-15 \
    cmake \
    lcov \
    valgrind \
    clang-tidy \
    cppcheck \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /builds
```

### Utiliser l'image dans le pipeline

```yaml
image: registry.gitlab.com/mon-groupe/mon-image-c:latest

build:
  script:
    - gcc-13 --version  # Déjà installé !
    - cmake -B build
    - cmake --build build
```

**Avantage** : Builds beaucoup plus rapides (pas d'installation de paquets)

---

## Déclencheurs (triggers)

### Sur push/merge request

```yaml
# Exécution sur toutes les branches
build:
  script:
    - make

# Exécution seulement sur main
deploy:
  script:
    - make deploy
  only:
    - main

# Exécution seulement sur merge requests
test:mr:
  script:
    - make test
  only:
    - merge_requests
```

### Sur tags (releases)

```yaml
release:
  stage: deploy
  script:
    - make package
  artifacts:
    paths:
      - dist/*.tar.gz
  only:
    - tags
  except:
    - branches
```

### Pipelines planifiés (scheduled)

Dans l'interface : `CI/CD → Schedules → New schedule`

```yaml
nightly:analysis:
  stage: analyze
  script:
    - make deep-analysis
  only:
    - schedules
```

---

## Règles avancées avec rules

`rules` remplace `only`/`except` avec plus de flexibilité :

```yaml
build:
  script:
    - make
  rules:
    - if: '$CI_PIPELINE_SOURCE == "merge_request_event"'
      when: always
    - if: '$CI_COMMIT_BRANCH == "main"'
      when: always
    - when: never

test:
  script:
    - make test
  rules:
    - if: '$CI_COMMIT_MESSAGE =~ /\[skip-tests\]/'
      when: never
    - when: on_success
```

**Conditions possibles** :
- Variables d'environnement
- Branche/tag
- Type de pipeline
- Fichiers modifiés

---

## Pages GitLab (documentation)

Pour publier la documentation HTML (Doxygen, etc.) :

```yaml
pages:
  stage: deploy
  before_script:
    - apt-get update -qq
    - apt-get install -y -qq doxygen graphviz
  script:
    - doxygen Doxyfile
    - mv docs/html/ public/
  artifacts:
    paths:
      - public
  only:
    - main
```

**Résultat** : Documentation accessible sur `https://username.gitlab.io/project/`

---

## Integration avec Codecov

```yaml
test:coverage:
  stage: test
  script:
    - cmake -B build -DCMAKE_C_FLAGS="--coverage"
    - cmake --build build
    - cd build && ctest
    - lcov --capture --directory . --output-file coverage.info
    - pip install codecov-cli
    - codecovcli upload-process -f coverage.info -t $CODECOV_TOKEN
  coverage: '/lines\.*: \d+\.\d+%/'
```

**Variable à ajouter** : `CODECOV_TOKEN` dans Settings → CI/CD → Variables

---

## Notifications et intégrations

### Slack

```yaml
notify:slack:
  stage: .post  # Stage spécial exécuté en dernier
  script:
    - |
      curl -X POST -H 'Content-type: application/json' \
      --data "{\"text\":\"Pipeline $CI_PIPELINE_ID finished with status $CI_JOB_STATUS\"}" \
      $SLACK_WEBHOOK_URL
  when: always
```

### Email

Configuré dans l'interface : `Settings → Integrations → Emails on push`

---

## Pipeline complet production-ready

Voici un exemple de pipeline complet et professionnel :

**.gitlab-ci.yml**

```yaml
image: ubuntu:22.04

variables:
  GIT_SUBMODULE_STRATEGY: recursive
  BUILD_DIR: "build"
  CACHE_KEY: "$CI_COMMIT_REF_SLUG"

cache:
  key: $CACHE_KEY
  paths:
    - build/CMakeCache.txt
    - build/CMakeFiles/
    - .ccache/

stages:
  - build
  - test
  - analyze
  - package
  - deploy

# ===== Template pour installation =====
.install_deps: &install_deps
  - apt-get update -qq
  - apt-get install -y -qq gcc g++ cmake make

# ===== BUILD STAGE =====

build:debug:
  stage: build
  before_script:
    - *install_deps
  script:
    - cmake -B $BUILD_DIR -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS="-g"
    - cmake --build $BUILD_DIR -j$(nproc)
  artifacts:
    name: "build-debug-$CI_COMMIT_SHORT_SHA"
    paths:
      - $BUILD_DIR/
    expire_in: 1 day

build:release:
  stage: build
  before_script:
    - *install_deps
  script:
    - cmake -B $BUILD_DIR -DCMAKE_BUILD_TYPE=Release
    - cmake --build $BUILD_DIR -j$(nproc)
  artifacts:
    name: "build-release-$CI_COMMIT_SHORT_SHA"
    paths:
      - $BUILD_DIR/mon_app
      - $BUILD_DIR/lib/
    expire_in: 1 week

# ===== TEST STAGE =====

test:unit:
  stage: test
  dependencies:
    - build:debug
  script:
    - cd $BUILD_DIR
    - ctest --output-on-failure --verbose
  artifacts:
    when: always
    paths:
      - $BUILD_DIR/Testing/

test:sanitizers:
  stage: test
  parallel:
    matrix:
      - SANITIZER: ["address", "undefined"]
  before_script:
    - *install_deps
  script:
    - cmake -B $BUILD_DIR -DCMAKE_C_FLAGS="-fsanitize=$SANITIZER -g"
    - cmake --build $BUILD_DIR
    - cd $BUILD_DIR && ctest
  allow_failure: true

test:coverage:
  stage: test
  before_script:
    - *install_deps
    - apt-get install -y -qq lcov
  script:
    - cmake -B $BUILD_DIR -DCMAKE_C_FLAGS="--coverage -O0"
    - cmake --build $BUILD_DIR
    - cd $BUILD_DIR && ctest
    - lcov --capture --directory . --output-file coverage.info
    - lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info
    - lcov --list coverage.info
  coverage: '/lines\.*: \d+\.\d+%/'
  artifacts:
    paths:
      - $BUILD_DIR/coverage.info

# ===== ANALYZE STAGE =====

analyze:valgrind:
  stage: analyze
  dependencies:
    - build:debug
  before_script:
    - apt-get update -qq
    - apt-get install -y -qq valgrind
  script:
    - valgrind --leak-check=full --error-exitcode=1 $BUILD_DIR/mon_app
  artifacts:
    when: always
    paths:
      - valgrind-*.log

analyze:static:
  stage: analyze
  before_script:
    - apt-get update -qq
    - apt-get install -y -qq clang-tidy cppcheck cmake
  script:
    - cmake -B $BUILD_DIR -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    - clang-tidy -p $BUILD_DIR src/*.c > clang-tidy-report.txt || true
    - cppcheck --enable=all --suppress=missingIncludeSystem src/ > cppcheck-report.txt 2>&1 || true
  artifacts:
    paths:
      - clang-tidy-report.txt
      - cppcheck-report.txt
  allow_failure: true

# ===== PACKAGE STAGE =====

package:tarball:
  stage: package
  dependencies:
    - build:release
  script:
    - tar -czf mon-app-linux-x64.tar.gz -C $BUILD_DIR mon_app
  artifacts:
    name: "release-$CI_COMMIT_TAG"
    paths:
      - mon-app-linux-x64.tar.gz
    expire_in: 1 year
  only:
    - tags

# ===== DEPLOY STAGE =====

deploy:artifacts:
  stage: deploy
  dependencies:
    - build:release
  script:
    - echo "Déploiement des binaires"
  artifacts:
    name: "release-$CI_COMMIT_SHORT_SHA"
    paths:
      - $BUILD_DIR/mon_app
  only:
    - main

pages:
  stage: deploy
  before_script:
    - apt-get update -qq
    - apt-get install -y -qq doxygen graphviz
  script:
    - doxygen Doxyfile
    - mv docs/html/ public/
  artifacts:
    paths:
      - public
  only:
    - main
```

---

## Visualisation des pipelines

GitLab offre une excellente visualisation :

### Dans l'interface

- **Pipelines** : Vue d'ensemble de tous les pipelines
- **Jobs** : Liste de tous les jobs
- **Graphs** : Visualisation graphique des dépendances
- **Needs** : Vue des dépendances `needs:`

### Badges de statut

Ajoutez un badge dans votre README :

```markdown
[![pipeline status](https://gitlab.com/username/project/badges/main/pipeline.svg)](https://gitlab.com/username/project/-/commits/main)

[![coverage report](https://gitlab.com/username/project/badges/main/coverage.svg)](https://gitlab.com/username/project/-/commits/main)
```

---

## Bonnes pratiques GitLab CI pour C

### 1. Utilisez des images Docker

Préférez des images avec les outils déjà installés :

```yaml
image: gcc:13  # Image officielle GCC
```

### 2. Cache intelligemment

```yaml
cache:
  key:
    files:
      - CMakeLists.txt  # Invalide si CMakeLists.txt change
  paths:
    - build/
```

### 3. Artifacts ciblés

Ne publiez que ce qui est nécessaire :

```yaml
artifacts:
  paths:
    - build/mon_app  # Pas tout le dossier build/
```

### 4. Parallélisation maximale

```yaml
parallel:
  matrix:
    - GCC: ["11", "12", "13"]
      OS: ["ubuntu:22.04", "ubuntu:24.04"]
```

### 5. allow_failure pour analyses

```yaml
analyze:experimental:
  script:
    - run_experimental_check
  allow_failure: true  # N'empêche pas le merge
```

### 6. Stages bien nommés

```yaml
stages:
  - prepare    # Installation, setup
  - build      # Compilation
  - test       # Tests rapides
  - analyze    # Analyses longues
  - package    # Création de packages
  - deploy     # Déploiement
```

### 7. Réutilisation avec extends

```yaml
.build_template:
  stage: build
  before_script:
    - apt-get update && apt-get install -y gcc cmake

build:gcc-12:
  extends: .build_template
  script:
    - gcc-12 --version
    - cmake -B build && cmake --build build

build:gcc-13:
  extends: .build_template
  script:
    - gcc-13 --version
    - cmake -B build && cmake --build build
```

---

## Limites et quotas

### GitLab.com (SaaS)

**Free tier** :
- 400 minutes CI/CD par mois
- 10 Go de stockage artifacts/mois
- 5 Go transfer bandwidth

**Premium** :
- 10 000 minutes/mois
- 100 Go stockage

**Ultimate** :
- 50 000 minutes/mois
- 500 Go stockage

### GitLab Self-Hosted

- ✅ **Illimité** si vous hébergez vos runners
- ✅ Contrôle total des ressources
- ✅ Pas de quotas de minutes

**Recommandation pour projets C** : Self-hosted pour compilation intensive.

---

## Migration depuis GitHub Actions

### Correspondances

| GitHub Actions | GitLab CI |
|----------------|-----------|
| `.github/workflows/ci.yml` | `.gitlab-ci.yml` |
| `on: push` | `only: - branches` |
| `runs-on: ubuntu-latest` | `image: ubuntu:latest` |
| `steps:` | `script:` |
| `uses: actions/checkout@v4` | `GIT_STRATEGY: clone` |
| `uses: actions/upload-artifact@v4` | `artifacts: paths:` |
| `needs:` | `needs:` ou `dependencies:` |

### Exemple de conversion

**GitHub Actions** :
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
        path: app
```

**GitLab CI** :
```yaml
build:
  image: ubuntu:latest
  script:
    - make
  artifacts:
    paths:
      - app
```

---

## Sécurité

### Variables protégées

Dans Settings → CI/CD → Variables :
- Cochez "Protected" pour limiter aux branches protégées
- Cochez "Masked" pour masquer dans les logs

### Runners sécurisés

Pour les projets sensibles :
- Utilisez des specific runners (pas shared)
- Limitez l'accès aux tags
- Activez "Lock to current projects"

### Secrets dans artifacts

❌ **Ne jamais** inclure de secrets dans les artifacts

---

## Résumé

**GitLab CI/CD = CI/CD intégré à GitLab**

**Fichier principal** : `.gitlab-ci.yml`

**Architecture** :
```
Pipeline
  └── Stages (séquentiels)
       └── Jobs (parallèles dans un stage)
            └── Scripts
```

**Concepts clés** :
- **Stages** : Étapes séquentielles du pipeline
- **Jobs** : Tâches parallèles dans un stage
- **Artifacts** : Fichiers partagés entre stages
- **Cache** : Optimisation entre pipelines
- **Runners** : Machines exécutant les jobs

**Avantages pour C** :
- Self-hosted facile (illimité)
- Cache avancé
- Container Registry intégré
- Pipeline visualisé
- Parfait pour entreprise

---

## Prochaines étapes

Maintenant que vous connaissez GitLab CI, vous pouvez :

- Configurer votre premier pipeline
- Installer des runners self-hosted
- Créer des images Docker personnalisées
- Intégrer tous les outils C (sanitizers, Valgrind, etc.)
- Automatiser complètement votre workflow

---

## Ressources complémentaires

- [Documentation GitLab CI/CD](https://docs.gitlab.com/ee/ci/)
- [Référence .gitlab-ci.yml](https://docs.gitlab.com/ee/ci/yaml/)
- [GitLab Runner](https://docs.gitlab.com/runner/)
- [CI/CD Examples](https://docs.gitlab.com/ee/ci/examples/)

---

**🎯 Points clés à retenir :**

- ✅ GitLab CI = Fichier unique `.gitlab-ci.yml`
- ✅ Organisation : Stages (séquentiels) → Jobs (parallèles)
- ✅ Runners self-hosted faciles = minutes illimitées
- ✅ Cache avancé pour accélérer les builds
- ✅ Artifacts partagés entre stages automatiquement
- ✅ Parfait pour projets d'entreprise et compilation intensive
- ✅ Matrix builds avec `parallel:`
- ✅ Images Docker personnalisées recommandées

⏭️ [Jenkins : pipelines déclaratifs](/30-integration-continue/03-jenkins.md)
