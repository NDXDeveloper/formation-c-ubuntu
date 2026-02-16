🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 31.2 Compiler pour ARM depuis x86

## Introduction

Dans cette section, nous allons voir comment compiler des programmes C pour l'architecture **ARM** depuis une machine **x86_64** (votre PC Ubuntu typique). C'est l'un des cas de cross-compilation les plus courants, notamment pour développer des applications destinées aux :

- **Raspberry Pi** (toutes versions)
- **Smartphones Android**
- **Serveurs ARM** (AWS Graviton, Ampere Altra)
- **Dispositifs embarqués** (routeurs, caméras IP, NAS)
- **Apple Silicon** (M1/M2/M3 via Linux)

## Comprendre les architectures ARM

### ARM 32 bits vs ARM 64 bits

ARM existe en deux grandes variantes modernes :

| Variante | Nom technique | Utilisation | Exemples |
|----------|--------------|-------------|----------|
| **ARM 32 bits** | `armv7l`, `armhf` | Anciens dispositifs | Raspberry Pi 2/3 (32 bits), routeurs |
| **ARM 64 bits** | `aarch64`, `arm64` | Dispositifs modernes | Raspberry Pi 3+/4, AWS Graviton, smartphones |

### Comment identifier l'architecture cible ?

Sur votre dispositif ARM, exécutez :

```bash
# Affiche l'architecture
uname -m

# Résultats possibles :
# armv7l    -> ARM 32 bits
# aarch64   -> ARM 64 bits
# armv6l    -> ARM 32 bits (Raspberry Pi Zero)
```

### Les ABI (Application Binary Interface)

Pour ARM 32 bits, il existe plusieurs ABI :

- **`armel`** : Soft float (calculs flottants en logiciel) - très lent
- **`armhf`** : Hard float (utilise le coprocesseur FPU) - **recommandé**
- **`gnueabihf`** : GNU ABI avec hard float - **standard actuel**

Pour ARM 64 bits, l'ABI est généralement `gnu`.

## Installation des toolchains ARM sur Ubuntu

### Méthode 1 : Paquets Ubuntu (recommandé pour débuter)

Ubuntu fournit des toolchains ARM prêtes à l'emploi :

#### Pour ARM 32 bits (armhf)

```bash
# Installation de la toolchain ARM 32 bits
sudo apt-get update  
sudo apt-get install gcc-arm-linux-gnueabihf \  
                     g++-arm-linux-gnueabihf \
                     binutils-arm-linux-gnueabihf

# Vérification
arm-linux-gnueabihf-gcc --version
```

#### Pour ARM 64 bits (aarch64)

```bash
# Installation de la toolchain ARM 64 bits
sudo apt-get update  
sudo apt-get install gcc-aarch64-linux-gnu \  
                     g++-aarch64-linux-gnu \
                     binutils-aarch64-linux-gnu

# Vérification
aarch64-linux-gnu-gcc --version
```

#### Installation des deux toolchains

```bash
# Si vous développez pour différentes cibles ARM
sudo apt-get install gcc-arm-linux-gnueabihf \
                     g++-arm-linux-gnueabihf \
                     gcc-aarch64-linux-gnu \
                     g++-aarch64-linux-gnu
```

### Vérification de l'installation

```bash
# Vérifier les compilateurs disponibles
which arm-linux-gnueabihf-gcc  
which aarch64-linux-gnu-gcc  

# Afficher l'architecture cible
arm-linux-gnueabihf-gcc -dumpmachine
# Résultat : arm-linux-gnueabihf

aarch64-linux-gnu-gcc -dumpmachine
# Résultat : aarch64-linux-gnu
```

### Structure de la toolchain

Une fois installée, la toolchain comprend :

```
/usr/bin/
├── arm-linux-gnueabihf-gcc        # Compilateur C
├── arm-linux-gnueabihf-g++        # Compilateur C++
├── arm-linux-gnueabihf-as         # Assembleur
├── arm-linux-gnueabihf-ld         # Éditeur de liens
├── arm-linux-gnueabihf-ar         # Archiveur (bibliothèques statiques)
├── arm-linux-gnueabihf-objdump    # Inspection de binaires
├── arm-linux-gnueabihf-strip      # Suppression des symboles
└── ... (autres outils)

/usr/arm-linux-gnueabihf/
├── include/    # Headers système pour ARM
└── lib/        # Bibliothèques système pour ARM
```

## Premier programme : Hello World

### Étape 1 : Créer le code source

Créez un fichier `hello.c` :

```c
#include <stdio.h>

int main(void) {
    printf("Hello from ARM!\n");
    printf("Running on %s architecture\n",
#ifdef __aarch64__
        "ARM 64 bits (aarch64)"
#elif __arm__
        "ARM 32 bits (armv7)"
#else
        "Unknown"
#endif
    );
    return 0;
}
```

### Étape 2 : Compilation pour ARM 32 bits

```bash
# Compilation
arm-linux-gnueabihf-gcc hello.c -o hello_arm32

# Vérification du binaire
file hello_arm32
```

**Résultat attendu :**
```
hello_arm32: ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV),  
dynamically linked, interpreter /lib/ld-linux-armhf.so.3,  
for GNU/Linux 3.2.0, not stripped  
```

### Étape 3 : Compilation pour ARM 64 bits

```bash
# Compilation
aarch64-linux-gnu-gcc hello.c -o hello_arm64

# Vérification du binaire
file hello_arm64
```

**Résultat attendu :**
```
hello_arm64: ELF 64-bit LSB executable, ARM aarch64, version 1 (SYSV),  
dynamically linked, interpreter /lib/ld-linux-aarch64.so.1,  
for GNU/Linux 3.7.0, not stripped  
```

### Étape 4 : Tentative d'exécution (échouera)

```bash
# Essayer de lancer sur votre machine x86_64
./hello_arm32

# Erreur attendue :
# bash: ./hello_arm32: cannot execute binary file: Exec format error
```

C'est normal ! Le binaire est en code ARM et ne peut pas s'exécuter sur x86_64.

## Options de compilation importantes

### Options de base

```bash
# Compilation simple
arm-linux-gnueabihf-gcc hello.c -o hello

# Avec optimisation
arm-linux-gnueabihf-gcc -O2 hello.c -o hello

# Avec informations de debug
arm-linux-gnueabihf-gcc -g hello.c -o hello

# Mode strict (warnings et erreurs)
arm-linux-gnueabihf-gcc -Wall -Werror hello.c -o hello
```

### Options spécifiques ARM

#### Cibler une version ARM précise

```bash
# Pour Raspberry Pi 3 (Cortex-A53)
arm-linux-gnueabihf-gcc -mcpu=cortex-a53 hello.c -o hello

# Pour Raspberry Pi 4 (Cortex-A72)
arm-linux-gnueabihf-gcc -mcpu=cortex-a72 hello.c -o hello

# Architecture ARMv7
arm-linux-gnueabihf-gcc -march=armv7-a hello.c -o hello

# Architecture ARMv8 (64 bits)
aarch64-linux-gnu-gcc -march=armv8-a hello.c -o hello
```

#### Options d'optimisation ARM

```bash
# Optimiser pour la taille (important pour l'embarqué)
arm-linux-gnueabihf-gcc -Os hello.c -o hello

# Utiliser le FPU (calculs flottants matériels)
arm-linux-gnueabihf-gcc -mfpu=neon-vfpv4 -mfloat-abi=hard hello.c -o hello

# Optimisation pour ARM Cortex-A avec NEON
arm-linux-gnueabihf-gcc -mcpu=cortex-a7 -mfpu=neon-vfpv4 -O3 hello.c -o hello
```

### Options de linkage

```bash
# Linkage statique (inclut toutes les bibliothèques)
arm-linux-gnueabihf-gcc -static hello.c -o hello

# Linkage dynamique (par défaut)
arm-linux-gnueabihf-gcc hello.c -o hello

# Spécifier des bibliothèques
arm-linux-gnueabihf-gcc hello.c -o hello -lm -lpthread
```

## Compilation de projets multi-fichiers

### Structure d'un projet

```
projet/
├── src/
│   ├── main.c
│   ├── utils.c
│   └── calcul.c
├── include/
│   ├── utils.h
│   └── calcul.h
└── Makefile
```

### Exemple de fichiers

**include/utils.h**
```c
#ifndef UTILS_H
#define UTILS_H

void afficher_architecture(void);

#endif
```

**src/utils.c**
```c
#include <stdio.h>
#include "utils.h"

void afficher_architecture(void) {
#ifdef __aarch64__
    printf("Architecture: ARM 64 bits\n");
#elif __arm__
    printf("Architecture: ARM 32 bits\n");
#else
    printf("Architecture: Non-ARM\n");
#endif
}
```

**src/main.c**
```c
#include <stdio.h>
#include "utils.h"

int main(void) {
    printf("=== Programme cross-compilé ===\n");
    afficher_architecture();
    return 0;
}
```

### Compilation manuelle

```bash
# Compilation des fichiers objets
arm-linux-gnueabihf-gcc -c -Iinclude src/utils.c -o utils.o  
arm-linux-gnueabihf-gcc -c -Iinclude src/main.c -o main.o  

# Linkage
arm-linux-gnueabihf-gcc utils.o main.o -o programme

# Vérification
file programme
```

### Makefile pour cross-compilation

**Makefile**
```makefile
# Choix de la toolchain
CROSS_COMPILE ?= arm-linux-gnueabihf-  
CC = $(CROSS_COMPILE)gcc  

# Options de compilation
CFLAGS = -Wall -Werror -O2 -Iinclude  
LDFLAGS =  

# Fichiers sources
SRCS = src/main.c src/utils.c  
OBJS = $(SRCS:.c=.o)  

# Cible principale
TARGET = programme

# Règle par défaut
all: $(TARGET)

# Compilation du programme
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)

# Compilation des fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -f $(OBJS) $(TARGET)

# Compilation pour différentes architectures
arm32:
	$(MAKE) CROSS_COMPILE=arm-linux-gnueabihf-

arm64:
	$(MAKE) CROSS_COMPILE=aarch64-linux-gnu-

.PHONY: all clean arm32 arm64
```

**Utilisation du Makefile :**

```bash
# Compiler pour ARM 32 bits
make arm32

# Compiler pour ARM 64 bits
make arm64

# Nettoyage
make clean
```

## Gestion des dépendances

### Problème : Bibliothèques externes

Supposons que votre programme utilise `libcurl` :

```c
#include <curl/curl.h>

int main(void) {
    CURL *curl = curl_easy_init();
    // ...
    return 0;
}
```

Sur votre machine x86_64, vous feriez :
```bash
gcc programme.c -lcurl -o programme
```

Mais en cross-compilation, vous devez avoir **libcurl compilée pour ARM**.

### Solution 1 : Installer les bibliothèques ARM

Ubuntu fournit des versions ARM des bibliothèques courantes :

```bash
# Installer libcurl pour ARM 32 bits
sudo apt-get install libcurl4-openssl-dev:armhf

# Installer libcurl pour ARM 64 bits
sudo apt-get install libcurl4-openssl-dev:arm64
```

**Note :** Cela nécessite d'activer le multiarch sur Ubuntu :

```bash
# Ajouter l'architecture ARM
sudo dpkg --add-architecture armhf  
sudo apt-get update  
```

### Solution 2 : Utiliser un sysroot

Le **sysroot** est une copie du système de fichiers de votre cible :

```bash
# Structure typique d'un sysroot
sysroot/
├── lib/
│   └── arm-linux-gnueabihf/
│       ├── libc.so.6
│       ├── libcurl.so.4
│       └── ...
├── usr/
│   ├── include/
│   │   ├── stdio.h
│   │   ├── curl/
│   │   └── ...
│   └── lib/
│       └── arm-linux-gnueabihf/
└── etc/
```

**Compilation avec sysroot :**

```bash
arm-linux-gnueabihf-gcc \
    --sysroot=/path/to/sysroot \
    programme.c -lcurl -o programme
```

### Solution 3 : Compiler les dépendances

Pour des projets complexes, vous devrez peut-être compiler les bibliothèques vous-même pour ARM.

## Cas pratique : Raspberry Pi

### Identifier votre Raspberry Pi

| Modèle | Architecture | Toolchain |
|--------|--------------|-----------|
| Pi Zero, Pi 1 | ARMv6 (32 bits) | Toolchain spécifique |
| Pi 2, Pi 3 (OS 32 bits) | ARMv7 (32 bits) | `arm-linux-gnueabihf-` |
| Pi 3 (OS 64 bits), Pi 4, Pi 5 | ARMv8 (64 bits) | `aarch64-linux-gnu-` |

### Workflow complet pour Raspberry Pi

#### 1. Sur votre PC Ubuntu (compilation)

```bash
# Compiler pour Raspberry Pi 4 (64 bits)
aarch64-linux-gnu-gcc \
    -Wall -O2 \
    -march=armv8-a \
    mon_programme.c -o mon_programme

# Vérifier le binaire
file mon_programme
# ELF 64-bit LSB executable, ARM aarch64...
```

#### 2. Transférer sur le Raspberry Pi

```bash
# Transfert via SCP
scp mon_programme pi@192.168.1.100:/home/pi/

# Ou via rsync
rsync -avz mon_programme pi@192.168.1.100:/home/pi/
```

#### 3. Sur le Raspberry Pi (exécution)

```bash
# Se connecter en SSH
ssh pi@192.168.1.100

# Rendre exécutable (si nécessaire)
chmod +x mon_programme

# Lancer
./mon_programme
```

### Script d'automatisation

**deploy.sh**
```bash
#!/bin/bash

# Configuration
TARGET_HOST="pi@192.168.1.100"  
TARGET_DIR="/home/pi/bin"  
PROGRAM="mon_programme"  
SOURCE="mon_programme.c"  

# Compilation
echo "Compilation pour ARM64..."  
aarch64-linux-gnu-gcc -O2 -Wall $SOURCE -o $PROGRAM  

if [ $? -ne 0 ]; then
    echo "Erreur de compilation!"
    exit 1
fi

# Vérification
echo "Vérification du binaire..."  
file $PROGRAM  

# Transfert
echo "Transfert vers le Raspberry Pi..."  
scp $PROGRAM $TARGET_HOST:$TARGET_DIR/  

if [ $? -eq 0 ]; then
    echo "Déploiement réussi!"
    echo "Pour exécuter : ssh $TARGET_HOST '$TARGET_DIR/$PROGRAM'"
else
    echo "Erreur de transfert!"
    exit 1
fi
```

**Utilisation :**
```bash
chmod +x deploy.sh
./deploy.sh
```

## Debugging à distance

### Configuration du debugging

**Sur le PC (compilation avec symboles) :**

```bash
# Compiler avec informations de debug
aarch64-linux-gnu-gcc -g -O0 programme.c -o programme

# Transférer
scp programme pi@192.168.1.100:/home/pi/
```

**Sur le Raspberry Pi (lancer gdbserver) :**

```bash
# Installer gdbserver si nécessaire
sudo apt-get install gdbserver

# Lancer le programme sous gdbserver
gdbserver :1234 ./programme
```

**Sur le PC (se connecter avec gdb) :**

```bash
# Installer gdb multiarch
sudo apt-get install gdb-multiarch

# Lancer gdb
gdb-multiarch programme

# Dans gdb, se connecter au Raspberry Pi
(gdb) target remote 192.168.1.100:1234
(gdb) break main
(gdb) continue
```

## Optimisation pour ARM

### Taille du binaire

Pour les systèmes embarqués avec peu de mémoire :

```bash
# Optimiser pour la taille
arm-linux-gnueabihf-gcc -Os programme.c -o programme

# Supprimer les symboles de debug
arm-linux-gnueabihf-strip programme

# Comparer les tailles
ls -lh programme
```

### Performance

```bash
# Optimisation maximale
arm-linux-gnueabihf-gcc -O3 -march=armv7-a -mfpu=neon programme.c -o programme

# Avec optimisations spécifiques ARM
arm-linux-gnueabihf-gcc \
    -O3 \
    -mcpu=cortex-a53 \
    -mfpu=neon-vfpv4 \
    -mfloat-abi=hard \
    programme.c -o programme
```

## Vérification et analyse des binaires

### Commande `file`

```bash
# Informations de base
file programme

# Résultat pour ARM 32 bits :
# ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV)

# Résultat pour ARM 64 bits :
# ELF 64-bit LSB executable, ARM aarch64, version 1 (SYSV)
```

### Commande `readelf`

```bash
# En-tête ELF
arm-linux-gnueabihf-readelf -h programme

# Sections
arm-linux-gnueabihf-readelf -S programme

# Symboles
arm-linux-gnueabihf-readelf -s programme

# Dépendances dynamiques
arm-linux-gnueabihf-readelf -d programme
```

### Commande `objdump`

```bash
# Désassemblage
arm-linux-gnueabihf-objdump -d programme

# Sections
arm-linux-gnueabihf-objdump -h programme

# Symboles
arm-linux-gnueabihf-objdump -t programme
```

### Vérifier les dépendances

```bash
# Lister les bibliothèques dynamiques nécessaires
arm-linux-gnueabihf-readelf -d programme | grep NEEDED

# Résultat typique :
#  0x00000001 (NEEDED)    Shared library: [libc.so.6]
```

## Tester sans matériel : QEMU

### Installation de QEMU

```bash
# Installer QEMU pour ARM
sudo apt-get install qemu-user qemu-user-static

# Vérifier l'installation
qemu-arm --version  
qemu-aarch64 --version  
```

### Exécuter un binaire ARM sur x86_64

```bash
# Exécuter un binaire ARM 32 bits
qemu-arm -L /usr/arm-linux-gnueabihf ./hello_arm32

# Exécuter un binaire ARM 64 bits
qemu-aarch64 -L /usr/aarch64-linux-gnu ./hello_arm64
```

**Explication de l'option `-L` :**
- Elle spécifie le chemin du sysroot
- Contient les bibliothèques système nécessaires
- Obligatoire pour les binaires dynamiquement liés

### QEMU avec binfmt

Pour une utilisation transparente :

```bash
# Installer binfmt-support
sudo apt-get install binfmt-support

# Après configuration, exécution directe
./hello_arm32
# QEMU est appelé automatiquement en arrière-plan
```

## Erreurs courantes et solutions

### Erreur : "cannot find -lXXX"

**Problème :** Bibliothèque manquante pour l'architecture ARM

**Solution :**
```bash
# Installer la version ARM de la bibliothèque
sudo apt-get install libXXX-dev:armhf

# Ou spécifier le chemin manuellement
arm-linux-gnueabihf-gcc programme.c -L/path/to/libs -lXXX -o programme
```

### Erreur : "cannot execute binary file"

**Problème :** Tentative d'exécution d'un binaire ARM sur x86_64

**Solution :**
- Utiliser QEMU pour tester : `qemu-arm -L /usr/arm-linux-gnueabihf ./programme`
- Ou transférer sur du matériel ARM réel

### Erreur : Architecture incompatible

**Problème :** Binaire ARM 64 bits sur système ARM 32 bits

**Solution :**
- Compiler avec la bonne toolchain (`arm-linux-gnueabihf-gcc` pour 32 bits)
- Vérifier l'architecture de la cible avec `uname -m`

### Warning : "implicit declaration of function"

**Problème :** Header manquant ou chemin d'inclusion incorrect

**Solution :**
```bash
# Spécifier le chemin des headers
arm-linux-gnueabihf-gcc -I/usr/arm-linux-gnueabihf/include programme.c -o programme
```

## Bonnes pratiques

### 1. Toujours vérifier l'architecture cible

```bash
# Sur la machine cible
uname -m

# Adapter la toolchain en conséquence
```

### 2. Utiliser un Makefile avec variable CROSS_COMPILE

```makefile
CROSS_COMPILE ?= arm-linux-gnueabihf-  
CC = $(CROSS_COMPILE)gcc  
```

Permet de changer facilement :
```bash
make CROSS_COMPILE=aarch64-linux-gnu-
```

### 3. Tester avec QEMU avant le déploiement

```bash
# Compilation
arm-linux-gnueabihf-gcc programme.c -o programme

# Test en émulation
qemu-arm -L /usr/arm-linux-gnueabihf ./programme

# Déploiement seulement si OK
scp programme target:/path/
```

### 4. Documenter la cible

Dans votre README :
```markdown
## Cross-compilation

### Cible
- Architecture: ARM 64 bits (aarch64)
- OS: Debian 11 (Bullseye)
- Hardware: Raspberry Pi 4

### Compilation
```bash
make arm64
```
```

### 5. Automatiser avec des scripts

Créer des scripts pour :
- Compilation
- Tests QEMU
- Déploiement
- Tout le pipeline

## Récapitulatif

| Aspect | ARM 32 bits | ARM 64 bits |
|--------|-------------|-------------|
| **Toolchain** | `arm-linux-gnueabihf-gcc` | `aarch64-linux-gnu-gcc` |
| **Architecture** | `armv7l`, `armhf` | `aarch64`, `arm64` |
| **Installation** | `apt install gcc-arm-linux-gnueabihf` | `apt install gcc-aarch64-linux-gnu` |
| **QEMU** | `qemu-arm` | `qemu-aarch64` |
| **Exemple cible** | Raspberry Pi 2/3 (32 bits) | Raspberry Pi 4 (64 bits) |
| **Sysroot** | `/usr/arm-linux-gnueabihf/` | `/usr/aarch64-linux-gnu/` |

## Commandes de référence rapide

```bash
# Installation des toolchains
sudo apt-get install gcc-arm-linux-gnueabihf gcc-aarch64-linux-gnu

# Compilation ARM 32 bits
arm-linux-gnueabihf-gcc programme.c -o programme

# Compilation ARM 64 bits
aarch64-linux-gnu-gcc programme.c -o programme

# Vérification
file programme

# Test avec QEMU
qemu-arm -L /usr/arm-linux-gnueabihf ./programme_arm32  
qemu-aarch64 -L /usr/aarch64-linux-gnu ./programme_arm64  

# Transfert SSH
scp programme user@target:/path/

# Déploiement complet
make clean && make arm64 && scp programme pi@raspberry:/home/pi/
```

## Conclusion

Compiler pour ARM depuis x86_64 est devenu très accessible grâce aux toolchains Ubuntu. Les points essentiels :

1. **Choisir la bonne toolchain** : `arm-linux-gnueabihf-` (32 bits) ou `aarch64-linux-gnu-` (64 bits)
2. **Installer les dépendances ARM** si nécessaire
3. **Tester avec QEMU** avant le déploiement
4. **Automatiser** le processus de build et déploiement

La cross-compilation ARM est indispensable pour :
- ✅ Développer confortablement sur votre PC
- ✅ Compiler rapidement (PC plus puissant que Raspberry Pi)
- ✅ Intégrer dans un pipeline CI/CD
- ✅ Cibler plusieurs architectures facilement

---


⏭️ [CMake et cross-compilation](/31-cross-compilation-deploiement/03-cmake-cross-compilation.md)
