# Chapitre 15 - Exemples : Compilation et exécution

## 01_asan_heap_overflow.c
- **Section** : 01.1 - AddressSanitizer
- **Description** : Heap buffer overflow détecté par ASan
- **Fichier source** : 01.1-address-sanitizer.md
- **Compilation** : `gcc -g -fsanitize=address -o 01_asan 01_asan_heap_overflow.c`
- **Exécution** : `./01_asan`
- **Sortie attendue** : ASan signale `heap-buffer-overflow` (écriture au-delà du tableau de 5 entiers)

## 02_ubsan_int_overflow.c
- **Section** : 01.2 - UndefinedBehaviorSanitizer
- **Description** : Dépassement d'entier signé détecté par UBSan
- **Fichier source** : 01.2-undefined-behavior-sanitizer.md
- **Compilation** : `gcc -g -fsanitize=undefined -o 02_ubsan 02_ubsan_int_overflow.c`
- **Exécution** : `./02_ubsan`
- **Sortie attendue** : UBSan signale `signed integer overflow: 2147483647 + 1`

## 03_tsan_race_condition.c
- **Section** : 01.3 - ThreadSanitizer
- **Description** : Race condition détectée par TSan
- **Fichier source** : 01.3-thread-sanitizer.md
- **Compilation** : `gcc -g -fsanitize=thread -pthread -o 03_tsan 03_tsan_race_condition.c`
- **Exécution** : `./03_tsan`
- **Sortie attendue** : TSan signale `data race` sur la variable `compteur`
- **Note** : TSan est incompatible avec le kernel 6.17+ (erreur runtime `FATAL: ThreadSanitizer: unexpected memory mapping`). La compilation fonctionne, seule l'exécution échoue.

## 04_lsan_leak.c
- **Section** : 01.4 - LeakSanitizer
- **Description** : Fuite mémoire détectée par LSan
- **Fichier source** : 01.4-leak-sanitizer.md
- **Compilation** : `gcc -g -fsanitize=leak -o 04_lsan 04_lsan_leak.c`
- **Exécution** : `./04_lsan`
- **Sortie attendue** : LSan signale `80 byte(s) leaked in 1 allocation(s)` (tableau de 20 int non libéré)

## 05_core_dump.c
- **Section** : 03.1 - Core dumps
- **Description** : Programme avec pile d'appels provoquant un segfault
- **Fichier source** : 03.1-core-dumps.md
- **Compilation** : `gcc -g -O0 -Wall -Wextra -o 05_core 05_core_dump.c`
- **Exécution** : `./05_core`
- **Sortie attendue** : Affiche `Début du programme` et `Je vais crasher...` puis segfault (exit code 139). Analyse du core dump avec `gdb ./05_core core` pour voir la pile d'appels.

## 06_valgrind_leak.c
- **Section** : 04.1 - Memcheck
- **Description** : Fuite mémoire simple détectée par Valgrind
- **Fichier source** : 04.1-memcheck.md
- **Compilation** : `gcc -g -Wall -Wextra -Werror -pedantic -std=c17 -o 06_valgrind_leak 06_valgrind_leak.c`
- **Exécution** : `valgrind --leak-check=full ./06_valgrind_leak`
- **Sortie attendue** : Valgrind signale `40 bytes in 1 blocks are definitely lost`

## 07_valgrind_out_of_bounds.c
- **Section** : 04.1 - Memcheck
- **Description** : Accès hors limites détecté par Valgrind
- **Fichier source** : 04.1-memcheck.md
- **Compilation** : `gcc -g -Wall -Wextra -pedantic -std=c17 -o 07_valgrind_oob 07_valgrind_out_of_bounds.c`
- **Exécution** : `valgrind ./07_valgrind_oob`
- **Sortie attendue** : Valgrind signale `Invalid write of size 4` (écriture à l'index 10 d'un tableau de 10 éléments)
- **Note** : Bug intentionnel (`i <= 10` au lieu de `i < 10`), compilé sans `-Werror`

## 08_valgrind_uninit.c
- **Section** : 04.1 - Memcheck
- **Description** : Valeur non initialisée détectée par Valgrind
- **Fichier source** : 04.1-memcheck.md
- **Compilation** : `gcc -g -Wall -Wextra -pedantic -std=c17 -o 08_valgrind_uninit 08_valgrind_uninit.c`
- **Exécution** : `valgrind ./08_valgrind_uninit`
- **Sortie attendue** : Valgrind signale `Conditional jump or move depends on uninitialised value(s)` (lecture de values[3] non initialisé)
- **Note** : Bug intentionnel, compilé sans `-Werror`

## 09_valgrind_double_free.c
- **Section** : 04.1 - Memcheck
- **Description** : Double free détecté par Valgrind
- **Fichier source** : 04.1-memcheck.md
- **Compilation** : `gcc -g -Wall -Wextra -pedantic -std=c17 -o 09_valgrind_df 09_valgrind_double_free.c`
- **Exécution** : `valgrind ./09_valgrind_df`
- **Sortie attendue** : Valgrind signale `Invalid free()` (double libération du même bloc mémoire)
- **Note** : Bug intentionnel (`-Wuse-after-free`), compilé sans `-Werror`

## 10_valgrind_use_after_free.c
- **Section** : 04.1 - Memcheck
- **Description** : Use-after-free détecté par Valgrind
- **Fichier source** : 04.1-memcheck.md
- **Compilation** : `gcc -g -Wall -Wextra -pedantic -std=c17 -o 10_valgrind_uaf 10_valgrind_use_after_free.c`
- **Exécution** : `valgrind ./10_valgrind_uaf`
- **Sortie attendue** : Valgrind signale `Invalid read of size 4` (lecture de `*data` après `free(data)`)
- **Note** : Bug intentionnel (`-Wuse-after-free`), compilé sans `-Werror`

## 11_helgrind_race.c
- **Section** : 05.1 - Helgrind
- **Description** : Race condition détectée par Helgrind
- **Fichier source** : 05.1-helgrind.md
- **Compilation** : `gcc -g -Wall -Wextra -Werror -pedantic -std=c17 -pthread -o 11_helgrind 11_helgrind_race.c`
- **Exécution** : `valgrind --tool=helgrind ./11_helgrind`
- **Sortie attendue** : Helgrind signale `Possible data race` sur la variable `compteur` (accès concurrent sans mutex)

## 12_gcov_coverage.c
- **Section** : 07.1 - gcov
- **Description** : Mesure de couverture de code avec gcov
- **Fichier source** : 07.1-gcov.md
- **Compilation** : `gcc -g -Wall -Wextra -Werror -pedantic -std=c17 -fprofile-arcs -ftest-coverage -o 12_gcov 12_gcov_coverage.c`
- **Exécution** : `./12_gcov && gcov 12_gcov_coverage.c`
- **Sortie attendue** : Le programme affiche `Hello, World!`. gcov rapporte `Lines executed: 66.67% of 9` (la fonction `print_goodbye()` n'est jamais appelée)
- **Note** : gcov génère des fichiers `.gcno` et `.gcda` dans le répertoire courant

## Notes générales

- Les exemples 01-04 utilisent les sanitizers (flags `-fsanitize=...`) et ne nécessitent pas `-pedantic`/`-std=c17`
- L'exemple 05 est compilé avec `-g -O0` pour faciliter le débogage avec GDB
- Les exemples 06-10 sont conçus pour être analysés avec Valgrind (compilation avec `-g` obligatoire)
- Les exemples 07-10 contiennent des bugs intentionnels (pédagogiques) et sont compilés sans `-Werror`
- L'exemple 11 nécessite `-pthread` pour les threads POSIX
- L'exemple 12 nécessite les flags `-fprofile-arcs -ftest-coverage` pour la couverture de code
