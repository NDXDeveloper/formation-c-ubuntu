# Compilation des exemples - Chapitre 32

## Flags standard

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17
```

## Programmes simples

```bash
# 01 - Doxygen : programme documenté avec syntaxe Doxygen
gcc -Wall -Wextra -Werror -pedantic -std=c17 01_doxygen_demo.c -o 01_doxygen_demo

# 02 - Legacy CSV : parser C89 style (pratiques obsolètes intentionnelles)
gcc -Wall -Wextra -Werror -pedantic -std=c17 02_legacy_csv.c -o 02_legacy_csv

# 03 - Modern CSV : parser C11 style (strdup/strtok_r via POSIX)
gcc -Wall -Wextra -Werror -pedantic -std=c17 -D_POSIX_C_SOURCE=200809L 03_modern_csv.c -o 03_modern_csv

# 04 - Pthread mutex : concurrence pre-C11
gcc -Wall -Wextra -Werror -pedantic -std=c17 -pthread 04_pthread_mutex.c -o 04_pthread_mutex

# 05 - C11 threads : concurrence standard C11
gcc -Wall -Wextra -Werror -pedantic -std=c17 -pthread 05_c11_threads.c -o 05_c11_threads

# 06 - C11 atomics : opérations atomiques lock-free
gcc -Wall -Wextra -Werror -pedantic -std=c17 -pthread 06_c11_atomics.c -o 06_c11_atomics

# 09 - Modernization : patterns C99/C11 combinés
gcc -Wall -Wextra -Werror -pedantic -std=c17 09_modernization.c -o 09_modernization
```

## Projets multi-fichiers

### 07 - Unity test (nécessite Unity framework)

```bash
cd 07_unity_test  
gcc -Wall -Wextra -Werror -pedantic -std=c17 \
    -I <chemin_vers_unity>/src \
    test_example.c my_module.c <chemin_vers_unity>/src/unity.c \
    -o test_example
./test_example
```

### 08 - Check test (nécessite libcheck : `sudo apt install check`)

```bash
cd 08_check_test  
gcc -Wall -Wextra -Werror -pedantic -std=c17 \
    test_check.c my_module.c \
    -pthread -lcheck_pic -lrt -lm -lsubunit \
    -o test_check
./test_check
```

## Notes

- **03** nécessite `-D_POSIX_C_SOURCE=200809L` pour `strdup()` et `strtok_r()`
- **04/05/06** nécessitent `-pthread` pour le multithreading
- **07** nécessite le framework Unity (source à compiler)
- **08** nécessite le package `check` (`sudo apt install check`)
