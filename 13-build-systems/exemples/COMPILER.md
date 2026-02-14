# Chapitre 13 - Build Systems : Compilation des exemples

## Fichiers standalone

### 02_fibonacci.c
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -O2 -o 02_fibonacci 02_fibonacci.c
./02_fibonacci
# Sortie : Fib(40) = 102334155
# Note : -O2 recommandé, le calcul est lent sans optimisation
```

### 03_build_type_detect.c
```bash
# Mode Debug (par défaut)
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 03_build_type_detect 03_build_type_detect.c
./03_build_type_detect
# Sortie : Build: DEBUG (non optimise)

# Mode Release (avec NDEBUG)
gcc -Wall -Wextra -Werror -pedantic -std=c17 -DNDEBUG -o 03_build_type_detect 03_build_type_detect.c
./03_build_type_detect
# Sortie : Build: RELEASE (optimise)
```

## Projets multi-fichiers

### 01_hello_cmake/ (CMake)
```bash
cd 01_hello_cmake
cmake -B build
cmake --build build
./build/hello
# Sortie : Hello, CMake!
#          Mon premier projet avec CMake sur Ubuntu.

# Nettoyage
rm -rf build
```

### 04_make_projet/ (Make)
```bash
cd 04_make_projet
make
./programme
# Sortie : Résultat : 8

# Nettoyage
make clean
```

### 05_make_vs_cmake/ (Make et CMake)
```bash
cd 05_make_vs_cmake

# Build avec Make
make
./calculator
# Sortie : Calculatrice - Make vs CMake
#          ============================
#          10.0 + 3.0 = 13.00
#          10.0 - 3.0 = 7.00
#          10.0 * 3.0 = 30.00
#          10.0 / 3.0 = 3.33
make clean

# Build avec CMake
cmake -B build
cmake --build build
./build/calculator
# Même sortie que ci-dessus

# Nettoyage
rm -rf build
```
