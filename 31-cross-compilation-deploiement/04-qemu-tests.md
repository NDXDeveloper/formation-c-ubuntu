🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 31.4 QEMU pour tester les binaires

## Introduction

Après avoir cross-compilé un programme pour ARM (ou toute autre architecture), vous vous retrouvez face à un problème : **comment le tester sans avoir le matériel cible sous la main ?**

C'est là qu'intervient **QEMU** (Quick EMUlator), un émulateur open-source qui permet d'exécuter des programmes compilés pour une architecture sur une machine d'architecture différente.

### Pourquoi QEMU ?

Imaginez ces scénarios :

1. **Vous développez pour Raspberry Pi** mais vous n'en avez pas sous la main
2. **Vous compilez pour 5 architectures différentes** et vous voulez toutes les tester rapidement
3. **Vous intégrez la cross-compilation dans un pipeline CI/CD** et voulez valider automatiquement les binaires
4. **Vous debuggez un problème spécifique à ARM** mais il est plus confortable de travailler sur votre PC

QEMU résout tous ces problèmes en **émulant** l'architecture cible.

### QEMU vs Matériel réel

| Aspect | QEMU | Matériel réel |
|--------|------|---------------|
| **Vitesse** | Plus lent (émulation) | Natif, rapide |
| **Disponibilité** | Toujours disponible | Peut ne pas être accessible |
| **Debugging** | Facile (GDB intégré) | Plus complexe |
| **Tests automatisés** | Parfait pour CI/CD | Difficile à automatiser |
| **Coût** | Gratuit | Coût du matériel |
| **Fidélité** | ~95% des cas | 100% |

## Les deux modes de QEMU

QEMU fonctionne en deux modes principaux :

### 1. QEMU User Mode (mode utilisateur)

**C'est celui que nous utiliserons principalement.**

- ✅ Exécute un **binaire unique** pour une autre architecture
- ✅ Rapide et simple à utiliser
- ✅ Parfait pour tester des applications
- ✅ Traduit les appels système à la volée

**Exemple :**
```bash
# Exécuter un binaire ARM sur x86_64
qemu-arm -L /usr/arm-linux-gnueabihf ./programme_arm
```

### 2. QEMU System Mode (mode système)

- Émule un **système complet** avec CPU, mémoire, périphériques
- Plus lent et complexe
- Utile pour tester un OS complet ou des drivers
- Nécessite une image disque et un kernel

**Exemple :**
```bash
# Démarrer une machine virtuelle ARM complète
qemu-system-arm -M virt -kernel kernel.img -hda disk.img
```

Pour nos besoins de test de programmes cross-compilés, **le mode utilisateur suffit largement**.

## Installation de QEMU

### Sur Ubuntu

```bash
# Installation de QEMU user mode pour plusieurs architectures
sudo apt-get update
sudo apt-get install qemu-user qemu-user-static

# Vérifier l'installation
qemu-arm --version
qemu-aarch64 --version
qemu-mips --version
```

### Vérifier les architectures supportées

```bash
# Lister tous les émulateurs disponibles
ls /usr/bin/qemu-*

# Résultat typique :
# qemu-aarch64        (ARM 64 bits)
# qemu-arm            (ARM 32 bits)
# qemu-mips           (MIPS)
# qemu-mips64         (MIPS 64 bits)
# qemu-ppc            (PowerPC)
# qemu-riscv64        (RISC-V 64 bits)
# qemu-x86_64         (x86_64)
# ... et bien d'autres
```

### Installation de qemu-user-static

`qemu-user-static` permet une exécution transparente :

```bash
sudo apt-get install qemu-user-static binfmt-support

# Vérifier
ls /usr/bin/qemu-*-static
```

## Premier test : Hello World

### Étape 1 : Compiler pour ARM

```bash
# Créer un programme simple
cat > hello.c << 'EOF'
#include <stdio.h>

int main(void) {
    printf("Hello from ARM!\n");
    printf("Programme exécuté avec succès.\n");
    return 0;
}
EOF

# Compiler pour ARM 32 bits
arm-linux-gnueabihf-gcc hello.c -o hello_arm32

# Compiler pour ARM 64 bits
aarch64-linux-gnu-gcc hello.c -o hello_arm64
```

### Étape 2 : Tenter l'exécution native (échouera)

```bash
# Essayer d'exécuter directement
./hello_arm32

# Erreur attendue :
# bash: ./hello_arm32: cannot execute binary file: Exec format error
```

### Étape 3 : Exécuter avec QEMU

```bash
# Exécuter le binaire ARM 32 bits
qemu-arm -L /usr/arm-linux-gnueabihf ./hello_arm32

# Résultat :
# Hello from ARM!
# Programme exécuté avec succès.

# Exécuter le binaire ARM 64 bits
qemu-aarch64 -L /usr/aarch64-linux-gnu ./hello_arm64

# Résultat :
# Hello from ARM!
# Programme exécuté avec succès.
```

**Ça marche ! 🎉**

## Comprendre l'option -L (sysroot)

### Pourquoi -L est nécessaire ?

L'option `-L` indique à QEMU où trouver les **bibliothèques système** de l'architecture cible.

```bash
qemu-arm -L /usr/arm-linux-gnueabihf ./programme
         │   │
         │   └─ Chemin du sysroot ARM
         └───── Option pour spécifier le sysroot
```

### Structure du sysroot

```
/usr/arm-linux-gnueabihf/
├── lib/
│   ├── libc.so.6           # Bibliothèque C standard
│   ├── libm.so.6           # Bibliothèque mathématique
│   ├── libpthread.so.0     # Bibliothèque threads
│   └── ld-linux-armhf.so.3 # Chargeur dynamique
└── ...
```

### Chemins sysroot courants

| Architecture | Chemin sysroot typique |
|--------------|------------------------|
| ARM 32 bits  | `/usr/arm-linux-gnueabihf` |
| ARM 64 bits  | `/usr/aarch64-linux-gnu` |
| MIPS         | `/usr/mips-linux-gnu` |
| RISC-V 64    | `/usr/riscv64-linux-gnu` |

### Sysroot personnalisé

Si vous avez un sysroot personnalisé :

```bash
# Utiliser un sysroot personnalisé
qemu-aarch64 -L /home/user/mon-sysroot ./programme
```

## Options QEMU utiles

### Options de base

```bash
# Afficher l'aide
qemu-arm --help

# Afficher la version
qemu-arm --version

# Spécifier le sysroot
qemu-arm -L /usr/arm-linux-gnueabihf ./programme

# Passer des arguments au programme
qemu-arm -L /usr/arm-linux-gnueabihf ./programme arg1 arg2
```

### Options de debugging

```bash
# Mode verbeux (affiche les appels système)
qemu-arm -strace ./programme

# Afficher les bibliothèques chargées
qemu-arm -d exec ./programme

# CPU logging (très verbeux !)
qemu-arm -d cpu ./programme

# Combiner plusieurs options de debug
qemu-arm -strace -d exec ./programme
```

### Options de performance

```bash
# Spécifier la taille de la pile
qemu-arm -s 8388608 ./programme  # 8 MB de pile

# Définir des variables d'environnement
qemu-arm -E LD_LIBRARY_PATH=/custom/path ./programme
```

## QEMU avec différentes architectures

### ARM 32 bits

```bash
# Compilation
arm-linux-gnueabihf-gcc programme.c -o programme_arm32

# Exécution
qemu-arm -L /usr/arm-linux-gnueabihf ./programme_arm32
```

### ARM 64 bits

```bash
# Compilation
aarch64-linux-gnu-gcc programme.c -o programme_arm64

# Exécution
qemu-aarch64 -L /usr/aarch64-linux-gnu ./programme_arm64
```

### MIPS

```bash
# Installation de la toolchain MIPS
sudo apt-get install gcc-mips-linux-gnu

# Compilation
mips-linux-gnu-gcc programme.c -o programme_mips

# Exécution
qemu-mips -L /usr/mips-linux-gnu ./programme_mips
```

### RISC-V 64 bits

```bash
# Installation de la toolchain RISC-V
sudo apt-get install gcc-riscv64-linux-gnu

# Compilation
riscv64-linux-gnu-gcc programme.c -o programme_riscv64

# Exécution
qemu-riscv64 -L /usr/riscv64-linux-gnu ./programme_riscv64
```

## Programmes avec arguments et entrées/sorties

### Passer des arguments

```c
// programme.c
#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("Nombre d'arguments : %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("  argv[%d] = %s\n", i, argv[i]);
    }
    return 0;
}
```

```bash
# Compiler
arm-linux-gnueabihf-gcc programme.c -o programme

# Exécuter avec arguments
qemu-arm -L /usr/arm-linux-gnueabihf ./programme arg1 arg2 arg3

# Résultat :
# Nombre d'arguments : 4
#   argv[0] = ./programme
#   argv[1] = arg1
#   argv[2] = arg2
#   argv[3] = arg3
```

### Redirection entrée/sortie

```bash
# Redirection de sortie
qemu-arm -L /usr/arm-linux-gnueabihf ./programme > output.txt

# Redirection d'entrée
qemu-arm -L /usr/arm-linux-gnueabihf ./programme < input.txt

# Pipe
echo "test" | qemu-arm -L /usr/arm-linux-gnueabihf ./programme
```

### Variables d'environnement

```bash
# Définir une variable d'environnement
qemu-arm -E MY_VAR=value -L /usr/arm-linux-gnueabihf ./programme

# Ou utiliser export
export MY_VAR=value
qemu-arm -L /usr/arm-linux-gnueabihf ./programme
```

## Gestion des codes de retour

QEMU propage correctement les codes de retour :

```c
// test_return.c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <exit_code>\n", argv[0]);
        return 1;
    }

    int code = atoi(argv[1]);
    printf("Exiting with code: %d\n", code);
    return code;
}
```

```bash
# Compiler
arm-linux-gnueabihf-gcc test_return.c -o test_return

# Tester différents codes de retour
qemu-arm -L /usr/arm-linux-gnueabihf ./test_return 0
echo "Code de retour : $?"  # Affiche : 0

qemu-arm -L /usr/arm-linux-gnueabihf ./test_return 42
echo "Code de retour : $?"  # Affiche : 42

qemu-arm -L /usr/arm-linux-gnueabihf ./test_return 1
echo "Code de retour : $?"  # Affiche : 1
```

## Debugging avec QEMU et GDB

QEMU s'intègre parfaitement avec GDB pour le debugging :

### Étape 1 : Compiler avec symboles de debug

```bash
# Compiler avec -g
arm-linux-gnueabihf-gcc -g -O0 programme.c -o programme
```

### Étape 2 : Lancer QEMU en mode debug

```bash
# Démarrer QEMU avec serveur GDB sur le port 1234
qemu-arm -g 1234 -L /usr/arm-linux-gnueabihf ./programme

# Le programme attend la connexion GDB
```

### Étape 3 : Connecter GDB

Dans un autre terminal :

```bash
# Installer gdb-multiarch (supporte plusieurs architectures)
sudo apt-get install gdb-multiarch

# Démarrer gdb-multiarch
gdb-multiarch ./programme

# Dans GDB, se connecter à QEMU
(gdb) target remote localhost:1234

# Mettre des breakpoints
(gdb) break main
(gdb) break fonction_importante

# Continuer l'exécution
(gdb) continue

# Inspecter des variables
(gdb) print ma_variable

# Step by step
(gdb) step
(gdb) next

# Afficher la pile
(gdb) backtrace
```

### Exemple de session GDB

```bash
# Terminal 1 : Lancer QEMU
qemu-arm -g 1234 -L /usr/arm-linux-gnueabihf ./programme

# Terminal 2 : GDB
gdb-multiarch ./programme
(gdb) target remote localhost:1234
Remote debugging using localhost:1234
(gdb) break main
Breakpoint 1 at 0x10454: file programme.c, line 5.
(gdb) continue
Continuing.

Breakpoint 1, main () at programme.c:5
5           printf("Début du programme\n");
(gdb) list
3       int main(void) {
4           int x = 42;
5           printf("Début du programme\n");
6           printf("x = %d\n", x);
7           return 0;
8       }
(gdb) print x
$1 = 42
(gdb) next
Début du programme
6           printf("x = %d\n", x);
(gdb) continue
Continuing.
x = 42
[Inferior 1 (Remote target) exited normally]
```

## Scripts d'automatisation

### Script simple pour tester un binaire

**test-qemu.sh**

```bash
#!/bin/bash

# Configuration
PROGRAM="$1"
ARCH="$2"

if [ -z "$PROGRAM" ] || [ -z "$ARCH" ]; then
    echo "Usage: $0 <programme> <arch>"
    echo "Architectures supportées : arm32, arm64, mips"
    exit 1
fi

# Définir QEMU et sysroot selon l'architecture
case "$ARCH" in
    arm32)
        QEMU="qemu-arm"
        SYSROOT="/usr/arm-linux-gnueabihf"
        ;;
    arm64)
        QEMU="qemu-aarch64"
        SYSROOT="/usr/aarch64-linux-gnu"
        ;;
    mips)
        QEMU="qemu-mips"
        SYSROOT="/usr/mips-linux-gnu"
        ;;
    *)
        echo "Architecture non supportée : $ARCH"
        exit 1
        ;;
esac

# Vérifier que le programme existe
if [ ! -f "$PROGRAM" ]; then
    echo "Erreur : $PROGRAM introuvable"
    exit 1
fi

# Vérifier que c'est un binaire pour la bonne architecture
echo "=== Vérification du binaire ==="
file "$PROGRAM"

# Exécuter avec QEMU
echo ""
echo "=== Exécution avec QEMU ==="
$QEMU -L "$SYSROOT" "$PROGRAM"

# Capturer le code de retour
EXIT_CODE=$?

echo ""
echo "=== Résultat ==="
if [ $EXIT_CODE -eq 0 ]; then
    echo "✓ Programme exécuté avec succès (code: $EXIT_CODE)"
else
    echo "✗ Échec de l'exécution (code: $EXIT_CODE)"
fi

exit $EXIT_CODE
```

**Utilisation :**

```bash
chmod +x test-qemu.sh

# Tester un binaire ARM 32 bits
./test-qemu.sh ./programme_arm32 arm32

# Tester un binaire ARM 64 bits
./test-qemu.sh ./programme_arm64 arm64
```

### Script de test automatisé

**test-all-archs.sh**

```bash
#!/bin/bash

set -e

# Couleurs
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m'

# Source à compiler
SOURCE="programme.c"

# Architectures à tester
declare -A TOOLCHAINS=(
    ["arm32"]="arm-linux-gnueabihf-gcc"
    ["arm64"]="aarch64-linux-gnu-gcc"
)

declare -A QEMU_CMDS=(
    ["arm32"]="qemu-arm -L /usr/arm-linux-gnueabihf"
    ["arm64"]="qemu-aarch64 -L /usr/aarch64-linux-gnu"
)

echo -e "${BLUE}=== Test multi-architectures avec QEMU ===${NC}"
echo ""

# Compiler et tester chaque architecture
for arch in "${!TOOLCHAINS[@]}"; do
    echo -e "${BLUE}--- Architecture: $arch ---${NC}"

    COMPILER="${TOOLCHAINS[$arch]}"
    QEMU="${QEMU_CMDS[$arch]}"
    BINARY="programme_${arch}"

    # Compilation
    echo "Compilation..."
    if $COMPILER "$SOURCE" -o "$BINARY"; then
        echo -e "${GREEN}✓ Compilation réussie${NC}"
    else
        echo -e "${RED}✗ Échec de la compilation${NC}"
        continue
    fi

    # Vérification
    echo "Type de binaire :"
    file "$BINARY"

    # Exécution avec QEMU
    echo "Exécution avec QEMU..."
    if $QEMU "./$BINARY"; then
        echo -e "${GREEN}✓ Exécution réussie${NC}"
    else
        echo -e "${RED}✗ Échec de l'exécution${NC}"
    fi

    echo ""
done

echo -e "${GREEN}=== Tests terminés ===${NC}"
```

## Intégration avec CMake

CMake peut utiliser QEMU automatiquement pour exécuter les tests :

**CMakeLists.txt**

```cmake
cmake_minimum_required(VERSION 3.10)
project(ProjetAvecTests C)

# Activer les tests
enable_testing()

# Détecter la cross-compilation
if(CMAKE_CROSSCOMPILING)
    message(STATUS "Cross-compilation détectée pour ${CMAKE_SYSTEM_PROCESSOR}")

    # Configurer QEMU comme émulateur
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
        find_program(QEMU_ARM qemu-arm)
        if(QEMU_ARM)
            set(CMAKE_CROSSCOMPILING_EMULATOR
                "${QEMU_ARM};-L;/usr/arm-linux-gnueabihf")
            message(STATUS "QEMU ARM configuré : ${QEMU_ARM}")
        endif()
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
        find_program(QEMU_AARCH64 qemu-aarch64)
        if(QEMU_AARCH64)
            set(CMAKE_CROSSCOMPILING_EMULATOR
                "${QEMU_AARCH64};-L;/usr/aarch64-linux-gnu")
            message(STATUS "QEMU AArch64 configuré : ${QEMU_AARCH64}")
        endif()
    endif()
endif()

# Créer l'exécutable
add_executable(programme main.c)

# Ajouter des tests
add_test(NAME test_basic COMMAND programme)
add_test(NAME test_with_args COMMAND programme arg1 arg2)

# Les tests s'exécuteront automatiquement via QEMU si configuré !
```

**Exécution des tests :**

```bash
# Configuration pour ARM
mkdir build-arm && cd build-arm
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-arm64.cmake ..
make

# Lancer les tests (utilise QEMU automatiquement)
ctest --verbose

# Résultat :
# Test project /path/to/build-arm
#     Start 1: test_basic
# 1/2 Test #1: test_basic .......................   Passed    0.12 sec
#     Start 2: test_with_args
# 2/2 Test #2: test_with_args ...................   Passed    0.13 sec
#
# 100% tests passed, 0 tests failed out of 2
```

## Intégration CI/CD

### GitHub Actions avec QEMU

**.github/workflows/cross-compile-test.yml**

```yaml
name: Cross-compilation et tests QEMU

on: [push, pull_request]

jobs:
  test-arm:
    runs-on: ubuntu-latest

    strategy:
      matrix:
        arch:
          - { name: arm32, compiler: arm-linux-gnueabihf-gcc, qemu: qemu-arm, sysroot: /usr/arm-linux-gnueabihf }
          - { name: arm64, compiler: aarch64-linux-gnu-gcc, qemu: qemu-aarch64, sysroot: /usr/aarch64-linux-gnu }

    steps:
    - uses: actions/checkout@v3

    - name: Installation des dépendances
      run: |
        sudo apt-get update
        sudo apt-get install -y \
          gcc-arm-linux-gnueabihf \
          gcc-aarch64-linux-gnu \
          qemu-user-static

    - name: Compilation
      run: |
        ${{ matrix.arch.compiler }} programme.c -o programme_${{ matrix.arch.name }}
        file programme_${{ matrix.arch.name }}

    - name: Test avec QEMU
      run: |
        ${{ matrix.arch.qemu }} -L ${{ matrix.arch.sysroot }} \
          ./programme_${{ matrix.arch.name }}

    - name: Upload du binaire
      uses: actions/upload-artifact@v3
      with:
        name: programme-${{ matrix.arch.name }}
        path: programme_${{ matrix.arch.name }}
```

### GitLab CI avec QEMU

**.gitlab-ci.yml**

```yaml
image: ubuntu:22.04

stages:
  - build
  - test

before_script:
  - apt-get update -qq
  - apt-get install -y -qq gcc-arm-linux-gnueabihf gcc-aarch64-linux-gnu qemu-user-static

build:arm32:
  stage: build
  script:
    - arm-linux-gnueabihf-gcc programme.c -o programme_arm32
  artifacts:
    paths:
      - programme_arm32

test:arm32:
  stage: test
  dependencies:
    - build:arm32
  script:
    - file programme_arm32
    - qemu-arm -L /usr/arm-linux-gnueabihf ./programme_arm32

build:arm64:
  stage: build
  script:
    - aarch64-linux-gnu-gcc programme.c -o programme_arm64
  artifacts:
    paths:
      - programme_arm64

test:arm64:
  stage: test
  dependencies:
    - build:arm64
  script:
    - file programme_arm64
    - qemu-aarch64 -L /usr/aarch64-linux-gnu ./programme_arm64
```

## Limitations et différences avec le matériel réel

### Ce que QEMU émule bien

- ✅ Instructions CPU standard
- ✅ Appels système Linux
- ✅ Arithmétique et logique
- ✅ Allocation mémoire
- ✅ Threads (pthread)
- ✅ Réseau (sockets)
- ✅ Fichiers et I/O

### Ce que QEMU n'émule pas parfaitement

- ❌ Timings précis (QEMU est plus lent)
- ❌ Périphériques matériels spécifiques (GPIO, I2C, SPI)
- ❌ Instructions CPU très spécialisées
- ❌ Comportement exact du cache
- ❌ Interactions bas niveau avec le hardware
- ❌ Real-time constraints

### Quand QEMU ne suffit pas

Vous devez tester sur du matériel réel pour :

- **Systèmes embarqués** avec interactions matérielles
- **Contraintes temps réel** strictes
- **Drivers** de périphériques
- **Optimisations** de performance finales
- **Problèmes** qui ne se reproduisent que sur le matériel réel

**Stratégie recommandée :**

```
1. Développement initial → PC avec compilation native
2. Tests fonctionnels → QEMU
3. Validation finale → Matériel réel
```

## Performance : QEMU vs natif

### Benchmark simple

```c
// benchmark.c
#include <stdio.h>
#include <time.h>

int main(void) {
    clock_t start = clock();

    // Calcul simple
    long long sum = 0;
    for (long long i = 0; i < 100000000; i++) {
        sum += i;
    }

    clock_t end = clock();
    double duration = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Résultat : %lld\n", sum);
    printf("Temps : %.2f secondes\n", duration);

    return 0;
}
```

**Résultats typiques :**

| Environnement | Temps | Ratio |
|---------------|-------|-------|
| x86_64 natif | 0.08s | 1.0x |
| ARM natif (Raspberry Pi 4) | 0.15s | ~2x |
| QEMU ARM (sur x86_64) | 0.50s | ~6x |

QEMU est **plus lent** que le natif, mais reste suffisant pour les tests fonctionnels.

## Exécution transparente avec binfmt

### Qu'est-ce que binfmt ?

**binfmt_misc** est un mécanisme du kernel Linux qui permet d'exécuter automatiquement des binaires via QEMU, sans avoir à taper `qemu-arm` explicitement.

### Configuration

```bash
# Installer binfmt-support
sudo apt-get install binfmt-support qemu-user-static

# Vérifier la configuration
cat /proc/sys/fs/binfmt_misc/status
# Devrait afficher : enabled

# Lister les formats binaires enregistrés
ls /proc/sys/fs/binfmt_misc/
```

### Utilisation transparente

Une fois binfmt configuré :

```bash
# Compiler pour ARM
arm-linux-gnueabihf-gcc programme.c -o programme

# Exécuter directement !
./programme

# QEMU est appelé automatiquement en arrière-plan
```

**Avantages :**
- ✅ Exécution naturelle, comme un binaire natif
- ✅ Scripts shell fonctionnent normalement
- ✅ Makefiles et outils de build ne nécessitent pas de modifications

### Vérifier qu'un binaire s'exécute via binfmt

```bash
# Compiler un binaire ARM
arm-linux-gnueabihf-gcc hello.c -o hello_arm

# Afficher quel interpréteur sera utilisé
readelf -l hello_arm | grep interpreter
# Affiche : /lib/ld-linux-armhf.so.3

# Exécuter (binfmt invoque QEMU automatiquement)
./hello_arm
```

## Cas d'usage avancés

### Tester un programme avec des bibliothèques externes

```c
// programme_curl.c
#include <stdio.h>
#include <curl/curl.h>

int main(void) {
    CURL *curl = curl_easy_init();
    if (curl) {
        printf("libcurl initialisée avec succès\n");
        curl_easy_cleanup(curl);
        return 0;
    } else {
        fprintf(stderr, "Erreur d'initialisation libcurl\n");
        return 1;
    }
}
```

```bash
# Installer libcurl pour ARM
sudo dpkg --add-architecture armhf
sudo apt-get update
sudo apt-get install libcurl4-openssl-dev:armhf

# Compiler
arm-linux-gnueabihf-gcc programme_curl.c -o programme_curl -lcurl

# Tester avec QEMU
qemu-arm -L /usr/arm-linux-gnueabihf ./programme_curl

# Résultat :
# libcurl initialisée avec succès
```

### Profiling avec QEMU

QEMU peut générer des traces pour analyser les performances :

```bash
# Générer un trace d'exécution
qemu-arm -d exec,cpu,in_asm -D trace.log \
    -L /usr/arm-linux-gnueabihf ./programme

# Le fichier trace.log contient les instructions exécutées
less trace.log
```

### Tester des programmes multi-threadés

```c
// threads.c
#include <stdio.h>
#include <pthread.h>

void* thread_func(void* arg) {
    int id = *(int*)arg;
    printf("Thread %d s'exécute\n", id);
    return NULL;
}

int main(void) {
    pthread_t threads[4];
    int ids[4] = {1, 2, 3, 4};

    for (int i = 0; i < 4; i++) {
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }

    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Tous les threads ont terminé\n");
    return 0;
}
```

```bash
# Compiler avec pthread
arm-linux-gnueabihf-gcc threads.c -o threads -pthread

# Tester avec QEMU
qemu-arm -L /usr/arm-linux-gnueabihf ./threads

# Résultat :
# Thread 1 s'exécute
# Thread 2 s'exécute
# Thread 3 s'exécute
# Thread 4 s'exécute
# Tous les threads ont terminé
```

## Bonnes pratiques

### 1. Toujours vérifier le binaire avant QEMU

```bash
# Vérifier l'architecture
file programme

# Vérifier les dépendances
arm-linux-gnueabihf-readelf -d programme | grep NEEDED
```

### 2. Utiliser un script wrapper

Créez un alias ou un script pour simplifier :

```bash
# ~/.bashrc
alias qemu-arm-run='qemu-arm -L /usr/arm-linux-gnueabihf'
alias qemu-arm64-run='qemu-aarch64 -L /usr/aarch64-linux-gnu'

# Utilisation
qemu-arm-run ./programme
```

### 3. Automatiser les tests

Utilisez QEMU dans vos scripts de test :

```bash
#!/bin/bash
# run-tests.sh

for test in tests/*_test; do
    echo "Running $test..."
    if qemu-arm -L /usr/arm-linux-gnueabihf "$test"; then
        echo "✓ $test passed"
    else
        echo "✗ $test failed"
        exit 1
    fi
done
```

### 4. Documenter le workflow

Dans votre README :

```markdown
## Tests

Pour tester les binaires ARM sans matériel :

```bash
# Installer QEMU
sudo apt-get install qemu-user-static

# Compiler et tester
make arm64
qemu-aarch64 -L /usr/aarch64-linux-gnu ./programme
```
```

### 5. Vérifier la disponibilité de QEMU

Dans vos scripts :

```bash
# Vérifier que QEMU est disponible
if ! command -v qemu-arm &> /dev/null; then
    echo "Erreur : QEMU non installé"
    echo "Installez avec : sudo apt-get install qemu-user-static"
    exit 1
fi
```

## Dépannage

### Erreur : "qemu-arm: Could not open '/lib/ld-linux-armhf.so.3'"

**Cause :** Sysroot incorrect ou manquant

**Solution :**
```bash
# Vérifier le sysroot
ls /usr/arm-linux-gnueabihf/lib/ld-linux-armhf.so.3

# Spécifier le bon chemin
qemu-arm -L /usr/arm-linux-gnueabihf ./programme
```

### Erreur : "Illegal instruction"

**Cause :** Le binaire utilise des instructions non supportées par QEMU

**Solution :**
```bash
# Compiler avec des options moins spécifiques
arm-linux-gnueabihf-gcc -march=armv7-a programme.c -o programme

# Ou utiliser une version plus récente de QEMU
```

### Programme qui bloque

**Cause :** Problème de threads ou de synchronisation

**Solution :**
```bash
# Exécuter avec strace pour débugger
qemu-arm -strace -L /usr/arm-linux-gnueabihf ./programme
```

### Performance très lente

**Cause :** C'est normal, QEMU émule

**Conseils :**
- Compilez en mode Release (`-O2` ou `-O3`)
- Réduisez les itérations dans les tests
- Utilisez des tests unitaires ciblés plutôt que des tests end-to-end lourds

## Résumé des commandes

```bash
# Installation
sudo apt-get install qemu-user qemu-user-static

# Exécution basique
qemu-arm -L /usr/arm-linux-gnueabihf ./programme

# Avec arguments
qemu-arm -L /usr/arm-linux-gnueabihf ./programme arg1 arg2

# Mode debug
qemu-arm -g 1234 -L /usr/arm-linux-gnueabihf ./programme

# Avec strace
qemu-arm -strace -L /usr/arm-linux-gnueabihf ./programme

# Différentes architectures
qemu-arm       # ARM 32 bits
qemu-aarch64   # ARM 64 bits
qemu-mips      # MIPS
qemu-riscv64   # RISC-V 64 bits
```

## Conclusion

QEMU est un outil **indispensable** pour la cross-compilation moderne. Il permet de :

- ✅ **Tester** rapidement des binaires pour différentes architectures
- ✅ **Debugger** avec GDB sans matériel cible
- ✅ **Automatiser** les tests dans les pipelines CI/CD
- ✅ **Développer** confortablement sur votre PC
- ✅ **Valider** les binaires avant le déploiement

**Workflow recommandé :**

```
1. Développement → PC natif
2. Cross-compilation → Toolchain appropriée
3. Tests fonctionnels → QEMU
4. Validation finale → Matériel réel
5. CI/CD → QEMU pour tests automatisés
```

QEMU ne remplace pas complètement le test sur matériel réel, mais il couvre **95% des cas d'usage** et accélère considérablement le cycle de développement.

---


⏭️ [Packaging Linux : .deb](/31-cross-compilation-deploiement/05-packaging-deb.md)
