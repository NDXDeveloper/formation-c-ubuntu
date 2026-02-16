# Chapitre 31 - Cross-compilation et deploiement : Compilation des exemples

Note : Ces programmes sont concus pour la cross-compilation ARM mais sont  
compiles ici en natif (x86_64). Les branches #ifdef ARM prendront le chemin
"else" (Unknown/Non-ARM/x86_64).

## Exemples

### 01_hello_arch.c
- **Section** : 31.2 - Compiler pour ARM
- **Description** : Hello world avec detection d'architecture via preprocesseur
- **Fichier source** : 02-compiler-arm.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 01_hello_arch 01_hello_arch.c
  ```
- **Sortie attendue** (natif x86_64) :
  ```
  Hello from ARM!
  Running on Unknown architecture
  ```

### 02_cross_multi/ (projet multi-fichiers)
- **Section** : 31.2 - Compiler pour ARM
- **Description** : Projet multi-fichiers avec detection d'architecture
- **Fichier source** : 02-compiler-arm.md
- **Fichiers** : `main.c`, `utils.c`, `utils.h`
- **Compilation** :
  ```bash
  cd 02_cross_multi/
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o programme main.c utils.c -I.
  ```
- **Sortie attendue** (natif x86_64) :
  ```
  === Programme cross-compile ===
  Architecture: Non-ARM
  ```

### 03_cmake_native/ (projet multi-fichiers)
- **Section** : 31.3 - CMake et cross-compilation
- **Description** : Projet CMake avec detection architecture et version compilateur
- **Fichier source** : 03-cmake-cross-compilation.md
- **Fichiers** : `main.c`, `utils.c`, `utils.h`
- **Compilation** :
  ```bash
  cd 03_cmake_native/
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o programme main.c utils.c -I.
  ```
- **Sortie attendue** (natif x86_64) :
  ```
  === Programme cross-compile ===
  Architecture: x86_64
  Compile avec: 13.3.0
  ```

### 04_hello_qemu.c
- **Section** : 31.4 - QEMU pour tester les binaires
- **Description** : Hello world simple pour test QEMU
- **Fichier source** : 04-qemu-tests.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 04_hello_qemu 04_hello_qemu.c
  ```
- **Sortie attendue** :
  ```
  Hello from ARM!
  Programme execute avec succes.
  ```

### 05_args_qemu.c
- **Section** : 31.4 - QEMU pour tester les binaires
- **Description** : Programme avec arguments en ligne de commande (test QEMU)
- **Fichier source** : 04-qemu-tests.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 05_args_qemu 05_args_qemu.c
  ```
- **Execution** : `./05_args_qemu arg1 arg2 arg3`
- **Sortie attendue** :
  ```
  Nombre d'arguments : 4
    argv[0] = ./05_args_qemu
    argv[1] = arg1
    argv[2] = arg2
    argv[3] = arg3
  ```

### 06_return_code.c
- **Section** : 31.4 - QEMU pour tester les binaires
- **Description** : Programme avec code de retour configurable (test QEMU)
- **Fichier source** : 04-qemu-tests.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 06_return_code 06_return_code.c
  ```
- **Execution** : `./06_return_code 42; echo $?`
- **Sortie attendue** :
  ```
  Exiting with code: 42
  42
  ```

### 07_benchmark.c
- **Section** : 31.4 - QEMU pour tester les binaires
- **Description** : Benchmark simple pour mesurer les performances (QEMU vs natif)
- **Fichier source** : 04-qemu-tests.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 07_benchmark 07_benchmark.c
  ```
- **Sortie attendue** :
  ```
  Resultat : 4999999950000000
  Temps : <variable> secondes
  ```

### 08_threads.c
- **Section** : 31.4 - QEMU pour tester les binaires
- **Description** : Programme multi-thread pour test QEMU avec pthread
- **Fichier source** : 04-qemu-tests.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 08_threads 08_threads.c -pthread
  ```
- **Sortie attendue** (ordre des threads variable) :
  ```
  Thread 1 s'execute
  Thread 2 s'execute
  Thread 3 s'execute
  Thread 4 s'execute
  Tous les threads ont termine
  ```

### 09_hello_rpm.c
- **Section** : 31.6 - Packaging RPM
- **Description** : Programme simple pour demonstration packaging RPM
- **Fichier source** : 06-packaging-rpm.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 09_hello_rpm 09_hello_rpm.c
  ```
- **Sortie attendue** :
  ```
  Mon Programme v1.0.0
  Hello from RPM!
  ```

### 10_appimage_main.c
- **Section** : 31.7 - AppImage
- **Description** : Programme pour demonstration packaging AppImage
- **Fichier source** : 07-appimage.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 10_appimage_main 10_appimage_main.c
  ```
- **Execution** : `./10_appimage_main test1 test2`
- **Sortie attendue** :
  ```
  === Mon Application ===
  Version: 1.0.0
  Arguments recus:
    1: test1
    2: test2
  Application executee avec succes!
  ```

### 11_hello_docker.c
- **Section** : 31.8 - Docker multi-stage
- **Description** : Programme simple pour demonstration Docker multi-stage build
- **Fichier source** : 08-docker-multi-stage.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 11_hello_docker 11_hello_docker.c
  ```
- **Sortie attendue** :
  ```
  Hello from Docker!
  Application compilee dans un conteneur multi-stage
  ```

### 12_libmymath/ (bibliotheque partagee + test)
- **Section** : 31.9 - Distribution de bibliotheques
- **Description** : Bibliotheque partagee .so et programme de test
- **Fichier source** : 09-distribution-bibliotheques.md
- **Fichiers** : `mymath.h`, `add.c`, `multiply.c`, `power.c`, `test_mymath.c`
- **Compilation** :
  ```bash
  cd 12_libmymath/

  # Construire la bibliotheque partagee
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -fPIC -shared \
      -Wl,-soname,libmymath.so.1 -o libmymath.so.1.0.0 \
      add.c multiply.c power.c -I.

  # Creer les liens symboliques
  ln -sf libmymath.so.1.0.0 libmymath.so.1
  ln -sf libmymath.so.1 libmymath.so

  # Compiler le programme de test
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o test_mymath \
      test_mymath.c -L. -lmymath -I.
  ```
- **Execution** : `LD_LIBRARY_PATH=. ./test_mymath`
- **Sortie attendue** :
  ```
  mymath version: 1.0.0
  2 + 3 = 5
  4 * 5 = 20
  2^8 = 256
  ```
