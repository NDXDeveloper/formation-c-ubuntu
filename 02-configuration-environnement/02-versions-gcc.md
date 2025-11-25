🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 2.2 Les versions de GCC et leur gestion

## Introduction

Lorsque vous avez installé GCC dans la section précédente, vous avez probablement obtenu une version spécifique, comme GCC 11 ou GCC 12. Mais pourquoi existe-t-il plusieurs versions de GCC ? Et comment les gérer sur votre système Ubuntu ? Cette section répond à ces questions.

### Pourquoi plusieurs versions de GCC ?

**Analogie :** Imaginez GCC comme un traducteur qui traduit votre code C (que les humains comprennent) en langage machine (que l'ordinateur comprend). Au fil du temps, ce traducteur s'améliore : il apprend de nouvelles expressions (fonctionnalités du langage), devient plus intelligent (optimisations), et corrige ses erreurs (bugs).

Les différentes versions de GCC existent pour plusieurs raisons :

1. **Support de nouvelles normes C** : C99, C11, C17, C23... chaque version ajoute de nouvelles fonctionnalités au langage
2. **Amélioration des optimisations** : Les versions plus récentes génèrent souvent du code plus rapide
3. **Correction de bugs** : Chaque version corrige des problèmes de la précédente
4. **Compatibilité** : Certains projets anciens nécessitent des versions spécifiques

---

## Comprendre les numéros de version

### Format de versionnement

Les versions de GCC suivent le format : **Majeure.Mineure.Patch**

Exemple : **GCC 11.4.0**
- **11** : Version majeure (changements importants)
- **4** : Version mineure (améliorations et corrections)
- **0** : Patch (corrections de bugs mineures)

### Les versions majeures importantes

Voici un aperçu des versions majeures de GCC et leurs apports significatifs :

| Version | Année | Nouveautés principales |
|---------|-------|------------------------|
| GCC 4.9 | 2014 | Support complet de C11 |
| GCC 5.x | 2015 | Nouvelle ABI C++ (changements de compatibilité) |
| GCC 7.x | 2017 | Amélioration du support C++17 |
| GCC 8.x | 2018 | Détection améliorée des bugs |
| GCC 9.x | 2019 | Meilleures optimisations |
| GCC 10.x | 2020 | Support initial de C++20 |
| GCC 11.x | 2021 | Améliorations C++20 et C2x |
| GCC 12.x | 2022 | Support partiel de C23 |
| GCC 13.x | 2023 | Nouvelles optimisations |
| GCC 14.x | 2024 | Support étendu de C23 |

**Note pour débutants :** Pour apprendre le C, n'importe quelle version de GCC 7 ou supérieure convient parfaitement. Les différences sont subtiles pour un usage de base.

---

## Vérifier la version installée

### Version par défaut

Pour connaître la version de GCC utilisée par défaut sur votre système :

```bash
gcc --version
```

Sortie typique :
```
gcc (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0
Copyright (C) 2021 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.
```

Ici, la version est **GCC 11.4.0**.

### Version détaillée avec les capacités

Pour obtenir plus d'informations sur ce que votre version de GCC supporte :

```bash
gcc -v
```

Cette commande affiche :
- La configuration de compilation de GCC
- Les options activées
- Le chemin d'installation
- La cible (architecture)

Exemple de sortie (extrait) :
```
Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-linux-gnu/11/lto-wrapper
Target: x86_64-linux-gnu
Thread model: posix
gcc version 11.4.0 (Ubuntu 11.4.0-1ubuntu1~22.04)
```

---

## Quelle version de GCC pour quelle version d'Ubuntu ?

Chaque version d'Ubuntu est livrée avec une version spécifique de GCC par défaut :

| Ubuntu Version | Nom de code | GCC par défaut |
|----------------|-------------|----------------|
| Ubuntu 18.04 LTS | Bionic | GCC 7.5 |
| Ubuntu 20.04 LTS | Focal | GCC 9.4 |
| Ubuntu 22.04 LTS | Jammy | GCC 11.4 |
| Ubuntu 24.04 LTS | Noble | GCC 13.x |

**Important :** Vous n'êtes pas limité à la version par défaut ! Vous pouvez installer et utiliser plusieurs versions en parallèle.

---

## Installer plusieurs versions de GCC

Il est courant d'avoir plusieurs versions de GCC installées simultanément. Voici comment faire.

### Lister les versions disponibles

Pour voir quelles versions de GCC sont disponibles dans les dépôts Ubuntu :

```bash
apt-cache search gcc | grep "^gcc-[0-9]"
```

Sortie typique :
```
gcc-9 - GNU C compiler
gcc-10 - GNU C compiler
gcc-11 - GNU C compiler
gcc-12 - GNU C compiler
```

### Installer une version spécifique

#### Exemple 1 : Installer GCC 12

```bash
sudo apt update
sudo apt install gcc-12 g++-12
```

**Note :** On installe à la fois `gcc-12` (compilateur C) et `g++-12` (compilateur C++).

#### Exemple 2 : Installer plusieurs versions

```bash
sudo apt install gcc-10 g++-10 gcc-11 g++-11 gcc-12 g++-12
```

Après cette commande, vous aurez GCC 10, 11 et 12 installés en parallèle.

### Vérifier les versions installées

Pour voir toutes les versions de GCC disponibles sur votre système :

```bash
ls /usr/bin/gcc*
```

Sortie possible :
```
/usr/bin/gcc
/usr/bin/gcc-10
/usr/bin/gcc-11
/usr/bin/gcc-12
/usr/bin/gcc-ar
/usr/bin/gcc-nm
/usr/bin/gcc-ranlib
```

---

## Gérer les versions avec update-alternatives

Ubuntu fournit un système appelé **update-alternatives** qui permet de gérer facilement plusieurs versions d'un même programme.

### Qu'est-ce que update-alternatives ?

C'est comme un **aiguilleur de gare** : quand vous tapez `gcc` dans le terminal, le système doit savoir quelle version appeler. `update-alternatives` gère cette décision.

### Configurer les alternatives pour GCC

#### Étape 1 : Enregistrer les versions disponibles

Supposons que vous avez GCC 11 et GCC 12 installés. Enregistrons-les :

```bash
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 110
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 120
```

**Explication de la commande :**
- `--install /usr/bin/gcc` : Le lien symbolique à créer
- `gcc` : Le nom du groupe d'alternatives
- `/usr/bin/gcc-11` : Le chemin vers la version spécifique
- `110` : La priorité (plus le nombre est élevé, plus la priorité est haute)

#### Étape 2 : Faire de même pour g++

```bash
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 110
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 120
```

### Choisir la version par défaut

Pour choisir quelle version de GCC utiliser par défaut :

```bash
sudo update-alternatives --config gcc
```

Vous verrez un menu interactif :

```
There are 2 choices for the alternative gcc (providing /usr/bin/gcc).

  Selection    Path              Priority   Status
------------------------------------------------------------
* 0            /usr/bin/gcc-12    120       auto mode
  1            /usr/bin/gcc-11    110       manual mode
  2            /usr/bin/gcc-12    120       manual mode

Press <enter> to keep the current choice[*], or type selection number:
```

Tapez le numéro correspondant à la version souhaitée et appuyez sur Entrée.

### Vérifier le changement

```bash
gcc --version
```

La version affichée devrait correspondre à votre choix.

---

## Utiliser une version spécifique temporairement

Vous n'êtes pas obligé de changer la version par défaut. Vous pouvez utiliser une version spécifique pour un seul projet ou une seule compilation.

### Méthode 1 : Appeler directement la version

Au lieu d'utiliser `gcc`, utilisez `gcc-12` ou `gcc-11` :

```bash
gcc-12 monprogramme.c -o monprogramme
```

Cela compilera avec GCC 12, même si la version par défaut est GCC 11.

### Méthode 2 : Variable d'environnement CC

Vous pouvez définir la variable d'environnement `CC` (C Compiler) :

```bash
export CC=gcc-12
gcc monprogramme.c -o monprogramme
```

Ou en une ligne :

```bash
CC=gcc-12 gcc monprogramme.c -o monprogramme
```

### Méthode 3 : Alias dans votre shell

Ajoutez dans votre `~/.bashrc` :

```bash
alias gcc12='gcc-12'
alias gcc11='gcc-11'
```

Rechargez la configuration :

```bash
source ~/.bashrc
```

Maintenant vous pouvez utiliser :

```bash
gcc12 monprogramme.c -o monprogramme
```

---

## Spécifier la norme C à utiliser

Indépendamment de la version de GCC, vous pouvez spécifier quelle norme du langage C utiliser avec l'option `-std`.

### Les normes C disponibles

```bash
# C89/C90 (ANSI C)
gcc -std=c89 programme.c -o programme

# C99
gcc -std=c99 programme.c -o programme

# C11 (recommandé pour la production)
gcc -std=c11 programme.c -o programme

# C17 (corrections de C11)
gcc -std=c17 programme.c -o programme

# C23 (version la plus récente, support partiel)
gcc -std=c23 programme.c -o programme
```

**Conseil pour débutants :** Utilisez `-std=c11` ou `-std=c17`. Ce sont des normes stables et largement supportées.

### Extensions GNU

Par défaut, GCC active des extensions GNU (fonctionnalités non standard). Pour un C strict :

```bash
# C11 strict, sans extensions GNU
gcc -std=c11 -pedantic programme.c -o programme

# Encore plus strict
gcc -std=c11 -pedantic-errors programme.c -o programme
```

L'option `-pedantic` affiche des avertissements pour les extensions non standard.

---

## Compatibilité entre versions

### Code compilé avec GCC 11 vs GCC 12

**Question fréquente :** Si je compile mon programme avec GCC 11, puis-je l'exécuter sur une machine avec GCC 12 ?

**Réponse :** Oui ! Le programme compilé est indépendant de la version de GCC. Ce qui compte, c'est la compatibilité des bibliothèques système.

### ABI (Application Binary Interface)

L'ABI définit comment les programmes communiquent au niveau binaire. Les versions majeures de GCC maintiennent généralement la compatibilité ABI pour le C (contrairement au C++).

**En pratique :** Un programme C compilé avec GCC 7, 8, 9, 10, 11, 12 ou 13 fonctionnera sur le même système Linux sans problème.

---

## Gérer les versions dans un projet avec CMake

Si vous utilisez CMake (nous verrons cela plus tard), vous pouvez spécifier la version de GCC dans votre `CMakeLists.txt` :

```cmake
# Spécifier GCC 12
set(CMAKE_C_COMPILER gcc-12)
set(CMAKE_CXX_COMPILER g++-12)
```

Ou depuis la ligne de commande :

```bash
cmake -DCMAKE_C_COMPILER=gcc-12 -DCMAKE_CXX_COMPILER=g++-12 ..
```

---

## Quelle version choisir ?

### Pour apprendre le C

**Recommandation :** GCC 11 ou 12 sont parfaits. Ils ont un excellent support de C11 et C17.

```bash
sudo apt install gcc-11 g++-11
```

### Pour un projet de production

**Recommandation :** Utilisez la version LTS (Long Term Support) de votre distribution Ubuntu :
- Ubuntu 20.04 LTS → GCC 9
- Ubuntu 22.04 LTS → GCC 11
- Ubuntu 24.04 LTS → GCC 13

Cela garantit la stabilité et le support à long terme.

### Pour expérimenter les nouvelles fonctionnalités

**Recommandation :** La dernière version stable (GCC 13 ou 14).

```bash
sudo apt install gcc-13 g++-13
```

### Pour des projets legacy (anciens)

Si vous devez maintenir du code ancien qui ne compile qu'avec une vieille version :

```bash
sudo apt install gcc-7 g++-7
```

---

## Vérifier le support des normes C

Pour savoir quelles normes C votre version de GCC supporte bien :

| GCC Version | C99 | C11 | C17 | C23 |
|-------------|-----|-----|-----|-----|
| GCC 7.x     | ✅  | ✅  | ✅  | ❌  |
| GCC 8.x     | ✅  | ✅  | ✅  | ❌  |
| GCC 9.x     | ✅  | ✅  | ✅  | ⚠️  |
| GCC 10.x    | ✅  | ✅  | ✅  | ⚠️  |
| GCC 11.x    | ✅  | ✅  | ✅  | ⚠️  |
| GCC 12.x    | ✅  | ✅  | ✅  | ⚠️  |
| GCC 13.x    | ✅  | ✅  | ✅  | 🔄  |

**Légende :**
- ✅ : Support complet et stable
- ⚠️ : Support partiel/expérimental
- 🔄 : Support en cours de développement
- ❌ : Non supporté

---

## Désinstaller une version de GCC

Si vous n'avez plus besoin d'une version spécifique :

```bash
sudo apt remove gcc-10 g++-10
```

Pour supprimer complètement (y compris les fichiers de configuration) :

```bash
sudo apt purge gcc-10 g++-10
```

**Attention :** Ne désinstallez jamais la version par défaut de votre système Ubuntu sans savoir ce que vous faites, car d'autres programmes pourraient en dépendre.

---

## Résolution de problèmes

### Conflit entre versions

**Symptôme :** Messages d'erreur étranges ou comportement inattendu.

**Solution :** Vérifiez quelle version est réellement utilisée :

```bash
which gcc
gcc --version
```

Si nécessaire, reconfigurer avec `update-alternatives`.

### Bibliothèques incompatibles

**Symptôme :** Erreurs de linkage avec des bibliothèques externes.

**Solution :** Assurez-vous que la bibliothèque a été compilée avec une version compatible de GCC, ou recompilez-la avec votre version actuelle.

### Headers introuvables

**Symptôme :** Erreurs du type `fatal error: stdio.h: No such file or directory`

**Solution :** Installez les headers de développement :

```bash
sudo apt install build-essential
```

---

## Bonnes pratiques

### 1. Documenter la version utilisée

Dans votre README ou documentation de projet, indiquez :

```markdown
## Compilation

Ce projet nécessite GCC 11 ou supérieur.

```bash
gcc --version  # Doit afficher 11.x ou plus
gcc -std=c11 -Wall -Wextra main.c -o programme
```
```

### 2. Utiliser des Makefiles ou CMake

Spécifiez explicitement le compilateur et les options dans vos scripts de build plutôt que de supposer une configuration.

### 3. Tester avec plusieurs versions

Pour des projets importants, testez la compilation avec plusieurs versions de GCC pour assurer la portabilité.

### 4. Ne pas mélanger les versions

Lors de la compilation d'un projet, utilisez la même version de GCC pour tous les fichiers source. Ne compilez pas `file1.c` avec GCC 11 et `file2.c` avec GCC 12 dans le même projet.

---

## Récapitulatif

À ce stade, vous savez :

- ✅ Pourquoi il existe plusieurs versions de GCC
- ✅ Comment vérifier votre version installée
- ✅ Comment installer plusieurs versions en parallèle
- ✅ Comment gérer les versions avec `update-alternatives`
- ✅ Comment choisir la version adaptée à vos besoins
- ✅ Comment spécifier une norme C avec `-std`
- ✅ Les différences de support entre les versions

**Prochaine étape :** Maintenant que vous maîtrisez les versions de GCC, nous allons choisir et configurer un éditeur de code ou un IDE pour écrire confortablement vos programmes C (section 2.3).

---

## Points clés à retenir

1. **GCC évolue** : Nouvelles versions = nouvelles fonctionnalités + meilleures optimisations
2. **Plusieurs versions possibles** : Ubuntu permet d'installer GCC 7, 8, 9, 10, 11, 12, 13... en parallèle
3. **update-alternatives** : L'outil pour gérer facilement les versions par défaut
4. **-std=c11 ou c17** : Les normes recommandées pour apprendre et produire du code stable
5. **Version LTS** : Pour la production, utilisez la version fournie avec Ubuntu LTS
6. **gcc-XX pour utiliser directement** : `gcc-12 file.c` pour utiliser GCC 12 sans changer le défaut

---


⏭️ [Choix de l'IDE/Éditeur](/02-configuration-environnement/03-choix-ide-editeur.md)
