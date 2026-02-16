🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 30.4 Gestion des caches

## Introduction

La **gestion du cache** est une technique d'optimisation cruciale en CI/CD qui consiste à **réutiliser des fichiers** d'un build à l'autre au lieu de les régénérer à chaque fois. Pour les projets C, où la compilation peut être longue, un cache bien configuré peut réduire les temps de build de **plusieurs minutes à quelques secondes**.

Imaginez que vous compilez un projet avec 100 fichiers source. Sans cache, chaque push recompile les 100 fichiers (même ceux qui n'ont pas changé). Avec cache, seuls les fichiers modifiés sont recompilés, divisant souvent le temps de build par 5 à 10.

---

## Qu'est-ce que le cache en CI/CD ?

### Définition

Le **cache** est un mécanisme qui sauvegarde des fichiers entre les exécutions du pipeline CI/CD. Ces fichiers sont stockés et récupérés lors du prochain build pour accélérer l'exécution.

### Cache vs Artifacts

| Aspect | Cache | Artifacts |
|--------|-------|-----------|
| **Objectif** | Accélérer les builds | Sauvegarder les résultats |
| **Garantie** | Best effort (peut être manquant) | Toujours présent |
| **Portée** | Entre pipelines | Entre jobs/stages du même pipeline |
| **Durée** | Jours/semaines | Configurable (jours/mois) |
| **Contenu** | Fichiers intermédiaires | Fichiers finaux |
| **Exemple** | Objets compilés (.o), dépendances | Binaires finaux, rapports |

**Règle simple** :
- **Cache** = optimisation (peut être supprimé sans casser le build)
- **Artifacts** = résultats (nécessaires au pipeline)

---

## Pourquoi le cache est crucial pour les projets C ?

### Les temps de compilation

La compilation C peut être coûteuse :

```
Petit projet (10 fichiers) : 5-10 secondes  
Projet moyen (100 fichiers) : 1-5 minutes  
Grand projet (1000+ fichiers) : 10-60 minutes  
Très grand projet (type Chromium) : Plusieurs heures  
```

### Impact du cache

**Sans cache** :
```
Commit 1 : Compile 100 fichiers → 3 minutes  
Commit 2 : Compile 100 fichiers → 3 minutes (même si 1 seul fichier modifié !)  
Commit 3 : Compile 100 fichiers → 3 minutes  
```

**Avec cache** :
```
Commit 1 : Compile 100 fichiers → 3 minutes (cache créé)  
Commit 2 : Compile 1 fichier modifié → 10 secondes (99 depuis le cache)  
Commit 3 : Compile 2 fichiers modifiés → 20 secondes  
```

**Gain** : 90% de temps économisé !

---

## Ce qu'il faut mettre en cache pour les projets C

### 1. Objets compilés (.o, .obj)

**Quoi** : Fichiers objets générés par le compilateur

**Où** :
- CMake : `build/CMakeFiles/**/*.o`
- Make : `*.o`

**Pourquoi** : Évite de recompiler les fichiers .c non modifiés

```yaml
cache:
  paths:
    - build/**/*.o
```

### 2. Cache du compilateur (ccache)

**Quoi** : Cache de compilation intelligent

**Où** : `~/.ccache/` ou `.ccache/`

**Pourquoi** : Détecte automatiquement quels fichiers recompiler

```yaml
cache:
  paths:
    - .ccache/
```

### 3. Fichiers de configuration CMake

**Quoi** : CMakeCache.txt, CMakeFiles/

**Où** : `build/CMakeCache.txt`, `build/CMakeFiles/`

**Pourquoi** : Évite la phase de configuration CMake

```yaml
cache:
  paths:
    - build/CMakeCache.txt
    - build/CMakeFiles/
```

### 4. Dépendances externes

**Quoi** : Bibliothèques téléchargées

**Où** : Dépend du gestionnaire (conan, vcpkg)

**Pourquoi** : Évite de retélécharger à chaque build

```yaml
cache:
  paths:
    - ~/.conan/
    - vcpkg_installed/
```

### 5. Outils de build

**Quoi** : Ninja, autres outils

**Où** : `.build_tools/`

**Pourquoi** : Évite de les réinstaller

---

## Ce qu'il ne faut PAS mettre en cache

### ❌ Binaires finaux

Les exécutables compilés ne doivent **pas** être en cache, utilisez les artifacts :

```yaml
# ❌ Mauvais
cache:
  paths:
    - build/mon_app

# ✅ Bon
artifacts:
  paths:
    - build/mon_app
```

### ❌ Secrets et credentials

**Jamais** mettre de mots de passe ou tokens en cache.

### ❌ Fichiers très volumineux

Le cache a des limites de taille (ex: 10 GB sur GitHub Actions).

### ❌ Fichiers générés aléatoirement

Fichiers qui changent à chaque build (logs avec timestamps, etc.).

---

## Cache avec GitHub Actions

### Configuration de base

```yaml
name: Build avec Cache

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4

    - name: Cache des objets compilés
      uses: actions/cache@v4
      with:
        path: |
          build/
          ~/.ccache
        key: ${{ runner.os }}-build-${{ hashFiles('**/*.c', '**/*.h', 'CMakeLists.txt') }}
        restore-keys: |
          ${{ runner.os }}-build-

    - name: Compiler
      run: |
        cmake -B build
        cmake --build build
```

### Explication des paramètres

#### path

Liste des fichiers/dossiers à mettre en cache :

```yaml
path: |
  build/
  ~/.ccache
  vendor/
```

#### key

Clé unique identifiant ce cache. Si la clé change, un nouveau cache est créé.

**Composantes typiques** :
- `${{ runner.os }}` : Système d'exploitation (Linux, macOS, Windows)
- `${{ hashFiles('**/*.c') }}` : Hash des fichiers sources

**Exemple** : `Linux-build-a3f5d9e8`

#### restore-keys

Clés de fallback si la clé exacte n'existe pas :

```yaml
key: Linux-build-a3f5d9e8-main  
restore-keys: |  
  Linux-build-a3f5d9e8-
  Linux-build-
```

**Comportement** :
1. Cherche `Linux-build-a3f5d9e8-main` (exact match)
2. Sinon, cherche `Linux-build-a3f5d9e8-*` (prefix match)
3. Sinon, cherche `Linux-build-*`
4. Sinon, pas de cache

### Stratégies de clés de cache

#### 1. Par contenu des fichiers sources

```yaml
key: ${{ runner.os }}-${{ hashFiles('src/**/*.c', 'include/**/*.h') }}
```

**Avantage** : Cache invalidé seulement si le code change  
**Inconvénient** : Hash peut être coûteux sur gros projets  

#### 2. Par branche

```yaml
key: ${{ runner.os }}-${{ github.ref_name }}-build
```

**Avantage** : Chaque branche a son cache  
**Inconvénient** : Pas de réutilisation entre branches  

#### 3. Par date

```yaml
key: ${{ runner.os }}-build-${{ github.run_number }}  
restore-keys: |  
  ${{ runner.os }}-build-
```

**Avantage** : Cache toujours utilisé  
**Inconvénient** : Nouveau cache à chaque run  

#### 4. Hybride (recommandé)

```yaml
key: ${{ runner.os }}-${{ github.ref_name }}-${{ hashFiles('**/CMakeLists.txt', 'src/**') }}  
restore-keys: |  
  ${{ runner.os }}-${{ github.ref_name }}-
  ${{ runner.os }}-
```

**Avantage** : Équilibre entre invalidation et réutilisation

---

## Cache avec ccache (recommandé pour C)

### Qu'est-ce que ccache ?

**ccache** (Compiler Cache) est un outil qui met automatiquement en cache les résultats de compilation. Il est **beaucoup plus intelligent** qu'un simple cache de fichiers.

**Fonctionnement** :
1. Intercepte les appels au compilateur
2. Hash les fichiers sources + flags de compilation
3. Si déjà compilé → retourne le résultat du cache
4. Sinon → compile normalement et sauvegarde

### Installation

```bash
sudo apt-get install ccache
```

### Utilisation dans le pipeline

```yaml
name: Build avec ccache

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4

    - name: Installer ccache
      run: sudo apt-get install -y ccache

    - name: Cache ccache
      uses: actions/cache@v4
      with:
        path: ~/.ccache
        key: ${{ runner.os }}-ccache-${{ github.sha }}
        restore-keys: |
          ${{ runner.os }}-ccache-

    - name: Configurer ccache
      run: |
        ccache --max-size=500M
        ccache --set-config=compression=true
        ccache --zero-stats

    - name: Compiler avec ccache
      env:
        CC: ccache gcc
        CXX: ccache g++
      run: |
        cmake -B build -DCMAKE_C_COMPILER_LAUNCHER=ccache
        cmake --build build -j$(nproc)

    - name: Statistiques ccache
      run: ccache --show-stats
```

### Configuration CMake pour ccache

**CMakeLists.txt**
```cmake
find_program(CCACHE_PROGRAM ccache)  
if(CCACHE_PROGRAM)  
    message(STATUS "ccache trouvé: ${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()
```

### Avantages de ccache

- ✅ **Intelligent** : Détecte les changements réels (pas juste les timestamps)
- ✅ **Cross-build** : Fonctionne même avec différents flags de compilation
- ✅ **Compression** : Cache compressé pour économiser l'espace
- ✅ **Statistiques** : Affiche taux de hit/miss

---

## Cache avec GitLab CI

### Configuration de base

```yaml
image: ubuntu:22.04

variables:
  CCACHE_DIR: "$CI_PROJECT_DIR/.ccache"

cache:
  key: "$CI_COMMIT_REF_SLUG"
  paths:
    - build/
    - .ccache/

stages:
  - build

build:
  stage: build
  before_script:
    - apt-get update && apt-get install -y gcc cmake ccache
  script:
    - ccache --max-size=500M
    - export CC="ccache gcc"
    - cmake -B build
    - cmake --build build -j$(nproc)
    - ccache --show-stats
```

### Cache par branche

```yaml
cache:
  key: "$CI_COMMIT_REF_SLUG"  # Une clé par branche
  paths:
    - build/
    - .ccache/
```

### Cache global

```yaml
cache:
  key: "global-cache"  # Cache partagé entre toutes les branches
  paths:
    - .ccache/
```

### Cache avec fichiers

```yaml
cache:
  key:
    files:
      - CMakeLists.txt
      - src/**/*.c
  paths:
    - build/
```

**Comportement** : Cache invalidé si ces fichiers changent.

### Policies de cache

```yaml
build:
  cache:
    key: "$CI_COMMIT_REF_SLUG"
    paths:
      - build/
    policy: pull-push  # Récupère ET met à jour le cache

test:
  cache:
    key: "$CI_COMMIT_REF_SLUG"
    paths:
      - build/
    policy: pull  # Récupère seulement, ne met pas à jour
```

**Policies** :
- `pull-push` (défaut) : Télécharge et upload le cache
- `pull` : Télécharge seulement (lecture seule)
- `push` : Upload seulement

---

## Cache avec Jenkins

### Configuration avec Workspace Caching Plugin

```groovy
pipeline {
    agent any

    options {
        // Cache du workspace
        buildDiscarder(logRotator(numToKeepStr: '10'))
    }

    stages {
        stage('Build') {
            steps {
                // Utiliser ccache
                sh '''
                    export CCACHE_DIR=${WORKSPACE}/.ccache
                    export CC="ccache gcc"
                    cmake -B build
                    cmake --build build
                '''
            }
        }
    }
}
```

### Cache avec stash

Pour les petits fichiers entre stages :

```groovy
stage('Build') {
    steps {
        sh 'cmake -B build && cmake --build build'
        stash name: 'build-cache', includes: 'build/**/*.o'
    }
}

stage('Test') {
    steps {
        unstash 'build-cache'
        sh 'cd build && ctest'
    }
}
```

### Cache ccache persistant

Sur un agent dédié, configurez `CCACHE_DIR` :

```groovy
environment {
    CCACHE_DIR = '/var/jenkins/ccache'
}

stages {
    stage('Build') {
        steps {
            sh '''
                export CC="ccache gcc"
                cmake -B build
                cmake --build build
            '''
        }
    }
}
```

**Avantage** : Cache persiste entre tous les builds.

---

## Workflow complet avec cache optimisé

### GitHub Actions

```yaml
name: Build Optimisé avec Cache

on:
  push:
    branches: [ main, develop ]
  pull_request:

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
    - name: Checkout
      uses: actions/checkout@v4
      with:
        submodules: recursive

    - name: Installer les dépendances
      run: |
        sudo apt-get update
        sudo apt-get install -y gcc cmake ccache ninja-build

    - name: Cache ccache
      uses: actions/cache@v4
      with:
        path: ~/.ccache
        key: ccache-${{ runner.os }}-${{ github.sha }}
        restore-keys: |
          ccache-${{ runner.os }}-

    - name: Cache CMake
      uses: actions/cache@v4
      with:
        path: |
          build/CMakeCache.txt
          build/CMakeFiles
          build/_deps
        key: cmake-${{ runner.os }}-${{ hashFiles('**/CMakeLists.txt') }}
        restore-keys: |
          cmake-${{ runner.os }}-

    - name: Configurer ccache
      run: |
        ccache --max-size=1G
        ccache --set-config=compression=true
        ccache --zero-stats
        echo "CC=ccache gcc" >> $GITHUB_ENV
        echo "CXX=ccache g++" >> $GITHUB_ENV

    - name: Configurer CMake
      run: |
        cmake -B build \
          -G Ninja \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_C_COMPILER_LAUNCHER=ccache \
          -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

    - name: Compiler
      run: cmake --build build -j$(nproc)

    - name: Statistiques ccache
      run: |
        echo "=== Statistiques ccache ==="
        ccache --show-stats

    - name: Tester
      run: cd build && ctest --output-on-failure

    - name: Uploader les binaires
      uses: actions/upload-artifact@v4
      with:
        name: binaries
        path: build/bin/
```

### GitLab CI

```yaml
image: ubuntu:22.04

variables:
  CCACHE_DIR: "$CI_PROJECT_DIR/.ccache"
  GIT_SUBMODULE_STRATEGY: recursive

cache:
  key: "$CI_COMMIT_REF_SLUG-$CI_PIPELINE_ID"
  paths:
    - build/CMakeCache.txt
    - build/CMakeFiles/
    - .ccache/
  policy: pull-push

stages:
  - build
  - test

before_script:
  - apt-get update -qq
  - apt-get install -y -qq gcc cmake ccache ninja-build

build:
  stage: build
  script:
    - ccache --max-size=1G
    - ccache --set-config=compression=true
    - ccache --zero-stats
    - export CC="ccache gcc"
    - export CXX="ccache g++"
    - cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
    - cmake --build build -j$(nproc)
    - ccache --show-stats
  artifacts:
    paths:
      - build/
    expire_in: 1 hour

test:
  stage: test
  dependencies:
    - build
  cache:
    policy: pull  # Seulement lecture
  script:
    - cd build && ctest --output-on-failure
```

---

## Optimisations avancées

### 1. Ninja au lieu de Make

Ninja est plus rapide que Make pour les incrémental builds :

```yaml
- name: Configurer avec Ninja
  run: |
    sudo apt-get install -y ninja-build
    cmake -B build -G Ninja
```

**Gain** : 20-30% plus rapide

### 2. Compilation en parallèle

```bash
cmake --build build -j$(nproc)  # Utilise tous les cores
```

### 3. Gold Linker (ld.gold)

Plus rapide que le linker par défaut :

```cmake
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
```

### 4. Precompiled Headers (PCH)

Pour les headers utilisés partout :

```cmake
target_precompile_headers(mon_app PRIVATE common.h)
```

### 5. Unity Builds

Compile plusieurs fichiers .c ensemble :

```cmake
set(CMAKE_UNITY_BUILD ON)
```

**Attention** : Peut masquer certains problèmes d'include.

---

## Mesurer l'efficacité du cache

### Temps de build

```yaml
- name: Mesurer le temps de build
  run: |
    START=$(date +%s)
    cmake --build build
    END=$(date +%s)
    echo "Temps de build: $((END-START))s"
```

### Statistiques ccache

```yaml
- name: Statistiques ccache
  run: |
    ccache --show-stats
    # Affiche :
    # - cache hit rate (% de réutilisation)
    # - cache miss (nouvelles compilations)
    # - files in cache
```

**Objectif** : Hit rate > 80% pour un cache efficace

### Comparaison avant/après

```yaml
# Premier build (sans cache)
Build time: 180s  
ccache hit rate: 0%  

# Deuxième build (avec cache, 1 fichier modifié)
Build time: 15s  # 92% plus rapide !  
ccache hit rate: 95%  
```

---

## Problèmes courants et solutions

### Cache pas utilisé

**Symptômes** :
- Temps de build identiques
- ccache hit rate à 0%

**Causes possibles** :
1. Clé de cache change à chaque fois
2. Chemins de cache incorrects
3. Cache expiré/supprimé

**Solution** :
```yaml
# Vérifier que la clé est stable
key: ${{ runner.os }}-build-stable

# Vérifier les chemins
- name: Debug cache
  run: |
    ls -la ~/.ccache
    ccache --show-stats
```

### Cache trop volumineux

**Symptômes** :
- Upload du cache très long
- Espace disque saturé

**Solutions** :
```bash
# Limiter la taille du cache
ccache --max-size=500M

# Nettoyer le cache
ccache --cleanup
```

### Cache corrompu

**Symptômes** :
- Erreurs de compilation étranges
- Build qui passe puis échoue

**Solution** :
```bash
# Réinitialiser le cache
ccache --clear
```

Ou changer la clé de cache dans le workflow.

### Faux positifs du cache

**Symptômes** :
- Code modifié mais ancien binaire utilisé

**Cause** : Cache trop agressif

**Solution** :
```yaml
# Invalider le cache sur certains fichiers
key: ${{ hashFiles('**/CMakeLists.txt', 'src/**') }}
```

---

## Limites des caches par plateforme

### GitHub Actions

- **Taille max par cache** : 10 GB
- **Taille totale** : 10 GB par dépôt
- **Durée** : 7 jours sans utilisation
- **Nombre** : Illimité (mais LRU cleanup si > 10 GB)

### GitLab CI

- **Taille** : Pas de limite stricte (bon sens)
- **Durée** : Pas d'expiration automatique
- **Gestion** : Manuelle via l'interface

### Jenkins

- **Taille** : Limitée par l'espace disque de l'agent
- **Durée** : Illimitée (tant que l'agent existe)
- **Gestion** : Manuelle

---

## Bonnes pratiques

### 1. Toujours utiliser ccache pour C

```yaml
- name: Setup ccache
  run: |
    sudo apt-get install -y ccache
    echo "CC=ccache gcc" >> $GITHUB_ENV
```

### 2. Clés de cache stables

```yaml
# ✅ Bon : clé stable basée sur contenu
key: ${{ runner.os }}-${{ hashFiles('**/CMakeLists.txt') }}

# ❌ Mauvais : clé change à chaque run
key: ${{ github.sha }}
```

### 3. restore-keys appropriés

```yaml
key: Linux-main-abc123  
restore-keys: |  
  Linux-main-
  Linux-
```

Permet de réutiliser un cache partiel.

### 4. Ne pas cacher les artifacts

```yaml
# ❌ Mauvais
cache:
  paths:
    - build/mon_app  # C'est un artifact !

# ✅ Bon
cache:
  paths:
    - build/**/*.o   # Objets intermédiaires
artifacts:
  paths:
    - build/mon_app  # Binaire final
```

### 5. Limiter la taille

```bash
ccache --max-size=1G  # Évite de remplir le cache
```

### 6. Nettoyer périodiquement

```yaml
- name: Nettoyer le cache si > 7 jours
  run: |
    find ~/.ccache -mtime +7 -delete
```

### 7. Documenter la stratégie

Dans le README :
```markdown
## Cache CI/CD

Ce projet utilise ccache pour accélérer les builds.  
Clé de cache : `OS-branch-hash(CMakeLists.txt)`  
Durée : 7 jours  
```

---

## Stratégies par type de projet

### Petit projet (<10 fichiers)

**Stratégie** : Cache simple, pas forcément ccache

```yaml
cache:
  paths:
    - build/
  key: ${{ runner.os }}-build
```

### Projet moyen (10-100 fichiers)

**Stratégie** : ccache recommandé

```yaml
cache:
  paths:
    - ~/.ccache
  key: ${{ runner.os }}-ccache-${{ hashFiles('src/**') }}
```

### Grand projet (100+ fichiers)

**Stratégie** : ccache + cache CMake + Ninja

```yaml
cache:
  paths:
    - ~/.ccache
    - build/CMakeFiles/
  key: ${{ runner.os }}-${{ hashFiles('**/CMakeLists.txt', 'src/**/*.c') }}
```

### Très grand projet (1000+ fichiers)

**Stratégie** : ccache + distributed compilation (distcc) + runners puissants

---

## Cache et sécurité

### Risques

1. **Cache poisoning** : Cache corrompu volontairement
2. **Leak de données** : Informations sensibles dans le cache
3. **Tampering** : Modification du cache

### Protections

✅ **Ne jamais cacher de secrets**
```yaml
cache:
  paths:
    - build/
    # PAS DE ~/.ssh, ~/.aws, etc.
```

✅ **Cache isolé par branche/PR**
```yaml
key: ${{ github.ref }}-${{ hashFiles('**') }}
```

✅ **Vérification d'intégrité**
```yaml
- name: Vérifier le cache
  run: |
    if [ -f ~/.ccache/ccache.conf ]; then
      md5sum ~/.ccache/ccache.conf
    fi
```

---

## Comparaison des solutions de cache

| Aspect | GitHub Cache | GitLab Cache | Jenkins | ccache |
|--------|--------------|--------------|---------|--------|
| **Setup** | Simple | Simple | Manuel | Moyen |
| **Efficacité** | Moyenne | Bonne | Excellente | Excellente |
| **Intelligent** | ❌ Non | ❌ Non | ❌ Non | ✅ Oui |
| **Cross-build** | ⚠️ Limité | ⚠️ Limité | ✅ Oui | ✅ Oui |
| **Taille max** | 10 GB | Illimitée | Illimitée | Configurable |
| **Recommandé pour C** | ⚠️ Avec ccache | ⚠️ Avec ccache | ✅ Oui | ✅ Absolument |

**Recommandation** : Utilisez **toujours ccache** + cache de la plateforme pour `~/.ccache`

---

## Résumé

**Le cache accélère les builds en réutilisant des fichiers intermédiaires.**

**Pour les projets C** :
- ✅ **Toujours utiliser ccache**
- ✅ Cacher `~/.ccache` ou `.ccache/`
- ✅ Cacher `build/CMakeFiles/` pour CMake
- ✅ Clé stable basée sur contenu

**Configuration minimale** :
```yaml
- uses: actions/cache@v4
  with:
    path: ~/.ccache
    key: ${{ runner.os }}-ccache-${{ hashFiles('**/*.c') }}
    restore-keys: |
      ${{ runner.os }}-ccache-

- run: |
    export CC="ccache gcc"
    cmake -B build && cmake --build build
```

**Gain typique** : 80-95% de réduction du temps de build

**Cache ≠ Artifacts** :
- Cache = optimisation (fichiers intermédiaires)
- Artifacts = résultats (fichiers finaux)

---

## Prochaines étapes

Maintenant que vous maîtrisez le cache, vous pouvez :

- Optimiser vos workflows existants
- Mesurer l'impact du cache (avant/après)
- Configurer ccache dans tous vos projets
- Ajuster les clés de cache selon vos besoins
- Combiner avec d'autres optimisations (Ninja, Unity builds)

---

## Ressources complémentaires

- [GitHub Actions Cache Documentation](https://docs.github.com/en/actions/using-workflows/caching-dependencies-to-speed-up-workflows)
- [GitLab CI Cache Documentation](https://docs.gitlab.com/ee/ci/caching/)
- [ccache Documentation](https://ccache.dev/)
- [CMake ccache Integration](https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_LAUNCHER.html)

---

**🎯 Points clés à retenir :**

- ✅ Cache = réutilisation de fichiers entre builds pour accélérer
- ✅ Cache ≠ Artifacts (optimisation vs résultats)
- ✅ ccache est **essentiel** pour les projets C
- ✅ Cacher `~/.ccache`, `build/CMakeFiles/`
- ✅ Clé de cache stable : `${{ hashFiles('**/*.c') }}`
- ✅ restore-keys pour fallback
- ✅ Limiter la taille : `ccache --max-size=1G`
- ✅ Gain typique : 80-95% de réduction du temps
- ✅ Ne jamais cacher de secrets ou binaires finaux

⏭️ [Notifications et reporting](/30-integration-continue/05-notifications-reporting.md)
