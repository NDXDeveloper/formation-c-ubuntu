🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 2.1 Installation de la toolchain

## Introduction

Avant de pouvoir écrire et exécuter des programmes en C sur Ubuntu, nous devons installer ce qu'on appelle une **toolchain** (chaîne d'outils). Mais qu'est-ce que c'est exactement ?

### Qu'est-ce qu'une toolchain ?

Une toolchain est un ensemble d'outils logiciels qui travaillent ensemble pour transformer votre code source (le code que vous écrivez) en un programme exécutable (que l'ordinateur peut lancer). C'est comme une chaîne de production dans une usine : chaque outil a un rôle spécifique.

**Analogie simple :** Imaginez que vous voulez construire une maison. Vous avez besoin de plusieurs outils : une scie pour couper le bois, un marteau pour planter les clous, un niveau pour vérifier l'alignement, etc. En programmation C, c'est pareil : vous avez besoin de plusieurs outils qui travaillent ensemble.

### Les composants essentiels de la toolchain C

Notre toolchain pour le C sous Ubuntu comprend principalement :

1. **GCC (GNU Compiler Collection)** : Le compilateur qui traduit votre code C en langage machine
2. **Make** : Un outil pour automatiser la compilation de projets complexes
3. **GDB (GNU Debugger)** : Un débogueur pour traquer et corriger les erreurs
4. **Bibliothèques de développement** : Des collections de code pré-écrit que vous pourrez utiliser

---

## Installation sur Ubuntu

Ubuntu facilite grandement l'installation de tous ces outils grâce au paquet **build-essential**. Ce paquet contient tout ce dont vous avez besoin pour commencer à développer en C.

### Étape 1 : Mettre à jour les listes de paquets

Avant toute installation, il est recommandé de mettre à jour la liste des paquets disponibles. Ouvrez un terminal et exécutez :

```bash
sudo apt update
```

**Que fait cette commande ?**
- `sudo` : Exécute la commande avec les privilèges administrateur (vous devrez entrer votre mot de passe)
- `apt` : Le gestionnaire de paquets d'Ubuntu
- `update` : Met à jour la liste des paquets disponibles et leurs versions

### Étape 2 : Installer build-essential

Maintenant, installons la toolchain complète :

```bash
sudo apt install build-essential
```

Cette commande installe automatiquement :
- GCC (le compilateur C et C++)
- G++ (le compilateur C++)
- Make
- libc6-dev (la bibliothèque standard C)
- dpkg-dev
- Et d'autres outils essentiels

L'installation prend généralement quelques minutes selon votre connexion internet.

### Étape 3 : Vérifier l'installation

Une fois l'installation terminée, vérifions que tout fonctionne correctement.

#### Vérifier GCC

```bash
gcc --version
```

Vous devriez voir une sortie similaire à :

```
gcc (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0  
Copyright (C) 2021 Free Software Foundation, Inc.  
This is free software; see the source for copying conditions.
```

Le numéro de version peut varier selon votre version d'Ubuntu. L'important est que la commande retourne une version sans erreur.

#### Vérifier Make

```bash
make --version
```

Vous devriez voir :

```
GNU Make 4.3  
Built for x86_64-pc-linux-gnu  
Copyright (C) 1988-2020 Free Software Foundation, Inc.
```

#### Vérifier GDB (optionnel mais recommandé)

Si GDB n'est pas installé automatiquement, installez-le avec :

```bash
sudo apt install gdb
```

Puis vérifiez :

```bash
gdb --version
```

---

## Installation d'outils complémentaires

Bien que `build-essential` soit suffisant pour commencer, voici quelques outils supplémentaires très utiles :

### CMake (build system moderne)

```bash
sudo apt install cmake
```

CMake est le build system standard pour les projets C modernes. Nous l'utiliserons intensivement dans le module 5.

### Valgrind (détection de fuites mémoire)

```bash
sudo apt install valgrind
```

Valgrind est un outil indispensable pour détecter les problèmes de gestion mémoire dans vos programmes.

### clang-format (formatage automatique du code)

```bash
sudo apt install clang-format
```

Cet outil vous permettra de formater automatiquement votre code selon des conventions établies.

### pkg-config (gestion des dépendances)

```bash
sudo apt install pkg-config
```

Utile pour gérer les bibliothèques externes dans vos projets.

---

## Comprendre ce qui a été installé

Maintenant que tout est installé, voyons où se trouvent ces outils et comment ils fonctionnent ensemble.

### Où sont installés les outils ?

Les exécutables principaux se trouvent généralement dans `/usr/bin/` :

```bash
which gcc
# Résultat : /usr/bin/gcc
```

La commande `which` vous indique le chemin complet d'un programme.

### Les bibliothèques

Les fichiers d'en-tête (header files) de la bibliothèque standard C se trouvent dans `/usr/include/` :

```bash
ls /usr/include/stdio.h
```

Ce fichier `stdio.h` contient les déclarations pour les fonctions d'entrée/sortie comme `printf()`.

Les bibliothèques compilées se trouvent dans `/usr/lib/` ou `/lib/` :

```bash
ls /usr/lib/x86_64-linux-gnu/libc.so.6
```

---

## Tester votre installation

Pour vous assurer que tout fonctionne, créons un petit programme de test.

### Créer un fichier test.c

Créez un fichier nommé `test.c` avec le contenu suivant :

```c
#include <stdio.h>

int main(void) {
    printf("Installation réussie ! GCC fonctionne.\n");
    return 0;
}
```

### Compiler le programme

```bash
gcc test.c -o test
```

**Explication de la commande :**
- `gcc` : Le compilateur
- `test.c` : Le fichier source à compiler
- `-o test` : Indique le nom du fichier exécutable de sortie (`test`)

### Exécuter le programme

```bash
./test
```

Si vous voyez s'afficher :
```
Installation réussie ! GCC fonctionne.
```

Félicitations ! Votre toolchain est correctement installée et fonctionnelle.

---

## Résolution de problèmes courants

### Erreur : "gcc: command not found"

Si cette erreur apparaît après l'installation, essayez :

1. Vérifier que l'installation s'est bien terminée :
   ```bash
   sudo apt install --reinstall build-essential
   ```

2. Vérifier votre PATH (la liste des répertoires où le système cherche les programmes) :
   ```bash
   echo $PATH
   ```
   Assurez-vous que `/usr/bin` est présent dans la liste.

### Problème de permissions

Si vous obtenez des erreurs de permission lors de l'installation :
- Assurez-vous d'utiliser `sudo`
- Vérifiez que votre utilisateur fait partie du groupe `sudo`

### Espace disque insuffisant

L'installation de build-essential nécessite environ 200-300 Mo d'espace disque. Vérifiez l'espace disponible avec :

```bash
df -h
```

---

## Versions et alternatives

### Versions de GCC

Ubuntu inclut généralement la version stable de GCC. Si vous avez besoin d'une version spécifique :

```bash
# Voir toutes les versions disponibles
apt-cache search gcc | grep gcc-[0-9]

# Installer une version spécifique (exemple GCC 12)
sudo apt install gcc-12
```

### Alternatives à GCC : Clang

Clang est une alternative moderne à GCC. Pour l'installer :

```bash
sudo apt install clang
```

Clang offre généralement des messages d'erreur plus clairs, ce qui peut être utile pour les débutants.

---

## Récapitulatif

À ce stade, vous avez :

- ✅ Installé la toolchain complète pour le développement en C
- ✅ Vérifié que GCC, Make et GDB sont fonctionnels
- ✅ Compilé et exécuté votre premier programme test
- ✅ Compris les composants de base de votre environnement de développement

**Prochaine étape :** Maintenant que votre environnement est prêt, nous allons explorer les différentes versions de GCC et comment les gérer (section 2.2), puis choisir et configurer votre éditeur de code (section 2.3).

---

## Points clés à retenir

1. **build-essential** est le paquet tout-en-un pour développer en C sous Ubuntu
2. **GCC** est le compilateur qui transforme votre code en programme exécutable
3. Toujours **vérifier l'installation** avec `--version` après installation
4. Les outils sont installés dans `/usr/bin/`, les bibliothèques dans `/usr/lib/`
5. Un simple `gcc fichier.c -o programme` suffit pour compiler un programme basique

---


⏭️ [Les versions de GCC et leur gestion](/02-configuration-environnement/02-versions-gcc.md)
