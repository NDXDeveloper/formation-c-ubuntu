🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 18. Threads et Concurrence (POSIX)

## Introduction générale

Bienvenue dans le chapitre sur la **programmation multi-threadée** avec POSIX Threads (pthreads). Ce chapitre vous accompagnera dans l'apprentissage d'un des aspects les plus puissants et les plus complexes de la programmation système sous Linux : la **concurrence**.

### Qu'est-ce que la programmation concurrente ?

Jusqu'à présent, tous les programmes que vous avez écrits s'exécutaient de manière **séquentielle** : une instruction après l'autre, du début à la fin. C'est comme suivre une recette de cuisine étape par étape, en attendant que chaque étape soit terminée avant de passer à la suivante.

La **programmation concurrente** permet à plusieurs tâches de s'exécuter **en parallèle** ou de manière **entrelacée**. C'est comme si plusieurs cuisiniers travaillaient ensemble dans la même cuisine, chacun préparant une partie du repas simultanément.

**Analogie simple** :

```
Programmation séquentielle :
===========================
Vous (seul) :
  1. Éplucher les légumes  ████████░░░░░░░░░░░░
  2. Faire bouillir l'eau  ░░░░░░░░████████░░░░
  3. Cuire les pâtes       ░░░░░░░░░░░░░░██████

Temps total : ~30 minutes


Programmation concurrente :
===========================
Vous et un ami :
  Vous    : Éplucher légumes ████████░░░░░░░░
  Ami     : Faire bouillir   ████████░░░░░░░░
  Vous    : Cuire les pâtes  ░░░░░░░░██████░░

Temps total : ~15 minutes (2x plus rapide !)
```

### Pourquoi apprendre la programmation concurrente ?

#### 1. Performance sur processeurs multi-cœurs

Les ordinateurs modernes possèdent **plusieurs cœurs** (2, 4, 8, 16, ou plus). Sans programmation concurrente, votre programme n'utilise qu'**un seul cœur**, laissant les autres inactifs.

```
Processeur 4 cœurs :

Sans threads (séquentiel) :
┌────────┬────────┬────────┬────────┐
│ ████   │        │        │        │  25% utilisation
└────────┴────────┴────────┴────────┘

Avec threads (parallèle) :
┌────────┬────────┬────────┬────────┐
│ ████   │ ████   │ ████   │ ████   │  100% utilisation
└────────┴────────┴────────┴────────┘
  4x plus rapide potentiellement !
```

**Exemple concret** : Encoder une vidéo
- 1 thread : 40 minutes
- 4 threads : 10-12 minutes
- 8 threads : 5-7 minutes

#### 2. Réactivité des applications

Les threads permettent de garder une application **réactive** pendant des opérations longues.

**Exemple** : Interface graphique
```c
// ❌ Sans threads : L'interface gèle
bouton_clicked() {
    telecharger_fichier();  // Prend 30 secondes
    // Pendant ce temps, l'interface ne répond plus
}

// ✅ Avec threads : L'interface reste fluide
bouton_clicked() {
    creer_thread(telecharger_fichier);
    // L'interface continue de répondre
}
```

#### 3. Modélisation naturelle de certains problèmes

Certaines applications sont **naturellement concurrentes** :

- **Serveur web** : Un thread par client
- **Traitement d'images** : Un thread par région
- **Jeux vidéo** : Threads séparés pour physique, rendu, IA, son
- **Bases de données** : Traiter plusieurs requêtes simultanément

### Les défis de la concurrence

La programmation concurrente est **puissante mais dangereuse**. Elle introduit de nouveaux types de bugs difficiles à détecter :

#### 1. Race Conditions (Conditions de course)

Quand deux threads accèdent à la même donnée simultanément :

```c
// Variable partagée
int compteur = 0;

// Thread 1
compteur++;  // Lit 0, écrit 1

// Thread 2 (en même temps)
compteur++;  // Lit 0, écrit 1

// Résultat : compteur = 1 (au lieu de 2) !
```

#### 2. Deadlocks (Interblocages)

Quand les threads attendent mutuellement et sont bloqués indéfiniment :

```
Thread 1 : Possède A, attend B
           ↓              ↑
Thread 2 : Possède B, attend A

→ Blocage permanent !
```

#### 3. Comportements non déterministes

Le même programme peut produire des **résultats différents** à chaque exécution :

```bash
$ ./programme
Résultat : 1847392

$ ./programme
Résultat : 1923847

$ ./programme
Résultat : 1956234

# Quel est le bon résultat ? 2000000 !
```

Ces bugs sont **insidieux** car :
- Ils n'apparaissent pas toujours
- Ils dépendent du timing (peuvent disparaître en mode debug)
- Ils sont difficiles à reproduire

**Rassurez-vous !** Ce chapitre vous apprendra à les éviter et à les corriger.

---

## Ce que vous allez apprendre

Ce chapitre est structuré pour vous guider progressivement, des bases aux concepts avancés.

### Partie 1 : Les fondamentaux (Sections 18.1-18.5)

**18.1 Introduction à pthread**
- Qu'est-ce qu'un thread ?
- Différence entre threads et processus
- Premier programme multi-threadé

**18.2 Création et terminaison de threads**
- `pthread_create()` : Créer des threads
- `pthread_join()` : Attendre la fin d'un thread
- Passage de paramètres et récupération de résultats

**18.3 Passage de paramètres**
- Techniques pour passer des données aux threads
- Structures de paramètres
- Gestion de la mémoire

**18.4 Threads détachés vs joinable**
- Deux modes d'exécution des threads
- Gestion du cycle de vie
- Quand utiliser chaque mode

**18.5 Problèmes de concurrence : Race conditions**
- Identification des bugs de concurrence
- Pourquoi les race conditions se produisent
- Détection avec les outils (ThreadSanitizer, Helgrind)

### Partie 2 : Synchronisation (Sections 18.6-18.9)

**18.6 Mutex et sections critiques**
- **La solution principale** aux race conditions
- Protéger les données partagées
- Utilisation correcte des mutex

**18.7 Deadlocks**
- Comprendre les interblocages
- Les 4 conditions de Coffman
- Prévention et détection

**18.8 Variables de condition**
- Attente efficace d'une condition
- Pattern producteur-consommateur
- Signalisation entre threads

**18.9 Sémaphores POSIX**
- Comptage de ressources
- Limitation d'accès concurrent
- Différences avec les mutex

### Partie 3 : Optimisations et patterns avancés (Sections 18.10-18.13)

**18.10 Atomics C11**
- Opérations atomiques sans verrous (lock-free)
- Types atomiques (`atomic_int`, `atomic_bool`)
- Ordres mémoire et synchronisation

**18.11 Read-write locks**
- Optimisation pour lectures fréquentes
- Plusieurs lecteurs, un seul écrivain
- Quand les utiliser

**18.12 Thread-local storage**
- Variables uniques par thread
- Éviter la synchronisation
- Contexte de thread

**18.13 Barrières de threads**
- Points de rendez-vous
- Synchronisation par phases
- Calculs parallèles itératifs

---

## Pré-requis

Avant de commencer ce chapitre, assurez-vous de maîtriser :

✅ **Programmation C de base**
- Pointeurs et allocation dynamique (`malloc`, `free`)
- Structures et tableaux
- Fonctions et pointeurs de fonction

✅ **Compilation et linking**
- Utilisation de `gcc`
- Options de compilation

✅ **Concepts système**
- Processus (fork, exec)
- Mémoire virtuelle (pile, tas)

**Note** : Pas besoin d'être expert ! Si vous êtes arrivé jusqu'ici dans le tutoriel, vous avez les bases nécessaires.

---

## Outils nécessaires

### Compilateur avec support pthread

```bash
# Vérifier la version de GCC
gcc --version

# GCC 4.8+ recommandé (pour C11 et outils modernes)
```

### Option de compilation

**Toujours** compiler avec `-pthread` :

```bash
gcc -Wall -Wextra -pthread programme.c -o programme
```

**Important** : `-pthread` fait trois choses :
1. Définit des macros nécessaires
2. Link avec la bibliothèque pthread
3. Active des optimisations thread-safe

### Outils de détection de bugs

**ThreadSanitizer (TSan)** : Détecte les race conditions

```bash
gcc -fsanitize=thread -g -O1 -pthread programme.c -o programme
./programme
```

**Helgrind (Valgrind)** : Détecte problèmes de synchronisation

```bash
valgrind --tool=helgrind ./programme
```

**GDB** : Débugger avec support threads

```bash
gdb ./programme
(gdb) info threads
(gdb) thread 2
```

Nous apprendrons à utiliser ces outils au fur et à mesure.

---

## Approche pédagogique

Ce chapitre suit une approche **progressive et pratique** :

### 1. Apprentissage par l'exemple

Chaque concept est introduit avec des **exemples concrets** :
- Code complet et exécutable
- Explications ligne par ligne
- Sortie commentée

### 2. Erreurs courantes

Nous montrons les **erreurs typiques** et comment les corriger :
```c
// ❌ Code incorrect (avec explication du problème)
// ✅ Code correct (avec explication de la solution)
```

### 3. Visualisations

Des **diagrammes ASCII** pour comprendre visuellement :
```
Thread 1: ═══════╗
                 ║
Thread 2: ═════╗ ║
               ║ ║
               ▼ ▼
          [SYNC POINT]
```

### 4. Patterns réutilisables

Des **patterns éprouvés** que vous pourrez réutiliser :
- Producteur-consommateur
- Pool de workers
- Fork-Join
- Read-Write cache

### 5. Bonnes pratiques

À chaque section, les **règles d'or** à suivre pour écrire du code thread-safe.

---

## Philosophie de ce chapitre

### Commencer simple

Nous commençons par des exemples simples avec **2-3 threads** avant de passer à des cas complexes. Pas de panique si tout ne semble pas clair immédiatement - la programmation concurrente demande du temps et de la pratique.

### Comprendre avant d'optimiser

> *"Premature optimization is the root of all evil."* - Donald Knuth

Nous apprendrons d'abord à écrire du code **correct**, puis à l'optimiser. Un programme rapide mais bugué est inutile.

### Tester, tester, tester

La programmation concurrente nécessite des **tests rigoureux** :
- Tests avec différents nombres de threads
- Exécutions multiples (bugs non-déterministes)
- Utilisation des outils de détection (TSan, Helgrind)

### Documenter

Le code concurrent est **difficile à comprendre**. Nous insisterons sur :
- Commentaires clairs sur les invariants
- Documentation des stratégies de verrouillage
- Explication des choix de synchronisation

---

## Avertissements importants

### ⚠️ Complexité

La programmation multi-threadée est **intrinsèquement complexe**. Ne vous découragez pas si :
- Vous ne comprenez pas tout du premier coup
- Vos premiers programmes ont des bugs
- Certains concepts semblent contre-intuitifs

**C'est normal !** Même les programmeurs expérimentés font des erreurs en concurrence.

### ⚠️ Débogage difficile

Les bugs de concurrence sont **difficiles à déboguer** :
- Ils peuvent ne pas apparaître en mode debug
- Ils changent avec l'ordonnanceur
- Ils dépendent du timing

**Solution** : Suivre les bonnes pratiques dès le début.

### ⚠️ Pas toujours plus rapide

Créer des threads a un **coût** :
- Overhead de création
- Overhead de synchronisation
- Complexité accrue

**Règle** : N'utilisez les threads que quand le bénéfice justifie le coût.

---

## Conventions utilisées

Dans ce chapitre, nous utilisons les conventions suivantes :

### Code

```c
// ❌ Code incorrect (à éviter)
int mauvais_exemple;

// ✅ Code correct (à suivre)
int bon_exemple;

// ⚠️ Code dangereux (attention)
int code_attention;
```

### Symboles

- ✅ : Bonne pratique
- ❌ : Erreur / Mauvaise pratique
- ⚠️ : Attention / Piège
- 💡 : Astuce
- 🔒 : Verrouillage
- 🔓 : Déverrouillage

### Termes techniques

Certains termes seront utilisés en **anglais** car c'est l'usage dans la documentation :
- **Thread** (fil d'exécution)
- **Mutex** (mutual exclusion)
- **Deadlock** (interblocage)
- **Race condition** (condition de course)

Les termes français sont donnés entre parenthèses lors de la première utilisation.

---

## Ressources complémentaires

### Documentation officielle

```bash
# Pages man
man pthreads        # Vue d'ensemble
man pthread_create  # Création de threads
man pthread_mutex   # Mutex
```

### Livres recommandés

- **"Programming with POSIX Threads"** - David Butenhof (LA référence)
- **"The Art of Multiprocessor Programming"** - Herlihy & Shavit
- **"C++ Concurrency in Action"** - Anthony Williams (applicable au C)

### Ressources en ligne

- POSIX Thread Programming : https://computing.llnl.gov/tutorials/pthreads/
- ThreadSanitizer : https://github.com/google/sanitizers
- Linux man pages : https://man7.org/linux/man-pages/

---

## Plan d'apprentissage suggéré

### Pour les débutants

**Semaine 1-2** : Fondamentaux
- 18.1-18.2 : Création de threads
- 18.3 : Passage de paramètres
- Pratique : Programmes simples avec 2-3 threads

**Semaine 3-4** : Synchronisation de base
- 18.5 : Race conditions (comprendre le problème)
- 18.6 : Mutex (la solution principale)
- Pratique : Protéger des données partagées

**Semaine 5-6** : Concepts avancés
- 18.7 : Deadlocks
- 18.8 : Variables de condition
- Pratique : Producteur-consommateur

**Semaine 7-8** : Maîtrise
- 18.9 : Sémaphores
- 18.11-18.13 : Mécanismes avancés
- Projet : Application complète

### Pour les programmeurs expérimentés

Si vous connaissez déjà les threads (Java, C++, Python), vous pouvez :
1. Parcourir rapidement 18.1-18.4 (syntaxe pthread)
2. Vous concentrer sur 18.5-18.9 (spécificités POSIX)
3. Explorer 18.11-18.13 (optimisations)

---

## Premiers pas

Une fois cette introduction terminée, vous êtes prêt à commencer !

**Prochaine étape** : [18.1 Introduction à pthread](18.1_introduction_pthread.md)

Dans la section suivante, vous allez :
- Créer votre premier thread
- Comprendre la différence threads/processus
- Voir un programme "Hello World" multi-threadé

**Conseil** : Ayez votre éditeur et terminal ouverts. La meilleure façon d'apprendre la programmation concurrente est de **pratiquer** en parallèle de la lecture.

---

## Message de motivation

La programmation multi-threadée est un **super-pouvoir**. Une fois maîtrisée, elle vous permettra de :

✨ **Créer des applications plus rapides** - Exploiter pleinement les processeurs modernes

✨ **Résoudre des problèmes complexes** - Modéliser des systèmes naturellement concurrents

✨ **Devenir un meilleur programmeur** - Comprendre profondément les systèmes

✨ **Augmenter votre valeur** - Compétence recherchée dans l'industrie

**Oui, c'est difficile.** Mais c'est aussi **fascinant** et **gratifiant**.

Prenez votre temps, pratiquez régulièrement, et n'hésitez pas à revenir sur les sections précédentes si nécessaire.

**Vous êtes prêt. Commençons ! 🚀**

---

## Vue d'ensemble des sections

```
18. Threads et Concurrence (POSIX)
│
├── 18.1  Introduction à pthread
├── 18.2  Création et terminaison de threads
├── 18.3  Passage de paramètres
├── 18.4  Threads détachés vs joinable
├── 18.5  Problèmes de concurrence : Race conditions
│
├── 18.6  Mutex et sections critiques          ← Solution principale
├── 18.7  Deadlocks
├── 18.8  Variables de condition
├── 18.9  Sémaphores POSIX
│
├── 18.10 Atomics C11                          ← Lock-free
├── 18.11 Read-write locks                     ← Optimisations
├── 18.12 Thread-local storage
└── 18.13 Barrières de threads
```

**Temps estimé** : 20-40 heures pour maîtriser (selon expérience)

**Niveau** : Intermédiaire à avancé

**Prérequis** : C de base, compilation, concepts système

---

💡 **Prêt à plonger dans le monde fascinant de la concurrence ?**

👉 **[Commencer avec la section 18.1 - Introduction à pthread](18.1_introduction_pthread.md)**

Bonne chance et bon apprentissage ! 🎓

⏭️ [Introduction à pthread](/18-threads-et-concurrence/01-introduction-pthread.md)
