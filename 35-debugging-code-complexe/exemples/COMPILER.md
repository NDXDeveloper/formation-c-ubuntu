# Chapitre 35 - Debugging de code complexe : Compilation des exemples

## Exemples

### 01_core_dump.c
- **Section** : 35.1 - Core dumps en production
- **Description** : Bug intentionnel - buffer overflow declenchant un crash potentiel
- **Fichier source** : 01-core-dumps-production.md
- **Compilation** (sans -Werror, bug intentionnel) :
  ```bash
  gcc -Wall -Wextra -std=c17 -g -o 01_core_dump 01_core_dump.c
  ```
- **Sortie attendue** : Pas de sortie (le off-by-one est silencieux sans sanitizer)
- **Avec ASan** :
  ```bash
  gcc -fsanitize=address -g -o 01_core_dump 01_core_dump.c
  ```
  ASan detecte : `heap-buffer-overflow`

### 02_signal_backtrace.c
- **Section** : 35.1 - Core dumps en production
- **Description** : Gestionnaire de signal avec backtrace pour diagnostic crash
- **Fichier source** : 01-core-dumps-production.md
- **Compilation** (sans -pedantic, execinfo.h non-standard, -rdynamic pour backtrace) :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 -g -rdynamic -o 02_signal_backtrace 02_signal_backtrace.c
  ```
- **Sortie attendue** :
  ```
  Installation du gestionnaire de signal...
  Declenchement d'un crash volontaire...
  Error: signal 11:
  ./02_signal_backtrace(signal_handler+0x...)[0x...]
  ...
  ./02_signal_backtrace(cause_crash+0x...)[0x...]
  ./02_signal_backtrace(main+0x...)[0x...]
  ...
  ```
- **Note** : Exit code 139 (SIGSEGV) - crash intentionnel avec backtrace

### 03_leak_simple.c
- **Section** : 35.2 - Resolution des fuites memoire
- **Description** : Bug intentionnel - fuite memoire simple (allocation sans free)
- **Fichier source** : 02-resolution-memory-leaks.md
- **Compilation** (sans -Werror, bug intentionnel) :
  ```bash
  gcc -Wall -Wextra -std=c17 -g -o 03_leak_simple 03_leak_simple.c
  ```
- **Sortie attendue** :
  ```
  Allocation de 10 buffers sans liberation...
  Termine (10240 octets fuites)
  ```
- **Avec Valgrind** :
  ```bash
  valgrind --leak-check=full ./03_leak_simple
  ```
  Memcheck detecte : `definitely lost: 10,240 bytes in 10 blocks`

### 04_leak_example.c
- **Section** : 35.2 - Resolution des fuites memoire
- **Description** : Bug intentionnel - trois types de fuites memoire (pour Valgrind)
- **Fichier source** : 02-resolution-memory-leaks.md
- **Compilation** (sans -Werror, bugs intentionnels) :
  ```bash
  gcc -Wall -Wextra -std=c17 -g -o 04_leak_example 04_leak_example.c
  ```
- **Sortie attendue** :
  ```
  Demonstration de 3 types de fuites memoire :
    Fuite 1 : allocation directe (100 octets)
    Fuite 2 : retour de fonction ignore (12 octets)
    Fuite 3 : pointeur ecrase (50 octets)
  Termine (162 octets fuites au total)
  ```
- **Avec Valgrind** :
  ```bash
  valgrind --leak-check=full --show-leak-kinds=all ./04_leak_example
  ```
  Memcheck detecte 3 fuites : 100 + 12 + 50 = 162 octets

### 05_race_condition.c
- **Section** : 35.3 - Investigation des race conditions
- **Description** : Bug intentionnel - race condition sur compteur partage
- **Fichier source** : 03-investigation-race-conditions.md
- **Compilation** (sans -Werror, bug intentionnel) :
  ```bash
  gcc -Wall -Wextra -std=c17 -g -o 05_race_condition 05_race_condition.c -lpthread
  ```
- **Sortie attendue** : `Counter: <valeur != 200000> (attendu: 200000)`
- **Avec TSan** :
  ```bash
  gcc -fsanitize=thread -g -o 05_race_condition 05_race_condition.c -lpthread
  ```
  TSan detecte : `data race` sur `counter`
- **Avec Helgrind** :
  ```bash
  valgrind --tool=helgrind ./05_race_condition
  ```

### 06_deadlock.c
- **Section** : 35.4 - Debugging des deadlocks
- **Description** : Bug intentionnel - deadlock classique (2 threads, 2 mutex)
- **Fichier source** : 04-debugging-deadlocks.md
- **Compilation** (sans -Werror, bug intentionnel) :
  ```bash
  gcc -Wall -Wextra -std=c17 -g -o 06_deadlock 06_deadlock.c -lpthread
  ```
- **Sortie attendue** (alarm timeout apres 5 secondes) :
  ```
  Thread 1: Locking A...
  Thread 2: Locking B...
  Thread 1: Locking B...
  Thread 2: Locking A...

  [TIMEOUT] Deadlock detecte ! Le programme est bloque.
  ```
- **Note** : Le programme bloque indefiniment (deadlock), alarm(5) le termine apres 5s
- **Avec Helgrind** :
  ```bash
  valgrind --tool=helgrind ./06_deadlock
  ```
  Helgrind detecte : `lock order violated`

### 07_timing.c
- **Section** : 35.5 - Performance debugging
- **Description** : Mesure de temps d'execution avec clock_gettime (CLOCK_MONOTONIC)
- **Fichier source** : 05-performance-debugging.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 07_timing 07_timing.c
  ```
- **Sortie attendue** :
  ```
  Elapsed: <variable> seconds
  Sum: 499999500000
  ```

### 08_rdtsc.c
- **Section** : 35.5 - Performance debugging
- **Description** : Micro-benchmark avec RDTSC (compteur de cycles CPU x86)
- **Fichier source** : 05-performance-debugging.md
- **Compilation** (sans -pedantic, x86intrin.h non-standard) :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 -o 08_rdtsc 08_rdtsc.c
  ```
- **Sortie attendue** :
  ```
  Cycles: <variable>
  Sum: 499500
  ```
- **Note** : Specifique x86/x86_64 (utilise __rdtsc())
