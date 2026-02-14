🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 14.5 ldd et ldconfig

## Introduction

Lorsque vous travaillez avec des bibliothèques dynamiques sous Linux, deux outils sont absolument essentiels : **ldd** et **ldconfig**. Ces commandes vous permettent de comprendre et de gérer les dépendances de vos programmes.

- **ldd** : Affiche les bibliothèques dynamiques dont dépend un exécutable
- **ldconfig** : Gère le cache des bibliothèques dynamiques du système

Dans cette section, nous allons explorer ces deux outils en détail et apprendre à les utiliser efficacement pour diagnostiquer et résoudre les problèmes de dépendances.

---

## ldd : List Dynamic Dependencies

### Qu'est-ce que ldd ?

**ldd** (List Dynamic Dependencies) est un utilitaire qui affiche la liste des bibliothèques partagées (fichiers `.so`) dont un programme a besoin pour s'exécuter.

### Syntaxe de base

```bash
ldd [options] /chemin/vers/executable
```

### Premier exemple simple

Prenons un programme standard comme `ls` :

```bash
$ ldd /bin/ls
    linux-vdso.so.1 (0x00007ffc8d3e9000)
    libselinux.so.1 => /lib/x86_64-linux-gnu/libselinux.so.1 (0x00007f5e8c9f0000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f5e8c7e0000)
    libpcre2-8.so.0 => /lib/x86_64-linux-gnu/libpcre2-8.so.0 (0x00007f5e8c740000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f5e8ca50000)
```

**Interprétation** :
- Chaque ligne représente une bibliothèque nécessaire
- Le format est : `nom_biblio => chemin_complet (adresse_mémoire)`
- `/bin/ls` dépend de 5 bibliothèques

---

## Comprendre la sortie de ldd

### Structure d'une ligne

```
libselinux.so.1 => /lib/x86_64-linux-gnu/libselinux.so.1 (0x00007f5e8c9f0000)
│               │  │                                      │
│               │  │                                      └─ Adresse mémoire (chargement)
│               │  └─ Chemin absolu de la bibliothèque
│               └─ Séparateur (flèche)
└─ Nom de la bibliothèque demandée (SONAME)
```

### Cas particuliers

#### 1. Bibliothèque introuvable

```bash
$ ldd mon_programme
    libmylib.so => not found
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f...)
```

❌ **"not found"** : La bibliothèque n'est pas trouvée dans les chemins de recherche.

**Solution** : Vérifier LD_LIBRARY_PATH ou installer la bibliothèque manquante.

---

#### 2. linux-vdso.so.1

```bash
linux-vdso.so.1 (0x00007ffc8d3e9000)
```

✅ **linux-vdso** : Bibliothèque virtuelle fournie par le kernel (pas un fichier réel)
- Optimise certains appels système
- Toujours présente, pas de chemin associé
- **Ignorez-la** lors du débogage des dépendances

---

#### 3. Le dynamic linker

```bash
/lib64/ld-linux-x86-64.so.2 (0x00007f5e8ca50000)
```

✅ **ld-linux-x86-64.so.2** : Le dynamic linker lui-même
- Responsable du chargement des bibliothèques
- Toujours présent pour les programmes dynamiques

---

### Options utiles de ldd

#### Option -v (verbose)

Affiche des informations détaillées sur les versions :

```bash
$ ldd -v /bin/ls
    linux-vdso.so.1 (0x00007fff...)
    libselinux.so.1 => /lib/x86_64-linux-gnu/libselinux.so.1 (0x00007f...)
        Version information:
        /bin/ls:
            libc.so.6 (GLIBC_2.34) => /lib/x86_64-linux-gnu/libc.so.6
            libc.so.6 (GLIBC_2.4) => /lib/x86_64-linux-gnu/libc.so.6
```

💡 **Utile pour** : Vérifier les versions de glibc ou autres bibliothèques critiques.

---

#### Option -r (relocations)

Affiche les relocations de données et de fonctions :

```bash
$ ldd -r mon_programme
    libmylib.so => ./lib/libmylib.so (0x00007f...)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f...)
undefined symbol: ma_fonction_manquante    (mon_programme)
```

⚠️ **Affiche les symboles non définis** : Très utile pour détecter les problèmes de liaison.

---

#### Option -u (unused)

Affiche les dépendances inutilisées :

```bash
$ ldd -u mon_programme
Unused direct dependencies:
    /lib/x86_64-linux-gnu/libm.so.6
```

💡 **Utile pour** : Optimiser la liste des bibliothèques liées (retirer `-lm` si non utilisée).

---

## Cas d'usage pratiques de ldd

### Cas 1 : Diagnostiquer "Library not found"

**Problème** :
```bash
$ ./mon_programme
./mon_programme: error while loading shared libraries: libmylib.so: cannot open shared object file
```

**Diagnostic avec ldd** :
```bash
$ ldd mon_programme
    linux-vdso.so.1 (0x00007fff...)
    libmylib.so => not found  ❌
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f...)
```

**Solution** :
```bash
# Trouver où est la bibliothèque
$ find / -name libmylib.so 2>/dev/null
/home/user/mylibs/libmylib.so

# Ajouter le chemin
$ export LD_LIBRARY_PATH=/home/user/mylibs:$LD_LIBRARY_PATH

# Vérifier
$ ldd mon_programme
    libmylib.so => /home/user/mylibs/libmylib.so (0x00007f...) ✅
```

---

### Cas 2 : Vérifier les dépendances avant distribution

Avant de distribuer votre application, vérifiez qu'elle ne dépend pas de bibliothèques non-standard :

```bash
$ ldd mon_application
    linux-vdso.so.1 (0x00007fff...)
    libcustom.so => /home/dev/mylibs/libcustom.so (0x00007f...) ⚠️
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f...)
```

⚠️ **Problème** : `/home/dev/mylibs/libcustom.so` n'existera pas sur la machine de l'utilisateur !

**Solution** : Utiliser RPATH ou inclure la bibliothèque dans le package.

---

### Cas 3 : Comparer les dépendances entre deux versions

```bash
# Version 1.0
$ ldd myapp-v1.0
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6
    libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6

# Version 2.0
$ ldd myapp-v2.0
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6
    libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6
    libssl.so.3 => /lib/x86_64-linux-gnu/libssl.so.3  ← Nouvelle dépendance
```

💡 **Utile pour** : Documenter les dépendances additionnelles d'une nouvelle version.

---

### Cas 4 : Vérifier un exécutable statique

```bash
$ ldd mon_programme_statique
    not a dynamic executable
```

✅ **"not a dynamic executable"** : Le programme est lié statiquement (pas de dépendances .so).

---

### Cas 5 : Détecter les conflits de versions

```bash
$ LD_LIBRARY_PATH=/tmp/old_libs ldd mon_programme
    libssl.so.1.1 => /tmp/old_libs/libssl.so.1.1 (0x00007f...)  ⚠️
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f...)

$ ldd mon_programme
    libssl.so.3 => /lib/x86_64-linux-gnu/libssl.so.3 (0x00007f...)  ✅
```

⚠️ **Attention** : Deux versions différentes de libssl selon l'environnement.

---

## ldconfig : Gérer le cache des bibliothèques

### Qu'est-ce que ldconfig ?

**ldconfig** est un utilitaire système qui :
1. **Crée et met à jour** le cache des bibliothèques partagées (`/etc/ld.so.cache`)
2. **Crée les liens symboliques** nécessaires pour les bibliothèques (SONAME)
3. **Permet au dynamic linker** de trouver rapidement les bibliothèques

### Pourquoi un cache ?

Sans cache, le dynamic linker devrait parcourir tous les répertoires système à chaque lancement de programme. Le cache accélère considérablement cette recherche.

```
Sans cache : Parcourir /lib, /usr/lib, /usr/local/lib... à chaque fois  
Avec cache : Consulter /etc/ld.so.cache (fichier binaire optimisé)  
```

---

## Le fichier /etc/ld.so.cache

### Qu'est-ce que c'est ?

```bash
$ file /etc/ld.so.cache
/etc/ld.so.cache: cache ld.so data

$ ls -lh /etc/ld.so.cache
-rw-r--r-- 1 root root 87K Nov 26 10:30 /etc/ld.so.cache
```

**Contenu** : Base de données binaire mappant les noms de bibliothèques vers leurs chemins absolus.

⚠️ **Attention** : Ne modifiez jamais ce fichier manuellement ! Utilisez toujours `ldconfig`.

---

## Configuration de ldconfig

### Fichiers de configuration

#### 1. /etc/ld.so.conf

Fichier principal qui liste les répertoires à scanner :

```bash
$ cat /etc/ld.so.conf
include /etc/ld.so.conf.d/*.conf
```

💡 **Note** : Ce fichier inclut généralement d'autres fichiers de configuration.

---

#### 2. /etc/ld.so.conf.d/*.conf

Répertoire contenant des fichiers de configuration modulaires :

```bash
$ ls /etc/ld.so.conf.d/
fakeroot-x86_64-linux-gnu.conf  
libc.conf  
x86_64-linux-gnu.conf  

$ cat /etc/ld.so.conf.d/x86_64-linux-gnu.conf
# Multiarch support
/usr/local/lib/x86_64-linux-gnu
/lib/x86_64-linux-gnu
/usr/lib/x86_64-linux-gnu
```

✅ **Bonne pratique** : Créez vos propres fichiers `.conf` ici plutôt que de modifier `/etc/ld.so.conf`.

---

## Utilisation de ldconfig

### Syntaxe de base

```bash
sudo ldconfig [options] [répertoires...]
```

⚠️ **Important** : `ldconfig` nécessite les privilèges root (sudo).

---

### Mise à jour du cache

```bash
# Mettre à jour le cache en scannant les répertoires configurés
$ sudo ldconfig

# Pas de sortie = succès ✅
```

**Quand l'utiliser ?**
- Après avoir installé une nouvelle bibliothèque dans `/usr/local/lib`
- Après avoir modifié `/etc/ld.so.conf.d/*.conf`
- Après avoir mis à jour des bibliothèques système

---

### Afficher le contenu du cache

#### Option -p (print)

```bash
$ ldconfig -p
1283 libs found in cache `/etc/ld.so.cache'
    libzstd.so.1 (libc6,x86-64) => /lib/x86_64-linux-gnu/libzstd.so.1
    libz.so.1 (libc6,x86-64) => /lib/x86_64-linux-gnu/libz.so.1
    libxml2.so.2 (libc6,x86-64) => /lib/x86_64-linux-gnu/libxml2.so.2
    ...
```

💡 **Utile pour** : Vérifier si une bibliothèque est dans le cache système.

---

#### Rechercher une bibliothèque spécifique

```bash
$ ldconfig -p | grep libssl
    libssl.so.3 (libc6,x86-64) => /lib/x86_64-linux-gnu/libssl.so.3
    libssl.so.1.1 (libc6,x86-64) => /usr/lib/x86_64-linux-gnu/libssl.so.1.1
```

✅ **Pratique** : Trouver rapidement où est installée une bibliothèque.

---

### Ajouter un répertoire personnalisé

#### Méthode 1 : Modifier la configuration

```bash
# Créer un fichier de configuration
$ sudo nano /etc/ld.so.conf.d/myapp.conf

# Contenu :
/home/user/mylibs
/opt/mycompany/lib

# Sauvegarder et mettre à jour le cache
$ sudo ldconfig

# Vérifier
$ ldconfig -p | grep mylib
    libmylib.so => /home/user/mylibs/libmylib.so ✅
```

---

#### Méthode 2 : Spécifier directement un répertoire

```bash
# Scanner un répertoire spécifique et mettre à jour le cache
$ sudo ldconfig /home/user/mylibs

# Vérifier
$ ldconfig -p | grep mylib
    libmylib.so => /home/user/mylibs/libmylib.so ✅
```

⚠️ **Attention** : Cette méthode ne persiste pas après le prochain `ldconfig` sans argument.

---

### Option -n : Mode "n'ajoute pas au cache"

```bash
# Scanner un répertoire et créer les liens symboliques sans mettre à jour le cache
$ sudo ldconfig -n /home/user/mylibs
```

💡 **Utile pour** : Créer rapidement les liens symboliques SONAME sans affecter le cache système.

---

### Option -v : Mode verbeux

```bash
$ sudo ldconfig -v
/usr/local/lib/x86_64-linux-gnu:
/lib/x86_64-linux-gnu:
    libz.so.1 -> libz.so.1.2.11
    libxml2.so.2 -> libxml2.so.2.9.13
/usr/lib/x86_64-linux-gnu:
    libssl.so.3 -> libssl.so.3.0.2
...
```

✅ **Affiche** : Les répertoires scannés et les liens symboliques créés.

---

## Création automatique des liens symboliques

### Le problème du SONAME

Les bibliothèques utilisent un système de versioning :

```bash
libmylib.so.1.2.3  ← Fichier réel (version complète)  
libmylib.so.1      ← SONAME (version majeure)  
libmylib.so        ← Lien pour la compilation  
```

**ldconfig** crée automatiquement ces liens :

```bash
$ ls -l /usr/local/lib/
-rwxr-xr-x 1 root root 245K libmylib.so.1.2.3
lrwxrwxrwx 1 root root   17 libmylib.so.1 -> libmylib.so.1.2.3  ← Créé par ldconfig  
lrwxrwxrwx 1 root root   13 libmylib.so -> libmylib.so.1        ← Créé manuellement  
```

---

### Exemple complet

```bash
# Copier une nouvelle bibliothèque
$ sudo cp libmylib.so.2.0.0 /usr/local/lib/

# Avant ldconfig
$ ls -l /usr/local/lib/libmylib*
-rwxr-xr-x 1 root root 245K libmylib.so.2.0.0

# Exécuter ldconfig
$ sudo ldconfig

# Après ldconfig
$ ls -l /usr/local/lib/libmylib*
-rwxr-xr-x 1 root root 245K libmylib.so.2.0.0
lrwxrwxrwx 1 root root   17 libmylib.so.2 -> libmylib.so.2.0.0  ✅

# Créer le lien de développement manuellement
$ sudo ln -s libmylib.so.2 /usr/local/lib/libmylib.so

# Résultat final
$ ls -l /usr/local/lib/libmylib*
-rwxr-xr-x 1 root root 245K libmylib.so.2.0.0
lrwxrwxrwx 1 root root   17 libmylib.so.2 -> libmylib.so.2.0.0  
lrwxrwxrwx 1 root root   13 libmylib.so -> libmylib.so.2  
```

---

## Cas d'usage pratiques combinés (ldd + ldconfig)

### Cas 1 : Installer une bibliothèque depuis les sources

**Étapes complètes** :

```bash
# 1. Compiler la bibliothèque
$ cd libmylib-2.0
$ ./configure --prefix=/usr/local
$ make
$ sudo make install
# Installe dans /usr/local/lib/libmylib.so.2.0.0

# 2. Mettre à jour le cache avec ldconfig
$ sudo ldconfig

# 3. Vérifier avec ldconfig -p
$ ldconfig -p | grep libmylib
    libmylib.so.2 (libc6,x86-64) => /usr/local/lib/libmylib.so.2.0.0 ✅

# 4. Vérifier avec ldd sur un programme qui l'utilise
$ ldd mon_programme
    libmylib.so.2 => /usr/local/lib/libmylib.so.2.0.0 (0x00007f...) ✅
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f...)
```

---

### Cas 2 : Résoudre un conflit de versions

**Problème** :
```bash
$ ./mon_programme
./mon_programme: /usr/lib/libmylib.so.1: version `MYLIB_2.0' not found
```

**Diagnostic** :

```bash
# 1. Vérifier les dépendances
$ ldd mon_programme
    libmylib.so.1 => /usr/lib/libmylib.so.1 (0x00007f...)  ← Ancienne version

# 2. Vérifier ce qui est dans le cache
$ ldconfig -p | grep libmylib
    libmylib.so.1 => /usr/lib/libmylib.so.1                  ← Ancienne
    libmylib.so.2 => /usr/local/lib/libmylib.so.2.0.0        ← Nouvelle

# 3. Vérifier les versions avec ldd -v
$ ldd -v mon_programme
    libmylib.so.1 (MYLIB_2.0) => /usr/lib/libmylib.so.1  ← Programme veut MYLIB_2.0
        Version information:
        /usr/lib/libmylib.so.1:
            MYLIB_1.5  ← Mais la bibliothèque ne fournit que 1.5 ❌
```

**Solution** : Installer la bonne version ou recompiler.

---

### Cas 3 : Nettoyer les anciennes bibliothèques

**Situation** : Vous avez plusieurs versions d'une bibliothèque.

```bash
# 1. Lister toutes les versions
$ ldconfig -p | grep libmylib
    libmylib.so.2 => /usr/local/lib/libmylib.so.2.0.0
    libmylib.so.1 => /usr/lib/libmylib.so.1.5.0
    libmylib.so.1 => /opt/old/lib/libmylib.so.1.0.0

# 2. Vérifier quels programmes utilisent chaque version
$ for prog in /usr/bin/*; do
    ldd "$prog" 2>/dev/null | grep libmylib && echo "  ^ $prog"
done

# 3. Supprimer la version inutilisée
$ sudo rm /opt/old/lib/libmylib.so.1.0.0

# 4. Mettre à jour le cache
$ sudo ldconfig

# 5. Vérifier
$ ldconfig -p | grep libmylib
    libmylib.so.2 => /usr/local/lib/libmylib.so.2.0.0  ✅
    libmylib.so.1 => /usr/lib/libmylib.so.1.5.0        ✅
```

---

### Cas 4 : Déboguer un programme qui crash au démarrage

```bash
# 1. Vérifier les dépendances de base
$ ldd mon_programme
    libspecial.so => /usr/lib/libspecial.so (0x00007f...)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f...)

# 2. Vérifier les symboles non définis
$ ldd -r mon_programme
undefined symbol: special_function_v2    (./mon_programme)  ❌

# 3. Vérifier que la bibliothèque est à jour
$ ldconfig -p | grep libspecial
    libspecial.so => /usr/lib/libspecial.so  ← Version 1.0

# 4. Vérifier les versions disponibles
$ ls -l /usr/lib/libspecial*
lrwxrwxrwx 1 root root   18 libspecial.so -> libspecial.so.1.0.0
-rwxr-xr-x 1 root root 150K libspecial.so.1.0.0

# Conclusion : La bibliothèque est trop ancienne
# Solution : Mettre à jour vers la version 2.0
```

---

## Différences et complémentarité

| Outil | Rôle | Quand l'utiliser ? | Privilèges requis |
|-------|------|-------------------|-------------------|
| **ldd** | Lister les dépendances d'un exécutable | Diagnostic, debugging | Non (utilisateur) |
| **ldconfig** | Gérer le cache système des bibliothèques | Après installation de .so | Oui (sudo) |

### Workflow typique

```
1. Installer une bibliothèque
   └─> sudo cp libmylib.so /usr/local/lib/

2. Mettre à jour le cache
   └─> sudo ldconfig

3. Vérifier que la bibliothèque est trouvée
   └─> ldconfig -p | grep libmylib

4. Vérifier qu'un programme la trouve
   └─> ldd mon_programme
```

---

## Problèmes courants et solutions

### Problème 1 : ldd affiche "not found" mais ldconfig -p trouve la bibliothèque

```bash
$ ldconfig -p | grep libmylib
    libmylib.so.2 => /home/user/mylibs/libmylib.so.2  ✅

$ ldd mon_programme
    libmylib.so.2 => not found  ❌
```

**Cause** : `/home/user/mylibs` n'est pas dans les chemins de recherche standard.

**Solution** :
```bash
# Option 1 : Ajouter à la configuration système
$ sudo nano /etc/ld.so.conf.d/mylibs.conf
/home/user/mylibs

$ sudo ldconfig

# Option 2 : Utiliser LD_LIBRARY_PATH (temporaire)
$ export LD_LIBRARY_PATH=/home/user/mylibs:$LD_LIBRARY_PATH
```

---

### Problème 2 : ldconfig ne crée pas les liens symboliques

```bash
$ sudo cp libmylib.so.1.2.3 /usr/local/lib/
$ sudo ldconfig
$ ls /usr/local/lib/libmylib*
-rwxr-xr-x 1 root root 245K libmylib.so.1.2.3  ← Pas de lien ❌
```

**Cause** : La bibliothèque n'a pas de SONAME défini.

**Vérification** :
```bash
$ readelf -d /usr/local/lib/libmylib.so.1.2.3 | grep SONAME
# Vide = pas de SONAME
```

**Solution** : Créer les liens manuellement
```bash
$ sudo ln -s libmylib.so.1.2.3 /usr/local/lib/libmylib.so.1
$ sudo ln -s libmylib.so.1 /usr/local/lib/libmylib.so
```

---

### Problème 3 : ldconfig ne trouve pas un répertoire

```bash
$ sudo ldconfig /opt/mylibs
ldconfig: Can't stat /opt/mylibs: Permission denied
```

**Causes possibles** :

1. **Permissions insuffisantes** :
```bash
$ ls -ld /opt/mylibs
drwx------ 2 user user 4096 /opt/mylibs  ← Pas accessible pour root

# Solution
$ sudo chmod 755 /opt/mylibs
```

2. **Le répertoire n'existe pas** :
```bash
$ sudo mkdir -p /opt/mylibs
```

---

### Problème 4 : ldd sur un script au lieu d'un binaire

```bash
$ ldd mon_script.sh
    not a dynamic executable
```

✅ **Normal** : Les scripts (bash, python, etc.) ne sont pas des exécutables ELF.

**Vérification** :
```bash
$ file mon_script.sh
mon_script.sh: Bourne-Again shell script, ASCII text executable
```

---

## Commandes avancées

### Afficher uniquement les bibliothèques manquantes

```bash
$ ldd mon_programme | grep "not found"
    libcustom.so => not found
    libspecial.so.1 => not found
```

---

### Compter le nombre de dépendances

```bash
$ ldd mon_programme | wc -l
8
```

---

### Vérifier les dépendances de tous les binaires d'un répertoire

```bash
$ for bin in /usr/bin/*; do
    echo "=== $bin ==="
    ldd "$bin" 2>/dev/null | grep "not found"
done
```

---

### Comparer les bibliothèques dans le cache entre deux systèmes

```bash
# Système 1
$ ldconfig -p > system1_libs.txt

# Système 2
$ ldconfig -p > system2_libs.txt

# Comparer
$ diff system1_libs.txt system2_libs.txt
```

---

## Sécurité et bonnes pratiques

### ⚠️ Attention avec ldd

**ldd exécute partiellement le programme** pour analyser ses dépendances.

```bash
# ❌ DANGEREUX : Ne jamais utiliser ldd sur un binaire non fiable
$ ldd /tmp/binaire_suspect

# ✅ Alternative sécurisée : objdump
$ objdump -p /tmp/binaire_suspect | grep NEEDED
```

---

### ✅ Bonnes pratiques avec ldconfig

1. **Toujours utiliser sudo** :
```bash
# ❌ Ne fonctionnera pas
$ ldconfig
ldconfig: Can't create temporary cache file /etc/ld.so.cache~: Permission denied

# ✅
$ sudo ldconfig
```

2. **Créer des fichiers de configuration dédiés** :
```bash
# ✅ BON
$ sudo nano /etc/ld.so.conf.d/myapp.conf

# ❌ MAUVAIS (modifie le fichier principal)
$ sudo nano /etc/ld.so.conf
```

3. **Documenter les chemins ajoutés** :
```bash
$ sudo nano /etc/ld.so.conf.d/myapp.conf
# MyApp custom libraries
# Version 2.0 - 2025-11-26
/opt/myapp/lib
/usr/local/lib/myapp
```

---

## Tableau récapitulatif

| Commande | Action | Exemple |
|----------|--------|---------|
| `ldd programme` | Lister les dépendances | `ldd /bin/ls` |
| `ldd -v programme` | Infos détaillées + versions | `ldd -v mon_app` |
| `ldd -r programme` | Vérifier symboles non définis | `ldd -r mon_app` |
| `ldd -u programme` | Trouver dépendances inutilisées | `ldd -u mon_app` |
| `ldconfig` | Mettre à jour le cache | `sudo ldconfig` |
| `ldconfig -p` | Afficher le cache | `ldconfig -p` |
| `ldconfig -p \| grep lib` | Chercher une bibliothèque | `ldconfig -p \| grep libssl` |
| `ldconfig -v` | Mode verbeux | `sudo ldconfig -v` |
| `ldconfig -n dir` | Créer liens sans cache | `sudo ldconfig -n /opt/lib` |

---

## Workflow complet : De la compilation à l'exécution

### Scénario : Installer une bibliothèque personnalisée

```bash
# 1. Compiler la bibliothèque (avec SONAME)
$ gcc -shared -fPIC -Wl,-soname,libmylib.so.2 -o libmylib.so.2.0.0 mylib.c

# 2. Installer dans /usr/local/lib
$ sudo cp libmylib.so.2.0.0 /usr/local/lib/
$ sudo chmod 755 /usr/local/lib/libmylib.so.2.0.0

# 3. Créer les liens symboliques avec ldconfig
$ sudo ldconfig -v
/usr/local/lib:
    libmylib.so.2 -> libmylib.so.2.0.0  ✅

# 4. Créer le lien de développement
$ sudo ln -s libmylib.so.2 /usr/local/lib/libmylib.so

# 5. Vérifier avec ldconfig -p
$ ldconfig -p | grep libmylib
    libmylib.so.2 (libc6,x86-64) => /usr/local/lib/libmylib.so.2.0.0  ✅

# 6. Compiler un programme qui l'utilise
$ gcc main.c -lmylib -o mon_programme

# 7. Vérifier les dépendances avec ldd
$ ldd mon_programme
    libmylib.so.2 => /usr/local/lib/libmylib.so.2.0.0 (0x00007f...)  ✅
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f...)

# 8. Exécuter
$ ./mon_programme
Hello from libmylib 2.0!  ✅
```

---

## Alternatives et outils complémentaires

### objdump : Alternative sécurisée à ldd

```bash
# Lister les bibliothèques requises sans exécuter le programme
$ objdump -p mon_programme | grep NEEDED
  NEEDED               libmylib.so.2
  NEEDED               libc.so.6
```

✅ **Sécurisé** : N'exécute pas le binaire

---

### readelf : Informations détaillées

```bash
# Afficher les dépendances dynamiques
$ readelf -d mon_programme | grep NEEDED
 0x0000000000000001 (NEEDED)     Shared library: [libmylib.so.2]
 0x0000000000000001 (NEEDED)     Shared library: [libc.so.6]

# Afficher le RPATH/RUNPATH
$ readelf -d mon_programme | grep -E 'RPATH|RUNPATH'
 0x000000000000000f (RPATH)      Library rpath: [$ORIGIN/../lib]
```

---

### nm : Lister les symboles

```bash
# Lister les symboles exportés par une bibliothèque
$ nm -D /usr/local/lib/libmylib.so | grep " T "
00000000000011a9 T my_function
0000000000001205 T my_other_function
```

💡 **T** = Symbole dans la section texte (fonction)

---

## Conclusion

**ldd** et **ldconfig** sont deux outils complémentaires essentiels :

### ldd : Diagnostic
- ✅ Visualiser les dépendances d'un programme
- ✅ Déboguer les problèmes "library not found"
- ✅ Vérifier les versions de bibliothèques
- ⚠️ Attention : Exécute partiellement le programme

### ldconfig : Gestion
- ✅ Maintenir le cache système à jour
- ✅ Créer automatiquement les liens symboliques SONAME
- ✅ Ajouter des répertoires personnalisés au cache
- ⚠️ Nécessite les privilèges root

**Règle d'or** :
1. Installez une bibliothèque → Lancez `sudo ldconfig`
2. Un programme ne démarre pas → Utilisez `ldd` pour diagnostiquer
3. Vérifiez régulièrement le cache avec `ldconfig -p`

---

## Pour aller plus loin

**Sections connexes recommandées** :
- **14.1** : Bibliothèques statiques (.a)
- **14.2** : Bibliothèques dynamiques (.so) - Création et SONAME
- **14.3** : Différences statiques vs dynamiques
- **14.4** : Gestion du LD_LIBRARY_PATH
- **15.2** : Utilisation de GDB (débogage avancé)

**Commandes à explorer** :
- `man ldd` : Documentation complète de ldd
- `man ldconfig` : Documentation complète de ldconfig
- `man ld.so` : Fonctionnement du dynamic linker
- `man objdump` : Alternative sécurisée à ldd

**Ressources externes** :
- *Linux System Programming* : Chapitre sur les bibliothèques partagées
- *How To Write Shared Libraries* (Ulrich Drepper) : Guide technique approfondi

---

**🎯 Points clés à retenir** :
1. **ldd** = Lister les dépendances d'un exécutable (diagnostic)
2. **ldconfig** = Mettre à jour le cache système (gestion)
3. `ldd programme` pour voir les dépendances
4. `sudo ldconfig` après installation d'une bibliothèque
5. `ldconfig -p | grep biblio` pour chercher une bibliothèque dans le cache
6. Toujours vérifier avec `ldd` après compilation
7. Attention : `ldd` exécute partiellement le programme (utiliser `objdump` pour les binaires non fiables)

⏭️ [Création d'une API propre](/14-bibliotheques/06-creation-api-propre.md)
