# Chapitre 18 - Threads et concurrence : Guide de compilation

## Compilation par défaut

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -pthread -o programme fichier.c
```

**Note** : Tous les programmes de ce chapitre nécessitent `-pthread` pour la liaison avec la bibliothèque POSIX Threads.

Les fichiers utilisant `usleep()`, `pthread_barrier_t`, `PTHREAD_MUTEX_RECURSIVE` ou d'autres extensions POSIX définissent `_DEFAULT_SOURCE` en début de fichier.

---

## Liste des exemples

### 01_hello_thread.c
- **Section** : 18.1 - Introduction à pthread
- **Description** : Premier programme multi-threadé - Hello World
- **Fichier source** : 01-introduction-pthread.md
- **Sortie attendue** : Messages du thread principal et du thread secondaire avec leurs IDs

### 02_print_number.c
- **Section** : 18.1 - Introduction à pthread
- **Description** : Passage de paramètre et valeur de retour d'un thread
- **Fichier source** : 01-introduction-pthread.md
- **Sortie attendue** : Thread reçoit un nombre, le multiplie par 2, retourne le résultat

### 03_threads_multiples.c
- **Section** : 18.2 - Création et terminaison
- **Description** : Création de 5 threads avec identifiants uniques
- **Fichier source** : 02-creation-terminaison.md
- **Sortie attendue** : 5 threads affichent leur numéro et ID pthread (ordre non déterministe)

### 04_retour_structure.c
- **Section** : 18.2 - Création et terminaison
- **Description** : Retour d'une structure allouée depuis un thread via pthread_join
- **Fichier source** : 02-creation-terminaison.md
- **Sortie attendue** : Résultats statistiques (count, average, status)

### 05_timing_join.c
- **Section** : 18.2 - Création et terminaison
- **Description** : Comportement bloquant de pthread_join mesuré avec clock_gettime
- **Fichier source** : 02-creation-terminaison.md
- **Sortie attendue** : Le join bloque ~2 secondes, temps mesuré affiché

### 06_pthread_exit_main.c
- **Section** : 18.2 - Création et terminaison
- **Description** : pthread_exit dans main permet aux threads de continuer
- **Fichier source** : 02-creation-terminaison.md
- **Sortie attendue** : Le main quitte mais les threads finissent leur travail

### 07_passage_entier.c
- **Section** : 18.3 - Passage de paramètres
- **Description** : Trois méthodes de passage d'entier (adresse, cast, malloc)
- **Fichier source** : 03-passage-parametres.md
- **Sortie attendue** : Les 3 méthodes affichent la valeur 42

### 08_passage_structure.c
- **Section** : 18.3 - Passage de paramètres
- **Description** : Passage d'une structure comme paramètre de thread
- **Fichier source** : 03-passage-parametres.md
- **Sortie attendue** : Thread affiche nom, âge, département depuis la structure

### 09_calcul_parallele.c
- **Section** : 18.3 - Passage de paramètres
- **Description** : Somme parallèle avec 4 threads (0 à 100)
- **Fichier source** : 03-passage-parametres.md
- **Sortie attendue** : Sommes partielles puis total = 5050 (vérifié par formule n(n+1)/2)

### 10_equation.c
- **Section** : 18.3 - Passage de paramètres
- **Description** : Résolution d'équation du second degré dans un thread
- **Fichier source** : 03-passage-parametres.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -pthread -o 10 10_equation.c -lm`
- **Sortie attendue** : Solutions de l'équation ax²+bx+c=0

### 11_communication_bidir.c
- **Section** : 18.3 - Passage de paramètres
- **Description** : Communication bidirectionnelle - thread transforme un message
- **Fichier source** : 03-passage-parametres.md
- **Sortie attendue** : Message envoyé puis transformé en majuscules

### 12_thread_joinable.c
- **Section** : 18.4 - Threads détachés vs joinable
- **Description** : Thread joinable avec récupération de valeur de retour
- **Fichier source** : 04-detaches-vs-joinable.md
- **Sortie attendue** : Thread fait son travail, main récupère le résultat via join

### 13_thread_detache.c
- **Section** : 18.4 - Threads détachés vs joinable
- **Description** : Thread détaché avec pthread_detach
- **Fichier source** : 04-detaches-vs-joinable.md
- **Sortie attendue** : Thread détaché travaille pendant que main attend 3 secondes

### 14_attr_detache.c
- **Section** : 18.4 - Threads détachés vs joinable
- **Description** : Création directe d'un thread détaché via attributs
- **Fichier source** : 04-detaches-vs-joinable.md
- **Sortie attendue** : Thread créé directement en mode détaché

### 15_serveur_detache.c
- **Section** : 18.4 - Threads détachés vs joinable
- **Description** : Simulation serveur avec threads détachés pour chaque client
- **Fichier source** : 04-detaches-vs-joinable.md
- **Sortie attendue** : 5 clients simulés traités par des threads détachés

### 16_auto_detachement.c
- **Section** : 18.4 - Threads détachés vs joinable
- **Description** : Thread qui se détache lui-même au démarrage
- **Fichier source** : 04-detaches-vs-joinable.md
- **Sortie attendue** : Thread s'auto-détache puis travaille

### 17_race_condition.c
- **Section** : 18.5 - Race conditions
- **Description** : Race condition sur un compteur partagé (bug intentionnel)
- **Fichier source** : 05-race-conditions.md
- **Sortie attendue** : Valeur obtenue < 2000000 (race condition visible)
- **NOTE** : Exemple volontairement bugué à des fins pédagogiques

### 18_race_banque.c
- **Section** : 18.5 - Race conditions
- **Description** : Race condition sur compte bancaire (bug intentionnel)
- **Fichier source** : 05-race-conditions.md
- **Sortie attendue** : Solde final != 1000 (incohérence due à la race condition)
- **NOTE** : Exemple volontairement bugué à des fins pédagogiques

### 19_mutex_compteur.c
- **Section** : 18.6 - Mutex
- **Description** : Compteur protégé par mutex (corrige la race condition)
- **Fichier source** : 06-mutex.md
- **Sortie attendue** : Attendu = Obtenu = 2000000

### 20_mutex_banque.c
- **Section** : 18.6 - Mutex
- **Description** : Compte bancaire protégé par mutex
- **Fichier source** : 06-mutex.md
- **Sortie attendue** : Solde final = 1000 (cohérent)

### 21_mutex_stats.c
- **Section** : 18.6 - Mutex
- **Description** : Structure statistiques avec mutex intégré
- **Fichier source** : 06-mutex.md
- **Sortie attendue** : Statistiques agrégées (count, sum, min, max, avg) avec total cohérent

### 22_mutex_performance.c
- **Section** : 18.6 - Mutex
- **Description** : Benchmark de l'overhead d'un mutex
- **Fichier source** : 06-mutex.md
- **Sortie attendue** : Temps sans/avec mutex et facteur d'overhead (~3-5x)

### 23_deadlock_ordre.c
- **Section** : 18.7 - Deadlocks
- **Description** : Prévention de deadlock par tri des mutex (adresse)
- **Fichier source** : 07-deadlocks.md
- **Sortie attendue** : Transferts bancaires entre 2 comptes sans deadlock

### 24_mutex_recursif.c
- **Section** : 18.7 - Deadlocks
- **Description** : Mutex récursif pour éviter l'auto-deadlock
- **Fichier source** : 07-deadlocks.md
- **Sortie attendue** : Traversée récursive d'un arbre binaire par 2 threads

### 25_cond_basique.c
- **Section** : 18.8 - Variables de condition
- **Description** : Variable de condition basique (waiter/signaler)
- **Fichier source** : 08-variables-condition.md
- **Sortie attendue** : Waiter attend, signaler prépare et envoie le signal, waiter continue

### 26_prodcons_cond.c
- **Section** : 18.8 - Variables de condition
- **Description** : Producteur-consommateur simple avec variables de condition
- **Fichier source** : 08-variables-condition.md
- **Sortie attendue** : Producteur produit 10 items, consommateur les consomme en alternance

### 27_file_taches.c
- **Section** : 18.8 - Variables de condition
- **Description** : File de tâches avec arrêt propre (shutdown pattern)
- **Fichier source** : 08-variables-condition.md
- **Sortie attendue** : Workers traitent les tâches de la file puis s'arrêtent proprement

### 28_sem_basique.c
- **Section** : 18.9 - Sémaphores
- **Description** : Sémaphore compteur limitant l'accès concurrent (max 2 threads)
- **Fichier source** : 09-semaphores.md
- **Sortie attendue** : 5 threads accèdent par groupes de 2 maximum (~6 secondes)

### 29_sem_pool.c
- **Section** : 18.9 - Sémaphores
- **Description** : Pool de connexions avec sémaphore compteur
- **Fichier source** : 09-semaphores.md
- **Sortie attendue** : 8 threads utilisent un pool de 3 connexions

### 30_sem_prodcons.c
- **Section** : 18.9 - Sémaphores
- **Description** : Producteur-consommateur avec sémaphores (buffer circulaire)
- **Fichier source** : 09-semaphores.md
- **Sortie attendue** : Production et consommation de 20 items via buffer de taille 5

### 31_atomic_intro.c
- **Section** : 18.10.1 - Introduction à stdatomic
- **Description** : Compteur atomique vs race condition
- **Fichier source** : 10.1-introduction-stdatomic.md
- **Sortie attendue** : Valeur finale = 2000000 (atomique, pas de race)

### 32_atomic_bool.c
- **Section** : 18.10.2 - Types atomiques
- **Description** : atomic_bool comme flag d'arrêt gracieux entre threads
- **Fichier source** : 10.2-types-atomiques.md
- **Sortie attendue** : 3 threads travaillent ~2s puis s'arrêtent, total travail affiché

### 33_atomic_histogram.c
- **Section** : 18.10.2 - Types atomiques
- **Description** : Histogramme concurrent avec tableau d'atomiques
- **Fichier source** : 10.2-types-atomiques.md
- **Sortie attendue** : 10 buckets avec distribution ~uniforme, total = 400000

### 34_cas_increment.c
- **Section** : 18.10.4 - Compare-and-swap (CAS)
- **Description** : Incrémentation avec CAS - opération lock-free
- **Fichier source** : 10.4-compare-and-swap.md
- **Sortie attendue** : Compteur final = 400000 (4 threads × 100000)

### 35_rwlock_cache.c
- **Section** : 18.11 - Read-write locks
- **Description** : Cache de configuration avec lectures partagées / écritures exclusives
- **Fichier source** : 11-read-write-locks.md
- **Sortie attendue** : 5 lecteurs lisent la config, 2 écrivains la mettent à jour

### 36_acquire_release.c
- **Section** : 18.10.3 - Memory ordering
- **Description** : Pattern acquire/release pour synchronisation producteur-consommateur
- **Fichier source** : 10.3-memory-ordering.md
- **Sortie attendue** : Producteur écrit 20 valeurs, consommateur les lit (synchronisé)

### 37_benchmark_atomics.c
- **Section** : 18.10.6 - Performance et cas d'usage
- **Description** : Benchmark comparatif relaxed / seq_cst / mutex
- **Fichier source** : 10.6-performance-cas-usage.md
- **Sortie attendue** : Temps et ops/sec pour chaque méthode, speedup relaxed/mutex

### 38_tls_thread_local.c
- **Section** : 18.12 - Thread-local storage
- **Description** : _Thread_local - chaque thread a sa propre copie des variables
- **Fichier source** : 12-thread-local-storage.md
- **Sortie attendue** : 3 threads comptent indépendamment de 1 à 5 (~5 secondes)

### 39_tls_pthread_key.c
- **Section** : 18.12 - Thread-local storage
- **Description** : pthread_key_t avec destructeur automatique pour données par thread
- **Fichier source** : 12-thread-local-storage.md
- **Sortie attendue** : 3 threads comptent indépendamment, nettoyage automatique à la fin

### 40_tls_benchmark.c
- **Section** : 18.12 - Thread-local storage
- **Description** : Benchmark comparatif global+mutex vs _Thread_local vs pthread_key_t
- **Fichier source** : 12-thread-local-storage.md
- **Sortie attendue** : Temps pour chaque méthode (TLS ~100x plus rapide que mutex)

### 41_barrier_simple.c
- **Section** : 18.13 - Barrières de threads
- **Description** : Barrière simple - synchronisation de 5 threads entre deux phases
- **Fichier source** : 13-barrieres-threads.md
- **Sortie attendue** : Tous les threads attendent la barrière puis passent à Phase 2 (~4 secondes)

### 42_barrier_simulation.c
- **Section** : 18.13 - Barrières de threads
- **Description** : Simulation itérative de grille avec barrières entre phases
- **Fichier source** : 13-barrieres-threads.md
- **Sortie attendue** : 10 itérations terminées successivement

### 43_barrier_manuelle.c
- **Section** : 18.13 - Barrières de threads
- **Description** : Implémentation manuelle d'une barrière avec mutex et cond variable
- **Fichier source** : 13-barrieres-threads.md
- **Sortie attendue** : 4 threads font 3 itérations avec synchronisation à chaque barrière

---

## Notes de compilation

| Fichier | Flag supplémentaire | Raison |
|---------|---------------------|--------|
| 10_equation.c | `-lm` (à la fin) | Utilise `sqrt()` de `<math.h>` |
| 23, 24, 26, 27, 29, 30, 32, 35, 36, 41, 42, 43 | `_DEFAULT_SOURCE` (dans le code) | Extensions POSIX (`usleep`, `pthread_barrier_t`, `PTHREAD_MUTEX_RECURSIVE`) |
| 17, 18 | — | Bugs intentionnels (race conditions pédagogiques) |

## Script de compilation rapide

```bash
#!/bin/bash
# Compiler tous les exemples du chapitre 18
FLAGS="-Wall -Wextra -Werror -pedantic -std=c17 -pthread"

for f in *.c; do
    base="${f%.c}"
    if [ "$f" = "10_equation.c" ]; then
        gcc $FLAGS -o "$base" "$f" -lm
    else
        gcc $FLAGS -o "$base" "$f"
    fi

    if [ $? -eq 0 ]; then
        echo "OK: $f"
    else
        echo "ERREUR: $f"
    fi
done
```
