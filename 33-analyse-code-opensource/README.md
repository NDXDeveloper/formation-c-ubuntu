🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 33. Analyse de code open-source

## Introduction

**De l'apprentissage théorique à la pratique professionnelle**

Vous avez parcouru un long voyage à travers ce tutoriel. Vous avez maîtrisé :
- La syntaxe du C et ses subtilités (Modules 1-2)
- La gestion de la mémoire et les pointeurs (Module 3)
- Les structures de données et l'architecture modulaire (Module 4)
- L'outillage DevOps moderne (Module 5)
- La programmation système Linux (Module 6)
- Les techniques avancées et l'optimisation (Module 7)
- L'intégration continue et le déploiement (Module 8)

**Maintenant, il est temps de passer au niveau supérieur : apprendre des meilleurs.**

Ce module est différent. Plutôt que d'apprendre de nouvelles syntaxes ou techniques isolées, vous allez **étudier du code professionnel** utilisé par des millions de personnes dans le monde entier. Vous allez comprendre **comment** et **pourquoi** les experts ont fait certains choix architecturaux, et **comment appliquer** ces leçons à vos propres projets.

**L'analyse de code open-source est le pont entre la théorie et la pratique professionnelle.**

---

## 🎯 Pourquoi analyser du code open-source ?

### 1. Apprendre des meilleurs

**Les projets open-source sont une université gratuite**

Imaginez avoir accès à :
- 📚 Des **millions de lignes de code** écrites par des experts
- 🔍 L'**historique complet** des décisions techniques (commits, PRs, discussions)
- 💬 Les **débats** qui ont mené aux choix d'architecture
- 🧪 Les **tests** qui garantissent la qualité
- 📖 La **documentation** expliquant le "pourquoi" et pas seulement le "comment"

C'est exactement ce que l'open-source vous offre.

**Exemple concret** :

```
Question : "Comment implémenter une hash table performante ?"

Réponse traditionnelle :
- Lire un livre d'algorithmique
- Implémenter une version basique
- Espérer que ça fonctionne en production

Réponse via l'open-source :
- Étudier dict.c dans Redis (hash table production-ready)
- Voir le rehashing incrémental en action
- Comprendre pourquoi deux tables sont utilisées
- Observer comment les collisions sont gérées
- Lire les tests qui couvrent tous les cas limites
- Appliquer ces techniques à votre projet
```

**Vous n'apprenez pas qu'une hash table fonctionne, vous apprenez comment elle fonctionne en production avec 100 000+ requêtes par seconde.**

---

### 2. Comprendre les architectures réelles

**Les livres enseignent la théorie, l'open-source enseigne la réalité**

#### **Ce que les livres vous disent** :

```c
// Théorie : "Utilisez malloc() pour l'allocation dynamique"
void *ptr = malloc(size);  
if (ptr == NULL) {  
    // Gérer l'erreur
}
```

#### **Ce que le code professionnel vous montre** :

```c
// Redis (src/zmalloc.c) : Wrapper qui track la mémoire
void *zmalloc(size_t size) {
    void *ptr = malloc(size + PREFIX_SIZE);
    if (!ptr) zmalloc_oom_handler(size);

    *((size_t*)ptr) = size;  // Stocker la taille
    update_zmalloc_stat_alloc(size);  // Tracking

    return (char*)ptr + PREFIX_SIZE;
}

// Nginx (src/core/ngx_palloc.c) : Memory pools
ngx_pool_t *pool = ngx_create_pool(16384, log);  
void *ptr = ngx_palloc(pool, size);  // Ultra-rapide  
// Tout libéré en une fois : ngx_destroy_pool(pool);

// Git (git-compat-util.h) : Macros pour sécurité
#define ALLOC_ARRAY(x, alloc) (x) = xmalloc(st_mult(sizeof(*(x)), (alloc)))
```

**Leçon** : Les pros ne se contentent pas de `malloc()`. Ils ajoutent :
- Tracking de la mémoire (Redis)
- Pools pour la performance (Nginx)
- Protection contre les overflows (Git)

**Vous apprenez les patterns de production, pas juste la syntaxe.**

---

### 3. Découvrir les patterns et anti-patterns

#### **Patterns que vous découvrirez** :

**Pattern 1 : Object Pooling (Redis)**
```c
// Réutiliser des objets au lieu de les recréer
shared.integers[42];  // Pré-alloué, partagé entre toutes les clés
```

**Pattern 2 : Lazy Loading (Git)**
```c
// Ne charger que quand nécessaire
struct commit *c = lookup_commit(oid);  // Rapide (juste alloc)  
parse_commit(c);  // Lent (lecture disque) - seulement si nécessaire  
```

**Pattern 3 : Event Loop (Nginx, Redis)**
```c
// Un seul thread gère des milliers de connexions
while (!stop) {
    events = epoll_wait(epfd, events, MAX, timeout);
    for (int i = 0; i < events; i++) {
        handle_event(events[i]);
    }
}
```

**Pattern 4 : Goto Cleanup (Linux Kernel, Git)**
```c
int function(void) {
    char *buf = NULL;
    int fd = -1;
    int result = -1;

    buf = malloc(1024);
    if (!buf) goto cleanup;

    fd = open("file", O_RDONLY);
    if (fd < 0) goto cleanup;

    result = 0;  // Succès

cleanup:
    if (buf) free(buf);
    if (fd >= 0) close(fd);
    return result;
}
```

---

#### **Anti-patterns que vous éviterez** :

❌ **Anti-pattern 1 : String concatenation avec strcat()**
```c
// MAUVAIS : Buffer overflow facile
char buf[100];  
strcpy(buf, str1);  
strcat(buf, str2);  // ❌ Pas de vérification de taille  

// BON : Redis utilise sds (Simple Dynamic String)
sds s = sdsnew(str1);  
s = sdscat(s, str2);  // ✅ Agrandissement automatique  
```

❌ **Anti-pattern 2 : Pas de vérification de malloc()**
```c
// MAUVAIS
void *ptr = malloc(size);
*ptr = value;  // ❌ Crash si malloc échoue

// BON : Git utilise xmalloc()
void *ptr = xmalloc(size);  // Arrête le programme si échec
*ptr = value;  // ✅ Toujours safe
```

**En étudiant du code professionnel, vous apprenez non seulement ce qu'il faut faire, mais aussi ce qu'il ne faut PAS faire.**

---

### 4. Préparer à contribuer

**Analyser avant de contribuer**

Avant de pouvoir contribuer efficacement à un projet, vous devez :
- 📖 **Comprendre son architecture** : Comment est-il structuré ?
- 🎨 **Connaître son style** : Quelles conventions sont utilisées ?
- 🔧 **Maîtriser ses patterns** : Quels idiomes reviennent souvent ?
- 🧪 **Saisir sa philosophie** : Quelles sont les priorités (performance ? lisibilité ?) ?

**Pipeline d'apprentissage** :

```
1. Analyser du code professionnel
   ↓
2. Comprendre les patterns et l'architecture
   ↓
3. Identifier les bonnes pratiques
   ↓
4. Appliquer à vos propres projets
   ↓
5. Contribuer à des projets open-source
   ↓
6. Devenir un développeur reconnu
```

**Ce module couvre les étapes 1-4 et vous prépare pour l'étape 5 (section 33.5).**

---

### 5. Développer une compétence professionnelle essentielle

**La capacité à lire du code est plus importante que celle d'en écrire**

**Statistiques réelles** :
- 📊 Un développeur passe **70-80%** de son temps à lire du code
- 📊 Seulement **20-30%** à en écrire
- 📊 **90%** du code que vous toucherez en entreprise est existant

**En entreprise, vous serez souvent confronté à** :
- Hériter d'un projet legacy de 100 000+ lignes
- Déboguer du code écrit par quelqu'un qui a quitté l'entreprise
- Intégrer une bibliothèque externe
- Faire une code review
- Reverse-engineer une fonctionnalité

**La capacité à comprendre rapidement du code inconnu est une compétence qui se monnaie très cher.**

---

## 🗺️ Vue d'ensemble du module

### Structure du module

Ce module est organisé en deux grandes parties :

#### **Partie 1 : Méthodologie et études de cas (Sections 33.1-33.4)**

**33.1 - Méthodologie de lecture de code**
- Comment aborder un projet inconnu
- Techniques pour naviguer dans une grande codebase
- Outils pour analyser et comprendre le code
- Stratégies pour identifier les parties importantes

**33.2 - Étude de cas : Git**
- Architecture d'un système de versioning distribué
- Structures de données (objets, commits, trees)
- Patterns de conception (object pooling, lazy loading)
- Leçons applicables à vos projets

**33.3 - Étude de cas : Redis**
- Architecture d'une base de données in-memory
- Structures de données avancées (SDS, dict, skiplist)
- Event loop et performance
- Gestion optimisée de la mémoire

**33.4 - Étude de cas : Nginx**
- Architecture multi-process event-driven
- Gestion de milliers de connexions simultanées
- Memory pools et zero-copy
- Parser HTTP en machine à états

---

#### **Partie 2 : Contribution (Section 33.5)**

**33.5 - Contribution à des projets C open-source**
- 33.5.1 : Trouver un bon premier issue
- 33.5.2 : Comprendre le coding style
- 33.5.3 : Soumettre une Pull Request

**Cette partie transforme vos connaissances en contributions concrètes.**

---

### Pourquoi ces trois projets ?

**Git, Redis, et Nginx** ont été choisis pour des raisons spécifiques :

#### **Git : La complexité maîtrisée**

✅ **Ce que vous apprendrez** :
- Architecture modulaire d'un grand projet (300K lignes)
- Gestion de structures de données complexes
- Système de plugins et d'extensibilité
- Tests exhaustifs et CI/CD

✅ **Patterns clés** :
- Object pooling et caching
- Lazy loading
- Table de dispatch pour les commandes
- API en couches

✅ **Applicable à** :
- Outils CLI complexes
- Systèmes avec beaucoup de commandes
- Projets nécessitant extensibilité

---

#### **Redis : La performance ultime**

✅ **Ce que vous apprendrez** :
- Architecture single-threaded haute performance
- Structures de données optimisées (SDS, dict avec rehashing incrémental)
- Event loop et multiplexage I/O
- Gestion agressive de la mémoire

✅ **Patterns clés** :
- Simple Dynamic Strings (SDS)
- Rehashing incrémental
- Shared objects (Flyweight)
- Encodings multiples pour économiser la mémoire

✅ **Applicable à** :
- Bases de données in-memory
- Caches haute performance
- Systèmes temps-réel
- Serveurs I/O-bound

---

#### **Nginx : La scalabilité**

✅ **Ce que vous apprendrez** :
- Architecture master/workers
- Event-driven non-bloquant
- Memory pools pour allocation rapide
- Zero-copy avec sendfile()

✅ **Patterns clés** :
- Multi-process event loop
- State machines pour parsing
- Pool allocators
- Configuration modulaire

✅ **Applicable à** :
- Serveurs web et proxies
- Load balancers
- Systèmes gérant des milliers de connexions
- Applications événementielles

---

### Comparaison des architectures

| Aspect | Git | Redis | Nginx |
|--------|-----|-------|-------|
| **Architecture** | Single-process, multi-commandes | Single-process, event-driven | Multi-process, event-driven |
| **Threads** | 1 | 1 (+ threads I/O optionnels) | 1 par worker |
| **Performance** | Pas critique | Ultra-haute (100K+ ops/sec) | Haute (10K+ connexions) |
| **Mémoire** | Modérée | Critique (tout en RAM) | Optimisée (pools) |
| **I/O** | Filesystem-bound | Network + RAM | Network-bound |
| **Use case** | Versioning, CLI | Cache, DB in-memory | Web server, proxy |

**Chaque projet représente une catégorie d'architecture différente. En les étudiant tous les trois, vous couvrez un large spectre de patterns professionnels.**

---

## 🎓 Prérequis et préparation

### Connaissances requises

Pour tirer le maximum de ce module, vous devriez avoir :

#### **Niveau minimum** :

✅ **Syntaxe C** (Modules 1-2)
- Variables, fonctions, structures de contrôle
- Pointeurs de base
- Structures et types custom

✅ **Compilation** (Module 1)
- Comment compiler un projet
- Makefile basique
- Options de GCC

✅ **Lecture de code**
- Capacité à lire et comprendre du C
- Navigation dans des fichiers multiples

---

#### **Niveau recommandé** :

✅ **Gestion mémoire** (Module 3)
- Malloc/free
- Fuites mémoire
- Pointeurs avancés

✅ **Structures de données** (Module 4)
- Listes, arbres, hash tables
- Modularité

✅ **Outils** (Module 5)
- GDB pour déboguer
- Valgrind pour analyser la mémoire
- Git pour naviguer l'historique

✅ **Programmation système** (Module 6)
- Fichiers et I/O
- Processus
- Sockets (pour Nginx/Redis)

---

### Outils nécessaires

#### **Pour naviguer le code** :

```bash
# Installation des outils essentiels
sudo apt update  
sudo apt install -y \  
    git \
    build-essential \
    ctags \
    cscope \
    silversearcher-ag  # ou ripgrep

# Outils optionnels mais utiles
sudo apt install -y \
    tree \
    cloc \
    graphviz
```

---

#### **Pour analyser** :

```bash
# Outils d'analyse statique
sudo apt install -y \
    cppcheck \
    clang-tools

# Outils de visualisation
sudo apt install -y \
    doxygen \
    cflow
```

---

#### **IDE/Éditeur recommandé** :

**VS Code** avec extensions :
```json
{
    "recommendations": [
        "ms-vscode.cpptools",
        "eamodio.gitlens",
        "stkb.rewrap",
        "gruntfuggly.todo-tree"
    ]
}
```

**Vim** avec plugins :
```vim
" Navigation de code
Plug 'preservim/tagbar'  
Plug 'mileszs/ack.vim'  

" Git integration
Plug 'tpope/vim-fugitive'
```

---

### Workspace pour l'analyse

**Structure recommandée** :

```
~/code-analysis/
├── methodology/           # Notes sur les techniques
│   └── notes.md
│
├── projects/             # Projets à analyser
│   ├── git/
│   │   ├── README.md
│   │   └── src/
│   ├── redis/
│   │   ├── README.md
│   │   └── src/
│   └── nginx/
│       ├── README.md
│       └── src/
│
├── notes/                # Vos notes d'analyse
│   ├── git-analysis.md
│   ├── redis-analysis.md
│   └── nginx-analysis.md
│
└── patterns/             # Patterns identifiés
    ├── memory-pools.md
    ├── event-loops.md
    └── object-pooling.md
```

---

## 🧭 Comment utiliser ce module

### Approches d'apprentissage

#### **Approche 1 : Séquentielle (recommandée pour débutants)**

```
Semaine 1-2 : Section 33.1 (Méthodologie)
  ├─ Lire la méthodologie complète
  ├─ Pratiquer sur un petit projet
  └─ Créer vos propres notes

Semaine 3-4 : Section 33.2 (Git)
  ├─ Cloner Git
  ├─ Suivre l'analyse guidée
  ├─ Explorer par vous-même
  └─ Identifier les patterns

Semaine 5-6 : Section 33.3 (Redis)
  ├─ Cloner Redis
  ├─ Suivre l'analyse guidée
  ├─ Comparer avec Git
  └─ Noter les différences

Semaine 7-8 : Section 33.4 (Nginx)
  ├─ Cloner Nginx
  ├─ Suivre l'analyse guidée
  ├─ Comparer avec Git et Redis
  └─ Synthétiser les patterns

Semaine 9-12 : Section 33.5 (Contribution)
  ├─ Trouver un issue
  ├─ Comprendre le style
  ├─ Faire votre PR
  └─ Devenir contributeur
```

**Temps total estimé** : 3 mois à temps partiel (5-10h/semaine)

---

#### **Approche 2 : Par projet (si vous avez un intérêt spécifique)**

Si vous voulez approfondir un projet particulier :

```
Option A : Focus Git
  └─ 33.1 (Méthodologie) → 33.2 (Git) → Contribution à Git (33.5)

Option B : Focus Redis
  └─ 33.1 (Méthodologie) → 33.3 (Redis) → Contribution à Redis (33.5)

Option C : Focus Nginx
  └─ 33.1 (Méthodologie) → 33.4 (Nginx) → Contribution à Nginx (33.5)
```

**Avantage** : Vous devenez expert d'un projet  
**Inconvénient** : Moins de diversité de patterns  

---

#### **Approche 3 : Par pattern (pour développeurs expérimentés)**

Si vous cherchez des patterns spécifiques :

```
Pattern : Event Loops
  └─ Redis (section 33.3) + Nginx (section 33.4)

Pattern : Memory Management
  └─ Redis (SDS, allocations) + Nginx (pools)

Pattern : Modularité
  └─ Git (architecture modulaire) + Nginx (modules)

Pattern : Performance
  └─ Redis (optimisations in-memory) + Nginx (zero-copy)
```

**Avantage** : Approche thématique  
**Inconvénient** : Saute entre les projets  

---

### Méthodologie de travail recommandée

#### **Pour chaque projet étudié** :

**1. Première lecture** (1-2 heures)
- [ ] Lire l'introduction du projet
- [ ] Comprendre le but et l'architecture globale
- [ ] Identifier les fichiers clés

**2. Setup** (30 min - 1 heure)
- [ ] Cloner le projet
- [ ] Compiler
- [ ] Exécuter
- [ ] Générer la documentation (Doxygen si disponible)

**3. Analyse guidée** (3-5 heures)
- [ ] Suivre l'analyse de la section correspondante
- [ ] Ouvrir les fichiers mentionnés
- [ ] Lire le code en parallèle
- [ ] Prendre des notes

**4. Exploration personnelle** (2-4 heures)
- [ ] Choisir un module qui vous intéresse
- [ ] Tracer le flux d'exécution avec GDB
- [ ] Expérimenter (modifier, recompiler, tester)
- [ ] Documenter vos découvertes

**5. Synthèse** (1-2 heures)
- [ ] Résumer les patterns identifiés
- [ ] Noter ce qui pourrait s'appliquer à vos projets
- [ ] Identifier des issues pour contribuer (section 33.5)

---

## 💡 Conseils avant de commencer

### État d'esprit

#### **Ce que vous devez savoir** :

✅ **Vous ne comprendrez pas tout du premier coup**
- C'est normal
- Même les experts prennent du temps
- La compréhension vient par itérations

✅ **Chaque projet a son style**
- Git : Modulaire et bien documenté
- Redis : Compact et optimisé
- Nginx : Structuré et technique

✅ **Il n'y a pas de "bon" ou "mauvais" code**
- Chaque choix a des trade-offs
- Le contexte est crucial
- Apprenez le "pourquoi" pas juste le "comment"

✅ **L'échec fait partie du processus**
- Vous allez vous perdre dans le code
- Vous allez être confus
- C'est en se perdant qu'on apprend

---

### Erreurs à éviter

❌ **Vouloir tout comprendre à 100%**
- Impossible et inutile
- Concentrez-vous sur les parties importantes

❌ **Lire passivement**
- Lire du code comme un roman ne fonctionne pas
- Annotez, expérimentez, déboguez

❌ **Sauter la méthodologie (33.1)**
- Elle pose les bases essentielles
- Sans elle, vous serez perdu

❌ **Ne pas pratiquer**
- Lire l'analyse n'est pas suffisant
- Vous devez ouvrir le code et explorer

❌ **Se comparer aux experts**
- Linus Torvalds a mis des années à maîtriser Git
- Salvatore Sanfilippo a construit Redis progressivement
- Vous êtes en train d'apprendre, soyez patient

---

## 📊 Objectifs d'apprentissage

À la fin de ce module, vous serez capable de :

### Compétences techniques

- ✅ **Analyser** une codebase inconnue rapidement
- ✅ **Identifier** les patterns et architectures utilisés
- ✅ **Comprendre** les choix de design et leurs trade-offs
- ✅ **Naviguer** efficacement dans un grand projet
- ✅ **Utiliser** des outils d'analyse (ctags, cscope, GDB)
- ✅ **Extraire** des leçons applicables à vos projets

---

### Compétences de contribution

- ✅ **Lire** le code d'un projet open-source
- ✅ **Comprendre** son style et ses conventions
- ✅ **Identifier** des opportunités de contribution
- ✅ **Soumettre** une Pull Request de qualité
- ✅ **Communiquer** avec les mainteneurs

---

### Compétences professionnelles

- ✅ **Code review** : Critiquer du code constructivement
- ✅ **Onboarding** : Intégrer rapidement une nouvelle codebase
- ✅ **Debugging** : Déboguer du code inconnu
- ✅ **Architecture** : Concevoir des systèmes scalables
- ✅ **Best practices** : Appliquer les patterns professionnels

---

## 🎯 Mesurer votre progression

### Checklist par section

**Après la section 33.1 (Méthodologie)** :
- [ ] Je comprends comment approcher un projet inconnu
- [ ] Je connais les outils de navigation de code
- [ ] Je peux créer un plan d'analyse structuré

**Après la section 33.2 (Git)** :
- [ ] Je comprends l'architecture de Git
- [ ] J'ai identifié au moins 3 patterns réutilisables
- [ ] Je peux naviguer dans le code de Git

**Après la section 33.3 (Redis)** :
- [ ] Je comprends l'architecture événementielle
- [ ] Je connais les structures de données de Redis
- [ ] Je peux comparer Redis et Git

**Après la section 33.4 (Nginx)** :
- [ ] Je comprends l'architecture multi-process
- [ ] Je connais les memory pools et le zero-copy
- [ ] Je peux comparer les 3 architectures

**Après la section 33.5 (Contribution)** :
- [ ] J'ai soumis au moins une Pull Request
- [ ] Je comprends le processus de contribution
- [ ] Je peux aider d'autres débutants

---

## 🚀 Êtes-vous prêt ?

### Test de préparation

Répondez honnêtement à ces questions :

**Technique** :
- [ ] Je peux compiler un programme C de plusieurs fichiers
- [ ] Je comprends les pointeurs et l'allocation dynamique
- [ ] Je sais utiliser Git pour cloner et naviguer l'historique
- [ ] Je peux utiliser un débogueur (GDB) basiquement

**Mental** :
- [ ] Je suis prêt à passer du temps sur du code complexe
- [ ] Je ne serai pas découragé si je ne comprends pas tout
- [ ] Je suis curieux et motivé pour apprendre
- [ ] Je prendrai des notes et documenterai mes découvertes

**Logistique** :
- [ ] J'ai 5-10h par semaine à consacrer
- [ ] J'ai configuré mon environnement de développement
- [ ] J'ai accès à Internet pour cloner les projets
- [ ] J'ai de l'espace disque (quelques GB)

**Si vous avez coché au moins 75% des cases, vous êtes prêt !**

---

## 📚 Ressources préliminaires

### Lectures recommandées

**Avant de commencer** :

- **"Code Reading" de Diomidis Spinellis** : LA référence sur le sujet
- **"The Architecture of Open Source Applications"** : Analyses de projets célèbres
- **"Working Effectively with Legacy Code"** : Techniques pour code existant

**Articles** :
- "How to Read Code" (freecodecamp.org)
- "The Art of Reading Code" (blog.codinghorror.com)

---

### Projets pour s'échauffer

Avant d'attaquer Git/Redis/Nginx, pratiquez sur des projets plus petits :

**Niveau 1** : Petits projets (< 5000 lignes)
- `cJSON` : Parser JSON (~1500 lignes)
- `tree` : Visualisateur de répertoires (~3000 lignes)
- `sds` : Simple Dynamic Strings de Redis (~2000 lignes)

**Niveau 2** : Projets moyens (5000-50000 lignes)
- `zlib` : Compression (~15000 lignes)
- `jq` : Processeur JSON (~25000 lignes)
- `libuv` : Event loop (~30000 lignes)

**Niveau 3** : Prêt pour Git/Redis/Nginx !

---

## 💪 Message de motivation

**Vous êtes sur le point d'entrer dans le monde des "vrais" développeurs.**

L'analyse de code professionnel est ce qui sépare :
- Les développeurs juniors des développeurs seniors
- Ceux qui copient-collent de Stack Overflow de ceux qui comprennent
- Les programmeurs des ingénieurs logiciels

**C'est une compétence qui prend du temps à développer, mais qui paye pour toute votre carrière.**

**Les mainteneurs de Git, Redis, et Nginx sont parmi les meilleurs développeurs C au monde.** En étudiant leur code, vous avez accès à leur expertise gratuitement. C'est comme avoir un mentor de classe mondiale qui vous montre son code et explique ses choix.

**Profitez de cette opportunité.**

**Les prochaines sections vont changer votre façon de voir le code. Vous n'écrirez plus jamais de la même manière après avoir vu comment les experts le font.**

**Commençons ce voyage passionnant par la section 33.1 : Méthodologie de lecture de code.** 🚀

---

## 🗺️ Navigation

**Vous êtes ici** : 📍 Introduction du Module 33

**Prochaine étape** : Section 33.1 - Méthodologie de lecture de code

**Plan complet** :
```
📚 Module 33 : Analyse de code open-source
├── 📖 Introduction (vous êtes ici)
├── 🔍 33.1 : Méthodologie de lecture de code
├── 🔧 33.2 : Étude de cas - Git
├── ⚡ 33.3 : Étude de cas - Redis
├── 🌐 33.4 : Étude de cas - Nginx
└── 🤝 33.5 : Contribution à des projets open-source
    ├── 33.5.1 : Trouver un bon premier issue
    ├── 33.5.2 : Comprendre le coding style
    └── 33.5.3 : Soumettre une Pull Request
```

**Prenez votre temps, prenez des notes, et surtout : pratiquez !** 📝

**Prêt ? Allons-y !** ➡️

⏭️ [Méthodologie de lecture de code](/33-analyse-code-opensource/01-methodologie-lecture.md)
