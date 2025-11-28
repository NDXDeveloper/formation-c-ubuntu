🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 32.3 Gestion de versions avec Git

## Introduction générale

Imaginez que vous développez un projet C depuis plusieurs mois. Vous ajoutez une nouvelle fonctionnalité, et soudain... tout casse. Rien ne compile plus. Vous aimeriez revenir en arrière, mais vous avez modifié 20 fichiers et vous ne vous souvenez plus de ce qui fonctionnait. Vous tentez de tout défaire manuellement, mais vous aggravez la situation. Finalement, vous perdez des heures à tout reconstruire.

Ce scénario cauchemardesque arrive à tout développeur qui ne maîtrise pas le **contrôle de version**. Et dans le monde professionnel, travailler sans Git (ou un système équivalent) est tout simplement impensable.

Dans cette section, nous allons maîtriser **Git**, l'outil de gestion de versions le plus populaire au monde, et apprendre à l'utiliser efficacement pour des projets C.

---

## Pourquoi Git est indispensable ?

### 1. Machine à remonter le temps

Git est une **machine à remonter le temps** pour votre code. À tout moment, vous pouvez :

- Revenir à une version antérieure qui fonctionnait
- Voir exactement ce qui a changé entre deux versions
- Retrouver quand et par qui un bug a été introduit
- Expérimenter sans risque, sachant que vous pouvez tout annuler

**Exemple concret** :
```bash
# Oh non, j'ai tout cassé !
git status           # Voir ce qui a changé
git checkout .       # Tout annuler, revenir à la dernière version stable

# Ou revenir 3 commits en arrière
git reset --hard HEAD~3
```

Sans Git, vous devriez faire des copies manuelles du projet à chaque étape, nommer les dossiers `projet_v1`, `projet_v2_final`, `projet_v2_final_vraiment`... Vous avez déjà vu ça ? C'est le chaos.

### 2. Collaboration sans conflits

Git permet à plusieurs développeurs de travailler sur le même projet **simultanément** sans se marcher sur les pieds.

**Sans Git** :
```
Alice modifie parser.c
Bob modifie parser.c en même temps
→ Qui a la bonne version ?
→ Comment fusionner les changements ?
→ Conflit, perte de code, frustration
```

**Avec Git** :
```
Alice travaille sur sa branche : feature/json-export
Bob travaille sur sa branche : bugfix/memory-leak
→ Chacun avance indépendamment
→ Git fusionne automatiquement les changements
→ Les conflits sont détectés et résolus proprement
```

### 3. Traçabilité et documentation

Chaque changement dans Git est documenté :
- **Qui** a fait le changement
- **Quand** il a été fait
- **Pourquoi** (message de commit)
- **Quoi** exactement a été modifié

**Exemple d'historique Git** :
```
commit a3f7d2c
Author: Alice <alice@example.com>
Date:   2025-01-15 14:32:10

    Fix buffer overflow in parse_line()

    The function was not checking buffer size before strcpy(),
    causing crashes with lines > 256 chars. Now uses strncpy()
    with proper bounds checking.

    Fixes: #789
```

Cette traçabilité est **cruciale** pour :
- Comprendre pourquoi le code est ainsi
- Déboguer les régressions
- Auditer les changements de sécurité
- Onboarder de nouveaux développeurs

### 4. Standard industriel

**Git n'est pas une option, c'est une obligation** dans le monde professionnel :

- ✅ **100% des projets open-source** majeurs utilisent Git (Linux, Git lui-même, Redis, Nginx, etc.)
- ✅ **99% des entreprises tech** exigent Git dans leurs offres d'emploi
- ✅ **Toutes les plateformes** modernes sont basées sur Git : GitHub, GitLab, Bitbucket
- ✅ **Intégration CI/CD** : Tous les pipelines démarrent avec un commit Git

Ne pas connaître Git en 2025, c'est comme être développeur sans savoir utiliser un éditeur de texte.

---

## Git : Concepts fondamentaux

Avant de plonger dans les détails, comprenons les concepts clés.

### Qu'est-ce qu'un système de contrôle de version ?

Un **VCS** (Version Control System) est un logiciel qui enregistre les modifications de fichiers au fil du temps, permettant de :

1. **Revenir** à des versions antérieures
2. **Comparer** les changements entre versions
3. **Collaborer** avec d'autres développeurs
4. **Expérimenter** sans risque

**Analogie** : C'est comme la fonction "Annuler" de votre éditeur de texte, mais :
- Infiniment plus puissante
- Fonctionne sur des fichiers multiples
- Préserve tout l'historique
- Partageable entre développeurs

### Git vs autres VCS

| Système | Type | Popularité | Particularité |
|---------|------|------------|---------------|
| **Git** | Distribué | ⭐⭐⭐⭐⭐ | Rapide, branches légères |
| SVN (Subversion) | Centralisé | ⭐⭐ | Plus simple mais limité |
| Mercurial | Distribué | ⭐ | Similaire à Git |
| Perforce | Centralisé | ⭐ | Gros fichiers binaires |

**Pourquoi Git domine ?**
- Créé par Linus Torvalds (créateur de Linux) en 2005
- Conçu pour être rapide et efficace
- Branches ultra-légères (création instantanée)
- Workflow décentralisé (chacun a une copie complète)
- Écosystème riche (GitHub, GitLab, outils CLI/GUI)

### Architecture Git : Distribué

**Centralisé (SVN)** :
```
        Serveur Central
             |
    +--------+--------+
    |        |        |
  Alice    Bob    Charlie
```

Problème : Si le serveur tombe, personne ne peut travailler.

**Distribué (Git)** :
```
  GitHub (remote)
       |
   +---+---+
   |   |   |
Alice Bob Charlie
(chacun a une copie complète)
```

Avantage : Chacun peut travailler offline, fusionner localement, puis synchroniser.

### Les trois états de Git

Git gère vos fichiers en trois états principaux :

```
Working Directory  →  Staging Area  →  Repository
(Workspace)           (Index)           (Commits)
     |                    |                  |
   [Fichiers            [git add]         [git commit]
    modifiés]         préparation       historique permanent
```

**1. Working Directory (Répertoire de travail)** :
- Vos fichiers actuels sur le disque
- Vous y travaillez normalement

**2. Staging Area (Zone de transit)** :
- Fichiers préparés pour le prochain commit
- Vous choisissez ce qui sera commité

**3. Repository (Dépôt)** :
- Historique permanent des commits
- Base de données Git (.git/)

**Workflow typique** :
```bash
# 1. Modifier des fichiers (working directory)
vim src/parser.c

# 2. Préparer pour commit (staging)
git add src/parser.c

# 3. Enregistrer dans l'historique (repository)
git commit -m "Fix parser bug"
```

---

## Git pour les projets C : Spécificités

Les projets C ont des particularités qui influencent l'utilisation de Git.

### 1. Fichiers binaires générés

Un projet C génère beaucoup de fichiers binaires :

```
projet/
├── src/
│   ├── main.c          ← Source (versionner)
│   └── main.o          ← Binaire (NE PAS versionner)
├── build/
│   └── programme       ← Exécutable (NE PAS versionner)
└── .git/
```

**Règle d'or** : Ne JAMAIS versionner les fichiers générés (voir section 32.3.3).

### 2. Organisation modulaire

Les projets C sont souvent organisés en modules :

```
projet/
├── include/           # Headers publics
├── src/              # Sources
├── tests/            # Tests unitaires
├── lib/              # Bibliothèques externes
├── docs/             # Documentation
├── scripts/          # Scripts de build
└── CMakeLists.txt    # Configuration build
```

Git permet de gérer efficacement cette structure, avec des branches par module si nécessaire.

### 3. Compilation multi-plateforme

Le code C doit souvent compiler sur plusieurs plateformes (Linux, macOS, Windows). Git facilite :

- Branches par plateforme si nécessaire
- `.gitignore` adapté à chaque OS
- CI/CD pour tester sur toutes les plateformes
- Tags pour les releases multi-plateformes

### 4. Intégration avec les outils de build

Git s'intègre naturellement avec :

- **CMake** : Versionne CMakeLists.txt, ignore build/
- **Make** : Versionne Makefile, ignore *.o
- **Autotools** : Versionne configure.ac, ignore fichiers générés
- **CI/CD** : Déclenché par les commits Git

---

## Structure de la section 32.3

Cette section est divisée en trois parties essentielles qui couvrent tous les aspects de Git pour les projets C.

### 32.3.1 Commits atomiques

**Objectif** : Apprendre à créer des commits propres, logiques et maintenables.

**Contenu** :
- Qu'est-ce qu'un commit atomique ?
- Pourquoi c'est crucial (debugging, review, rollback)
- Comment décomposer son travail en commits logiques
- Messages de commit efficaces
- Utilisation de `git add -p` pour commits partiels
- Erreurs courantes à éviter

**Compétences acquises** :
- ✅ Créer des commits qui racontent une histoire claire
- ✅ Faciliter les code reviews
- ✅ Simplifier le debugging avec `git bisect`
- ✅ Permettre les rollbacks précis

**Pourquoi c'est important pour le C ?**

Les projets C sont souvent complexes (gestion mémoire, pointeurs, système). Des commits atomiques permettent de :
- Isoler les bugs facilement
- Revenir sur un changement problématique sans tout casser
- Comprendre l'évolution du code

### 32.3.2 Branches et workflow

**Objectif** : Maîtriser les branches Git et choisir le bon workflow pour votre projet.

**Contenu** :
- Concepts de branches (création, fusion, suppression)
- Workflows populaires : Git Flow, GitHub Flow, GitLab Flow
- Stratégies de fusion : merge vs rebase vs squash
- Gestion des conflits
- Protection des branches
- Pull Requests et code review
- Workflows en équipe

**Compétences acquises** :
- ✅ Créer et gérer des branches efficacement
- ✅ Choisir le bon workflow selon le projet
- ✅ Collaborer sans se marcher sur les pieds
- ✅ Résoudre les conflits proprement

**Pourquoi c'est important pour le C ?**

Dans les projets C :
- Features peuvent prendre plusieurs jours (branches longues)
- Bugfixes critiques nécessitent des hotfix branches
- Plusieurs versions en production simultanées (v1.x, v2.x)
- Expérimentation d'algorithmes sans casser le code stable

### 32.3.3 Gestion des binaires

**Objectif** : Gérer correctement les fichiers binaires dans un projet C.

**Contenu** :
- Pourquoi Git n'aime pas les binaires
- Fichiers à ne JAMAIS commiter (.o, .exe, build/)
- Configuration `.gitignore` pour projets C
- Quels binaires peuvent être versionnés
- Git LFS pour gros fichiers binaires
- Alternatives : gestionnaires de paquets (Conan, vcpkg)
- Nettoyer un dépôt pollué

**Compétences acquises** :
- ✅ Configurer `.gitignore` correctement
- ✅ Garder un dépôt léger et rapide
- ✅ Gérer les dépendances externes
- ✅ Éviter les erreurs de débutant

**Pourquoi c'est important pour le C ?**

Les projets C génèrent beaucoup de binaires :
- Fichiers objets (.o) : Un par fichier source
- Exécutables et bibliothèques (.exe, .so, .a)
- Fichiers de build (CMakeFiles/, build/)
- Fichiers de debug (core dumps, .dSYM)

Sans bonne gestion, votre dépôt Git devient un monstre de plusieurs Go, lent à cloner et à utiliser.

---

## Vue d'ensemble du workflow Git complet

Voici le workflow typique d'un développeur C avec Git, qui intègre les trois sous-sections :

```
1. SETUP (une fois)
   ├─ git clone projet
   ├─ Configurer .gitignore (32.3.3)
   └─ Comprendre le workflow du projet (32.3.2)
          ↓
2. DÉVELOPPEMENT (quotidien)
   ├─ Créer une branche feature (32.3.2)
   │    git checkout -b feature/json-export
   ├─ Coder en plusieurs étapes
   ├─ Faire des commits atomiques (32.3.1)
   │    git add src/json.c
   │    git commit -m "Add JSON serialization function"
   │    git add tests/test_json.c
   │    git commit -m "Add tests for JSON export"
   ├─ Vérifier qu'aucun binaire n'est ajouté (32.3.3)
   │    git status
   │    # Pas de .o, .exe, build/ dans la liste
   └─ Pusher la branche
        git push -u origin feature/json-export
          ↓
3. REVIEW ET MERGE (collaboration)
   ├─ Ouvrir une Pull Request (32.3.2)
   ├─ Code review par les pairs
   ├─ Corrections si nécessaire (nouveaux commits atomiques)
   └─ Merge dans main
          ↓
4. MAINTENANCE (continu)
   ├─ Nettoyer les branches mergées (32.3.2)
   ├─ Gérer les releases avec tags
   └─ Maintenir .gitignore à jour (32.3.3)
```

---

## Prérequis et installation

### Installation de Git

**Ubuntu/Debian** :
```bash
sudo apt update
sudo apt install git
```

**Fedora** :
```bash
sudo dnf install git
```

**macOS** :
```bash
brew install git
# Ou : Xcode Command Line Tools (déjà inclus)
```

**Windows** :
- Télécharger Git for Windows : https://git-scm.com/download/win
- Ou utiliser WSL2 (Windows Subsystem for Linux)

**Vérifier l'installation** :
```bash
git --version
# git version 2.39.0 ou supérieur
```

### Configuration initiale

Avant votre premier commit, configurez votre identité :

```bash
# Nom et email (obligatoire)
git config --global user.name "Votre Nom"
git config --global user.email "votre.email@example.com"

# Éditeur par défaut (optionnel)
git config --global core.editor "vim"
# Ou : nano, emacs, code (VS Code)

# Couleurs (optionnel mais recommandé)
git config --global color.ui auto

# Vérifier la configuration
git config --list
```

### Prérequis de connaissance

Pour suivre cette section efficacement, vous devriez :

- ✅ Savoir utiliser la ligne de commande Linux
- ✅ Avoir des bases en C (compilation, fichiers .c/.h)
- ✅ Comprendre la structure d'un projet C
- ✅ Être à l'aise avec un éditeur de texte

Si vous êtes complètement débutant en Git, c'est normal ! Cette section est conçue pour vous guider pas à pas.

---

## Git en contexte : Module 32 Documentation et Maintenance

Cette section s'intègre dans le **Module 32** qui couvre la documentation et la maintenance des projets C :

### Lien avec les autres sections

**32.1 Documentation du code (Doxygen)** :
- Les commentaires Doxygen sont versionnés avec Git
- La documentation générée (HTML/PDF) n'est PAS versionnée (voir 32.3.3)
- Git permet de suivre l'évolution de la documentation

**32.2 README et documentation utilisateur** :
- Le README est le premier fichier versionné dans Git
- Git permet de maintenir plusieurs versions de documentation (par branche/tag)
- Historique des changements visible pour les utilisateurs

**32.3 Gestion de versions avec Git** ← Vous êtes ici
- Cœur du système de maintenance
- Permet tous les workflows modernes

**32.4 Changelog et versioning sémantique** :
- Git tags marquent les versions
- Commits bien écrits facilitent la génération du changelog
- Historique Git = base pour le versioning

### Git : Colonne vertébrale du DevOps

Git est au centre de toutes les pratiques modernes :

```
     Git (code versionné)
            |
    +-------+-------+
    |       |       |
   CI/CD   Doc   Tests
    |       |       |
Déploiement auto  Validation
```

Sans Git, pas de :
- ❌ Intégration continue (CI)
- ❌ Déploiement automatisé (CD)
- ❌ Code review structurée
- ❌ Traçabilité des changements
- ❌ Rollback rapide en production

---

## Philosophie Git : Principes à adopter

Avant de plonger dans les commandes, adoptons la bonne mentalité.

### 1. Commit souvent, commit atomique

**Mauvaise approche** :
```
[Un gros commit après 3 jours]
"Modifications diverses"
```

**Bonne approche** :
```
[Plusieurs petits commits dans la journée]
"Add JSON parsing function"
"Add tests for JSON parser"
"Fix edge case with empty objects"
"Update documentation"
```

### 2. Branche pour chaque tâche

**Mauvaise approche** :
```
Tout sur main
→ Code cassé en permanence
→ Difficile de travailler à plusieurs
```

**Bonne approche** :
```
main (stable)
├─ feature/export-json (Alice)
├─ bugfix/memory-leak (Bob)
└─ hotfix/security-patch (Charlie)
```

### 3. Never break main

La branche `main` doit **toujours** être stable et déployable.

**Règles** :
- ✅ Code compile sur main
- ✅ Tests passent sur main
- ✅ Pas de push direct sur main (utiliser des PRs)
- ✅ Protection de branche activée

### 4. Les messages de commit sont de la documentation

**Mauvais message** :
```bash
git commit -m "fix"
```
Que corrige-t-on ? Impossible à savoir.

**Bon message** :
```bash
git commit -m "Fix buffer overflow in parse_line()

The function was not checking buffer bounds before strcpy(),
causing crashes with long input lines. Now uses strncpy()
with proper size validation.

Fixes: #789"
```

Quelqu'un qui lit ça dans 6 mois comprend immédiatement.

### 5. .gitignore dès le début

**Ne JAMAIS commiter** :
- ❌ Fichiers compilés (.o, .exe)
- ❌ Dossiers de build (build/, CMakeFiles/)
- ❌ Fichiers temporaires (.log, .tmp)
- ❌ Configurations IDE personnelles

Créez `.gitignore` **avant** votre premier commit.

---

## Ressources d'apprentissage

### Documentation officielle

- **Pro Git Book** (gratuit, complet) : https://git-scm.com/book/fr/v2
- **Git Reference** : https://git-scm.com/docs
- **Git Cheatsheet** : https://education.github.com/git-cheat-sheet-education.pdf

### Tutoriels interactifs

- **Learn Git Branching** : https://learngitbranching.js.org/
- **Git Immersion** : https://gitimmersion.com/
- **GitHub Learning Lab** : https://lab.github.com/

### Communautés

- **Stack Overflow** : Tag [git]
- **Reddit** : r/git
- **GitHub Community** : https://github.community/

---

## Outils Git pour débutants

### Interface graphique (GUI)

Si la ligne de commande vous intimide au début :

**Multi-plateforme** :
- **GitKraken** : Interface moderne et intuitive
- **GitHub Desktop** : Simple, intégré avec GitHub
- **Sourcetree** : Complet, gratuit

**Linux** :
- **gitg** : Léger et simple
- **GitCola** : Interface Qt

**Intégré aux IDEs** :
- **VS Code** : Extension GitLens
- **CLion** : Git intégré
- **Vim** : Plugin Fugitive

### Ligne de commande (recommandé à terme)

La ligne de commande est **plus puissante** et **universelle**. Vous la retrouverez sur tous les serveurs.

**Commandes essentielles** à connaître :
```bash
git status       # État actuel
git add          # Ajouter au staging
git commit       # Créer un commit
git push         # Envoyer vers remote
git pull         # Récupérer depuis remote
git branch       # Gérer les branches
git log          # Voir l'historique
git diff         # Voir les différences
```

Nous les verrons toutes en détail dans les sections suivantes.

---

## Estimation du temps d'apprentissage

Pour maîtriser Git dans le contexte d'un projet C :

**Débutant complet** :
- 32.3.1 Commits atomiques : 2-3 heures
- 32.3.2 Branches et workflow : 3-4 heures
- 32.3.3 Gestion des binaires : 1-2 heures
- **Total : 6-9 heures**

**Développeur avec bases Git** :
- Révision et approfondissement : 2-3 heures
- Focus sur spécificités C : 1-2 heures
- **Total : 3-5 heures**

**Remarque** : La vraie maîtrise vient avec la pratique quotidienne. Après 1-2 mois d'utilisation régulière, Git devient une seconde nature.

---

## Checklist : Êtes-vous prêt ?

Avant de commencer les sections détaillées, vérifiez :

- [ ] Git est installé (`git --version`)
- [ ] Configuration initiale faite (`git config --list`)
- [ ] Compte GitHub/GitLab créé (pour la pratique)
- [ ] SSH keys configurées (optionnel mais recommandé)
- [ ] Éditeur de texte configuré
- [ ] Projet C de test prêt (ou créez-en un simple)

Si tout est OK, vous êtes prêt à plonger dans les commits atomiques !

---

## Ce que vous allez apprendre

À la fin de cette section 32.3, vous serez capable de :

### Compétences techniques

- ✅ **Créer des commits atomiques** propres et logiques
- ✅ **Gérer des branches** efficacement (création, fusion, suppression)
- ✅ **Choisir et appliquer un workflow** adapté à votre projet
- ✅ **Résoudre des conflits** de merge proprement
- ✅ **Configurer .gitignore** pour éviter les binaires
- ✅ **Utiliser Git LFS** pour les gros fichiers si nécessaire
- ✅ **Collaborer en équipe** avec Pull Requests et code reviews
- ✅ **Maintenir un historique propre** avec rebase et squash

### Compétences professionnelles

- ✅ **Travailler comme un pro** : Workflows utilisés dans l'industrie
- ✅ **Communiquer efficacement** : Messages de commit clairs
- ✅ **Collaborer sans friction** : Branches, PRs, reviews
- ✅ **Déboguer efficacement** : Utiliser l'historique Git pour trouver les bugs
- ✅ **Documenter automatiquement** : L'historique Git raconte l'histoire du projet

---

## Motivation finale

Git peut sembler intimidant au début. Il y a beaucoup de commandes, de concepts, de workflows. Mais voici la vérité :

**Vous n'avez pas besoin de tout connaître pour être productif.**

80% de votre utilisation quotidienne de Git se résume à environ **10 commandes** :

```bash
git status
git add
git commit
git push
git pull
git branch
git checkout / git switch
git merge
git log
git diff
```

Le reste ? Vous l'apprendrez progressivement, au fur et à mesure des besoins.

**Commencez petit** : Utilisez Git pour un projet personnel. Commitez souvent. Créez des branches. Faites des erreurs (vous pouvez toujours revenir en arrière, c'est Git !). Et avec le temps, Git deviendra votre meilleur allié.

Comme le dit Linus Torvalds, créateur de Git :

> *"Git is not that hard to learn if you understand the basic concepts. Most people just memorize commands without understanding what's going on."*

Cette section vous donnera cette compréhension fondamentale.

---

## Prêt à commencer ?

Maintenant que vous comprenez l'importance de Git et ce que vous allez apprendre, passons à la pratique !

**➡️ Section suivante : 32.3.1 Commits atomiques**

Dans la prochaine section, vous apprendrez à créer des commits propres, logiques et maintenables - la base d'un bon workflow Git.

Bonne découverte de Git ! 🚀

---

## Références rapides

### Commandes Git de base (antisèche)

```bash
# Configuration
git config --global user.name "Nom"
git config --global user.email "email@example.com"

# Initialiser un dépôt
git init
git clone <url>

# Workflow de base
git status              # Voir l'état
git add <fichier>       # Ajouter au staging
git add .               # Ajouter tout
git commit -m "message" # Créer un commit
git push                # Envoyer vers remote
git pull                # Récupérer depuis remote

# Branches
git branch              # Lister les branches
git branch <nom>        # Créer une branche
git checkout <branche>  # Changer de branche
git checkout -b <nom>   # Créer et changer
git merge <branche>     # Fusionner une branche

# Historique
git log                 # Voir l'historique
git log --oneline       # Version compacte
git show <commit>       # Voir un commit
git diff                # Voir les différences

# Annulation
git checkout .          # Annuler modifications locales
git reset HEAD~1        # Annuler dernier commit
git revert <commit>     # Créer un commit inverse
```

Cette antisèche sera votre compagnon tout au long de votre apprentissage.

⏭️ [Commits atomiques](/32-documentation-maintenance/03.1-commits-atomiques.md)
