🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 30.1 GitHub Actions pour projets C

## Introduction

**GitHub Actions** est un système d'intégration continue et de déploiement continu (CI/CD) directement intégré à GitHub. C'est une plateforme d'automatisation puissante qui vous permet d'exécuter automatiquement des tâches (compilation, tests, analyses) à chaque modification de votre code C, sans configuration serveur complexe.

Pour les développeurs C, GitHub Actions représente une révolution : **plus besoin de configurer un serveur Jenkins ou Travis CI**. Tout se passe directement dans votre dépôt GitHub, gratuitement pour les projets open-source, avec des machines virtuelles Ubuntu, macOS et Windows à disposition.

---

## Pourquoi GitHub Actions pour les projets C ?

### Le contexte du développement C

Le langage C présente des défis spécifiques en matière de qualité et de fiabilité :

- **Gestion manuelle de la mémoire** : Risques de fuites, corruptions, segfaults
- **Comportements indéfinis** : Division par zéro, débordements, pointeurs invalides
- **Portabilité multi-plateformes** : Code devant fonctionner sur Linux, macOS, Windows
- **Absence de runtime de sécurité** : Pas de garbage collector, pas de protection automatique
- **Compilation dépendante de l'environnement** : Versions de GCC/Clang, bibliothèques système

**GitHub Actions résout ces problèmes** en vous permettant de :

1. ✅ **Compiler automatiquement** sur plusieurs plateformes et compilateurs
2. ✅ **Tester systématiquement** avec des outils d'analyse (sanitizers, Valgrind)
3. ✅ **Détecter les bugs** avant qu'ils n'atteignent vos utilisateurs
4. ✅ **Valider la qualité** avec analyse statique et couverture de code
5. ✅ **Distribuer facilement** vos binaires et packages

### Les avantages concrets

| Avant GitHub Actions | Avec GitHub Actions |
|----------------------|---------------------|
| Compilation manuelle sur sa machine | Compilation automatique à chaque commit |
| Tests lancés manuellement (parfois oubliés) | Tests exécutés systématiquement |
| Bugs découverts par les utilisateurs | Bugs détectés avant le merge |
| "Ça marche sur ma machine" | Validation multi-plateformes automatique |
| Oubli d'exécuter Valgrind | Analyse mémoire automatique |
| Partage manuel de binaires | Publication automatique d'artifacts |

**Résultat** : Code de meilleure qualité, livré plus rapidement, avec moins de bugs.

---

## Qu'est-ce qu'un pipeline CI/CD ?

### Définitions

**CI (Continuous Integration - Intégration Continue)** :
L'automatisation de la compilation et des tests à chaque modification du code. L'objectif est de détecter les problèmes **immédiatement** après leur introduction.

**CD (Continuous Deployment - Déploiement Continu)** :
L'automatisation de la distribution et du déploiement du code validé par la CI. Pour les projets C, cela signifie publier automatiquement des binaires, créer des releases, mettre à jour la documentation.

### Le cycle d'un projet C avec CI/CD

```
1. Développeur écrit du code
         ↓
2. Développeur fait un commit et push
         ↓
3. GitHub Actions détecte le changement
         ↓
4. Compilation automatique (plusieurs configs)
         ↓
5. Tests unitaires + sanitizers
         ↓
6. Analyse statique (clang-tidy, cppcheck)
         ↓
7. Analyse dynamique (Valgrind)
         ↓
8. Couverture de code
         ↓
9. ✅ Si tout passe → Merge autorisé
    ❌ Si échec → Développeur notifié
         ↓
10. Publication automatique (artifacts, releases)
```

**Sans CI/CD** : Étapes 3-9 sont manuelles, sujettes à oubli, inconsistantes.  
**Avec CI/CD** : Tout est automatique, reproductible, fiable.  

---

## Architecture de GitHub Actions

### Les concepts clés

GitHub Actions repose sur quelques concepts fondamentaux :

#### 1. Workflow (Flux de travail)

Un **workflow** est un processus automatisé défini dans un fichier YAML. C'est votre "recette" qui décrit toutes les étapes à exécuter.

**Exemple** : Un workflow qui compile votre projet, lance les tests, puis publie les binaires.

**Emplacement** : `.github/workflows/ci.yml`

#### 2. Job (Tâche)

Un **job** est une unité d'exécution contenant plusieurs étapes. Les jobs s'exécutent en parallèle par défaut.

**Exemple** : Un job "build" qui compile, un job "test" qui teste, un job "deploy" qui publie.

#### 3. Step (Étape)

Un **step** est une action individuelle dans un job : une commande shell ou une action réutilisable.

**Exemple** : "Installer GCC", "Compiler le projet", "Lancer les tests"

#### 4. Runner (Exécuteur)

Un **runner** est une machine virtuelle qui exécute vos workflows. GitHub fournit des runners gratuits pour Linux, macOS et Windows.

**Pour les projets C** : Vous utiliserez principalement `ubuntu-latest` (Ubuntu 22.04 ou 24.04)

#### 5. Event (Événement déclencheur)

Un **event** est ce qui déclenche l'exécution d'un workflow.

**Exemples courants** :
- `push` : À chaque commit poussé
- `pull_request` : Lors de la création/mise à jour d'une PR
- `release` : Lors de la création d'une release
- `schedule` : À intervalles réguliers (cron)

### Hiérarchie des concepts

```
Workflow (fichier .yml)
  └── Event (déclencheur)
  └── Jobs (tâches parallèles)
       ├── Job 1
       │    └── Steps (étapes séquentielles)
       │         ├── Step 1
       │         ├── Step 2
       │         └── Step 3
       └── Job 2
            └── Steps
                 ├── Step 1
                 └── Step 2
```

---

## Exemple concret : Premier workflow pour un projet C

Pour vous donner une idée concrète, voici ce que fait un workflow GitHub Actions typique pour un projet C :

### Le fichier `.github/workflows/ci.yml`

```yaml
name: CI

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
    - name: Récupérer le code
      uses: actions/checkout@v4

    - name: Installer GCC
      run: sudo apt-get install -y gcc cmake

    - name: Configurer avec CMake
      run: cmake -B build

    - name: Compiler
      run: cmake --build build

    - name: Lancer les tests
      run: cd build && ctest
```

### Ce que fait ce workflow

1. **Déclenchement** : À chaque `push` ou `pull_request`
2. **Runner** : Utilise une machine Ubuntu
3. **Étapes** :
   - Télécharge votre code source
   - Installe les outils nécessaires (GCC, CMake)
   - Configure le projet avec CMake
   - Compile le code
   - Exécute les tests

**Résultat** : En 2-3 minutes, vous savez si votre code compile et si les tests passent, **sans intervention manuelle**.

---

## Ce que nous allons apprendre

Dans les sections suivantes de ce module (30.1), vous allez découvrir comment :

### 1. **Structure du workflow** (30.1.1)
- Comprendre l'anatomie d'un fichier de workflow
- Maîtriser la syntaxe YAML de base
- Configurer les déclencheurs (triggers)

### 2. **Matrix build** (30.1.2)
- Tester sur plusieurs versions de GCC/Clang
- Compiler pour Linux, macOS et Windows
- Valider différents niveaux d'optimisation

### 3. **Build avec CMake en CI** (30.1.3)
- Intégrer CMake dans le pipeline
- Configurer les build types (Debug, Release)
- Gérer les dépendances

### 4. **Exécution des tests unitaires** (30.1.4)
- Automatiser les tests avec CTest
- Intégrer des frameworks de tests (Unity, Check)
- Générer des rapports de tests

### 5. **Vérification avec sanitizers** (30.1.5)
- Activer AddressSanitizer (ASan)
- Détecter les comportements indéfinis (UBSan)
- Trouver les race conditions (TSan)

### 6. **Analyse Valgrind automatisée** (30.1.6)
- Détecter les fuites mémoire
- Analyser les accès invalides
- Profiler les performances

### 7. **Code coverage et Codecov** (30.1.7)
- Mesurer la couverture de code avec gcov/lcov
- Intégrer Codecov pour la visualisation
- Suivre l'évolution de la qualité

### 8. **Static analysis** (30.1.8)
- Analyser avec clang-tidy
- Utiliser cppcheck et scan-build
- Détecter les bugs avant la compilation

### 9. **Publication d'artifacts** (30.1.9)
- Sauvegarder les binaires compilés
- Archiver les rapports
- Distribuer les packages

---

## Pourquoi chaque section est importante

| Section | Problème résolu | Bénéfice |
|---------|-----------------|----------|
| **Structure** | Comprendre comment ça fonctionne | Base pour tout le reste |
| **Matrix build** | "Ça marche sur ma machine uniquement" | Compatibilité multi-plateformes |
| **CMake** | Build manuel, configurations variables | Build reproductible |
| **Tests unitaires** | Régressions non détectées | Code fiable |
| **Sanitizers** | Bugs mémoire cachés | Détection précoce |
| **Valgrind** | Fuites mémoire subtiles | Analyse approfondie |
| **Coverage** | Code non testé | Qualité mesurable |
| **Static analysis** | Mauvaises pratiques | Code propre et sûr |
| **Artifacts** | Distribution manuelle | Livraison automatique |

---

## Prérequis pour ce module

Pour tirer le meilleur parti de ce module, vous devriez avoir :

### Connaissances techniques

- ✅ **Bases de C** : Syntaxe, compilation, pointeurs
- ✅ **Ligne de commande** : Commandes Linux de base
- ✅ **Git** : add, commit, push, pull request
- ✅ **CMake** : Notions de base (idéalement Module 13)

### Configuration nécessaire

- ✅ **Compte GitHub** : Gratuit, créez-en un sur github.com
- ✅ **Dépôt Git** : Un projet C existant ou créez-en un nouveau
- ✅ **Accès au dépôt** : Droits d'écriture pour activer Actions

**Pas besoin de** :
- Serveur de CI externe
- Configuration réseau complexe
- Installation logicielle (tout est dans le cloud)
- Carte bancaire (gratuit pour l'open-source)

---

## Limites et quotas GitHub Actions

### Pour les dépôts publics (open-source)

- ⭐ **Gratuit et illimité** en minutes de calcul
- Plusieurs runners disponibles simultanément
- Stockage d'artifacts : 500 MB (extensible)

### Pour les dépôts privés

**Comptes gratuits** :
- 2 000 minutes/mois
- 500 MB de stockage

**Comptes payants (Pro, Team, Enterprise)** :
- Minutes supplémentaires disponibles
- Plus de runners parallèles
- Stockage étendu

### Ressources par runner

**ubuntu-latest (recommandé pour C)** :
- 2 cœurs CPU
- 7 GB RAM
- 14 GB SSD
- Temps max : 6h par job

**Suffisant pour** : 99% des projets C (compilation, tests, analyses)

---

## Philosophie DevOps et culture CI/CD

### Les principes fondamentaux

#### 1. **Automatiser tout ce qui est répétitif**

Si vous le faites plus d'une fois, automatisez-le.

```
Compilation manuelle → workflow build  
Tests manuels → workflow test  
Distribution manuelle → workflow release  
```

#### 2. **Détecter les problèmes tôt**

Plus un bug est détecté tard, plus il coûte cher à corriger.

```
Bug en dev (1h) < Bug en review (5h) < Bug en prod (50h)
```

#### 3. **Faire des petits changements fréquents**

Préférez 10 petits commits bien testés à 1 gros commit risqué.

#### 4. **Mesurer et améliorer continuellement**

- Suivez la couverture de code
- Analysez les temps de build
- Identifiez les goulots d'étranglement

#### 5. **Échec rapide, feedback rapide**

Mieux vaut savoir en 5 minutes qu'un commit casse le build plutôt qu'en découvrir les conséquences 2 semaines plus tard.

### Le "shift-left" dans les tests

**Shift-left** signifie déplacer les tests "vers la gauche" dans le cycle de développement, c'est-à-dire **le plus tôt possible**.

```
Avant (shift-right) :  
Code → Développement → Review → QA → Production → Bug découvert 😱  

Après (shift-left) :  
Code → CI (tests immédiats) → Bug découvert ✅ → Correction → Review → Production  
```

Pour les projets C, cela signifie :
- Sanitizers activés **à chaque commit**
- Analyse statique **avant la review**
- Tests unitaires **avant le merge**
- Valgrind **en nocturne**

---

## L'écosystème GitHub Actions

### GitHub Marketplace

**GitHub Marketplace** (https://github.com/marketplace?type=actions) est un répertoire d'actions réutilisables créées par la communauté.

**Actions populaires pour C** :
- `actions/checkout@v4` : Récupérer le code
- `actions/upload-artifact@v4` : Publier des artifacts
- `codecov/codecov-action@v4` : Upload couverture
- `softprops/action-gh-release@v2` : Créer des releases

**Avantage** : Ne pas réinventer la roue, utiliser des solutions éprouvées.

### Communauté et documentation

- **Documentation officielle** : https://docs.github.com/actions
- **Workflows d'exemple** : https://github.com/actions/starter-workflows
- **Forum communautaire** : GitHub Community Discussions
- **Stack Overflow** : Tag `github-actions`

---

## Comparaison avec d'autres solutions CI/CD

### GitHub Actions vs autres plateformes

| Critère | GitHub Actions | Travis CI | Jenkins | GitLab CI |
|---------|----------------|-----------|---------|-----------|
| **Intégration GitHub** | ✅ Native | ⚠️ Plugin | ⚠️ Plugin | ❌ Non |
| **Configuration** | Simple (YAML) | Simple (YAML) | Complexe | Simple (YAML) |
| **Gratuit open-source** | ✅ Illimité | ⚠️ Limité | ✅ Auto-hébergé | ✅ Partagé |
| **Multi-plateforme** | ✅ Linux/Mac/Win | ✅ Oui | ✅ Oui | ✅ Oui |
| **Marketplace** | ✅ Riche | ⚠️ Limité | ⚠️ Plugins | ⚠️ Limité |
| **Maintenance serveur** | ❌ Aucune | ❌ Aucune | ✅ Nécessaire | ⚠️ Variable |

**Pour un projet C hébergé sur GitHub** : GitHub Actions est le choix le plus logique et simple.

### Pourquoi migrer vers GitHub Actions ?

Si vous utilisez actuellement Travis CI ou Jenkins :

**Travis CI** :
- ⚠️ Service en déclin, équipe réduite
- ⚠️ Gratuit limité même pour l'open-source
- ✅ Migration simple (syntaxe YAML similaire)

**Jenkins** :
- ⚠️ Maintenance de serveur nécessaire
- ⚠️ Configuration complexe
- ⚠️ Pas d'intégration native GitHub
- ✅ Puissant mais surpuissant pour la plupart des projets C

**GitHub Actions** offre le meilleur compromis : puissance, simplicité, intégration parfaite, gratuité pour l'open-source.

---

## Stratégie de mise en place progressive

### Phase 1 : Les bases (Semaine 1)

**Objectif** : Build automatique à chaque commit

```yaml
# .github/workflows/build.yml
- Checkout du code
- Installation des outils
- Compilation avec GCC
```

**Bénéfice immédiat** : Savoir si le code compile sur un environnement propre.

### Phase 2 : Les tests (Semaine 2)

**Objectif** : Tests automatiques

```yaml
- Compilation
- Exécution des tests unitaires
- Rapport de résultats
```

**Bénéfice** : Détection des régressions.

### Phase 3 : Qualité (Semaine 3-4)

**Objectif** : Analyse de qualité

```yaml
- Sanitizers (ASan, UBSan)
- Analyse statique (clang-tidy)
- Couverture de code
```

**Bénéfice** : Code plus propre et plus sûr.

### Phase 4 : Distribution (Semaine 5)

**Objectif** : Publication automatique

```yaml
- Build multi-plateformes
- Publication d'artifacts
- Création de releases
```

**Bénéfice** : Distribution simplifiée.

### Phase 5 : Optimisation (Continue)

**Objectif** : Amélioration continue

```yaml
- Analyse de performance
- Optimisation des temps de build
- Ajout de nouvelles vérifications
```

**Ne cherchez pas la perfection dès le début**. Commencez simple, ajoutez progressivement.

---

## Sécurité et bonnes pratiques

### Secrets et tokens

GitHub Actions peut stocker des **secrets** (tokens d'API, mots de passe) de manière sécurisée.

**Exemples** :
- `CODECOV_TOKEN` pour uploader la couverture
- `GITHUB_TOKEN` (fourni automatiquement) pour interagir avec GitHub

**Bonne pratique** : Ne jamais hardcoder de credentials dans les workflows.

### Permissions minimales

Donnez seulement les permissions nécessaires :

```yaml
permissions:
  contents: read      # Lire le code
  pull-requests: write # Commenter les PRs
```

### Validation des actions tierces

Utilisez des actions de sources fiables :
- ✅ Actions officielles GitHub (`actions/*`)
- ✅ Actions populaires et maintenues
- ⚠️ Vérifiez le code des actions inconnues

### Mise en cache sécurisée

Le cache peut accélérer les builds, mais attention :
- Ne cachez jamais de secrets
- Invalidez le cache si nécessaire
- Utilisez des clés de cache appropriées

---

## Métriques et indicateurs de succès

### Comment savoir si votre CI/CD fonctionne bien ?

**Indicateurs positifs** :

- ✅ **Temps de build stable** : < 10 minutes pour un projet C moyen
- ✅ **Taux de succès élevé** : > 95% des builds passent
- ✅ **Détection rapide** : Bugs trouvés en < 5 minutes après le push
- ✅ **Couverture croissante** : +2-5% par mois
- ✅ **Zéro warning** : Pas de warnings compilateur

**Signaux d'alerte** :

- ⚠️ **Builds trop longs** : > 30 minutes (à optimiser)
- ⚠️ **Taux d'échec élevé** : < 80% (instabilité)
- ⚠️ **Faux positifs fréquents** : Tests flaky à corriger
- ⚠️ **Ignorance des échecs** : Culture de "c'est normal que ça échoue"

---

## Vision d'ensemble : Le pipeline idéal pour un projet C

Voici à quoi ressemble un pipeline CI/CD mature pour un projet C :

```
┌─────────────────────────────────────────────────────────────┐
│                         COMMIT + PUSH                       │
└──────────────────────┬──────────────────────────────────────┘
                       │
          ┌────────────▼────────────┐
          │   GitHub Actions (CI)   │
          └────────────┬────────────┘
                       │
        ┌──────────────┴──────────────┐
        │                             │
   ┌────▼─────┐                 ┌─────▼────┐
   │  Build   │                 │  Tests   │
   └────┬─────┘                 └────┬─────┘
        │                            │
   • GCC 11, 12, 13           • Tests unitaires
   • Clang 14, 15             • ASan + UBSan
   • Debug & Release          • TSan (threads)
   • Linux / macOS / Win      • CTest
        │                             │
        └──────────────┬──────────────┘
                       │
              ┌────────▼────────┐
              │    Quality      │
              └────────┬────────┘
                       │
              • Valgrind
              • clang-tidy
              • cppcheck
              • Coverage (gcov/lcov)
                       │
              ┌────────▼────────┐
              │   Artifacts     │
              └────────┬────────┘
                       │
              • Binaries
              • Reports
              • Documentation
                       │
            ┌──────────▼──────────┐
            │     ✅ Success       │
            │  → Merge autorisé   │
            └─────────────────────┘
```

**Temps total** : 5-15 minutes  
**Confiance** : Maximale  

---

## Pour aller plus loin après ce module

Une fois que vous maîtriserez GitHub Actions pour vos projets C, vous pourrez explorer :

### Intégrations avancées

- **SonarQube/SonarCloud** : Qualité de code avancée
- **Dependabot** : Mise à jour automatique des dépendances
- **GitHub Pages** : Publication automatique de documentation
- **Docker** : Environnements de build reproductibles
- **Cross-compilation** : ARM, RISC-V, embedded

### Déploiement continu (CD)

- **GitHub Releases** : Publications automatiques
- **Package managers** : Distribution via apt, homebrew, etc.
- **Container registries** : Docker Hub, GitHub Container Registry
- **Infrastructure as Code** : Terraform, Ansible

### Monitoring et observabilité

- **Dashboards** : Visualisation des métriques de CI
- **Alertes** : Notifications Slack/Discord/Email
- **Analytics** : Temps de build, taux de succès

---

## Conclusion de l'introduction

GitHub Actions transforme la façon dont vous développez en C :

**Avant** :
- Compilation locale uniquement
- Tests oubliés ou manuels
- Bugs découverts tardivement
- Distribution compliquée

**Après** :
- Validation automatique multi-plateformes
- Qualité mesurée et améliorée
- Feedback immédiat
- Livraison simplifiée

**Le résultat** : Vous passez moins de temps à déboguer et à compiler, et plus de temps à créer de nouvelles fonctionnalités. Vos utilisateurs reçoivent un logiciel plus fiable, plus rapidement.

---

## Prêt à commencer ?

Dans la section suivante (**30.1.1 Structure du workflow**), nous allons créer notre premier workflow GitHub Actions pour un projet C, étape par étape, en comprenant chaque ligne du fichier YAML.

Vous apprendrez :
- Comment créer un fichier de workflow
- La syntaxe YAML de base
- Les sections essentielles (name, on, jobs, steps)
- Comment déclencher des workflows
- Comment visualiser les résultats dans l'interface GitHub

**Continuons !** 🚀

---

## Ressources complémentaires

### Documentation officielle

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Workflow syntax](https://docs.github.com/en/actions/using-workflows/workflow-syntax-for-github-actions)
- [GitHub Actions Marketplace](https://github.com/marketplace?type=actions)

### Exemples et templates

- [Starter workflows](https://github.com/actions/starter-workflows)
- [Awesome Actions](https://github.com/sdras/awesome-actions)

### Communauté

- [GitHub Community Forum](https://github.community/)
- [Stack Overflow - github-actions](https://stackoverflow.com/questions/tagged/github-actions)

---

**🎯 Points clés de cette introduction :**

- ✅ GitHub Actions = CI/CD intégré à GitHub
- ✅ Gratuit et illimité pour l'open-source
- ✅ Automatise compilation, tests, analyses, distribution
- ✅ Workflows définis en YAML dans `.github/workflows/`
- ✅ Runners fournis : Linux, macOS, Windows
- ✅ Détection précoce des bugs = économie de temps
- ✅ Approche progressive : commencer simple, améliorer continuellement
- ✅ Essentiel pour des projets C de qualité professionnelle

⏭️ [Structure du workflow](/30-integration-continue/01.1-structure-workflow.md)
