🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 12.4 Organisation d'un projet

## Introduction

Un projet C bien organisé est comme une maison bien rangée : on trouve ce qu'on cherche rapidement, on comprend la logique, et on peut facilement y apporter des modifications. À l'inverse, un projet mal structuré devient vite un **cauchemar** à maintenir, même pour son créateur original !

Dans cette section, nous allons découvrir :
- Les principes d'une bonne organisation
- Les structures de répertoires standards
- Les conventions de nommage
- Comment organiser un projet selon sa taille
- Les fichiers essentiels d'un projet
- Les bonnes pratiques professionnelles

**Objectif :** Vous donner les clés pour structurer vos projets comme un développeur professionnel.

---

## Pourquoi bien organiser son projet ?

### Les problèmes d'un projet mal organisé

```
mon_projet/
├── truc.c
├── machin.c
├── bidule.h
├── test.c
├── test2.c
├── final.c
├── final_vraiment.c
├── final_final_v2.c
└── backup_old.c
```

**Problèmes évidents :**
- ❌ Impossible de comprendre la structure
- ❌ Noms de fichiers non descriptifs
- ❌ Pas de séparation claire entre code source, tests, et builds
- ❌ Fichiers temporaires/obsolètes mélangés avec le code actif
- ❌ Collaborer devient impossible

### Les avantages d'un projet bien organisé

```
mon_projet/
├── src/           # Sources
├── include/       # Headers publics
├── tests/         # Tests unitaires
├── docs/          # Documentation
├── build/         # Fichiers compilés
├── examples/      # Exemples d'utilisation
├── README.md      # Documentation principale
├── LICENSE        # Licence
├── Makefile       # Build system
└── .gitignore     # Fichiers à ignorer par Git
```

**Avantages :**
- ✅ Structure claire et prévisible
- ✅ Facile à naviguer
- ✅ Simple à comprendre pour un nouveau contributeur
- ✅ Build et tests séparés du code source
- ✅ Prêt pour le versionnement (Git)
- ✅ Professionnel

---

## Principes fondamentaux d'organisation

### 1. Séparation des préoccupations

**Principe :** Chaque type de contenu va dans son propre répertoire.

```
projet/
├── src/           ← Code source (.c)
├── include/       ← Headers publics (.h)
├── tests/         ← Tests
├── docs/          ← Documentation
└── build/         ← Fichiers générés (ne pas versionner)
```

### 2. Hiérarchie claire

**Principe :** L'arborescence doit refléter la logique du projet.

```
projet/
├── src/
│   ├── core/          # Cœur de l'application
│   ├── utils/         # Utilitaires
│   └── modules/       # Modules fonctionnels
│       ├── network/
│       ├── database/
│       └── auth/
```

### 3. Noms explicites

**Principe :** Les noms doivent décrire le contenu.

```
❌ Mauvais :
├── f1.c
├── f2.c
└── stuff.h

✅ Bon :
├── user_manager.c
├── database_connector.c
└── config_parser.h
```

### 4. Cohérence

**Principe :** Une fois une convention choisie, s'y tenir partout.

```
✅ Cohérent :
├── user_manager.c
├── database_manager.c
└── config_parser.c

❌ Incohérent :
├── user_manager.c
├── DatabaseManager.c    # Style différent
└── cfg-parse.c          # Convention différente
```

---

## Structure standard pour un petit projet

### Organisation minimale

Pour un projet simple (1-10 fichiers sources) :

```
my_project/
├── src/
│   ├── main.c
│   ├── functions.c
│   └── utils.c
├── include/
│   ├── functions.h
│   └── utils.h
├── Makefile
└── README.md
```

### Exemple concret : Calculatrice

```
calculatrice/
├── src/
│   ├── main.c              # Point d'entrée
│   ├── operations.c        # Addition, soustraction, etc.
│   └── parser.c            # Analyse des expressions
├── include/
│   ├── operations.h        # Prototypes des opérations
│   └── parser.h            # Prototypes du parser
├── Makefile                # Compilation
└── README.md               # Documentation
```

### Contenu type d'un `README.md` minimaliste

```markdown
# Calculatrice

Calculatrice en ligne de commande supportant les opérations de base.

## Compilation

```bash
make
```

## Utilisation

```bash
./calculatrice "2 + 3 * 4"
```

## Fonctionnalités

- Addition, soustraction, multiplication, division
- Respect des priorités opératoires
- Gestion des parenthèses
```

---

## Structure standard pour un projet moyen

### Organisation complète

Pour un projet moyen (10-50 fichiers sources) :

```
project_name/
├── include/                 # Headers publics
│   └── project_name/        # Sous-dossier avec le nom du projet
│       ├── core.h
│       ├── utils.h
│       └── config.h
├── src/                     # Implémentations
│   ├── core/
│   │   ├── engine.c
│   │   └── manager.c
│   ├── utils/
│   │   ├── string_utils.c
│   │   └── file_utils.c
│   └── main.c
├── tests/                   # Tests unitaires
│   ├── test_core.c
│   └── test_utils.c
├── docs/                    # Documentation
│   ├── api.md
│   └── architecture.md
├── examples/                # Exemples d'utilisation
│   ├── example1.c
│   └── example2.c
├── build/                   # Fichiers compilés (ignoré par Git)
├── scripts/                 # Scripts utilitaires
│   └── build.sh
├── .gitignore               # Fichiers à ignorer
├── Makefile                 # Build system
├── README.md                # Documentation principale
└── LICENSE                  # Licence du projet
```

### Détail des répertoires

#### `include/`

Contient les **headers publics** destinés à être utilisés par d'autres projets.

**Convention :** Créer un sous-dossier avec le nom du projet pour éviter les conflits.

```
include/
└── mylib/           # Nom du projet
    ├── core.h       # API publique
    └── utils.h
```

**Pourquoi ?** Si quelqu'un utilise votre bibliothèque, il fera :

```c
#include <mylib/core.h>  // Pas de conflit avec d'autres "core.h"
```

#### `src/`

Contient tous les **fichiers sources** (`.c`) et les **headers privés** (`.h` non publics).

**Organisation par modules :**

```
src/
├── core/              # Module principal
│   ├── engine.c
│   ├── engine_internal.h   # Header privé (pas dans include/)
│   └── manager.c
├── network/           # Module réseau
│   ├── socket.c
│   └── protocol.c
└── main.c            # Point d'entrée
```

**Headers privés :** Utilisés uniquement en interne, pas exposés aux utilisateurs.

#### `tests/`

Contient les **tests unitaires** et **tests d'intégration**.

```
tests/
├── test_core.c           # Tests du module core
├── test_network.c        # Tests du module network
├── test_utils.c          # Tests des utilitaires
└── test_main.c           # Runner de tests
```

**Nommage :** Préfixe `test_` suivi du nom du module testé.

#### `docs/`

Contient la **documentation technique**.

```
docs/
├── api.md                # Documentation de l'API
├── architecture.md       # Architecture du projet
├── installation.md       # Guide d'installation
└── contributing.md       # Guide de contribution
```

**Formats courants :** Markdown (`.md`), reStructuredText (`.rst`), ou Doxygen.

#### `examples/`

Contient des **exemples d'utilisation** pour les utilisateurs.

```
examples/
├── basic_usage.c         # Exemple simple
├── advanced_usage.c      # Exemple avancé
└── README.md             # Explication des exemples
```

#### `build/`

Contient les **fichiers générés** par la compilation (`.o`, exécutables).

**Important :** Ce dossier ne doit **jamais** être versionné (ajouté à `.gitignore`).

#### `scripts/`

Contient des **scripts utilitaires** (build, déploiement, tests).

```
scripts/
├── build.sh              # Script de compilation
├── test.sh               # Script de lancement des tests
└── clean.sh              # Script de nettoyage
```

---

## Structure pour un grand projet

### Organisation avancée

Pour un projet complexe (50+ fichiers sources) :

```
large_project/
├── include/                    # Headers publics
│   └── large_project/
│       ├── core/
│       │   ├── engine.h
│       │   └── manager.h
│       ├── network/
│       │   ├── client.h
│       │   └── server.h
│       └── utils/
│           └── common.h
├── src/                        # Sources
│   ├── core/
│   │   ├── engine/
│   │   │   ├── engine.c
│   │   │   ├── engine_internal.h
│   │   │   └── engine_config.c
│   │   └── manager/
│   │       ├── manager.c
│   │       └── manager_state.c
│   ├── network/
│   │   ├── client.c
│   │   ├── server.c
│   │   └── protocol.c
│   ├── utils/
│   │   ├── string_utils.c
│   │   ├── memory_utils.c
│   │   └── log.c
│   └── main.c
├── tests/                      # Tests
│   ├── unit/                   # Tests unitaires
│   │   ├── test_engine.c
│   │   └── test_network.c
│   ├── integration/            # Tests d'intégration
│   │   └── test_full_stack.c
│   └── mocks/                  # Mocks pour les tests
│       └── mock_network.c
├── docs/                       # Documentation
│   ├── api/
│   │   ├── core.md
│   │   └── network.md
│   ├── guides/
│   │   ├── installation.md
│   │   └── quickstart.md
│   └── images/
│       └── architecture.png
├── examples/                   # Exemples
│   ├── client_example/
│   │   ├── simple_client.c
│   │   └── README.md
│   └── server_example/
│       ├── simple_server.c
│       └── README.md
├── third_party/                # Dépendances externes
│   ├── json_lib/
│   └── crypto_lib/
├── build/                      # Fichiers compilés
│   ├── debug/
│   └── release/
├── cmake/                      # Modules CMake personnalisés
│   └── FindMyLib.cmake
├── scripts/                    # Scripts
│   ├── build.sh
│   ├── test.sh
│   └── install.sh
├── .github/                    # Configuration GitHub
│   └── workflows/
│       └── ci.yml
├── CMakeLists.txt              # Configuration CMake
├── .gitignore
├── README.md
├── LICENSE
└── CHANGELOG.md                # Historique des versions
```

### Modules et sous-modules

Pour les grands projets, organiser le code en **modules indépendants** :

```
src/
├── core/          # Module central
├── network/       # Module réseau (indépendant du reste)
├── database/      # Module base de données
├── auth/          # Module authentification
└── ui/            # Module interface utilisateur
```

**Principe :** Chaque module peut être compilé et testé **indépendamment**.

---

## Conventions de nommage

### Fichiers

#### Sources (`.c`)

```
✅ Recommandé :
- snake_case : user_manager.c, database_connector.c
- Descriptif : Le nom doit refléter le contenu

❌ À éviter :
- CamelCase : UserManager.c (réservé au C++)
- Abréviation : usr_mgr.c (pas clair)
- Noms génériques : module1.c, utils.c (trop vague)
```

#### Headers (`.h`)

```
✅ Recommandé :
- Même nom que le .c correspondant : user_manager.h ↔ user_manager.c
- Suffixe _internal pour les headers privés : engine_internal.h

❌ À éviter :
- Noms différents entre .c et .h : user.c ↔ manager.h
```

#### Tests

```
✅ Recommandé :
- Préfixe test_ : test_user_manager.c
- Ou suffixe _test : user_manager_test.c

Soyez cohérent dans tout le projet !
```

### Répertoires

```
✅ Recommandé :
- snake_case : network_utils, database_drivers
- Court et descriptif : core, tests, docs

❌ À éviter :
- Espaces : "network utils/" (problèmes shell)
- Caractères spéciaux : "utils&tools/"
```

### Constantes et macros

Dans les fichiers `.h` :

```c
✅ Recommandé :
#define MAX_CONNECTIONS 100
#define DEFAULT_PORT 8080
#define VERSION "1.0.0"

❌ À éviter :
#define maxConnections 100   // Pas en majuscules
#define max 100              // Nom trop générique
```

### Variables globales

```c
✅ Recommandé :
extern int g_debug_level;      // Préfixe g_ pour "global"
extern char* g_config_path;

Ou :
extern int global_debug_level;  // Préfixe "global_"
```

### Fonctions

```c
✅ Recommandé :
void user_manager_create(void);
int database_connect(const char* host);
bool auth_verify_token(const char* token);

// Préfixe par module pour éviter les conflits
```

---

## Fichiers essentiels d'un projet

### README.md

**Le fichier le plus important** : première chose que les gens lisent.

**Contenu type :**

```markdown
# Nom du projet

Une courte description (1-2 phrases).

## Fonctionnalités

- Fonctionnalité 1
- Fonctionnalité 2
- Fonctionnalité 3

## Installation

### Dépendances

- GCC 9.0+
- CMake 3.10+
- libssl-dev

### Compilation

```bash
mkdir build && cd build
cmake ..
make
```

## Utilisation

```bash
./mon_programme --help
```

## Documentation

Voir [docs/](docs/) pour la documentation complète.

## Licence

Ce projet est sous licence MIT. Voir [LICENSE](LICENSE) pour plus de détails.

## Contact

- Auteur : Votre Nom
- Email : votre.email@example.com
- GitHub : https://github.com/votre_username/projet
```

### LICENSE

**Indispensable** pour tout projet public. Licences courantes :

- **MIT** : Très permissive, autorise usage commercial
- **GPL v3** : Copyleft, les dérivés doivent aussi être GPL
- **Apache 2.0** : Permissive avec clause de brevets
- **BSD 3-Clause** : Permissive, simple

**Exemple de fichier LICENSE (MIT) :**

```
MIT License

Copyright (c) 2025 Votre Nom

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction...
```

### .gitignore

**Essentiel** pour Git : empêche le versionnement de fichiers générés.

**Contenu type pour un projet C :**

```gitignore
# Fichiers compilés
*.o
*.obj
*.a
*.so
*.dylib
*.dll
*.exe

# Répertoires de build
build/
bin/
lib/
obj/

# Fichiers de debug
*.dSYM/
core
vgcore.*

# Éditeurs
.vscode/
.idea/
*.swp
*~

# CMake
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
Makefile

# Système
.DS_Store
Thumbs.db
```

### CHANGELOG.md

**Historique des versions** pour suivre les évolutions.

**Format :**

```markdown
# Changelog

Toutes les modifications notables de ce projet seront documentées ici.

## [1.1.0] - 2025-01-15

### Ajouté
- Support des connexions SSL
- Nouveau module d'authentification

### Modifié
- Amélioration des performances du parser (30% plus rapide)

### Corrigé
- Bug de fuite mémoire dans le gestionnaire de réseau

## [1.0.0] - 2024-12-01

### Ajouté
- Version initiale
- Fonctionnalités de base
```

### CONTRIBUTING.md

**Guide de contribution** pour les projets open-source.

**Contenu type :**

```markdown
# Contribuer au projet

Merci de votre intérêt ! Voici comment contribuer :

## Processus

1. Forkez le dépôt
2. Créez une branche (`git checkout -b feature/ma-fonctionnalite`)
3. Committez vos changements (`git commit -am 'Ajout de ma fonctionnalité'`)
4. Poussez vers la branche (`git push origin feature/ma-fonctionnalite`)
5. Ouvrez une Pull Request

## Standards de code

- Suivre le style K&R
- Commenter les fonctions publiques
- Ajouter des tests pour les nouvelles fonctionnalités
- Passer tous les tests avant de soumettre

## Tests

```bash
make test
```

## Rapport de bugs

Utilisez les GitHub Issues en fournissant :
- Système d'exploitation et version
- Version du projet
- Étapes pour reproduire
- Comportement attendu vs actuel
```

---

## Exemples de structures selon le type de projet

### 1. Application en ligne de commande

```
cli_tool/
├── src/
│   ├── commands/         # Commandes disponibles
│   │   ├── cmd_help.c
│   │   ├── cmd_version.c
│   │   └── cmd_run.c
│   ├── utils/
│   │   ├── arg_parser.c
│   │   └── output.c
│   └── main.c
├── include/
│   └── cli_tool/
│       ├── commands.h
│       └── utils.h
├── tests/
│   └── test_commands.c
├── Makefile
└── README.md
```

### 2. Bibliothèque réutilisable

```
libmylib/
├── include/
│   └── mylib/            # Headers publics
│       ├── core.h
│       └── utils.h
├── src/
│   ├── core.c
│   ├── core_internal.h   # Header privé
│   └── utils.c
├── tests/
│   ├── test_core.c
│   └── test_utils.c
├── examples/
│   └── simple_usage.c
├── docs/
│   └── api.md
├── CMakeLists.txt
├── README.md
└── LICENSE
```

### 3. Serveur réseau

```
network_server/
├── src/
│   ├── server/
│   │   ├── listener.c
│   │   ├── handler.c
│   │   └── thread_pool.c
│   ├── protocol/
│   │   ├── parser.c
│   │   └── serializer.c
│   ├── storage/
│   │   ├── database.c
│   │   └── cache.c
│   └── main.c
├── include/
│   └── server/
│       ├── server.h
│       ├── protocol.h
│       └── storage.h
├── tests/
│   ├── unit/
│   └── integration/
├── config/
│   └── server.conf.example
├── scripts/
│   ├── start_server.sh
│   └── stop_server.sh
├── docs/
│   ├── protocol.md
│   └── deployment.md
└── README.md
```

### 4. Projet embarqué

```
embedded_project/
├── src/
│   ├── drivers/          # Drivers hardware
│   │   ├── uart.c
│   │   ├── gpio.c
│   │   └── i2c.c
│   ├── hal/              # Hardware Abstraction Layer
│   │   └── hal_config.c
│   ├── app/              # Logique applicative
│   │   └── main.c
│   └── utils/
│       └── circular_buffer.c
├── include/
│   ├── drivers/
│   ├── hal/
│   └── app/
├── tests/
│   └── host/             # Tests sur PC (pas sur target)
├── linker/
│   └── stm32f4.ld        # Script linker
├── Makefile
└── README.md
```

---

## Organisation des headers publics vs privés

### Concept

**Headers publics** : API exposée aux utilisateurs
**Headers privés** : Implémentation interne

### Exemple : Module de gestion d'utilisateurs

**Structure :**

```
user_module/
├── include/              # API publique
│   └── user_manager/
│       └── user.h        # Types et fonctions publics
└── src/
    ├── user.c            # Implémentation
    └── user_internal.h   # Types et fonctions privés
```

**Header public (`include/user_manager/user.h`)** :

```c
#ifndef USER_MANAGER_USER_H
#define USER_MANAGER_USER_H

// Type opaque (les détails sont dans user_internal.h)
typedef struct User User;

// API publique
User* user_create(const char* name, const char* email);
void user_destroy(User* user);
const char* user_get_name(const User* user);

#endif
```

**Header privé (`src/user_internal.h`)** :

```c
#ifndef USER_INTERNAL_H
#define USER_INTERNAL_H

#include "user_manager/user.h"

// Définition complète (cachée des utilisateurs)
struct User {
    char name[64];
    char email[128];
    int id;
    time_t created_at;
};

// Fonctions internes (non exportées)
static inline bool user_validate_email(const char* email);

#endif
```

**Avantage :** Les utilisateurs ne voient que l'API publique, pas les détails d'implémentation.

---

## Gestion des dépendances externes

### Option 1 : Sous-modules Git

```
projet/
├── external/
│   ├── json-c/          # Submodule Git
│   └── libcurl/
└── src/
```

**Commandes Git :**

```bash
git submodule add https://github.com/json-c/json-c external/json-c
git submodule update --init --recursive
```

### Option 2 : Gestionnaire de paquets

**vcpkg** (Windows, Linux, macOS) :

```bash
vcpkg install curl
vcpkg install json-c
```

**conan** :

```bash
conan install .
```

### Option 3 : Copie locale (vendor)

```
projet/
├── third_party/
│   ├── stb_image.h      # Single-header library
│   └── miniz.c          # Bibliothèque légère
└── src/
```

**Avantage :** Pas de dépendance externe, contrôle total.
**Inconvénient :** Mises à jour manuelles.

---

## Build systems et organisation

### Avec Make

```
projet/
├── Makefile             # Build principal
├── src/
│   └── Makefile         # Build du module src (optionnel)
└── tests/
    └── Makefile         # Build des tests
```

### Avec CMake (recommandé)

```
projet/
├── CMakeLists.txt       # Configuration principale
├── src/
│   └── CMakeLists.txt   # Configuration du module src
├── tests/
│   └── CMakeLists.txt   # Configuration des tests
└── cmake/
    └── modules/         # Modules CMake personnalisés
```

**Avantages de CMake :**
- Multi-plateforme (Linux, Windows, macOS)
- Génère des Makefiles, des projets Visual Studio, etc.
- Gestion automatique des dépendances

---

## Bonnes pratiques résumées

### Organisation générale

| ✅ À FAIRE | ❌ À ÉVITER |
|-----------|------------|
| Séparer sources, headers, tests | Tout mélanger dans un seul dossier |
| Utiliser des noms descriptifs | Utiliser file1.c, test.c, old.c |
| Créer un README.md complet | Projet sans documentation |
| Ajouter un .gitignore | Versionner build/ et *.o |
| Utiliser un build system (Make/CMake) | Compiler à la main à chaque fois |
| Organiser par modules logiques | Fichiers géants de 5000 lignes |

### Nommage

| ✅ À FAIRE | ❌ À ÉVITER |
|-----------|------------|
| snake_case cohérent | Mélanger styles (snake_case + camelCase) |
| Préfixer les modules (user_create, db_connect) | Noms génériques (create, connect) |
| Noms explicites | Abréviations obscures (usr_mgr, db_conn) |
| MAJUSCULES pour les macros | Minuscules pour #define |

### Fichiers

| ✅ À FAIRE | ❌ À ÉVITER |
|-----------|------------|
| Un header (.h) par source (.c) | Un header pour plusieurs .c |
| Headers publics dans include/ | Tous les headers dans src/ |
| .gitignore bien configuré | Versionner les fichiers générés |
| LICENSE pour les projets publics | Projet sans licence claire |

---

## Check-list pour un nouveau projet

### Setup initial

- [ ] Créer la structure de répertoires (src/, include/, tests/, etc.)
- [ ] Initialiser Git (`git init`)
- [ ] Créer `.gitignore`
- [ ] Créer `README.md` avec description, installation, usage
- [ ] Choisir et ajouter un fichier `LICENSE`
- [ ] Créer le build system (Makefile ou CMakeLists.txt)

### Organisation du code

- [ ] Séparer les modules logiques dans des sous-dossiers
- [ ] Créer les headers publics dans `include/projet/`
- [ ] Créer les headers privés dans `src/`
- [ ] Utiliser des gardes d'inclusion dans tous les `.h`
- [ ] Ajouter des commentaires de documentation (Doxygen)

### Tests et qualité

- [ ] Créer un dossier `tests/` avec des tests unitaires
- [ ] Ajouter un script ou une cible Make pour lancer les tests
- [ ] Compiler avec `-Wall -Wextra -Werror`
- [ ] Utiliser un outil d'analyse statique (cppcheck, clang-tidy)

### Documentation

- [ ] Documenter l'API publique
- [ ] Créer des exemples d'utilisation dans `examples/`
- [ ] Ajouter un `CHANGELOG.md` pour les versions
- [ ] Créer un `CONTRIBUTING.md` si open-source

### CI/CD

- [ ] Configurer GitHub Actions ou GitLab CI
- [ ] Automatiser les tests à chaque commit
- [ ] Automatiser la compilation multi-plateforme
- [ ] Automatiser l'analyse de code (sanitizers, Valgrind)

---

## Outils pour visualiser la structure

### `tree` : Afficher l'arborescence

```bash
tree -L 2 -I 'build|*.o'
```

**Options :**
- `-L 2` : Profondeur maximale de 2 niveaux
- `-I 'pattern'` : Ignorer les fichiers/dossiers correspondants

**Sortie exemple :**

```
projet/
├── include
│   └── projet
├── src
│   ├── core
│   └── utils
├── tests
├── Makefile
└── README.md
```

### `find` : Lister les fichiers

```bash
# Tous les fichiers .c
find src -name "*.c"

# Tous les fichiers .h
find include -name "*.h"
```

---

## Évolution d'un projet

### Phase 1 : Prototype (1-5 fichiers)

```
prototype/
├── main.c
├── utils.c
├── utils.h
└── Makefile
```

**Focus :** Faire fonctionner rapidement.

### Phase 2 : MVP (5-20 fichiers)

```
mvp/
├── src/
│   ├── core.c
│   ├── network.c
│   └── main.c
├── include/
│   ├── core.h
│   └── network.h
├── Makefile
└── README.md
```

**Focus :** Structure de base, début de documentation.

### Phase 3 : Projet mature (20+ fichiers)

```
mature_project/
├── include/projet/
├── src/
│   ├── core/
│   ├── network/
│   └── utils/
├── tests/
├── docs/
├── examples/
├── CMakeLists.txt
├── .gitignore
├── README.md
└── LICENSE
```

**Focus :** Modularité, tests, documentation complète, CI/CD.

---

## Exemples de projets open-source bien organisés

### Projets à étudier

1. **Git** (https://github.com/git/git)
   - Structure très modulaire
   - Makefile complexe mais bien organisé

2. **Redis** (https://github.com/redis/redis)
   - Organisation simple et claire
   - Code très lisible

3. **SQLite** (https://www.sqlite.org/src)
   - Projet mono-fichier mais bien structuré
   - Documentation exemplaire

4. **curl** (https://github.com/curl/curl)
   - Gestion multi-plateforme
   - Build system sophistiqué (autotools + CMake)

### Que remarquer ?

- ✅ Hiérarchie claire
- ✅ Nommage cohérent
- ✅ Documentation complète
- ✅ Tests exhaustifs
- ✅ Scripts d'automatisation
- ✅ CI/CD configuré

---

## Conclusion

L'organisation d'un projet C est un **investissement** qui paie à long terme. Un projet bien structuré :

- Se compile facilement
- Se comprend rapidement
- Se maintient sans douleur
- Attire les contributeurs
- Inspire confiance

**Points clés à retenir :**

1. **Séparez** les sources (src/), headers publics (include/), tests (tests/)
2. **Nommez** de manière cohérente et descriptive
3. **Documentez** dès le début (README.md minimum)
4. **Versionner** avec Git et un .gitignore approprié
5. **Automatiser** la compilation (Make/CMake)
6. **Testez** régulièrement
7. **Inspirez-vous** des projets open-source reconnus

Un projet bien organisé est le signe d'un développeur professionnel. Prenez le temps de structurer correctement votre code, vous vous remercierez plus tard !

Dans la section suivante (12.5), nous explorerons les **conventions de nommage et standards** pour aller encore plus loin dans la qualité du code.

⏭️ [Conventions de nommage et standards](/12-organisation-code/05-conventions-nommage.md)
