# Chapitre 30 - Integration continue : Compilation des exemples

## Fichiers partages

- `calculator.h` : Header du module calculatrice (partage par 01-03)
- `calculator.c` : Implementation du module calculatrice (partage par 01-03)

## Exemples

### 01_test_unity.c
- **Section** : 30.1.4 - Execution des tests unitaires
- **Description** : Test de la calculatrice avec Unity (CI automatise)
- **Fichier source** : 01.4-execution-tests.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 -o 01_test_unity 01_test_unity.c calculator.c \
      /tmp/claude/ch29/unity_fw/src/unity.c -I/tmp/claude/ch29/unity_fw/src -I.
  ```
- **Sortie attendue** :
  ```
  01_test_unity.c:40:test_add_positive_numbers:PASS
  01_test_unity.c:41:test_add_with_zero:PASS
  01_test_unity.c:42:test_add_negative_numbers:PASS
  01_test_unity.c:43:test_subtract:PASS
  01_test_unity.c:44:test_multiply:PASS
  -----------------------
  5 Tests 0 Failures 0 Ignored
  OK
  ```

### 02_test_check.c
- **Section** : 30.1.4 - Execution des tests unitaires
- **Description** : Test de la calculatrice avec Check (CI automatise)
- **Fichier source** : 01.4-execution-tests.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 -o 02_test_check 02_test_check.c calculator.c \
      -lcheck -lm -lsubunit -lpthread
  ```
- **Sortie attendue** :
  ```
  Running suite(s): Calculator
  100%: Checks: 2, Failures: 0, Errors: 0
  ```

### 03_test_criterion.c
- **Section** : 30.1.4 - Execution des tests unitaires
- **Description** : Test de la calculatrice avec Criterion (simple + parametrique)
- **Fichier source** : 01.4-execution-tests.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 -o 03_test_criterion 03_test_criterion.c calculator.c \
      -lcriterion
  ```
- **Sortie attendue** :
  ```
  [====] Synthese: Testes: 6 | Valides: 6 | Echoues: 0 | Plantages: 0
  ```

### 04_asan_stack_overflow.c
- **Section** : 30.1.5 - Verification avec sanitizers
- **Description** : Bug intentionnel - acces hors limites tableau (pour ASan)
- **Fichier source** : 01.5-verification-sanitizers.md
- **Compilation** (sans -Werror, bug intentionnel) :
  ```bash
  gcc -Wall -Wextra -std=c17 -g -o 04_asan_stack_overflow 04_asan_stack_overflow.c
  ```
- **Sortie attendue** : Affiche array[0] a array[5], avec valeur aleatoire pour array[5]
- **Avec ASan** :
  ```bash
  gcc -fsanitize=address -g -o 04_asan_stack_overflow 04_asan_stack_overflow.c
  ```
  ASan detecte : `stack-buffer-overflow`

### 05_asan_use_after_free.c
- **Section** : 30.1.5 - Verification avec sanitizers
- **Description** : Bug intentionnel - utilisation apres liberation (pour ASan)
- **Fichier source** : 01.5-verification-sanitizers.md
- **Compilation** (sans -Werror, bug intentionnel) :
  ```bash
  gcc -Wall -Wextra -std=c17 -g -o 05_asan_use_after_free 05_asan_use_after_free.c
  ```
- **Sortie attendue** : `Value: <valeur aleatoire>` (use-after-free)
- **Avec ASan** :
  ```bash
  gcc -fsanitize=address -g -o 05_asan_use_after_free 05_asan_use_after_free.c
  ```
  ASan detecte : `heap-use-after-free`

### 06_asan_memory_leak.c
- **Section** : 30.1.5 - Verification avec sanitizers
- **Description** : Bug intentionnel - fuite memoire (pour ASan/LSan)
- **Fichier source** : 01.5-verification-sanitizers.md
- **Compilation** (sans -Werror, bug intentionnel) :
  ```bash
  gcc -Wall -Wextra -std=c17 -g -o 06_asan_memory_leak 06_asan_memory_leak.c
  ```
- **Sortie attendue** : Pas de sortie visible (le programme alloue sans liberer)
- **Avec ASan** :
  ```bash
  gcc -fsanitize=address -g -o 06_asan_memory_leak 06_asan_memory_leak.c
  ```
  LSan detecte : `detected memory leaks` (4000 octets en 10 allocations)

### 07_tsan_race_condition.c
- **Section** : 30.1.5 - Verification avec sanitizers
- **Description** : Bug intentionnel - race condition (pour TSan)
- **Fichier source** : 01.5-verification-sanitizers.md
- **Compilation** (sans -Werror, bug intentionnel) :
  ```bash
  gcc -Wall -Wextra -std=c17 -g -o 07_tsan_race_condition 07_tsan_race_condition.c -lpthread
  ```
- **Sortie attendue** : `Counter: <valeur != 200000>` (race condition)
- **Avec TSan** :
  ```bash
  gcc -fsanitize=thread -g -o 07_tsan_race_condition 07_tsan_race_condition.c -lpthread
  ```
  TSan detecte : `data race` sur `shared_counter`

### 08_valgrind_leak.c
- **Section** : 30.1.6 - Analyse Valgrind automatisee
- **Description** : Bug intentionnel - fuite memoire (pour Valgrind Memcheck)
- **Fichier source** : 01.6-analyse-valgrind.md
- **Compilation** (sans -Werror, bug intentionnel) :
  ```bash
  gcc -Wall -Wextra -std=c17 -g -o 08_valgrind_leak 08_valgrind_leak.c
  ```
- **Sortie attendue** :
  ```
  Allocated 400 bytes
  Allocated 400 bytes
  Allocated 400 bytes
  Allocated 400 bytes
  Allocated 400 bytes
  Done
  ```
- **Avec Valgrind** :
  ```bash
  valgrind --leak-check=full ./08_valgrind_leak
  ```
  Memcheck detecte : `definitely lost: 2,000 bytes in 5 blocks`

### 09_valgrind_use_after_free.c
- **Section** : 30.1.6 - Analyse Valgrind automatisee
- **Description** : Bug intentionnel - utilisation apres liberation (pour Valgrind)
- **Fichier source** : 01.6-analyse-valgrind.md
- **Compilation** (sans -Werror, bug intentionnel) :
  ```bash
  gcc -Wall -Wextra -std=c17 -g -o 09_valgrind_use_after_free 09_valgrind_use_after_free.c
  ```
- **Sortie attendue** :
  ```
  Value: 42
  New value: 99
  ```
- **Avec Valgrind** :
  ```bash
  valgrind --leak-check=full ./09_valgrind_use_after_free
  ```
  Memcheck detecte : `Invalid write of size 4` et `Invalid read of size 4`

### 10_valgrind_heap_overflow.c
- **Section** : 30.1.6 - Analyse Valgrind automatisee
- **Description** : Bug intentionnel - depassement de tampon heap (pour Valgrind)
- **Fichier source** : 01.6-analyse-valgrind.md
- **Compilation** (sans -Werror, bug intentionnel) :
  ```bash
  gcc -Wall -Wextra -std=c17 -g -o 10_valgrind_heap_overflow 10_valgrind_heap_overflow.c
  ```
- **Sortie attendue** : Pas de sortie visible (strcpy silencieux)
- **Avec Valgrind** :
  ```bash
  valgrind --leak-check=full ./10_valgrind_heap_overflow
  ```
  Memcheck detecte : `Invalid write of size 1` (strcpy depasse le buffer de 10 octets)

### 11_helgrind_race.c
- **Section** : 30.1.6 - Analyse Valgrind automatisee
- **Description** : Bug intentionnel - race condition (pour Helgrind)
- **Fichier source** : 01.6-analyse-valgrind.md
- **Compilation** (sans -Werror, bug intentionnel) :
  ```bash
  gcc -Wall -Wextra -std=c17 -g -o 11_helgrind_race 11_helgrind_race.c -lpthread
  ```
- **Sortie attendue** : `Counter: <valeur != 200000>` (race condition)
- **Avec Helgrind** :
  ```bash
  valgrind --tool=helgrind ./11_helgrind_race
  ```
  Helgrind detecte : `Possible data race` sur `counter`

### 12_bad_code.c
- **Section** : 30.1.8 - Static analysis
- **Description** : Code avec problemes intentionnels (pour analyse statique)
- **Fichier source** : 01.8-static-analysis.md
- **Compilation** (sans -Werror, bugs intentionnels) :
  ```bash
  gcc -Wall -Wextra -std=c17 -g -o 12_bad_code 12_bad_code.c
  ```
- **Sortie attendue** : `Result: 20`
- **Avec clang-tidy** :
  ```bash
  clang-tidy 12_bad_code.c --
  ```
  Detecte : variable non initialisee, pas de verification NULL apres malloc, fuite memoire
