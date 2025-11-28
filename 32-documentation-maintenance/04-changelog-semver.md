🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 32.4 Changelog et versioning sémantique

## Introduction

Vous avez développé une bibliothèque C qui parse des fichiers JSON. Un utilisateur vous contacte : *"J'ai mis à jour votre lib de la v1.2 à la v1.5, et mon code ne compile plus ! Qu'est-ce qui a changé ?"*

Vous répondez : *"Euh... regarde les commits Git ?"*

Mauvaise réponse. Votre utilisateur ne va pas lire 147 commits pour comprendre ce qui a cassé son code. Il a besoin d'un **CHANGELOG** : un document lisible qui liste tous les changements importants, et d'un **système de versioning cohérent** qui lui indique si la mise à jour est sûre ou non.

Dans cette section, nous allons apprendre :
- Qu'est-ce qu'un changelog et comment le maintenir
- Le versioning sémantique (SemVer) : la norme universelle
- Comment gérer les versions avec Git tags
- Comment automatiser la génération du changelog
- Les bonnes pratiques pour les projets C

---

## Le problème sans changelog ni versioning

### Scénario catastrophe

**Projet sans changelog** :

```
Versions disponibles : 0.3, 0.8, 1.0, 1.2, 1.5, 2.0

Utilisateur : "Quelle est la différence entre 1.2 et 1.5 ?"
Mainteneur : "Euh... plein de trucs ?"
Utilisateur : "Est-ce que je peux upgrader sans casser mon code ?"
Mainteneur : "Probablement ? Essaie et tu verras."
```

**Résultat** :
- ❌ Utilisateurs frustrés
- ❌ Peur de mettre à jour (code cassé)
- ❌ Pas de confiance dans le projet
- ❌ Maintenance cauchemardesque

**Projet avec changelog et versioning** :

```
Version 1.5.0 (2025-01-15)
### Added
- Support for UTF-8 strings in JSON parser

### Fixed
- Memory leak in json_free() when nested objects
- Crash on malformed input with trailing commas

### Changed
- json_parse() now returns NULL on error (was -1)
  ⚠️ BREAKING: Check return type in your code

Version 1.2.0 (2024-12-10)
### Added
- JSON pretty-printing with json_format()
```

**Résultat** :
- ✅ Utilisateurs informés clairement
- ✅ Changements cassants identifiés
- ✅ Décision éclairée pour la mise à jour
- ✅ Confiance dans le projet

---

## Qu'est-ce qu'un Changelog ?

### Définition

Un **CHANGELOG** (littéralement "journal des changements") est un fichier qui documente **tous les changements notables** entre chaque version d'un projet, dans un ordre chronologique inversé (plus récent en premier).

**Fichier typique** : `CHANGELOG.md` à la racine du projet.

### Objectifs d'un changelog

1. **Informer les utilisateurs** : Qu'est-ce qui a changé depuis la dernière version ?
2. **Éviter les surprises** : Signaler les changements cassants (breaking changes)
3. **Documenter l'évolution** : Historique des améliorations et corrections
4. **Faciliter les mises à jour** : Décider si upgrader ou non
5. **Communiquer** : Montrer que le projet est actif et maintenu

### Changelog vs Git log

**Git log** :
```
commit a3f7d2c Fix typo in comment
commit b8e4f1a Update README
commit c9d2e5b Fix memory leak
commit d1f3g4h Add new feature
commit e2g4h5i Fix another typo
commit f3h5i6j Refactor code
```

- 📝 Détails techniques
- 🔧 Commits de maintenance (typos, refactoring)
- 💻 Pour les développeurs du projet

**Changelog** :
```
## [1.5.0] - 2025-01-15
### Added
- JSON pretty-printing support

### Fixed
- Memory leak in json_free()
```

- 📋 Changements significatifs seulement
- 🎯 Regroupés par catégorie
- 👥 Pour les **utilisateurs** du projet

**Règle d'or** : Le changelog n'est PAS une copie du git log. C'est un document **rédigé pour les humains**, pas pour les machines.

---

## Format Keep a Changelog

Le standard de facto pour les changelogs est **Keep a Changelog** (https://keepachangelog.com).

### Structure de base

```markdown
# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added
- Feature en cours de développement

## [1.5.0] - 2025-01-15
### Added
- New feature X
- Support for Y

### Changed
- Behavior Z now works differently

### Deprecated
- Function old_parse() is deprecated, use new_parse()

### Removed
- Legacy function ancient_code()

### Fixed
- Bug #123: Crash on empty input
- Memory leak in parser

### Security
- Fixed CVE-2025-12345: Buffer overflow

## [1.4.0] - 2024-12-20
### Added
- Feature A

[Unreleased]: https://github.com/user/projet/compare/v1.5.0...HEAD
[1.5.0]: https://github.com/user/projet/compare/v1.4.0...v1.5.0
[1.4.0]: https://github.com/user/projet/releases/tag/v1.4.0
```

### Catégories standard

| Catégorie | Usage | Exemple |
|-----------|-------|---------|
| **Added** | Nouvelles fonctionnalités | Support for IPv6 |
| **Changed** | Modifications de comportement | Return type changed from int to size_t |
| **Deprecated** | Fonctionnalités obsolètes (mais encore présentes) | parse_v1() is deprecated |
| **Removed** | Fonctionnalités supprimées | Removed legacy API |
| **Fixed** | Corrections de bugs | Fixed segfault in parse() |
| **Security** | Correctifs de sécurité | Fixed buffer overflow CVE-2025-XXX |

### Section [Unreleased]

La section `[Unreleased]` contient les changements **non encore publiés** dans une version officielle :

```markdown
## [Unreleased]
### Added
- Work in progress: SQLite backend support
- New function json_validate()

### Fixed
- Crash when parsing deeply nested objects
```

**Pourquoi ?**
- Montre l'activité du projet
- Prépare la prochaine release
- Permet aux early adopters de suivre le développement
- Facile de copier-coller dans une nouvelle version

**Workflow** :
```
1. Développement : Ajouter dans [Unreleased]
2. Release : Créer une nouvelle section [1.6.0], copier depuis [Unreleased]
3. Vider [Unreleased] (ou garder les WIP)
```

---

## Versioning sémantique (SemVer)

### Qu'est-ce que SemVer ?

**Semantic Versioning** (versioning sémantique) est une convention de numérotation de versions qui utilise trois chiffres :

```
MAJOR.MINOR.PATCH
  |     |     |
  |     |     └─ Corrections de bugs (backward compatible)
  |     └─────── Nouvelles fonctionnalités (backward compatible)
  └───────────── Changements cassants (breaking changes)
```

**Exemple** : `2.4.1`
- MAJOR = 2
- MINOR = 4
- PATCH = 1

### Règles de SemVer

#### 1. PATCH (ex: 1.2.3 → 1.2.4)

Incrémentez PATCH quand vous faites des **corrections de bugs** qui ne cassent pas la compatibilité.

**Exemples pour C** :
```c
// v1.2.3
int parse_json(char *json) {
    // Bug : ne gère pas les chaînes vides
    if (strlen(json) < 2) return -1;
}

// v1.2.4 (PATCH)
int parse_json(char *json) {
    // Fix : gère maintenant les chaînes vides
    if (!json || strlen(json) < 2) return -1;
}
```

**Impact utilisateur** : Aucun changement de code nécessaire, juste recompiler.

#### 2. MINOR (ex: 1.2.4 → 1.3.0)

Incrémentez MINOR quand vous ajoutez des **nouvelles fonctionnalités** sans casser l'existant.

**Exemples pour C** :
```c
// v1.2.4
int parse_json(char *json);

// v1.3.0 (MINOR)
int parse_json(char *json);
int parse_json_file(const char *filename);  // ← Nouvelle fonction
void json_set_strict_mode(bool strict);     // ← Nouvelle fonction
```

**Impact utilisateur** : Code existant fonctionne sans modification. Nouvelles fonctionnalités disponibles si besoin.

#### 3. MAJOR (ex: 1.3.0 → 2.0.0)

Incrémentez MAJOR quand vous faites des **changements cassants** (breaking changes).

**Exemples pour C** :
```c
// v1.3.0
int parse_json(char *json);  // Retourne -1 en erreur

// v2.0.0 (MAJOR)
json_t* parse_json(char *json);  // Retourne NULL en erreur
// ⚠️ BREAKING: Type de retour changé !
```

**Impact utilisateur** : Code doit être adapté. Nécessite des modifications.

### Version 0.x.y (développement initial)

Avant la première version stable (1.0.0), utilisez `0.x.y` :

```
0.1.0 - Première version alpha
0.2.0 - Ajout de fonctionnalités
0.9.0 - Beta
1.0.0 - Première version stable publique
```

**Règle** : En version `0.x.y`, **tout peut changer** à tout moment. La stabilité de l'API n'est pas garantie.

### Suffixes de version (optionnels)

Pour les versions de test ou pré-releases :

```
1.0.0-alpha          # Version alpha
1.0.0-alpha.1        # Alpha 1
1.0.0-beta           # Version beta
1.0.0-beta.2         # Beta 2
1.0.0-rc.1           # Release Candidate 1
1.0.0                # Version finale stable
```

**Exemples** :
```bash
git tag v1.0.0-alpha.1
git tag v1.0.0-beta.1
git tag v1.0.0-rc.1
git tag v1.0.0
```

---

## Quand incrémenter quelle version ?

### Arbre de décision

```
Vous avez fait des changements. Quelle version incrémenter ?

┌─ Les changements cassent-ils le code existant ?
│
├─ OUI → MAJOR (2.0.0)
│   Exemples :
│   - Changer le type de retour d'une fonction
│   - Supprimer une fonction publique
│   - Modifier les paramètres d'une fonction
│   - Changer le comportement d'une fonction de façon incompatible
│
└─ NON → Les changements ajoutent-ils des fonctionnalités ?
    │
    ├─ OUI → MINOR (1.3.0)
    │   Exemples :
    │   - Ajouter une nouvelle fonction
    │   - Ajouter un nouveau paramètre optionnel
    │   - Nouvelle structure ou enum
    │
    └─ NON → PATCH (1.2.1)
        Exemples :
        - Correction de bugs
        - Amélioration de performance (sans changer l'API)
        - Correction de fuites mémoire
        - Mise à jour de documentation
```

### Exemples concrets pour projets C

#### Cas 1 : Correction de bug → PATCH

```c
// v1.2.0
size_t json_strlen(const char *str) {
    // Bug : ne gère pas str == NULL
    return strlen(str);
}

// v1.2.1 (PATCH)
size_t json_strlen(const char *str) {
    // Fix
    return str ? strlen(str) : 0;
}
```

**Changelog** :
```markdown
## [1.2.1] - 2025-01-16
### Fixed
- json_strlen() now handles NULL input without crashing
```

#### Cas 2 : Nouvelle fonctionnalité → MINOR

```c
// v1.2.1
typedef struct {
    char *data;
    size_t size;
} json_t;

// v1.3.0 (MINOR)
typedef struct {
    char *data;
    size_t size;
} json_t;

// ← Nouvelles fonctions
json_t* json_format(json_t *json, int indent);
void json_minify(json_t *json);
```

**Changelog** :
```markdown
## [1.3.0] - 2025-01-20
### Added
- json_format(): Pretty-print JSON with configurable indentation
- json_minify(): Remove whitespace from JSON
```

#### Cas 3 : Changement cassant → MAJOR

```c
// v1.3.0
int json_parse(const char *input, json_t **output);
// Retourne 0 = succès, -1 = erreur

// v2.0.0 (MAJOR)
json_t* json_parse(const char *input, json_error_t *error);
// Retourne pointeur ou NULL
// ⚠️ BREAKING: Signature complètement différente
```

**Changelog** :
```markdown
## [2.0.0] - 2025-02-01
### Changed
- ⚠️ BREAKING: json_parse() signature changed
  - Old: `int json_parse(const char *input, json_t **output)`
  - New: `json_t* json_parse(const char *input, json_error_t *error)`
  - Migration: Replace error checking from `if (ret < 0)` to `if (result == NULL)`

### Added
- New json_error_t structure for detailed error reporting
```

#### Cas 4 : Dépréciation → MINOR

```c
// v1.4.0
// Ancienne API (dépréciée)
__attribute__((deprecated("Use json_parse_ex() instead")))
int json_parse(const char *input);

// Nouvelle API
int json_parse_ex(const char *input, json_options_t *opts);
```

**Changelog** :
```markdown
## [1.4.0] - 2025-01-25
### Added
- json_parse_ex(): New parsing function with options

### Deprecated
- json_parse(): Use json_parse_ex() instead
  This function will be removed in v2.0.0
```

---

## Gérer les versions avec Git

### Tags Git

Les **tags** Git marquent des points spécifiques dans l'historique, typiquement les versions.

#### Créer un tag

```bash
# Tag léger (juste un pointeur)
git tag v1.2.0

# Tag annoté (recommandé : contient message, auteur, date)
git tag -a v1.2.0 -m "Release version 1.2.0"

# Tag avec message multilignes
git tag -a v1.2.0 -m "Release version 1.2.0

New features:
- JSON pretty-printing
- UTF-8 support

Bug fixes:
- Memory leak in parser
- Crash on malformed input"
```

#### Lister les tags

```bash
# Tous les tags
git tag

# Tags avec pattern
git tag -l "v1.*"

# Tags avec détails
git tag -n
```

#### Pousser les tags

```bash
# Pousser un tag spécifique
git push origin v1.2.0

# Pousser tous les tags
git push origin --tags
```

#### Supprimer un tag

```bash
# Localement
git tag -d v1.2.0

# Sur le remote
git push origin --delete v1.2.0
```

#### Checkout un tag

```bash
# Voir le code à une version spécifique
git checkout v1.2.0

# Créer une branche depuis un tag
git checkout -b hotfix-1.2.1 v1.2.0
```

### Convention de nommage des tags

**Format recommandé** : `v` + numéro de version SemVer

```bash
v1.0.0
v1.0.1
v1.1.0
v2.0.0
v2.0.0-rc.1
v2.0.0-beta.1
```

**Pourquoi le `v` ?**
- Distinction claire entre tags de version et autres tags
- Convention universelle (Linux kernel, Node.js, Go, etc.)
- Évite les conflits avec des noms de branches

### Workflow de release

Processus typique pour publier une nouvelle version :

```bash
# 1. Finaliser le code sur main
git checkout main
git pull origin main

# 2. Mettre à jour le CHANGELOG.md
vim CHANGELOG.md
# - Renommer [Unreleased] en [1.3.0]
# - Ajouter la date
# - Créer nouvelle section [Unreleased]

# 3. Mettre à jour le numéro de version dans le code
vim CMakeLists.txt  # PROJECT_VERSION
vim configure.ac    # AC_INIT version
vim src/version.h   # #define VERSION "1.3.0"

# 4. Commiter les changements de version
git add CHANGELOG.md CMakeLists.txt src/version.h
git commit -m "Bump version to 1.3.0"

# 5. Créer le tag
git tag -a v1.3.0 -m "Release version 1.3.0

New features:
- JSON formatting with indentation
- UTF-8 string support

Bug fixes:
- Memory leak in json_free()
- Crash on deeply nested objects"

# 6. Pousser
git push origin main
git push origin v1.3.0

# 7. Créer une release sur GitHub
gh release create v1.3.0 \
  --title "Version 1.3.0" \
  --notes-file CHANGELOG.md \
  dist/libmyjson-1.3.0.tar.gz
```

---

## Écrire un bon Changelog

### Principes généraux

#### 1. Écrivez pour les utilisateurs, pas pour vous

❌ **Mauvais** (jargon technique) :
```markdown
### Changed
- Refactored parser.c line 234-567
- Updated Makefile dependencies
```

✅ **Bon** (orienté utilisateur) :
```markdown
### Changed
- Improved parsing performance by 40%
- Build system now auto-detects dependencies
```

#### 2. Regroupez les changements par catégorie

❌ **Mauvais** (vrac) :
```markdown
- Fix bug in parser
- Add new function
- Update docs
- Fix another bug
- Add tests
```

✅ **Bon** (organisé) :
```markdown
### Added
- json_validate(): Validate JSON without parsing
- Comprehensive test suite

### Fixed
- Segfault when parsing empty objects
- Memory leak in error handling

### Changed
- Documentation updated with examples
```

#### 3. Soyez spécifique

❌ **Mauvais** (vague) :
```markdown
- Fixed bugs
- Improved performance
- Updated code
```

✅ **Bon** (précis) :
```markdown
- Fixed buffer overflow in parse_string() with strings > 4KB
- Improved parsing speed by 40% through optimized hash table
- Replaced deprecated strcpy() with strncpy() for safety
```

#### 4. Mentionnez les breaking changes

❌ **Mauvais** (caché) :
```markdown
### Changed
- Updated parse_json() function
```

✅ **Bon** (visible) :
```markdown
### Changed
- ⚠️ BREAKING: parse_json() now returns NULL on error instead of -1
  Migration guide: Replace `if (parse_json(...) < 0)` with `if (parse_json(...) == NULL)`
```

#### 5. Référencez les issues et PRs

```markdown
### Fixed
- Memory leak in json_free() when handling nested objects (#234)
- Crash on malformed UTF-8 input (#456, #457)

### Added
- JSON Schema validation support (!123)
  Thanks to @contributor for the PR
```

### Template de bonne entrée

```markdown
## [X.Y.Z] - YYYY-MM-DD

### Added
- **Feature name**: Brief description of what it does and why it's useful
  - Sub-detail if needed
- **Another feature**: Description

### Changed
- **Behavior change**: Clear description of old behavior → new behavior
  - Impact on users
  - Migration path if breaking change

### Deprecated
- **Function/API**: What's deprecated, what to use instead, when it will be removed
  ```c
  // Old (deprecated)
  old_function();

  // New (recommended)
  new_function();
  ```

### Removed
- **Feature/API**: What was removed and why
  - Alternative if available

### Fixed
- **Bug description**: What was wrong and how it's fixed (#issue-number)
- **Another bug**: Description (@contributor if external)

### Security
- **CVE-YYYY-XXXXX**: Description of security issue and fix
  Severity: High/Medium/Low
```

---

## Automatiser le Changelog

### Générer depuis les commits Git

Si vous suivez les bonnes pratiques de commits atomiques avec des messages conventionnels, vous pouvez générer le changelog automatiquement.

#### Conventional Commits

Format standardisé de messages de commit :

```
<type>(<scope>): <description>

[optional body]

[optional footer]
```

**Types** :
- `feat`: Nouvelle fonctionnalité (→ MINOR)
- `fix`: Correction de bug (→ PATCH)
- `docs`: Documentation seulement
- `style`: Formatage, whitespace
- `refactor`: Refactoring sans changement de comportement
- `test`: Ajout/modification de tests
- `chore`: Tâches de maintenance
- `perf`: Amélioration de performance
- `BREAKING CHANGE`: Changement cassant (→ MAJOR)

**Exemples** :
```bash
git commit -m "feat: add json_validate() function"
git commit -m "fix: prevent buffer overflow in parse_string()"
git commit -m "docs: update README with new examples"
git commit -m "feat!: change parse_json() return type

BREAKING CHANGE: parse_json() now returns NULL on error instead of -1"
```

#### Outil : git-cliff

**git-cliff** génère automatiquement un changelog depuis les commits :

```bash
# Installation
cargo install git-cliff

# Configuration .cliff.toml
[changelog]
header = """
# Changelog\n
All notable changes to this project will be documented in this file.\n
"""
body = """
{% for group, commits in commits | group_by(attribute="group") %}
    ### {{ group | upper_first }}
    {% for commit in commits %}
        - {{ commit.message | upper_first }} ({{ commit.id | truncate(length=7, end="") }})
    {% endfor %}
{% endfor %}
"""

# Générer le changelog
git cliff --output CHANGELOG.md

# Générer pour une plage de versions
git cliff v1.0.0..v2.0.0 --output CHANGELOG-2.0.0.md
```

#### Outil : standard-version

```bash
# Installation
npm install -g standard-version

# Génère changelog et bump la version automatiquement
standard-version

# Dry run
standard-version --dry-run

# Pour une version spécifique
standard-version --release-as major
standard-version --release-as minor
standard-version --release-as patch
```

#### Script simple maison

```bash
#!/bin/bash
# scripts/generate_changelog.sh

VERSION=$1
PREVIOUS_VERSION=$2

if [ -z "$VERSION" ]; then
    echo "Usage: $0 <version> [previous_version]"
    exit 1
fi

if [ -z "$PREVIOUS_VERSION" ]; then
    PREVIOUS_VERSION=$(git describe --tags --abbrev=0)
fi

echo "## [$VERSION] - $(date +%Y-%m-%d)"
echo ""

echo "### Added"
git log ${PREVIOUS_VERSION}..HEAD --oneline | grep "^[a-f0-9]* feat:" | sed 's/^[a-f0-9]* feat: /- /'
echo ""

echo "### Fixed"
git log ${PREVIOUS_VERSION}..HEAD --oneline | grep "^[a-f0-9]* fix:" | sed 's/^[a-f0-9]* fix: /- /'
echo ""

echo "### Changed"
git log ${PREVIOUS_VERSION}..HEAD --oneline | grep "BREAKING CHANGE" | sed 's/^[a-f0-9]* /- ⚠️ BREAKING: /'
```

**Usage** :
```bash
./scripts/generate_changelog.sh 1.3.0 v1.2.0 >> CHANGELOG.md
```

---

## Exemple complet de CHANGELOG pour projet C

```markdown
# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Work in progress: SQLite backend support

## [2.0.0] - 2025-02-01

### Changed
- ⚠️ **BREAKING**: Changed `json_parse()` function signature
  - Old: `int json_parse(const char *input, json_t **output)`
  - New: `json_t* json_parse(const char *input, json_error_t *error)`
  - **Migration**:
    ```c
    // Before (v1.x)
    json_t *result;
    if (json_parse(input, &result) < 0) {
        // Handle error
    }

    // After (v2.0)
    json_error_t error;
    json_t *result = json_parse(input, &error);
    if (result == NULL) {
        fprintf(stderr, "Error: %s at line %d\n", error.message, error.line);
    }
    ```

### Added
- `json_error_t` structure for detailed error reporting with line/column numbers
- `json_get_version()` function to query library version at runtime
- Support for JSON5 format (trailing commas, comments)

### Removed
- Legacy `json_parse_string()` function (deprecated since v1.5.0)
  Use `json_parse()` instead

### Fixed
- Stack overflow with deeply nested objects (>1000 levels)
- Incorrect handling of escaped Unicode characters (#342)

### Security
- Fixed buffer overflow in `json_unescape()` (CVE-2025-12345)
  Severity: High
  All users should upgrade immediately

## [1.5.0] - 2025-01-15

### Added
- `json_format()`: Pretty-print JSON with configurable indentation
  ```c
  char *formatted = json_format(json, 2);  // 2 spaces indent
  ```
- UTF-8 validation in string parsing
- Benchmark suite in `tests/benchmarks/`

### Fixed
- Memory leak in `json_free()` when freeing nested objects (#234)
- Crash on malformed input with trailing commas (#456)
- Incorrect parsing of floating-point numbers in scientific notation (#467)

### Changed
- Improved parsing performance by 40% through hash table optimization
- `json_get()` now returns `const json_t*` instead of `json_t*` for safety

### Deprecated
- `json_parse_string()` is deprecated, use `json_parse()` instead
  This function will be removed in v2.0.0

## [1.4.0] - 2024-12-20

### Added
- Support for parsing JSON from file: `json_parse_file(filename)`
- `json_merge()` function to merge two JSON objects
- Thread-safe parsing with `json_parse_r()` (reentrant version)

### Fixed
- Race condition in multi-threaded usage (#198)
- Memory corruption when parsing strings > 64KB (#201)

## [1.3.0] - 2024-11-10

### Added
- `json_validate()`: Validate JSON without full parsing
- Comprehensive test suite with 95% code coverage
- CMake support in addition to Make

### Fixed
- Segfault when parsing empty JSON objects `{}`
- Integer overflow with very large numbers

### Changed
- Documentation rewritten with more examples
- Build system improvements: faster compilation

## [1.2.0] - 2024-10-05

### Added
- `json_to_string()`: Serialize JSON back to string
- Support for NULL values in JSON

### Fixed
- Parser now correctly handles whitespace in all contexts

## [1.1.0] - 2024-09-01

### Added
- Basic JSON parsing functionality
- Support for objects, arrays, strings, numbers, booleans

## [1.0.0] - 2024-08-15

### Added
- Initial stable release
- Core JSON parsing engine
- Memory management utilities
- Comprehensive documentation

[Unreleased]: https://github.com/user/jsonlib/compare/v2.0.0...HEAD
[2.0.0]: https://github.com/user/jsonlib/compare/v1.5.0...v2.0.0
[1.5.0]: https://github.com/user/jsonlib/compare/v1.4.0...v1.5.0
[1.4.0]: https://github.com/user/jsonlib/compare/v1.3.0...v1.4.0
[1.3.0]: https://github.com/user/jsonlib/compare/v1.2.0...v1.3.0
[1.2.0]: https://github.com/user/jsonlib/compare/v1.1.0...v1.2.0
[1.1.0]: https://github.com/user/jsonlib/compare/v1.0.0...v1.1.0
[1.0.0]: https://github.com/user/jsonlib/releases/tag/v1.0.0
```

---

## Releases GitHub/GitLab

### Créer une release sur GitHub

Via l'interface web :
1. Allez dans **Releases** → **Create a new release**
2. Choisissez le tag (ou créez-en un nouveau)
3. Titre : `Version X.Y.Z`
4. Description : Copiez la section du CHANGELOG
5. Attachez des binaires compilés si nécessaire
6. Cochez "Pre-release" si c'est une beta/rc
7. Publish

Via la CLI GitHub (`gh`) :
```bash
# Créer une release avec le changelog
gh release create v1.5.0 \
  --title "Version 1.5.0 - JSON Formatting" \
  --notes-file <(sed -n '/## \[1.5.0\]/,/## \[1.4.0\]/p' CHANGELOG.md | head -n -1) \
  dist/libjson-1.5.0.tar.gz \
  dist/libjson-1.5.0-linux-x64.deb \
  dist/libjson-1.5.0-linux-x64.rpm

# Pre-release
gh release create v2.0.0-rc.1 \
  --title "Version 2.0.0 Release Candidate 1" \
  --notes "First release candidate for v2.0.0" \
  --prerelease
```

### Créer une release sur GitLab

```bash
# Via l'API GitLab
curl --request POST \
  --header "PRIVATE-TOKEN: $GITLAB_TOKEN" \
  --data "name=v1.5.0" \
  --data "tag_name=v1.5.0" \
  --data "description=$(cat CHANGELOG.md)" \
  "https://gitlab.com/api/v4/projects/$PROJECT_ID/releases"
```

---

## Bonnes pratiques

### 1. Maintenez le CHANGELOG à jour en continu

❌ **Mauvais** :
```
Commit tous les jours pendant 3 mois
→ Lors de la release : "Euh... qu'est-ce qu'on a fait déjà ?"
→ Générer le changelog en 10 minutes en lisant 500 commits
```

✅ **Bon** :
```
Chaque fois que vous mergez une PR/feature :
→ Ajouter une ligne dans [Unreleased]
→ 5 minutes par feature
→ Lors de la release : copier-coller [Unreleased] → [X.Y.Z]
```

### 2. Versionnez le CHANGELOG

Le fichier `CHANGELOG.md` doit être versionné dans Git :

```bash
git add CHANGELOG.md
git commit -m "Update CHANGELOG for v1.5.0"
```

### 3. Un CHANGELOG par projet

Si vous avez un monorepo avec plusieurs projets C :

```
monorepo/
├── libparser/
│   └── CHANGELOG.md
├── libutils/
│   └── CHANGELOG.md
└── app/
    └── CHANGELOG.md
```

Chaque sous-projet a son propre changelog et ses propres versions.

### 4. Dates ISO 8601

Utilisez toujours le format `YYYY-MM-DD` :

- ✅ `2025-01-15`
- ❌ `15/01/2025`
- ❌ `Jan 15, 2025`
- ❌ `15-01-2025`

### 5. Compatibilité descendante

Privilégiez la compatibilité :

```c
// v1.5.0 - Ajouter une fonctionnalité sans casser l'existant
int parse_json(const char *input);                      // Existant
int parse_json_ex(const char *input, json_opts_t *opts); // Nouveau

// v2.0.0 - Si vraiment nécessaire de casser
json_t* parse_json(const char *input, json_error_t *err);
```

**Période de transition** :
```
v1.5.0 : Introduire nouvelle API, déprécier l'ancienne
v1.6.0 : Avertissements de dépréciation
v2.0.0 : Supprimer ancienne API
```

### 6. Documentation de migration

Pour les breaking changes, fournissez un guide de migration :

```markdown
## Migration Guide v1.x → v2.0

### API Changes

#### json_parse() signature change

**Before (v1.x):**
```c
json_t *result;
int ret = json_parse(input, &result);
if (ret < 0) {
    fprintf(stderr, "Parse error\n");
}
```

**After (v2.0):**
```c
json_error_t error;
json_t *result = json_parse(input, &error);
if (result == NULL) {
    fprintf(stderr, "Parse error: %s at line %d\n",
            error.message, error.line);
}
```

### Removed Functions

- `json_parse_string()` → Use `json_parse()`
- `json_old_free()` → Use `json_free()`
```

### 7. Testez avant de releaser

```bash
# Checklist de release
[ ] Tous les tests passent
[ ] Pas de warnings de compilation
[ ] Valgrind clean
[ ] Documentation à jour
[ ] CHANGELOG complété
[ ] Version bumped dans le code
[ ] Tag créé
[ ] Binaires compilés et testés
[ ] Release publiée
```

---

## Intégration avec CI/CD

### Automatiser le bump de version

```yaml
# .github/workflows/release.yml
name: Release

on:
  push:
    tags:
      - 'v*'

jobs:
  release:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Extract version from tag
        id: version
        run: echo "VERSION=${GITHUB_REF#refs/tags/v}" >> $GITHUB_OUTPUT

      - name: Build
        run: |
          mkdir build && cd build
          cmake -DVERSION=${{ steps.version.outputs.VERSION }} ..
          make

      - name: Create tarball
        run: |
          tar czf libjson-${{ steps.version.outputs.VERSION }}.tar.gz \
            build/libjson.so README.md LICENSE CHANGELOG.md

      - name: Extract changelog for this version
        id: changelog
        run: |
          sed -n "/## \[${{ steps.version.outputs.VERSION }}\]/,/## \[/p" CHANGELOG.md | head -n -1 > release_notes.md

      - name: Create GitHub Release
        uses: softprops/action-gh-release@v1
        with:
          body_path: release_notes.md
          files: |
            libjson-${{ steps.version.outputs.VERSION }}.tar.gz
```

### Vérifier le versioning

```yaml
- name: Check version consistency
  run: |
    # Version dans le tag
    TAG_VERSION=${GITHUB_REF#refs/tags/v}

    # Version dans CMakeLists.txt
    CMAKE_VERSION=$(grep 'project.*VERSION' CMakeLists.txt | grep -oP '\d+\.\d+\.\d+')

    # Version dans le code
    CODE_VERSION=$(grep '#define VERSION' src/version.h | grep -oP '\d+\.\d+\.\d+')

    if [ "$TAG_VERSION" != "$CMAKE_VERSION" ] || [ "$TAG_VERSION" != "$CODE_VERSION" ]; then
      echo "❌ Version mismatch!"
      echo "Tag: $TAG_VERSION"
      echo "CMake: $CMAKE_VERSION"
      echo "Code: $CODE_VERSION"
      exit 1
    fi

    echo "✅ All versions match: $TAG_VERSION"
```

---

## Checklist de release

Avant de publier une nouvelle version :

### Code
- [ ] Tous les tests unitaires passent
- [ ] Compilation sans warnings
- [ ] Valgrind sans erreurs
- [ ] Coverage > seuil minimum (ex: 80%)
- [ ] Code formatté (clang-format)
- [ ] Pas de TODOs/FIXMEs dans le code

### Documentation
- [ ] README à jour
- [ ] CHANGELOG complété pour cette version
- [ ] Documentation API générée (Doxygen)
- [ ] Guide de migration si breaking changes
- [ ] Exemples à jour

### Version
- [ ] Numéro de version cohérent (tag, CMake, code)
- [ ] Respect du SemVer
- [ ] Tag Git créé et poussé
- [ ] Section [Unreleased] vidée ou réorganisée

### Distribution
- [ ] Binaires compilés et testés
- [ ] Archives créées (.tar.gz, .zip)
- [ ] Packages créés si applicable (.deb, .rpm)
- [ ] Release GitHub/GitLab publiée

### Communication
- [ ] Annonce sur le site/blog
- [ ] Tweet/post réseaux sociaux si applicable
- [ ] Email aux mainteneurs/utilisateurs majeurs
- [ ] Mise à jour gestionnaires de paquets (Conan, vcpkg)

---

## Erreurs courantes

### Erreur 1 : Oublier de mettre à jour le CHANGELOG

❌ **Symptôme** :
```
Release v1.5.0
CHANGELOG.md : dernière version = v1.3.0
```

✅ **Solution** : Automatiser avec un script CI qui vérifie la cohérence.

### Erreur 2 : Changer la version MAJOR pour rien

❌ **Mauvais** :
```
v1.5.0 : Ajout d'une fonction
→ v2.0.0 : "Parce que ça fait plus sérieux"
```

✅ **Bon** :
```
v1.5.0 : Ajout d'une fonction
→ v1.6.0 : Car pas de breaking change
```

### Erreur 3 : Breaking change sans bump MAJOR

❌ **Mauvais** :
```
v1.5.0 → v1.6.0
Changement de signature de fonction (breaking!)
```

✅ **Bon** :
```
v1.5.0 → v2.0.0
Changement de signature (breaking change → MAJOR)
```

### Erreur 4 : CHANGELOG vague

❌ **Mauvais** :
```markdown
## [1.5.0]
- Improvements
- Bug fixes
- Updates
```

✅ **Bon** :
```markdown
## [1.5.0]
### Added
- json_format(): Pretty-print with indentation

### Fixed
- Memory leak in json_free() (#234)
- Crash on empty input (#456)
```

---

## Outils et ressources

### Outils de versioning

- **git-cliff** : Générateur de changelog
  https://github.com/orhun/git-cliff

- **standard-version** : Automatise changelog + version
  https://github.com/conventional-changelog/standard-version

- **semantic-release** : Automatisation complète
  https://github.com/semantic-release/semantic-release

### Validateurs

- **SemVer.org** : Validateur de versions
  https://semver.org/

- **Commitlint** : Valide les messages de commit
  https://commitlint.js.org/

### Références

- **Keep a Changelog** : https://keepachangelog.com/
- **Semantic Versioning** : https://semver.org/
- **Conventional Commits** : https://www.conventionalcommits.org/
- **GitHub Releases** : https://docs.github.com/en/repositories/releasing-projects-on-github

---

## Conclusion

Un bon changelog et un versioning sémantique cohérent sont essentiels pour :

- ✅ **Communiquer** les changements clairement
- ✅ **Faciliter** les mises à jour
- ✅ **Prévenir** les bugs liés aux incompatibilités
- ✅ **Documenter** l'évolution du projet
- ✅ **Professionnaliser** votre projet

**Points clés à retenir** :

1. **CHANGELOG** : Documentez tous les changements notables
2. **SemVer** : MAJOR.MINOR.PATCH avec règles claires
3. **Tags Git** : Marquez chaque version
4. **Breaking changes** : Toujours visibles et documentés
5. **Automatisation** : Utilisez des outils pour faciliter le processus

Un projet sans changelog ni versioning clair est un projet qui n'inspire pas confiance. Prenez l'habitude de maintenir ces documents dès le début, et vos utilisateurs vous remercieront.

---


⏭️ [Maintenance du code legacy](/32-documentation-maintenance/05-maintenance-code-legacy.md)
