🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 16. Gestion des Fichiers et Entrées/Sorties

## Introduction au chapitre

La **gestion des fichiers et des entrées/sorties** (I/O - Input/Output) est l'un des aspects les plus fondamentaux de la programmation système sous Linux. Que vous développiez un simple script de traitement de texte ou un serveur web haute performance gérant des milliers de connexions, vous devrez maîtriser ces concepts.

> **💡 Citation :** "Sous Unix, tout est fichier." — Philosophie Unix (Ken Thompson & Dennis Ritchie)
>
> Cette philosophie signifie que Linux traite de manière uniforme les fichiers réguliers, les périphériques, les sockets réseau, les pipes, et bien d'autres ressources. Comprendre les I/O, c'est comprendre le cœur de Linux.

## Pourquoi ce chapitre est-il crucial ?

### 1. Fondement de toute interaction système

**Chaque programme** interagit avec le système via les I/O :
- 📝 **Lire et écrire des fichiers** : configuration, données, logs
- 🖥️ **Communiquer avec l'utilisateur** : stdin, stdout, stderr
- 🌐 **Échanger sur le réseau** : sockets (qui sont des fichiers spéciaux)
- 🔄 **Coordonner des processus** : pipes et FIFO
- 📊 **Accéder aux périphériques** : disques, terminaux, capteurs

**Sans I/O, votre programme ne peut rien faire d'utile !**

### 2. Impact direct sur les performances

Les opérations d'I/O sont **parmi les plus lentes** dans un ordinateur :

```
Latences approximatives :
┌────────────────────────────────────┐
│ Accès CPU cache       : 1 ns       │
│ Accès RAM             : 100 ns     │
│ Lecture SSD           : 50 µs      │ ← 500x plus lent
│ Lecture HDD           : 5 ms       │ ← 50 000x plus lent
│ Requête réseau (LAN)  : 0.5 ms     │
│ Requête réseau (WAN)  : 100 ms     │
└────────────────────────────────────┘
```

**Un programme qui gère mal ses I/O** peut être 10x, 100x, voire 1000x plus lent qu'un programme optimisé ! Ce chapitre vous apprendra à :
- Minimiser les appels système coûteux
- Utiliser le buffering intelligemment
- Gérer plusieurs sources d'I/O simultanément
- Exploiter l'asynchronisme pour la performance maximale

### 3. Sécurité et robustesse

Les erreurs d'I/O mal gérées sont une source majeure de :
- 🔒 **Failles de sécurité** : buffer overflows, path traversal, race conditions
- 💥 **Bugs et crashs** : descripteurs non fermés, fuites de ressources
- 📉 **Indisponibilité** : blocages, deadlocks, épuisement de ressources

**Ce chapitre vous enseignera les bonnes pratiques** pour écrire du code I/O sécurisé et robuste.

## La philosophie "Tout est fichier"

### Concept fondamental de Unix/Linux

Sous Linux, presque toutes les ressources du système sont accessibles via une **interface de fichier uniforme** :

```
┌─────────────────────────────────────────────────┐
│           INTERFACE UNIFIÉE : FICHIERS          │
├─────────────────────────────────────────────────┤
│                                                 │
│  Fonctions communes : open(), read(), write(),  │
│                      close(), lseek()...        │
│                                                 │
└──────────────┬──────────────────────────────────┘
               │
    ┌──────────┼────────┐
    │          │        │
    ▼          ▼        ▼
┌─────────┐ ┌────────┐ ┌────────┐
│Fichiers │ │Périph. │ │Sockets │
│réguliers│ │/dev/   │ │réseau  │
└─────────┘ └────────┘ └────────┘
    ▼        ▼        ▼
┌────────┐ ┌────────┐ ┌────────┐
│ Pipes  │ │ /proc  │ │ /sys   │
│ FIFO   │ │système │ │kernel  │
└────────┘ └────────┘ └────────┘
```

**Avantage :** Vous apprenez **une seule API** qui fonctionne pour presque tout !

### Exemples concrets

**Lire un fichier normal :**
```c
int fd = open("data.txt", O_RDONLY);  
read(fd, buffer, size);  
close(fd);  
```

**Lire depuis le clavier (stdin) :**
```c
read(STDIN_FILENO, buffer, size);  // Même fonction !
```

**Lire depuis un socket réseau :**
```c
int sock = socket(AF_INET, SOCK_STREAM, 0);
// ... connexion ...
read(sock, buffer, size);  // Toujours la même fonction !
```

**Lire des informations CPU :**
```c
int fd = open("/proc/cpuinfo", O_RDONLY);  
read(fd, buffer, size);  // Le système se présente comme un fichier  
```

**Cette uniformité est puissante** : les outils et techniques que vous apprenez s'appliquent partout.

## Vue d'ensemble du chapitre

Ce chapitre vous guidera à travers **tous les aspects** de la gestion des I/O sous Linux, du plus basique au plus avancé.

### Structure du chapitre

```
16. Gestion des Fichiers et Entrées/Sorties
│
├─ FONDATIONS (16.1 - 16.3)
│  ├─ 16.1 Descripteurs de fichiers
│  ├─ 16.2 Appels système
│  └─ 16.3 Différence appels système vs bibliothèque
│
├─ CONCEPTS ESSENTIELS (16.4 - 16.5)
│  ├─ 16.4 Permissions et modes
│  └─ 16.5 I/O bufferisé vs non bufferisé
│
└─ TECHNIQUES AVANCÉES (16.6 - 16.8)
   ├─ 16.6 dup, dup2 et redirection
   ├─ 16.7 I/O multiplexing
   └─ 16.8 I/O asynchrone (AIO)
```

### Les fondations (Sections 16.1-16.3)

**Ces sections couvrent les bases essentielles que vous devez absolument maîtriser.**

#### 16.1 Descripteurs de fichiers
Vous découvrirez :
- Ce qu'est un descripteur de fichier (le "ticket" pour accéder aux ressources)
- Les trois descripteurs standard : stdin (0), stdout (1), stderr (2)
- Comment ouvrir, lire, écrire et fermer des fichiers
- La table des descripteurs du processus

**Pourquoi c'est important :** Les descripteurs sont le fondement de toute I/O sous Linux.

#### 16.2 Appels système
Vous comprendrez :
- Ce qu'est un appel système (syscall) et pourquoi il existe
- Le coût des appels système (changement de contexte)
- Les principaux syscalls d'I/O : `open()`, `read()`, `write()`, `close()`
- Comment utiliser `strace` pour observer les syscalls

**Pourquoi c'est important :** Comprendre les syscalls est essentiel pour optimiser les performances.

#### 16.3 Différence appels système vs bibliothèque
Vous apprendrez :
- La différence entre `open()/read()` et `fopen()/fread()`
- Le rôle du buffering dans la bibliothèque C
- Quand utiliser l'un ou l'autre
- Impact sur les performances (~45x de différence !)

**Pourquoi c'est important :** Choisir le bon niveau d'abstraction fait une énorme différence.

### Les concepts essentiels (Sections 16.4-16.5)

**Ces sections approfondissent des aspects critiques de la gestion des I/O.**

#### 16.4 Permissions et modes
Vous maîtriserez :
- Le système de permissions Unix (lecture, écriture, exécution)
- La notation octale (0644, 0755, etc.)
- Les bits spéciaux : setuid, setgid, sticky bit
- Le umask et son influence
- Sécurité et bonnes pratiques

**Pourquoi c'est important :** Les permissions sont au cœur de la sécurité Linux.

#### 16.5 I/O bufferisé vs non bufferisé
Vous découvrirez :
- Comment fonctionne le buffering
- Les trois types : fully buffered, line buffered, unbuffered
- Impact massif sur les performances (benchmark)
- Quand flusher les buffers avec `fflush()`
- Risques de perte de données

**Pourquoi c'est important :** Le buffering peut rendre votre code 45x plus rapide ou causer des bugs subtils.

### Les techniques avancées (Sections 16.6-16.8)

**Ces sections vous transformeront en expert des I/O Linux.**

#### 16.6 dup, dup2 et redirection
Vous apprendrez :
- Comment dupliquer des descripteurs de fichiers
- La redirection des entrées/sorties (stdin, stdout, stderr)
- Comment implémenter `commande > fichier` du shell
- Les pipes et la communication inter-processus
- Sauvegarder et restaurer les descripteurs

**Pourquoi c'est important :** C'est le mécanisme derrière la puissance du shell Unix.

#### 16.7 I/O multiplexing
Vous maîtriserez :
- Comment surveiller plusieurs descripteurs simultanément
- `select()` : le mécanisme historique
- `poll()` : l'amélioration portable
- `epoll()` : la solution haute performance pour Linux
- Créer un serveur réseau gérant des milliers de connexions

**Pourquoi c'est important :** C'est la base de tous les serveurs modernes (Nginx, Redis, etc.).

#### 16.8 I/O asynchrone (AIO)
Vous découvrirez :
- La différence entre I/O bloquant, multiplexing et asynchrone
- POSIX AIO : l'API standard (mais limitée)
- `io_uring` : la révolution des I/O sous Linux
- Performance maximale avec zéro copie
- Cas d'usage : bases de données, serveurs haute performance

**Pourquoi c'est important :** Le summum de la performance I/O pour les applications critiques.

## Prérequis pour ce chapitre

### Connaissances requises

Avant d'aborder ce chapitre, vous devriez être à l'aise avec :

✅ **Programmation C de base**
- Variables, pointeurs, structures
- Allocation mémoire (`malloc`, `free`)
- Tableaux et chaînes de caractères

✅ **Concepts système de base**
- Processus et fichiers
- Ligne de commande Linux
- Compilation avec `gcc`

✅ **Modules précédents**
- Module 1 : Écosystème C sous Linux
- Module 2 : Fondamentaux du langage
- Module 3 : Gestion de la mémoire

### Connaissances recommandées (mais pas obligatoires)

⚠️ **Utiles mais pas bloquantes :**
- Notions de systèmes d'exploitation
- Utilisation basique du terminal Linux
- Expérience avec d'autres langages de programmation

**Si vous avez des lacunes :** Ne vous inquiétez pas ! Chaque concept est expliqué depuis le début avec des analogies et des exemples progressifs.

## Comment aborder ce chapitre ?

### Approche recommandée pour les débutants

**1. Suivre l'ordre des sections**
Les sections sont organisées de manière progressive. Chaque section s'appuie sur les précédentes.

**2. Pratiquer systématiquement**
Pour chaque section :
- ✅ Lisez attentivement les explications
- ✅ Tapez et exécutez les exemples de code
- ✅ Expérimentez en modifiant les exemples
- ✅ Utilisez `strace` pour observer ce qui se passe

**3. Ne pas tout retenir du premier coup**
C'est normal de :
- Relire certaines sections plusieurs fois
- Revenir sur les concepts de base
- Prendre des notes personnelles
- Faire des pauses entre les sections

**4. Construire votre compréhension progressivement**
```
Semaine 1 : Fondations (16.1-16.3)
    ↓
Semaine 2 : Concepts essentiels (16.4-16.5)
    ↓
Semaine 3 : Techniques avancées (16.6-16.8)
    ↓
Semaine 4 : Révision et projet pratique
```

### Approche pour les développeurs expérimentés

Si vous avez déjà de l'expérience :

**Parcours rapide :**
1. Survoler les sections 16.1-16.3 (révision)
2. Lire attentivement 16.5 (buffering - souvent mal compris)
3. Se concentrer sur 16.7-16.8 (multiplexing et AIO)

**Parcours ciblé selon vos besoins :**
- **Performance** → 16.5, 16.7, 16.8
- **Sécurité** → 16.4, 16.5 (flushing)
- **Réseau** → 16.7, 16.8
- **Système** → Tout le chapitre

## Outils indispensables

### Outils de développement

```bash
# Compilateur C
gcc --version

# Outils d'analyse
strace --version  # Tracer les appels système  
lsof              # Lister les fichiers ouverts  
man 2 open        # Manuel des appels système  
```

### Outils de debugging

```bash
# GDB pour déboguer
gdb ./programme

# Valgrind pour détecter les fuites
valgrind ./programme

# Vérifier les descripteurs ouverts
ls -l /proc/self/fd
```

### Environnement recommandé

**IDE/Éditeur :**
- VS Code avec extensions C/C++
- Vim/Neovim avec plugins
- CLion
- Tout éditeur avec complétion et debugger

**Système :**
- Linux (Ubuntu 20.04+, Fedora, Debian, etc.)
- Ou WSL2 sous Windows
- Ou VM Linux

## Conventions utilisées dans ce chapitre

### Notation du code

**Code complet exécutable :**
```c
#include <stdio.h>

int main(void) {
    printf("Programme complet\n");
    return 0;
}
```

**Extrait de code (contexte) :**
```c
// ... code précédent ...
int fd = open("file.txt", O_RDONLY);
// ... code suivant ...
```

**Pseudo-code conceptuel :**
```
while (données disponibles) {
    lire données
    traiter données
}
```

### Symboles utilisés

- ✅ **BON** : Bonne pratique à suivre
- ❌ **MAUVAIS** : Erreur à éviter
- ⚠️ **ATTENTION** : Point important à noter
- 💡 **ASTUCE** : Conseil pratique
- 🔥 **IMPORTANT** : Concept crucial
- 📝 **NOTE** : Information complémentaire

### Niveaux d'importance

**🔴 CRITIQUE :** Vous devez absolument maîtriser ce concept  
**🟡 IMPORTANT :** Concept essentiel pour la suite  
**🟢 UTILE :** Bon à savoir, mais pas bloquant  

## Objectifs d'apprentissage

À la fin de ce chapitre, vous serez capable de :

### Compétences fondamentales

- ✅ **Manipuler des fichiers** avec les appels système et la bibliothèque C
- ✅ **Comprendre les descripteurs** de fichiers et leur gestion
- ✅ **Gérer les permissions** et la sécurité des fichiers
- ✅ **Optimiser les performances** avec le buffering approprié
- ✅ **Déboguer les problèmes** d'I/O avec strace et autres outils

### Compétences avancées

- ✅ **Rediriger les entrées/sorties** avec dup et dup2
- ✅ **Créer des serveurs** gérant de multiples connexions avec I/O multiplexing
- ✅ **Implémenter des pipelines** de communication inter-processus
- ✅ **Utiliser l'I/O asynchrone** pour la performance maximale
- ✅ **Choisir la bonne technique** selon le contexte

### Compétences professionnelles

- ✅ **Lire et comprendre** du code système open-source
- ✅ **Écrire du code** robuste, performant et sécurisé
- ✅ **Diagnostiquer et résoudre** des problèmes de performance I/O
- ✅ **Contribuer** à des projets nécessitant de l'I/O avancé

## Ressources complémentaires

### Documentation essentielle

**Man pages Linux :**
```bash
man 2 open     # Appels système (section 2)  
man 3 fopen    # Bibliothèque C (section 3)  
man 7 aio      # Aperçu des APIs (section 7)  
```

**En ligne :**
- 📖 [Linux man pages online](https://man7.org/linux/man-pages/)
- 📖 [The Linux Programming Interface](http://man7.org/tlpi/) (livre de référence)
- 📖 [io_uring documentation](https://kernel.dk/io_uring.pdf)

### Communautés et forums

- 💬 [r/C_Programming](https://reddit.com/r/C_Programming)
- 💬 [Stack Overflow - tag [linux]](https://stackoverflow.com/questions/tagged/linux)
- 💬 [LinuxQuestions.org](https://www.linuxquestions.org)

### Projets pour pratiquer

**Après avoir terminé ce chapitre, essayez de créer :**

1. **Un cat amélioré** : Cloner la commande `cat` avec options
2. **Un serveur echo TCP** : Avec epoll ou io_uring
3. **Un système de logs** : Avec rotation et compression
4. **Un mini-shell** : Avec redirection et pipes
5. **Un moniteur système** : Lisant /proc et /sys

## Message final avant de commencer

Les entrées/sorties sont le **pont entre votre programme et le monde réel**. Que vous écriviez :
- Un outil CLI simple
- Un serveur web gérant des milliers de requêtes
- Une base de données haute performance
- Un système embarqué lisant des capteurs

**Vous aurez besoin de maîtriser les I/O.**

Ce chapitre est dense et technique, mais chaque concept est expliqué clairement avec des exemples concrets. Prenez votre temps, pratiquez régulièrement, et vous développerez une compréhension profonde qui vous servira tout au long de votre carrière.

> **💪 Conseil :** La meilleure façon d'apprendre les I/O, c'est de coder, d'expérimenter, de casser des choses, et de comprendre pourquoi elles se cassent. N'ayez pas peur de faire des erreurs - elles sont vos meilleures enseignantes !

---

**Prêt à commencer ?** Passons à la section 16.1 où vous découvrirez les **descripteurs de fichiers**, le concept fondamental de toutes les I/O sous Linux.

**Bonne formation ! 🚀**

⏭️ [Descripteurs de fichiers](/16-fichiers-et-io/01-descripteurs-fichiers.md)
