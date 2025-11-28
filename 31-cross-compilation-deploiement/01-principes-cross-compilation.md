🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 31.1 Cross-compilation : principes

## Introduction

La **cross-compilation** (compilation croisée) est le processus de compilation d'un programme sur une machine (l'**hôte**) pour qu'il s'exécute sur une autre machine ayant une architecture différente (la **cible**). C'est une technique fondamentale dans le développement moderne, particulièrement pour les systèmes embarqués, les dispositifs IoT, et les environnements cloud.

### Pourquoi la cross-compilation ?

Imaginons quelques scénarios concrets où la cross-compilation est indispensable :

1. **Développement embarqué** : Vous développez un programme pour un Raspberry Pi (architecture ARM) depuis votre PC (architecture x86_64). Le Raspberry Pi est souvent trop lent pour compiler de gros projets.

2. **IoT et microcontrôleurs** : Beaucoup de dispositifs IoT n'ont ni la puissance ni l'espace disque pour héberger un compilateur.

3. **Cloud et conteneurs** : Vous voulez créer des images Docker ARM pour les instances AWS Graviton depuis votre machine x86.

4. **Performance** : Compiler sur une machine puissante (serveur x86) pour déployer sur des machines moins performantes.

## Concepts fondamentaux

### Les trois architectures à connaître

Lorsqu'on parle de cross-compilation, trois termes reviennent constamment :

#### 1. **Build** (machine de compilation)
- C'est la machine sur laquelle vous **compilez** le code
- Exemple : Votre PC Ubuntu x86_64

#### 2. **Host** (machine hôte)
- C'est la machine sur laquelle le compilateur **s'exécutera**
- Dans la plupart des cas simples, Build = Host
- Devient différent dans des cas complexes (compilation d'un compilateur pour une autre plateforme)

#### 3. **Target** (machine cible)
- C'est la machine sur laquelle le **programme compilé s'exécutera**
- Exemple : Un Raspberry Pi ARM, un routeur MIPS, etc.

### Exemple simple

```
Build:  x86_64 (votre PC)      ──┐
Host:   x86_64 (votre PC)        ├──> Vous compilez
Target: ARM (Raspberry Pi)     ──┘    Le binaire ARM sera exécuté sur le Raspberry
```

## Les architectures courantes

### Principales architectures processeur

| Architecture | Description | Utilisation typique |
|--------------|-------------|---------------------|
| **x86_64** (amd64) | Architecture 64 bits d'Intel/AMD | PC, serveurs, cloud |
| **x86** (i386, i686) | Architecture 32 bits d'Intel/AMD | Systèmes legacy |
| **ARM** (armv7l) | Architecture 32 bits ARM | Raspberry Pi 2/3 (32 bits), smartphones |
| **ARM64** (aarch64) | Architecture 64 bits ARM | Raspberry Pi 3+/4, smartphones modernes, AWS Graviton |
| **MIPS** | Architecture MIPS | Routeurs, systèmes embarqués |
| **RISC-V** | Architecture open-source émergente | Systèmes embarqués, recherche |

### Comment connaître son architecture ?

Sur votre machine Ubuntu :

```bash
# Architecture du processeur
uname -m
# Résultat typique : x86_64, aarch64, armv7l...

# Informations détaillées
lscpu
```

## Le principe de fonctionnement

### Compilation native vs cross-compilation

#### Compilation native (ce que vous faites habituellement)

```
┌─────────────────────────────────────┐
│  Machine x86_64                     │
│                                     │
│  gcc fichier.c -o programme         │
│   │                    │            │
│   └──> Compilation ────┘            │
│                                     │
│  ./programme  ✓ Fonctionne !        │
└─────────────────────────────────────┘
```

Le compilateur `gcc` génère du code machine pour **l'architecture sur laquelle il s'exécute**.

#### Cross-compilation

```
┌─────────────────────────────────────┐
│  Machine x86_64 (Build)             │
│                                     │
│  arm-linux-gnueabihf-gcc fichier.c  │
│   │                                 │
│   └──> Génère du code ARM           │
│                                     │
│  ./programme  ✗ Ne fonctionne pas ! │
│  (code ARM sur machine x86)         │
└─────────────────────────────────────┘
           │
           │ Transfert du binaire
           ▼
┌─────────────────────────────────────┐
│  Raspberry Pi ARM (Target)          │
│                                     │
│  ./programme  ✓ Fonctionne !        │
└─────────────────────────────────────┘
```

### Les outils nécessaires

Pour faire de la cross-compilation, vous avez besoin d'une **toolchain** (chaîne d'outils) adaptée à l'architecture cible :

#### 1. Le compilateur croisé
- Exemple : `arm-linux-gnueabihf-gcc` (pour ARM 32 bits)
- Exemple : `aarch64-linux-gnu-gcc` (pour ARM 64 bits)
- Exemple : `mips-linux-gnu-gcc` (pour MIPS)

#### 2. Les bibliothèques système de la cible
- libc (bibliothèque C standard) pour l'architecture cible
- Autres bibliothèques système (pthread, math, etc.)

#### 3. L'éditeur de liens (linker)
- Lie le code compilé avec les bibliothèques de la cible

#### 4. Les outils binaires
- `as` (assembleur), `ld` (linker), `strip`, `objdump`, etc.

### Nomenclature des toolchains

Les noms des toolchains suivent généralement ce format :

```
<architecture>-<vendor>-<os>-<abi>-<outil>
```

**Exemples concrets :**

- `arm-linux-gnueabihf-gcc`
  - `arm` : Architecture cible
  - `linux` : Système d'exploitation
  - `gnueabihf` : ABI GNU avec support hardware floating point
  - `gcc` : L'outil (compilateur)

- `aarch64-linux-gnu-gcc`
  - `aarch64` : ARM 64 bits
  - `linux` : Système Linux
  - `gnu` : ABI GNU
  - `gcc` : Compilateur

- `x86_64-w64-mingw32-gcc`
  - `x86_64` : Architecture 64 bits
  - `w64` : Windows 64 bits
  - `mingw32` : Environnement MinGW
  - `gcc` : Compilateur

## Les défis de la cross-compilation

### 1. Les différences d'architecture

#### Endianness (ordre des octets)
- **Little-endian** : x86, x86_64, ARM (généralement)
- **Big-endian** : Certains MIPS, PowerPC

```c
// Sur une machine little-endian
uint32_t value = 0x12345678;
// En mémoire : 78 56 34 12

// Sur une machine big-endian
// En mémoire : 12 34 56 78
```

#### Taille des types
- `int` peut être 32 bits ou 16 bits selon l'architecture
- `long` est 32 bits sur ARM 32 bits, 64 bits sur x86_64
- Les pointeurs changent de taille (32 bits vs 64 bits)

**Bonne pratique :** Utilisez les types à taille fixe de `<stdint.h>` :

```c
#include <stdint.h>

int32_t  entier_32bits;  // Toujours 32 bits
uint64_t entier_64bits;  // Toujours 64 bits
```

### 2. Les dépendances

Votre programme a souvent besoin de bibliothèques externes. Lors de la cross-compilation, vous devez :

1. Avoir les bibliothèques **compilées pour l'architecture cible**
2. Indiquer au compilateur où trouver ces bibliothèques
3. Gérer les versions compatibles

### 3. Le sysroot

Le **sysroot** est un répertoire qui contient l'arborescence système de la cible :

```
sysroot/
├── lib/          # Bibliothèques de la cible
├── usr/
│   ├── include/  # Headers de la cible
│   └── lib/      # Bibliothèques supplémentaires
└── etc/
```

Vous indiquez au compilateur d'utiliser ce sysroot avec l'option `--sysroot` :

```bash
arm-linux-gnueabihf-gcc \
    --sysroot=/path/to/sysroot \
    fichier.c -o programme
```

## Workflow typique de cross-compilation

Voici les étapes classiques pour cross-compiler un projet :

### Étape 1 : Installation de la toolchain

```bash
# Sur Ubuntu, installation de la toolchain ARM
sudo apt-get install gcc-arm-linux-gnueabihf \
                     g++-arm-linux-gnueabihf \
                     binutils-arm-linux-gnueabihf
```

### Étape 2 : Vérification

```bash
# Vérifier que le compilateur est disponible
arm-linux-gnueabihf-gcc --version

# Vérifier l'architecture cible
arm-linux-gnueabihf-gcc -dumpmachine
# Affiche : arm-linux-gnueabihf
```

### Étape 3 : Compilation

```bash
# Compilation simple
arm-linux-gnueabihf-gcc hello.c -o hello_arm

# Vérification du binaire
file hello_arm
# Affiche : ELF 32-bit LSB executable, ARM, version 1 (SYSV)...
```

### Étape 4 : Transfert et exécution

```bash
# Copie sur le Raspberry Pi via SSH
scp hello_arm pi@192.168.1.100:/home/pi/

# Connexion et exécution
ssh pi@192.168.1.100
./hello_arm  # Fonctionne sur le Raspberry Pi !
```

## Les variables d'environnement importantes

Lors de la cross-compilation, plusieurs variables d'environnement sont utiles :

### CC et CXX
Définissent le compilateur C et C++ à utiliser :

```bash
export CC=arm-linux-gnueabihf-gcc
export CXX=arm-linux-gnueabihf-g++
```

### CROSS_COMPILE
Préfixe pour tous les outils de la toolchain :

```bash
export CROSS_COMPILE=arm-linux-gnueabihf-
# Les outils deviennent automatiquement :
# ${CROSS_COMPILE}gcc -> arm-linux-gnueabihf-gcc
# ${CROSS_COMPILE}ld  -> arm-linux-gnueabihf-ld
```

### ARCH
Architecture cible (utilisée notamment par le kernel Linux) :

```bash
export ARCH=arm
```

## Cas d'usage concrets

### 1. Compilation pour Raspberry Pi (ARM 32 bits)

```bash
# Installation de la toolchain
sudo apt-get install gcc-arm-linux-gnueabihf

# Compilation
arm-linux-gnueabihf-gcc -o mon_programme mon_programme.c

# Vérification
file mon_programme
# ELF 32-bit LSB executable, ARM...
```

### 2. Compilation pour Raspberry Pi 4 (ARM 64 bits)

```bash
# Installation de la toolchain
sudo apt-get install gcc-aarch64-linux-gnu

# Compilation
aarch64-linux-gnu-gcc -o mon_programme mon_programme.c

# Vérification
file mon_programme
# ELF 64-bit LSB executable, ARM aarch64...
```

### 3. Compilation pour Windows depuis Linux

```bash
# Installation de MinGW
sudo apt-get install gcc-mingw-w64

# Compilation pour Windows 64 bits
x86_64-w64-mingw32-gcc -o mon_programme.exe mon_programme.c

# Vérification
file mon_programme.exe
# PE32+ executable (console) x86-64, for MS Windows
```

## Différences avec l'émulation

Il est important de ne pas confondre cross-compilation et émulation :

### Cross-compilation
- Génère du code natif pour l'architecture cible
- Rapide : pas d'overhead d'exécution
- Le binaire ne peut **pas** s'exécuter sur la machine de build

### Émulation (QEMU)
- Permet d'**exécuter** des binaires d'une autre architecture
- Plus lent : émulation des instructions
- Utile pour **tester** les binaires cross-compilés

**Workflow complet :**

```bash
# 1. Cross-compilation
arm-linux-gnueabihf-gcc hello.c -o hello_arm

# 2. Test avec QEMU (émulation)
qemu-arm -L /usr/arm-linux-gnueabihf hello_arm

# 3. Déploiement sur matériel réel
scp hello_arm pi@raspberry:/home/pi/
```

## Avantages et inconvénients

### Avantages

✅ **Performance de compilation** : Compiler sur une machine puissante est plus rapide

✅ **Possibilité de compiler pour des cibles sans compilateur** : Microcontrôleurs, systèmes embarqués

✅ **Automatisation** : Intégration facile dans des pipelines CI/CD

✅ **Développement confortable** : Utiliser son environnement habituel

### Inconvénients

❌ **Complexité initiale** : Configuration de la toolchain et du sysroot

❌ **Dépendances** : Difficile de gérer les bibliothèques externes

❌ **Tests** : Le binaire ne peut pas être exécuté directement sur la machine de build

❌ **Debugging** : Plus complexe que le debugging natif

## Bonnes pratiques

### 1. Utiliser des outils de build modernes

**CMake** gère très bien la cross-compilation (voir section 31.3) :

```cmake
# Fichier toolchain.cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
```

### 2. Tester avec émulation

Utilisez QEMU pour valider les binaires avant le déploiement :

```bash
qemu-arm -L /usr/arm-linux-gnueabihf ./mon_programme_arm
```

### 3. Versionner la toolchain

Dans un contexte DevOps, documenter et versionner :
- La version de la toolchain
- La version du sysroot
- Les dépendances nécessaires

### 4. Automatiser dans le CI/CD

Intégrer la cross-compilation dans GitHub Actions, GitLab CI, etc. pour garantir la reproductibilité.

### 5. Écrire du code portable

```c
// ❌ Mauvais : suppose une taille spécifique
int* ptr = (int*)malloc(sizeof(int));

// ✅ Bon : portable
int* ptr = malloc(sizeof(*ptr));

// ✅ Bon : types à taille fixe
uint32_t value = 0x12345678;
```

## Résumé

La **cross-compilation** est une technique essentielle pour :
- Développer des applications embarquées
- Créer des binaires pour différentes architectures
- Optimiser les temps de compilation
- Intégrer dans des workflows DevOps modernes

**Points clés à retenir :**

1. **Build** (où vous compilez) ≠ **Target** (où le programme s'exécute)
2. Vous avez besoin d'une **toolchain** spécifique à l'architecture cible
3. Les toolchains ont des noms comme `arm-linux-gnueabihf-gcc`
4. Un **sysroot** contient les bibliothèques système de la cible
5. Les binaires cross-compilés ne peuvent pas s'exécuter sur la machine de build
6. QEMU permet de tester les binaires via émulation

Dans les prochaines sections, nous verrons :
- Comment configurer CMake pour la cross-compilation
- Comment utiliser QEMU pour tester
- Comment créer des packages pour différentes architectures

---


⏭️ [Compiler pour ARM depuis x86](/31-cross-compilation-deploiement/02-compiler-arm.md)
