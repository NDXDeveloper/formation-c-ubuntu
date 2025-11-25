# Table des Matières - Maîtriser C sur Ubuntu : De la Syntaxe au Système
## Formation Complète pour Développeurs et DevOps (Édition 2025)

---

## **Module 1 : L'Écosystème C sous Linux** *(Niveau Débutant)*

### 1. [Introduction et Histoire](01-ecosysteme-c-linux/README.md)
- 1.1 [Pourquoi le C en 2025 ?](01-ecosysteme-c-linux/01-pourquoi-le-c-en-2025.md)
- 1.2 [La relation symbiotique entre C et UNIX/Linux](01-ecosysteme-c-linux/02-relation-c-unix-linux.md)
- 1.3 [Normes du langage : ANSI C, C99, C11, C17, C23](01-ecosysteme-c-linux/03-normes-du-langage.md)
    - 1.3.1 [Focus sur C23 : Nouveautés et état de l'adoption](01-ecosysteme-c-linux/03.1-focus-c23.md)
    - 1.3.2 [Vérification du support compilateur](01-ecosysteme-c-linux/03.2-verification-support-compilateur.md)
    - 1.3.3 [Pourquoi C11 reste la base recommandée en production](01-ecosysteme-c-linux/03.3-c11-base-production.md)
- 1.4 [C vs C++ vs Rust : Quand choisir C ?](01-ecosysteme-c-linux/04-c-vs-cpp-vs-rust.md)

### 2. [Configuration de l'environnement Ubuntu](02-configuration-environnement/README.md)
- 2.1 [Installation de la toolchain](02-configuration-environnement/01-installation-toolchain.md)
- 2.2 [Les versions de GCC et leur gestion](02-configuration-environnement/02-versions-gcc.md)
- 2.3 [Choix de l'IDE/Éditeur](02-configuration-environnement/03-choix-ide-editeur.md)
- 2.4 [Configuration des extensions et outils de productivité](02-configuration-environnement/04-extensions-et-outils.md)
- **2.5 [DevContainers : Environnement reproductible moderne](02-configuration-environnement/05-devcontainers.md)** ⭐
    - 2.5.1 [Qu'est-ce qu'un DevContainer ?](02-configuration-environnement/05.1-quest-ce-quun-devcontainer.md)
    - 2.5.2 [Structure du fichier .devcontainer.json](02-configuration-environnement/05.2-structure-devcontainer-json.md)
    - 2.5.3 [Configuration GCC/GDB/CMake/Valgrind](02-configuration-environnement/05.3-configuration-outils.md)
    - 2.5.4 [Intégration VS Code et onboarding](02-configuration-environnement/05.4-integration-vscode.md)
    - 2.5.5 [DevContainers comme source de vérité](02-configuration-environnement/05.5-devcontainers-source-verite.md)

### 3. [La Chaîne de Compilation](03-chaine-de-compilation/README.md)
- 3.1 [Les 4 étapes détaillées](03-chaine-de-compilation/01-quatre-etapes.md)
- 3.2 [Visualisation du pipeline](03-chaine-de-compilation/02-visualisation-pipeline.md)
- 3.3 [Options de compilation GCC essentielles](03-chaine-de-compilation/03-options-gcc.md)
- 3.4 [Démystification du "Hello World"](03-chaine-de-compilation/04-hello-world.md)
- 3.5 [Inspection des fichiers intermédiaires](03-chaine-de-compilation/05-fichiers-intermediaires.md)
- 3.6 [Comprendre les symboles et la table des symboles](03-chaine-de-compilation/06-symboles.md)

---

## **Module 2 : Fondamentaux du Langage** *(Niveau Débutant)*

### 4. [Types de données et Représentation mémoire](04-types-et-memoire/README.md)
- 4.1 [Types primitifs et leur taille](04-types-et-memoire/01-types-primitifs.md)
- 4.2 [Signed vs Unsigned](04-types-et-memoire/02-signed-vs-unsigned.md)
- 4.3 [Les constantes et les littéraux](04-types-et-memoire/03-constantes-litteraux.md)
- 4.4 [Conversion de types](04-types-et-memoire/04-conversion-types.md)
- 4.5 [Endianness et portabilité](04-types-et-memoire/05-endianness.md)
- 4.6 [Nouveautés C23](04-types-et-memoire/06-nouveautes-c23.md)

### 5. [Opérateurs et Structures de contrôle](05-operateurs-et-controle/README.md)
- 5.1 [Opérateurs arithmétiques](05-operateurs-et-controle/01-operateurs-arithmetiques.md)
- 5.2 [Opérateurs logiques](05-operateurs-et-controle/02-operateurs-logiques.md)
- 5.3 [Opérateurs bit-à-bit](05-operateurs-et-controle/03-operateurs-bitwise.md)
- 5.4 [Conditions et optimisation](05-operateurs-et-controle/04-conditions.md)
- 5.5 [Boucles et contrôle de flux](05-operateurs-et-controle/05-boucles.md)
- 5.6 [Bonnes pratiques de formatage](05-operateurs-et-controle/06-bonnes-pratiques.md)
- **5.7 [Formatage automatique avec clang-format](05-operateurs-et-controle/07-clang-format.md)** ⭐
    - 5.7.1 [Installation et configuration](05-operateurs-et-controle/07.1-installation-config.md)
    - 5.7.2 [Styles courants](05-operateurs-et-controle/07.2-styles-courants.md)
    - 5.7.3 [Intégration IDE](05-operateurs-et-controle/07.3-integration-ide.md)
    - 5.7.4 [Pre-commit hooks Git](05-operateurs-et-controle/07.4-pre-commit-hooks.md)
    - 5.7.5 [Formatage dans le CI/CD](05-operateurs-et-controle/07.5-formatage-cicd.md)

### 6. [Les Fonctions](06-fonctions/README.md)
- 6.1 [Déclaration, définition et prototypes](06-fonctions/01-declaration-definition.md)
- 6.2 [La Pile : Comprendre les Stack Frames](06-fonctions/02-stack-frames.md)
- 6.3 [Passage par valeur](06-fonctions/03-passage-par-valeur.md)
- 6.4 [Valeurs de retour et codes d'erreur](06-fonctions/04-valeurs-retour.md)
- 6.5 [La fonction main et ses paramètres](06-fonctions/05-fonction-main.md)

---

## **Module 3 : La Gestion de la Mémoire** *(Niveau Intermédiaire)*

### 7. [Les Pointeurs : Démystification](07-pointeurs/README.md)
- 7.1 [Concept d'adresse mémoire](07-pointeurs/01-adresse-memoire.md)
- 7.2 [Déclaration, déréférencement et adresse](07-pointeurs/02-declaration-dereferencement.md)
- 7.3 [L'arithmétique des pointeurs](07-pointeurs/03-arithmetique-pointeurs.md)
- 7.4 [Pointeurs void* et casting](07-pointeurs/04-pointeurs-void.md)
- 7.5 [Pointeurs NULL et dangling pointers](07-pointeurs/05-pointeurs-null.md)
- 7.6 [Pointeurs constants et const](07-pointeurs/06-pointeurs-const.md)
- 7.7 [Pointeurs restreints (restrict)](07-pointeurs/07-pointeurs-restrict.md)

### 8. [Tableaux et Chaînes de caractères](08-tableaux-et-chaines/README.md)
- 8.1 [Relation tableaux et pointeurs](08-tableaux-et-chaines/01-relation-tableaux-pointeurs.md)
- 8.2 [Tableaux multidimensionnels](08-tableaux-et-chaines/02-tableaux-multidimensionnels.md)
- 8.3 [Les chaînes en C](08-tableaux-et-chaines/03-chaines-en-c.md)
- 8.4 [Manipulation de chaînes](08-tableaux-et-chaines/04-manipulation-chaines.md)
- 8.5 [Dangers des chaînes : Buffer Overflows](08-tableaux-et-chaines/05-dangers-buffer-overflows.md)
- 8.6 [Fonctions sécurisées](08-tableaux-et-chaines/06-fonctions-securisees.md)
- 8.7 [Strings littérales et immutabilité](08-tableaux-et-chaines/07-strings-litterales.md)

### 9. [Allocation Dynamique](09-allocation-dynamique/README.md)
- 9.1 [Stack vs Heap : Diagramme et différences](09-allocation-dynamique/01-stack-vs-heap.md)
- 9.2 [Durée de vie et performance](09-allocation-dynamique/02-duree-vie-performance.md)
- 9.3 [Les fonctions malloc, calloc, realloc et free](09-allocation-dynamique/03-fonctions-allocation.md)
- 9.4 [Gestion des erreurs d'allocation](09-allocation-dynamique/04-gestion-erreurs.md)
- 9.5 [Fuites mémoire : causes et prévention](09-allocation-dynamique/05-fuites-memoire.md)
- 9.6 [Double free et corruption de tas](09-allocation-dynamique/06-double-free.md)
- 9.7 [Stratégies d'allocation personnalisées](09-allocation-dynamique/07-strategies-allocation.md)

---

## **Module 4 : Structures de Données et Modularité** *(Niveau Intermédiaire)*

### 10. [Types définis par l'utilisateur](10-types-utilisateur/README.md)
- 10.1 [struct : Création d'objets complexes](10-types-utilisateur/01-structures.md)
- 10.2 [Alignement mémoire et padding](10-types-utilisateur/02-alignement-padding.md)
- 10.3 [Packed structures](10-types-utilisateur/03-packed-structures.md)
- 10.4 [union : Optimisation mémoire](10-types-utilisateur/04-unions.md)
- 10.5 [enum : Code lisible et gestion d'états](10-types-utilisateur/05-enumerations.md)
- 10.6 [typedef : Simplification de la syntaxe](10-types-utilisateur/06-typedef.md)
- 10.7 [Structures bit-fields](10-types-utilisateur/07-bit-fields.md)

### 11. [Structures de données dynamiques](11-structures-dynamiques/README.md)
- 11.1 [Listes chaînées simples](11-structures-dynamiques/01-listes-chainees.md)
- 11.2 [Listes doublement chaînées](11-structures-dynamiques/02-listes-doublement-chainees.md)
- 11.3 [Piles et Files](11-structures-dynamiques/03-piles-et-files.md)
- 11.4 [Tables de hachage](11-structures-dynamiques/04-tables-hachage.md)
- 11.5 [Arbres binaires](11-structures-dynamiques/05-arbres-binaires.md)
- 11.6 [Choix de la structure de données](11-structures-dynamiques/06-choix-structure.md)
- 11.7 [Gestion mémoire des structures dynamiques](11-structures-dynamiques/07-gestion-memoire.md)

### 12. [Organisation du code et Compilation modulaire](12-organisation-code/README.md)
- 12.1 [Fichiers d'en-tête et gardes d'inclusion](12-organisation-code/01-fichiers-entete.md)
- 12.2 [La portée des variables](12-organisation-code/02-portee-variables.md)
- 12.3 [Compilation séparée](12-organisation-code/03-compilation-separee.md)
- 12.4 [Organisation d'un projet](12-organisation-code/04-organisation-projet.md)
- 12.5 [Conventions de nommage et standards](12-organisation-code/05-conventions-nommage.md)
- 12.6 [Forward declarations et dépendances circulaires](12-organisation-code/06-forward-declarations.md)

---

## **Module 5 : Outillage DevOps et Automatisation** *(Niveau Avancé)*

### 13. [Build Systems : De Make à CMake](13-build-systems/README.md) ⭐
- **13.1 [CMake : Le standard moderne](13-build-systems/01-cmake.md)** 🔥
    - 13.1.1 [Pourquoi CMake ?](13-build-systems/01.1-pourquoi-cmake.md)
    - 13.1.2 [Installation et premiers pas](13-build-systems/01.2-installation-premiers-pas.md)
    - 13.1.3 [Structure d'un CMakeLists.txt](13-build-systems/01.3-structure-cmakelists.md)
    - 13.1.4 [Targets, properties et generators](13-build-systems/01.4-targets-properties.md)
    - 13.1.5 [Gestion des dépendances](13-build-systems/01.5-gestion-dependances.md)
    - 13.1.6 [Build types](13-build-systems/01.6-build-types.md)
    - 13.1.7 [Out-of-source builds](13-build-systems/01.7-out-of-source-builds.md)
    - 13.1.8 [Intégration VS Code et CLion](13-build-systems/01.8-integration-ide.md)
- 13.2 [GNU Make : Fondamentaux](13-build-systems/02-gnu-make.md)
    - 13.2.1 [Syntaxe des Makefile](13-build-systems/02.1-syntaxe-makefile.md)
    - 13.2.2 [Règles, cibles et dépendances](13-build-systems/02.2-regles-cibles.md)
    - 13.2.3 [Variables et macros](13-build-systems/02.3-variables-macros.md)
    - 13.2.4 [Pattern rules et automatic variables](13-build-systems/02.4-pattern-rules.md)
- 13.3 [Comparaison Make vs CMake](13-build-systems/03-make-vs-cmake.md)
- 13.4 [Autres build systems](13-build-systems/04-autres-build-systems.md)

### 14. [Les Bibliothèques](14-bibliotheques/README.md)
- 14.1 [Bibliothèques statiques (.a)](14-bibliotheques/01-bibliotheques-statiques.md)
    - 14.1.1 [Compilation avec ar et ranlib](14-bibliotheques/01.1-compilation-ar-ranlib.md)
    - 14.1.2 [Linking statique](14-bibliotheques/01.2-linking-statique.md)
- 14.2 [Bibliothèques dynamiques (.so)](14-bibliotheques/02-bibliotheques-dynamiques.md)
    - 14.2.1 [Compilation avec -fPIC](14-bibliotheques/02.1-compilation-fpic.md)
    - 14.2.2 [Versioning et SONAME](14-bibliotheques/02.2-versioning-soname.md)
    - 14.2.3 [Résolution de symboles au runtime](14-bibliotheques/02.3-resolution-symboles.md)
- 14.3 [Différences statiques vs dynamiques](14-bibliotheques/03-differences-statiques-dynamiques.md)
- 14.4 [Gestion du LD_LIBRARY_PATH](14-bibliotheques/04-ld-library-path.md)
- 14.5 [ldd et ldconfig](14-bibliotheques/05-ldd-ldconfig.md)
- 14.6 [Création d'une API propre](14-bibliotheques/06-creation-api-propre.md)
- 14.7 [Symbol visibility](14-bibliotheques/07-symbol-visibility.md)

### 15. [Débogage et Analyse de Qualité](15-debogage-et-qualite/README.md) ⭐
- **15.1 [Sanitizers : Première ligne de défense](15-debogage-et-qualite/01-sanitizers.md)** 🔥
    - 15.1.1 [AddressSanitizer (ASan)](15-debogage-et-qualite/01.1-address-sanitizer.md)
    - 15.1.2 [UndefinedBehaviorSanitizer (UBSan)](15-debogage-et-qualite/01.2-undefined-behavior-sanitizer.md)
    - 15.1.3 [ThreadSanitizer (TSan)](15-debogage-et-qualite/01.3-thread-sanitizer.md)
    - 15.1.4 [LeakSanitizer (LSan)](15-debogage-et-qualite/01.4-leak-sanitizer.md)
    - 15.1.5 [Compilation avec sanitizers](15-debogage-et-qualite/01.5-compilation-sanitizers.md)
    - 15.1.6 [Performance et overhead](15-debogage-et-qualite/01.6-performance-overhead.md)
    - 15.1.7 [Intégration dans le workflow](15-debogage-et-qualite/01.7-integration-workflow.md)
- 15.2 [Utilisation de GDB](15-debogage-et-qualite/02-utilisation-gdb.md)
    - 15.2.1 [Breakpoints, watchpoints, catchpoints](15-debogage-et-qualite/02.1-breakpoints.md)
    - 15.2.2 [Stepping](15-debogage-et-qualite/02.2-stepping.md)
    - 15.2.3 [Backtrace et analyse de la pile](15-debogage-et-qualite/02.3-backtrace.md)
    - 15.2.4 [Inspection des variables](15-debogage-et-qualite/02.4-inspection-variables.md)
    - 15.2.5 [GDB TUI mode](15-debogage-et-qualite/02.5-gdb-tui.md)
- 15.3 [GDB avancé](15-debogage-et-qualite/03-gdb-avance.md)
    - 15.3.1 [Core dumps](15-debogage-et-qualite/03.1-core-dumps.md)
    - 15.3.2 [Debugging distant](15-debogage-et-qualite/03.2-debugging-distant.md)
    - 15.3.3 [Scripts GDB](15-debogage-et-qualite/03.3-scripts-gdb.md)
    - 15.3.4 [rr (Time Travel Debugging)](15-debogage-et-qualite/03.4-time-travel-debugging.md)
- 15.4 [Détection de fuites avec Valgrind](15-debogage-et-qualite/04-valgrind.md)
    - 15.4.1 [Memcheck](15-debogage-et-qualite/04.1-memcheck.md)
    - 15.4.2 [Interprétation des rapports](15-debogage-et-qualite/04.2-interpretation-rapports.md)
    - 15.4.3 [Suppression files](15-debogage-et-qualite/04.3-suppression-files.md)
- 15.5 [Valgrind avancé](15-debogage-et-qualite/05-valgrind-avance.md)
    - 15.5.1 [Helgrind](15-debogage-et-qualite/05.1-helgrind.md)
    - 15.5.2 [Cachegrind](15-debogage-et-qualite/05.2-cachegrind.md)
    - 15.5.3 [Callgrind](15-debogage-et-qualite/05.3-callgrind.md)
    - 15.5.4 [Massif](15-debogage-et-qualite/05.4-massif.md)
- 15.6 [Analyse statique de code](15-debogage-et-qualite/06-analyse-statique.md)
    - 15.6.1 [cppcheck](15-debogage-et-qualite/06.1-cppcheck.md)
    - 15.6.2 [clang-tidy](15-debogage-et-qualite/06.2-clang-tidy.md)
    - 15.6.3 [Configuration et intégration continue](15-debogage-et-qualite/06.3-configuration-ci.md)
- 15.7 [Code coverage](15-debogage-et-qualite/07-code-coverage.md)
    - 15.7.1 [gcov](15-debogage-et-qualite/07.1-gcov.md)
    - 15.7.2 [lcov](15-debogage-et-qualite/07.2-lcov.md)
    - 15.7.3 [Intégration dans le CI](15-debogage-et-qualite/07.3-integration-ci.md)

---

## **Module 6 : Programmation Système Linux** *(Niveau Expert)*

### 16. [Gestion des Fichiers et Entrées/Sorties](16-fichiers-et-io/README.md)
- 16.1 [Descripteurs de fichiers](16-fichiers-et-io/01-descripteurs-fichiers.md)
- 16.2 [Appels système](16-fichiers-et-io/02-appels-systeme.md)
- 16.3 [Différence appels système vs bibliothèque](16-fichiers-et-io/03-systeme-vs-bibliotheque.md)
- 16.4 [Permissions et modes](16-fichiers-et-io/04-permissions-modes.md)
- 16.5 [I/O bufferisé vs non bufferisé](16-fichiers-et-io/05-io-bufferise.md)
- 16.6 [dup, dup2 et redirection](16-fichiers-et-io/06-dup-redirection.md)
- 16.7 [I/O multiplexing](16-fichiers-et-io/07-io-multiplexing.md)
- 16.8 [I/O asynchrone (AIO)](16-fichiers-et-io/08-io-asynchrone.md)

### 17. [Processus et Signaux](17-processus-et-signaux/README.md)
- 17.1 [Création de processus](17-processus-et-signaux/01-creation-processus.md)
- 17.2 [Les PIDs et hiérarchie](17-processus-et-signaux/02-pids-hierarchie.md)
- 17.3 [Processus orphelins et zombies](17-processus-et-signaux/03-orphelins-zombies.md)
- 17.4 [Variables d'environnement](17-processus-et-signaux/04-variables-environnement.md)
- 17.5 [Gestion des signaux](17-processus-et-signaux/05-gestion-signaux.md)
- 17.6 [Signaux courants](17-processus-et-signaux/06-signaux-courants.md)
- 17.7 [Masquage de signaux](17-processus-et-signaux/07-masquage-signaux.md)
- 17.8 [Pipes anonymes](17-processus-et-signaux/08-pipes-anonymes.md)
- 17.9 [Named pipes (FIFO)](17-processus-et-signaux/09-named-pipes.md)

### 18. [Threads et Concurrence (POSIX)](18-threads-et-concurrence/README.md) ⭐
- 18.1 [Introduction à pthread](18-threads-et-concurrence/01-introduction-pthread.md)
- 18.2 [Création et terminaison de threads](18-threads-et-concurrence/02-creation-terminaison.md)
- 18.3 [Passage de paramètres](18-threads-et-concurrence/03-passage-parametres.md)
- 18.4 [Threads détachés vs joinable](18-threads-et-concurrence/04-detaches-vs-joinable.md)
- 18.5 [Problèmes de concurrence : Race conditions](18-threads-et-concurrence/05-race-conditions.md)
- 18.6 [Mutex et sections critiques](18-threads-et-concurrence/06-mutex.md)
- 18.7 [Deadlocks](18-threads-et-concurrence/07-deadlocks.md)
- 18.8 [Variables de condition](18-threads-et-concurrence/08-variables-condition.md)
- 18.9 [Sémaphores POSIX](18-threads-et-concurrence/09-semaphores.md)
- **18.10 [Concurrence lock-free avec les Atomics (C11)](18-threads-et-concurrence/10-atomics.md)** 🔥
    - 18.10.1 [Introduction à stdatomic.h](18-threads-et-concurrence/10.1-introduction-stdatomic.md)
    - 18.10.2 [Types atomiques](18-threads-et-concurrence/10.2-types-atomiques.md)
    - 18.10.3 [Memory ordering](18-threads-et-concurrence/10.3-memory-ordering.md)
    - 18.10.4 [Compare-and-swap (CAS)](18-threads-et-concurrence/10.4-compare-and-swap.md)
    - 18.10.5 [Atomics vs mutex](18-threads-et-concurrence/10.5-atomics-vs-mutex.md)
    - 18.10.6 [Performance et cas d'usage](18-threads-et-concurrence/10.6-performance-cas-usage.md)
- 18.11 [Read-write locks](18-threads-et-concurrence/11-read-write-locks.md)
- 18.12 [Thread-local storage](18-threads-et-concurrence/12-thread-local-storage.md)
- 18.13 [Barrières de threads](18-threads-et-concurrence/13-barrieres-threads.md)

### 19. [Mémoire partagée et IPC avancé](19-memoire-partagee-ipc/README.md)
- 19.1 [Shared memory](19-memoire-partagee-ipc/01-shared-memory.md)
- 19.2 [Sémaphores System V](19-memoire-partagee-ipc/02-semaphores-system-v.md)
- 19.3 [Message queues System V](19-memoire-partagee-ipc/03-message-queues.md)
- 19.4 [POSIX IPC vs System V IPC](19-memoire-partagee-ipc/04-posix-vs-system-v.md)
- 19.5 [Memory-mapped files (mmap)](19-memoire-partagee-ipc/05-mmap.md)

### 20. [Réseau (Sockets)](20-reseau-sockets/README.md)
- 20.1 [Modèle Client/Serveur](20-reseau-sockets/01-modele-client-serveur.md)
- 20.2 [Architecture TCP/IP](20-reseau-sockets/02-architecture-tcp-ip.md)
- 20.3 [Sockets TCP](20-reseau-sockets/03-sockets-tcp.md)
- 20.4 [Sockets UDP](20-reseau-sockets/04-sockets-udp.md)
- 20.5 [Gestion des erreurs réseau](20-reseau-sockets/05-gestion-erreurs-reseau.md)
- 20.6 [Options de sockets](20-reseau-sockets/06-options-sockets.md)
- 20.7 [Résolution de noms](20-reseau-sockets/07-resolution-noms.md)
- 20.8 [Serveur concurrent](20-reseau-sockets/08-serveur-concurrent.md)
- 20.9 [Non-blocking I/O et epoll](20-reseau-sockets/09-non-blocking-io-epoll.md)
- 20.10 [Création d'un mini-serveur HTTP](20-reseau-sockets/10-mini-serveur-http.md)

### **21. [Introduction à eBPF](21-introduction-ebpf/README.md)** ⭐🔥
- **21.1 [Qu'est-ce qu'eBPF ?](21-introduction-ebpf/01-quest-ce-que-ebpf.md)**
    - 21.1.1 [Histoire : de BPF classique à eBPF](21-introduction-ebpf/01.1-histoire-bpf.md)
    - 21.1.2 [Pourquoi eBPF révolutionne Linux](21-introduction-ebpf/01.2-pourquoi-ebpf-revolutionne.md)
- **21.2 [Architecture eBPF](21-introduction-ebpf/02-architecture-ebpf.md)**
    - 21.2.1 [BPF programs](21-introduction-ebpf/02.1-bpf-programs.md)
    - 21.2.2 [BPF maps](21-introduction-ebpf/02.2-bpf-maps.md)
    - 21.2.3 [Le verifier](21-introduction-ebpf/02.3-verifier.md)
    - 21.2.4 [JIT compilation](21-introduction-ebpf/02.4-jit-compilation.md)
- **21.3 [Use cases DevOps et observabilité](21-introduction-ebpf/03-use-cases-devops.md)**
    - 21.3.1 [Tracing et monitoring](21-introduction-ebpf/03.1-tracing-monitoring.md)
    - 21.3.2 [Networking](21-introduction-ebpf/03.2-networking.md)
    - 21.3.3 [Sécurité](21-introduction-ebpf/03.3-securite.md)
    - 21.3.4 [Performance analysis](21-introduction-ebpf/03.4-performance-analysis.md)
- **21.4 [Toolchain eBPF](21-introduction-ebpf/04-toolchain-ebpf.md)**
    - 21.4.1 [libbpf](21-introduction-ebpf/04.1-libbpf.md)
    - 21.4.2 [bpftool](21-introduction-ebpf/04.2-bpftool.md)
    - 21.4.3 [Compilation et chargement](21-introduction-ebpf/04.3-compilation-chargement.md)
- **21.5 [Premier programme eBPF](21-introduction-ebpf/05-premier-programme.md)**
    - 21.5.1 [Hello World : tracer les appels système](21-introduction-ebpf/05.1-hello-world-tracing.md)
    - 21.5.2 [Utilisation de maps](21-introduction-ebpf/05.2-utilisation-maps.md)
    - 21.5.3 [Attacher à des hooks kernel](21-introduction-ebpf/05.3-hooks-kernel.md)
- **21.6 [eBPF dans l'écosystème moderne](21-introduction-ebpf/06-ecosysteme-moderne.md)**
    - 21.6.1 [Cilium](21-introduction-ebpf/06.1-cilium.md)
    - 21.6.2 [Pixie](21-introduction-ebpf/06.2-pixie.md)
    - 21.6.3 [Ressources pour aller plus loin](21-introduction-ebpf/06.3-ressources.md)

---

## **Module 7 : Techniques Avancées et Optimisation** *(Niveau Expert)*

### 22. [Pointeurs avancés](22-pointeurs-avances/README.md)
- 22.1 [Pointeurs de fonctions (Callbacks)](22-pointeurs-avances/01-pointeurs-fonctions.md)
- 22.2 [Tableaux de pointeurs de fonctions](22-pointeurs-avances/02-tableaux-pointeurs-fonctions.md)
- 22.3 [Pointeurs multi-niveaux](22-pointeurs-avances/03-pointeurs-multi-niveaux.md)
- 22.4 [Pointeurs opaques](22-pointeurs-avances/04-pointeurs-opaques.md)
- 22.5 [Fonctions variadiques](22-pointeurs-avances/05-fonctions-variadiques.md)
- 22.6 [va_list et macros variadiques](22-pointeurs-avances/06-va-list.md)

### 23. [Macros et Préprocesseur avancé](23-macros-preprocesseur/README.md)
- 23.1 [Macros paramétrées](23-macros-preprocesseur/01-macros-parametrees.md)
- 23.2 [Stringification et concaténation](23-macros-preprocesseur/02-stringification-concatenation.md)
- 23.3 [Compilation conditionnelle](23-macros-preprocesseur/03-compilation-conditionnelle.md)
- 23.4 [Macros cross-platform](23-macros-preprocesseur/04-macros-cross-platform.md)
- 23.5 [Dangers et pièges des macros](23-macros-preprocesseur/05-dangers-macros.md)
- 23.6 [Macros prédéfinies utiles](23-macros-preprocesseur/06-macros-predefinies.md)
- 23.7 [X-Macros](23-macros-preprocesseur/07-x-macros.md)

### 24. [Gestion avancée de la mémoire](24-gestion-memoire-avancee/README.md)
- **24.1 [RAII-like en C : __attribute__((cleanup))](24-gestion-memoire-avancee/01-raii-cleanup.md)**
    - 24.1.1 [Extension GNU](24-gestion-memoire-avancee/01.1-extension-gnu.md)
    - 24.1.2 [Utilisation dans systemd et GLib](24-gestion-memoire-avancee/01.2-systemd-glib.md)
    - 24.1.3 [Patterns pour fichiers, mémoire, locks](24-gestion-memoire-avancee/01.3-patterns.md)
    - 24.1.4 [Limitations et portabilité](24-gestion-memoire-avancee/01.4-limitations-portabilite.md)
- 24.2 [Custom allocators](24-gestion-memoire-avancee/02-custom-allocators.md)
- 24.3 [Garbage collection en C](24-gestion-memoire-avancee/03-garbage-collection.md)

### 25. [Gestion des erreurs robuste](25-gestion-erreurs/README.md)
- 25.1 [Codes de retour vs exceptions simulées](25-gestion-erreurs/01-codes-retour-vs-exceptions.md)
- 25.2 [La variable errno](25-gestion-erreurs/02-errno.md)
- 25.3 [Patterns de gestion d'erreurs](25-gestion-erreurs/03-patterns-gestion-erreurs.md)
- 25.4 [Assertions](25-gestion-erreurs/04-assertions.md)
- 25.5 [Design by contract](25-gestion-erreurs/05-design-by-contract.md)
- 25.6 [Logging](25-gestion-erreurs/06-logging.md)

### 26. [Sécurité et Code défensif](26-securite-code-defensif/README.md)
- 26.1 [Secure C Coding Standards (CERT C)](26-securite-code-defensif/01-cert-c-standards.md)
- 26.2 [Validation des entrées](26-securite-code-defensif/02-validation-entrees.md)
- 26.3 [Prévention des buffer overflows](26-securite-code-defensif/03-prevention-buffer-overflows.md)
- 26.4 [Format string vulnerabilities](26-securite-code-defensif/04-format-string-vulnerabilities.md)
- 26.5 [Integer overflow](26-securite-code-defensif/05-integer-overflow.md)
- 26.6 [Principe du moindre privilège](26-securite-code-defensif/06-moindre-privilege.md)
- 26.7 [Static analysis et fuzzing](26-securite-code-defensif/07-static-analysis-fuzzing.md)
- 26.8 [Compilation avec hardening flags](26-securite-code-defensif/08-hardening-flags.md)

### 27. [Optimisation et Performance](27-optimisation-performance/README.md)
- 27.1 [Flags d'optimisation GCC](27-optimisation-performance/01-flags-optimisation.md)
- 27.2 [Comprendre l'optimiseur](27-optimisation-performance/02-comprendre-optimiseur.md)
- 27.3 [Profiling](27-optimisation-performance/03-profiling.md)
- 27.4 [Cache awareness](27-optimisation-performance/04-cache-awareness.md)
- 27.5 [Branch prediction](27-optimisation-performance/05-branch-prediction.md)
- 27.6 [Optimisations algorithmiques](27-optimisation-performance/06-optimisations-algorithmiques.md)
- 27.7 [Vectorisation et SIMD](27-optimisation-performance/07-vectorisation-simd.md)
- 27.8 [Link-Time Optimization (LTO)](27-optimisation-performance/08-lto.md)
- 27.9 [Profile-Guided Optimization (PGO)](27-optimisation-performance/09-pgo.md)
- 27.10 [Benchmarking rigoureux](27-optimisation-performance/10-benchmarking.md)

### 28. [Interopérabilité](28-interoperabilite/README.md)
- 28.1 [Appeler du C depuis Python](28-interoperabilite/01-c-depuis-python.md)
- 28.2 [Appeler du C depuis Go](28-interoperabilite/02-c-depuis-go.md)
- 28.3 [Interfaçage avec Rust (FFI)](28-interoperabilite/03-interfacage-rust.md)
- 28.4 [ABI et compatibilité](28-interoperabilite/04-abi-compatibilite.md)
- 28.5 [extern "C" pour C++](28-interoperabilite/05-extern-c.md)
- 28.6 [Name mangling](28-interoperabilite/06-name-mangling.md)

---

## **Module 8 : C dans un Pipeline CI/CD** *(Bonus DevOps)* ⭐

### 29. [Tests et Qualité](29-tests-et-qualite/README.md)
- 29.1 [Philosophie du testing en C](29-tests-et-qualite/01-philosophie-testing.md)
- 29.2 [Tests unitaires : frameworks](29-tests-et-qualite/02-frameworks-tests-unitaires.md)
- 29.3 [Écriture de tests et assertions](29-tests-et-qualite/03-ecriture-tests.md)
- 29.4 [Mocking et stubbing](29-tests-et-qualite/04-mocking-stubbing.md)
- 29.5 [Tests d'intégration](29-tests-et-qualite/05-tests-integration.md)
- 29.6 [Mesure de couverture](29-tests-et-qualite/06-mesure-couverture.md)
- 29.7 [Test-Driven Development (TDD)](29-tests-et-qualite/07-tdd.md)

### 30. [Intégration Continue moderne](30-integration-continue/README.md) 🔥
- **30.1 [GitHub Actions pour projets C](30-integration-continue/01-github-actions.md)**
    - 30.1.1 [Structure du workflow](30-integration-continue/01.1-structure-workflow.md)
    - 30.1.2 [Matrix build](30-integration-continue/01.2-matrix-build.md)
    - 30.1.3 [Build avec CMake en CI](30-integration-continue/01.3-build-cmake-ci.md)
    - 30.1.4 [Exécution des tests unitaires](30-integration-continue/01.4-execution-tests.md)
    - 30.1.5 [Vérification avec sanitizers](30-integration-continue/01.5-verification-sanitizers.md)
    - 30.1.6 [Analyse Valgrind automatisée](30-integration-continue/01.6-analyse-valgrind.md)
    - 30.1.7 [Code coverage et Codecov](30-integration-continue/01.7-code-coverage-codecov.md)
    - 30.1.8 [Static analysis](30-integration-continue/01.8-static-analysis.md)
    - 30.1.9 [Publication d'artifacts](30-integration-continue/01.9-publication-artifacts.md)
- 30.2 [GitLab CI pour projets C](30-integration-continue/02-gitlab-ci.md)
- 30.3 [Jenkins : pipelines déclaratifs](30-integration-continue/03-jenkins.md)
- 30.4 [Gestion des caches](30-integration-continue/04-gestion-caches.md)
- 30.5 [Notifications et reporting](30-integration-continue/05-notifications-reporting.md)

### 31. [Cross-compilation et Déploiement](31-cross-compilation-deploiement/README.md)
- 31.1 [Cross-compilation : principes](31-cross-compilation-deploiement/01-principes-cross-compilation.md)
- 31.2 [Compiler pour ARM depuis x86](31-cross-compilation-deploiement/02-compiler-arm.md)
- 31.3 [CMake et cross-compilation](31-cross-compilation-deploiement/03-cmake-cross-compilation.md)
- 31.4 [QEMU pour tester les binaires](31-cross-compilation-deploiement/04-qemu-tests.md)
- 31.5 [Packaging Linux : .deb](31-cross-compilation-deploiement/05-packaging-deb.md)
- 31.6 [Packaging Linux : .rpm](31-cross-compilation-deploiement/06-packaging-rpm.md)
- 31.7 [AppImage](31-cross-compilation-deploiement/07-appimage.md)
- 31.8 [Conteneurisation : Docker multi-stage](31-cross-compilation-deploiement/08-docker-multi-stage.md)
- 31.9 [Distribution de bibliothèques partagées](31-cross-compilation-deploiement/09-distribution-bibliotheques.md)

### 32. [Documentation et Maintenance](32-documentation-maintenance/README.md)
- 32.1 [Documentation du code : Doxygen](32-documentation-maintenance/01-doxygen.md)
    - 32.1.1 [Syntaxe des commentaires](32-documentation-maintenance/01.1-syntaxe-commentaires.md)
    - 32.1.2 [Génération HTML/PDF](32-documentation-maintenance/01.2-generation-html-pdf.md)
    - 32.1.3 [Intégration dans le CI](32-documentation-maintenance/01.3-integration-ci.md)
- 32.2 [README et documentation utilisateur](32-documentation-maintenance/02-readme-documentation.md)
- 32.3 [Gestion de versions avec Git](32-documentation-maintenance/03-gestion-versions-git.md)
    - 32.3.1 [Commits atomiques](32-documentation-maintenance/03.1-commits-atomiques.md)
    - 32.3.2 [Branches et workflow](32-documentation-maintenance/03.2-branches-workflow.md)
    - 32.3.3 [Gestion des binaires](32-documentation-maintenance/03.3-gestion-binaires.md)
- 32.4 [Changelog et versioning sémantique](32-documentation-maintenance/04-changelog-semver.md)
- 32.5 [Maintenance du code legacy](32-documentation-maintenance/05-maintenance-code-legacy.md)
    - 32.5.1 [Refactoring progressif](32-documentation-maintenance/05.1-refactoring-progressif.md)
    - 32.5.2 [Ajout de tests](32-documentation-maintenance/05.2-ajout-tests.md)
    - 32.5.3 [Modernisation](32-documentation-maintenance/05.3-modernisation.md)

---

## **Module 9 : Projets de Synthèse et Études de Cas** *(Optionnel)*

### 33. [Analyse de code open-source](33-analyse-code-opensource/README.md)
- 33.1 [Méthodologie de lecture de code](33-analyse-code-opensource/01-methodologie-lecture.md)
- 33.2 [Étude de cas : Git](33-analyse-code-opensource/02-etude-cas-git.md)
- 33.3 [Étude de cas : Redis](33-analyse-code-opensource/03-etude-cas-redis.md)
- 33.4 [Étude de cas : Nginx](33-analyse-code-opensource/04-etude-cas-nginx.md)
- 33.5 [Contribution à des projets C open-source](33-analyse-code-opensource/05-contribution-opensource.md)
    - 33.5.1 [Trouver un bon premier issue](33-analyse-code-opensource/05.1-trouver-premier-issue.md)
    - 33.5.2 [Comprendre le coding style](33-analyse-code-opensource/05.2-coding-style.md)
    - 33.5.3 [Soumettre une Pull Request](33-analyse-code-opensource/05.3-soumettre-pr.md)

### 34. [Études de cas DevOps](34-etudes-cas-devops/README.md)
- 34.1 [Création d'un outil CLI système](34-etudes-cas-devops/01-outil-cli-systeme.md)
    - 34.1.1 [Parsing d'arguments](34-etudes-cas-devops/01.1-parsing-arguments.md)
    - 34.1.2 [Interaction avec le système](34-etudes-cas-devops/01.2-interaction-systeme.md)
    - 34.1.3 [Output formaté](34-etudes-cas-devops/01.3-output-formate.md)
- 34.2 [Parser de logs haute performance](34-etudes-cas-devops/02-parser-logs.md)
    - 34.2.1 [Lecture efficace de gros fichiers](34-etudes-cas-devops/02.1-lecture-gros-fichiers.md)
    - 34.2.2 [Expressions régulières](34-etudes-cas-devops/02.2-expressions-regulieres.md)
    - 34.2.3 [Agrégation et statistiques](34-etudes-cas-devops/02.3-agregation-statistiques.md)
- 34.3 [Agent de monitoring système](34-etudes-cas-devops/03-agent-monitoring.md)
    - 34.3.1 [Collecte de métriques](34-etudes-cas-devops/03.1-collecte-metriques.md)
    - 34.3.2 [Export Prometheus](34-etudes-cas-devops/03.2-export-prometheus.md)
    - 34.3.3 [Daemonisation](34-etudes-cas-devops/03.3-daemonisation.md)
- 34.4 [Serveur web minimaliste](34-etudes-cas-devops/04-serveur-web.md)
    - 34.4.1 [Architecture event-driven](34-etudes-cas-devops/04.1-architecture-event-driven.md)
    - 34.4.2 [HTTP parsing](34-etudes-cas-devops/04.2-http-parsing.md)
    - 34.4.3 [Static file serving](34-etudes-cas-devops/04.3-static-file-serving.md)

### 35. [Debugging de code complexe](35-debugging-code-complexe/README.md)
- 35.1 [Analyse de core dumps en production](35-debugging-code-complexe/01-core-dumps-production.md)
- 35.2 [Résolution de memory leaks](35-debugging-code-complexe/02-resolution-memory-leaks.md)
- 35.3 [Investigation de race conditions](35-debugging-code-complexe/03-investigation-race-conditions.md)
- 35.4 [Debugging de deadlocks](35-debugging-code-complexe/04-debugging-deadlocks.md)
- 35.5 [Performance debugging](35-debugging-code-complexe/05-performance-debugging.md)

---

## **Annexes et Ressources**

### A. [Références essentielles](annexes/references-essentielles/README.md)
- A.1 [Standards C officiels](annexes/references-essentielles/01-standards-c-officiels.md)
- A.2 [Man pages Linux essentielles](annexes/references-essentielles/02-man-pages-linux.md)
- A.3 [Livres de référence recommandés](annexes/references-essentielles/03-livres-reference.md)
- A.4 [Ressources en ligne et communautés](annexes/references-essentielles/04-ressources-en-ligne.md)

### B. [Antisèches (Cheat sheets)](annexes/antiseches/README.md)
- B.1 [Options GCC](annexes/antiseches/01-options-gcc.md)
- B.2 [Commandes GDB](annexes/antiseches/02-commandes-gdb.md)
- B.3 [Appels système Linux](annexes/antiseches/03-appels-systeme-linux.md)
- B.4 [POSIX threads API](annexes/antiseches/04-posix-threads-api.md)
- B.5 [CMake commandes principales](annexes/antiseches/05-cmake-commandes.md)
- B.6 [Flags de sanitizers et Valgrind](annexes/antiseches/06-flags-sanitizers-valgrind.md)

### C. [Configuration de l'environnement](annexes/configuration-environnement/README.md)
- C.1 [.vimrc optimisé pour C](annexes/configuration-environnement/01-vimrc-optimise.md)
- C.2 [VS Code settings.json et extensions](annexes/configuration-environnement/02-vscode-settings.md)
- C.3 [.clang-format exemple](annexes/configuration-environnement/03-clang-format-exemple.md)
- C.4 [.devcontainer.json complet](annexes/configuration-environnement/04-devcontainer-json-complet.md)
- C.5 [GitHub Actions workflow template](annexes/configuration-environnement/05-github-actions-template.md)

### D. [Glossaire](annexes/glossaire/README.md)
- D.1 [Terminologie C](annexes/glossaire/01-terminologie-c.md)
- D.2 [Terminologie système Linux](annexes/glossaire/02-terminologie-linux.md)
- D.3 [Acronymes DevOps](annexes/glossaire/03-acronymes-devops.md)
- D.4 [Termes eBPF](annexes/glossaire/04-termes-ebpf.md)

---

## 🎯 [Parcours recommandés selon le profil](parcours-recommandes/README.md)

- [Développeur Backend → DevOps](parcours-recommandes/01-backend-vers-devops.md)
- [SysAdmin → Ingénieur Système](parcours-recommandes/02-sysadmin-vers-systeme.md)
- [Embedded → Linux Embarqué](parcours-recommandes/03-embedded-vers-linux-embarque.md)
- [Débutant Complet → Ingénieur C Full-Stack](parcours-recommandes/04-debutant-vers-fullstack.md)
- [Formation Complète (intensive)](parcours-recommandes/05-formation-complete-intensive.md)

---

## 📊 [Ressources complémentaires](ressources-complementaires/README.md)

- [Matrice de Compétences par Parcours](ressources-complementaires/01-matrice-competences.md)
- [Critères d'évaluation et certification](ressources-complementaires/02-evaluation-certification.md)
- [Conseils pour réussir](ressources-complementaires/03-conseils-reussite.md)
- [Projets pratiques recommandés](ressources-complementaires/04-projets-pratiques.md)

---

**🚀 Cette formation vous transforme en ingénieur système Linux moderne, capable de coder en C avec les standards de qualité 2025, d'automatiser via CI/CD, et de maîtriser les technologies émergentes comme eBPF.**
