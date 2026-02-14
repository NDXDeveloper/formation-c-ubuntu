🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 14.2 Bibliothèques dynamiques (.so)

## Introduction

Après avoir exploré les bibliothèques statiques, nous allons maintenant découvrir les **bibliothèques dynamiques** (ou **bibliothèques partagées**), identifiables par leur extension `.so` sous Linux (pour "Shared Object").

Contrairement aux bibliothèques statiques où le code est copié dans chaque exécutable, les bibliothèques dynamiques adoptent une approche radicalement différente : le code est **partagé** entre plusieurs programmes et chargé en mémoire **au moment de l'exécution**.

Ce concept, révolutionnaire à l'époque de son introduction, est aujourd'hui au cœur du fonctionnement de tous les systèmes d'exploitation modernes.

---

## Qu'est-ce qu'une bibliothèque dynamique ?

### Définition

Une **bibliothèque dynamique** est un fichier contenant du code compilé qui :
- N'est **pas intégré** dans l'exécutable lors de la compilation
- Est **chargé en mémoire** au moment où le programme s'exécute
- Peut être **partagé** par plusieurs programmes simultanément
- Peut être **mis à jour** sans recompiler les programmes qui l'utilisent

### Analogie : La bibliothèque municipale

**Bibliothèque statique** :
- Vous photocopiez tous les livres dont vous avez besoin
- Vous emportez les copies chez vous (dans votre exécutable)
- Si un livre est mis à jour, vous devez refaire toutes les copies

**Bibliothèque dynamique** :
- Vous gardez juste une note des livres dont vous avez besoin
- Quand vous en avez besoin, vous allez à la bibliothèque municipale
- La bibliothèque (mémoire partagée) a une seule copie de chaque livre
- Si un livre est mis à jour, tout le monde en profite immédiatement

---

## Extension et convention de nommage

### Linux : `.so` (Shared Object)

```
libmath.so
│      │
│      └─ Shared Object
└──────── Préfixe obligatoire "lib"
```

### Autres systèmes d'exploitation

| OS | Extension | Exemple |
|----|-----------|---------|
| **Linux** | `.so` | `libmath.so` |
| **macOS** | `.dylib` | `libmath.dylib` |
| **Windows** | `.dll` | `math.dll` |

**Note :** Sous Linux, on utilise parfois aussi `.so.VERSION` (ex: `libmath.so.1`) pour le versioning.

### Où les trouver ?

Les bibliothèques dynamiques système sont généralement dans :
- `/lib` : Bibliothèques essentielles au démarrage
- `/usr/lib` : Bibliothèques utilisateur standards
- `/usr/local/lib` : Bibliothèques installées localement
- `/opt/lib` : Bibliothèques optionnelles

**Lister les bibliothèques système :**
```bash
ls /usr/lib/*.so
```

---

## Comparaison : Statique vs Dynamique

### Schéma visuel

```
BIBLIOTHÈQUES STATIQUES (.a)          BIBLIOTHÈQUES DYNAMIQUES (.so)
════════════════════════════          ═══════════════════════════════

Compilation :                         Compilation :
┌──────────┐                          ┌──────────┐
│ main.c   │                          │ main.c   │
└────┬─────┘                          └────┬─────┘
     │                                     │
     │ + libmath.a                         │ référence libmath.so
     │                                     │
     ▼                                     ▼
┌─────────────────┐                   ┌──────────────┐
│  Exécutable     │                   │  Exécutable  │
│                 │                   │   (petit)    │
│  main()         │                   │              │
│  addition()     │◄─ Code copié      │  main()      │
│  mult()         │                   │  ─référence─►│
│  ...            │                   └──────────────┘
│                 │
│  5 MB           │                        100 KB
└─────────────────┘

Exécution :                           Exécution :
./programme                           ./programme
    │                                     │
    │ Tout est déjà là                    │ Charge libmath.so
    │                                     │
    ▼                                     ▼
Fonctionne                            ┌──────────────────┐  
immédiatement                         │  Mémoire RAM     │  
                                      │                  │
                                      │ programme (100K) │
                                      │ libmath.so (1M)  │◄─┐
                                      │                  │  │
                                      └──────────────────┘  │
                                                            │
                                      Autre programme peut  │
                                      utiliser la même .so ─┘
```

### Tableau comparatif détaillé

| Critère | Bibliothèque statique (.a) | Bibliothèque dynamique (.so) |
|---------|----------------------------|------------------------------|
| **Intégration** | Code copié dans l'exécutable | Chargé au runtime |
| **Taille exécutable** | Grande (5-50 MB) | Petite (50-500 KB) |
| **Dépendances** | Aucune | Nécessite le fichier .so |
| **Mémoire RAM** | Dupliquée par programme | Partagée entre programmes |
| **Mise à jour** | Recompilation nécessaire | Remplacement du .so suffit |
| **Vitesse démarrage** | Rapide | Légèrement plus lente |
| **Portabilité** | Excellente (autonome) | Moyenne (dépend de .so) |
| **Compilation** | Simple avec `-l` | Nécessite `-fPIC` |
| **Déploiement** | Un fichier unique | Plusieurs fichiers |
| **Plugins** | ❌ Impossible | ✅ Possible avec `dlopen()` |

---

## Le principe de partage en mémoire

### Sans bibliothèques partagées

```
Mémoire RAM :
┌──────────────────────────────────────────────┐
│  Programme 1 :                               │
│    Code application    : 2 MB                │
│    Code libc (copie 1) : 3 MB                │
│    Code libmath (c1)   : 1 MB                │
│  Total : 6 MB                                │
├──────────────────────────────────────────────┤
│  Programme 2 :                               │
│    Code application    : 3 MB                │
│    Code libc (copie 2) : 3 MB  ← Duplication!│
│    Code libmath (c2)   : 1 MB  ← Duplication!│
│  Total : 7 MB                                │
├──────────────────────────────────────────────┤
│  Programme 3 :                               │
│    Code application    : 1 MB                │
│    Code libc (copie 3) : 3 MB  ← Duplication!│
│    Code libmath (c3)   : 1 MB  ← Duplication!│
│  Total : 5 MB                                │
└──────────────────────────────────────────────┘
Total RAM utilisée : 18 MB
```

### Avec bibliothèques partagées

```
Mémoire RAM :
┌──────────────────────────────────────────────┐
│  Programme 1 : 2 MB                          │
│  Programme 2 : 3 MB                          │
│  Programme 3 : 1 MB                          │
├──────────────────────────────────────────────┤
│  libc.so.6    : 3 MB  ◄──┐                   │
│                          │ Tous partagent    │
│  libmath.so.1 : 1 MB  ◄──┤ ces copies uniques│
│                          │                   │
└──────────────────────────┴───────────────────┘
Total RAM utilisée : 10 MB (économie de 44%!)
```

**Avantage majeur :** Plus vous avez de programmes utilisant les mêmes bibliothèques, plus vous économisez de mémoire !

---

## Avantages des bibliothèques dynamiques

### ✅ 1. Économie de mémoire

**Exemple concret :**
- Firefox utilise `libc.so.6` (2 MB)
- LibreOffice utilise `libc.so.6` (2 MB)
- GIMP utilise `libc.so.6` (2 MB)

**Avec linking statique :** 3 × 2 MB = 6 MB en RAM  
**Avec linking dynamique :** 1 × 2 MB = 2 MB en RAM  

**Économie :** 4 MB pour cet exemple simple. Imaginez avec 50 programmes !

### ✅ 2. Mise à jour facilitée

**Scénario :** Une faille de sécurité est découverte dans `libssl.so.1`.

**Avec bibliothèques statiques :**
```bash
# Il faut recompiler TOUS les programmes !
recompile nginx  
recompile curl  
recompile openssh  
recompile apache  
recompile 100+ autres programmes...  
# Déployer tous ces nouveaux binaires
```

**Avec bibliothèques dynamiques :**
```bash
# Remplacer UN SEUL fichier
sudo apt update  
sudo apt upgrade libssl1.1  
# Redémarrer les services
# Tous les programmes utilisent automatiquement la version corrigée !
```

### ✅ 3. Taille d'exécutable réduite

**Exemple réel :**

```bash
# Programme avec linking statique
$ ls -lh programme_static
-rwxr-xr-x 1 user user 2.3M programme_static

# Même programme avec linking dynamique
$ ls -lh programme_dynamic
-rwxr-xr-x 1 user user 24K programme_dynamic
```

**Ratio :** 100 fois plus petit !

**Impact :**
- Téléchargement plus rapide
- Moins d'espace disque utilisé
- Mise en cache plus efficace

### ✅ 4. Système de plugins

Les bibliothèques dynamiques permettent le **chargement à la demande** :

```c
// Charger un plugin à l'exécution
void *plugin = dlopen("plugin_compress.so", RTLD_LAZY);  
void (*compress)(char*) = dlsym(plugin, "compress_data");  
compress("mes données");  
dlclose(plugin);  
```

**Exemples d'utilisation :**
- **Navigateurs** : Extensions Firefox, Chrome
- **Éditeurs** : Plugins VS Code, Vim
- **Lecteurs multimédia** : Codecs VLC
- **Jeux** : Mods et extensions

### ✅ 5. Compatibilité et coexistence

Plusieurs versions d'une même bibliothèque peuvent coexister :

```
/usr/lib/
├── libmath.so.1 → libmath.so.1.2.3  (ancienne version)
└── libmath.so.2 → libmath.so.2.0.0  (nouvelle version)
```

- Anciens programmes continuent d'utiliser `libmath.so.1`
- Nouveaux programmes utilisent `libmath.so.2`
- **Pas de conflit !**

---

## Inconvénients des bibliothèques dynamiques

### ❌ 1. "Dependency Hell" (L'enfer des dépendances)

**Problème classique :**
```
$ ./programme
error while loading shared libraries: libmath.so.1: cannot open shared object file: No such file or directory
```

**Causes :**
- La bibliothèque n'est pas installée
- Mauvaise version installée
- Mauvais chemin de recherche

**Solution (complexe) :**
```bash
# Installer la bibliothèque manquante
sudo apt install libmath1

# Ou ajouter le chemin
export LD_LIBRARY_PATH=/path/to/lib
```

### ❌ 2. Démarrage plus lent

Le loader dynamique doit :
1. Trouver les bibliothèques
2. Les charger en mémoire
3. Résoudre les symboles
4. Mettre à jour les adresses

**Impact :**
- Quelques millisecondes pour un petit programme
- Peut atteindre 100-200 ms pour des applications complexes

### ❌ 3. Risques de compatibilité

**Scénario problématique :**

```
Programme compilé en 2020 :
    → Utilise libssl.so.1.0 (interface v1)

Système en 2025 :
    → Seulement libssl.so.3.0 disponible (interface v3, incompatible)

Résultat : Programme ne démarre pas ! 💥
```

### ❌ 4. Sécurité : Attaques par substitution

Un attaquant peut tenter de charger une bibliothèque malveillante :

```bash
# Mauvais acteur crée une fausse libc
LD_PRELOAD=/tmp/fake_libc.so /usr/bin/sudo
```

**Protection :** Les binaires setuid/setgid ignorent `LD_PRELOAD` et `LD_LIBRARY_PATH`.

### ❌ 5. Debugging plus complexe

Avec les bibliothèques dynamiques :
- Le code est réparti en plusieurs fichiers
- Les adresses changent à chaque exécution (ASLR)
- Le stack trace traverse plusieurs bibliothèques

```
Stacktrace typique :
#0  0x7f123456789 in ??? from libmath.so.1
#1  0x7f987654321 in ??? from libutils.so.2
#2  0x5555555556 in main ()
```

---

## Quand utiliser les bibliothèques dynamiques ?

### ✅ Cas d'usage idéaux

#### 1. **Applications système standards**

```bash
# Firefox, LibreOffice, GIMP, etc.
ls /usr/bin/firefox  
ldd /usr/bin/firefox | wc -l  
# Sortie : 150+ bibliothèques !
```

Toutes ces applications partagent les mêmes bibliothèques système.

#### 2. **Systèmes avec de nombreux programmes**

Un serveur Linux typique :
- 500+ processus actifs
- Tous utilisent `libc.so.6`, `libssl.so`, etc.
- Économie de RAM : plusieurs GB !

#### 3. **Applications nécessitant des mises à jour fréquentes**

```bash
# Mise à jour de sécurité
apt upgrade libssl1.1
# Tous les programmes en bénéficient sans recompilation !
```

#### 4. **Systèmes de plugins**

- Navigateurs (extensions)
- Serveurs (modules Apache, Nginx)
- Applications graphiques (filtres Photoshop, GIMP)

#### 5. **Distributions Linux standard**

Tous les packages `.deb` (Debian/Ubuntu) et `.rpm` (RedHat/Fedora) utilisent des bibliothèques dynamiques.

### ❌ Cas où éviter

#### 1. **Outils CLI standalone**

Pour un outil que vous distribuez indépendamment :

```bash
# Version statique : fonctionne partout
curl -L https://example.com/mytool -o mytool  
chmod +x mytool  
./mytool  # Fonctionne immédiatement !
```

#### 2. **Systèmes embarqués contraints**

- Pas de gestionnaire de packages
- Espace disque limité (mais pas de partage possible)
- Besoin de contrôle total

#### 3. **Conteneurs Docker minimalistes**

```dockerfile
FROM scratch  
COPY programme_statique /  
ENTRYPOINT ["/programme_statique"]  
```

Un conteneur de 5 MB au lieu de 500 MB !

#### 4. **Environnements hétérogènes**

Déploiement sur :
- Ubuntu 18.04, 20.04, 22.04
- Debian 10, 11, 12
- CentOS 7, 8, Rocky Linux

**Problème :** Différentes versions de `glibc`, `libssl`, etc.

**Solution :** Linking statique ou conteneurisation.

---

## Concepts clés à comprendre

### 1. Le loader dynamique (`ld.so`)

C'est le programme qui :
- Charge les bibliothèques en mémoire
- Résout les symboles (trouve les adresses des fonctions)
- Connecte tout ensemble

**Localisation :**
```bash
ls -l /lib64/ld-linux-x86-64.so.2
```

**Voir le loader d'un programme :**
```bash
readelf -l /bin/ls | grep interpreter
```

### 2. SONAME (Shared Object Name)

Le **nom de compatibilité** d'une bibliothèque :

```
libmath.so.1.2.3   ← Real name (fichier physique)  
libmath.so.1       ← SONAME (compatibilité)  
libmath.so         ← Linker name (développement)  
```

**Règle :** Même SONAME = binaire-compatible

### 3. Résolution de symboles

Le processus de "connecter" les appels de fonctions aux adresses réelles :

```
Programme :                Bibliothèque :
┌──────────────┐          ┌──────────────┐
│ call ???     │          │ addition:    │
│              │──résolu─►│   code...    │
│              │          │   ret        │
└──────────────┘          └──────────────┘
```

### 4. PIC (Position Independent Code)

Code compilé pour fonctionner **quelle que soit son adresse en mémoire** :

```
Sans PIC :
  jmp 0x1234  ← Adresse fixe (ne marche qu'à une position)

Avec PIC :
  jmp [rip+offset]  ← Adresse relative (marche partout)
```

---

## Exemple conceptuel : Création et utilisation

### Vue d'ensemble du workflow

```
┌─────────────────────────────────────────────────────┐
│           CRÉATION D'UNE BIBLIOTHÈQUE .so           │
└─────────────────────────────────────────────────────┘

Étape 1: Écriture du code
┌──────────┐  ┌──────────┐  ┌──────────┐
│ func1.c  │  │ func2.c  │  │ header.h │
└──────────┘  └──────────┘  └──────────┘
     │             │              │
     └─────┬───────┘              │
           ▼                      │
Étape 2: Compilation avec -fPIC   │
      gcc -fPIC -c                │
           │                      │
     ┌─────┴─────┐                │
     ▼           ▼                │
┌──────────┐  ┌──────────┐        │
│ func1.o  │  │ func2.o  │        │
│  (PIC)   │  │  (PIC)   │        │
└──────────┘  └──────────┘        │
     │             │              │
     └─────┬───────┘              │
           ▼                      │
Étape 3: Linking avec -shared     │
      gcc -shared                 │
           │                      │
           ▼                      │
    ┌─────────────┐               │
    │ libutils.so │               │
    └─────────────┘               │
           │                      │
           └──────────┬───────────┘
                      ▼
         ┌─────────────────────┐
         │  DISTRIBUTION       │
         │  - libutils.so      │
         │  - header.h         │
         └─────────────────────┘

┌─────────────────────────────────────────────────────┐
│        UTILISATION DE LA BIBLIOTHÈQUE .so           │
└─────────────────────────────────────────────────────┘

Projet utilisateur:
┌──────────┐  ┌─────────────┐  ┌──────────┐
│ main.c   │  │ libutils.so │  │ header.h │
└──────────┘  └─────────────┘  └──────────┘
     │             │              │
     │             │              └─> #include
     └─────┬───────┘
           ▼
   Compilation et linking
   gcc main.c -lutils -L.
           │
           ▼
    ┌─────────────┐
    │ programme   │ ← Petit exécutable
    │ (léger)     │   avec référence à
    └─────────────┘   libutils.so
           │
           ▼
    Exécution ./programme
           │
           ▼
    ┌──────────────────────┐
    │ Loader dynamique :   │
    │ 1. Cherche libutils  │
    │ 2. Charge en mémoire │
    │ 3. Résout symboles   │
    │ 4. Lance main()      │
    └──────────────────────┘
```

### Commandes de base (aperçu)

```bash
# Créer une bibliothèque dynamique
gcc -fPIC -shared math.c -o libmath.so

# Compiler un programme qui l'utilise
gcc main.c -lmath -L. -o programme

# Exécuter (en spécifiant où trouver la .so)
LD_LIBRARY_PATH=. ./programme

# Vérifier les dépendances
ldd programme
```

---

## Écosystème des bibliothèques dynamiques

### Sur un système Linux typique

```bash
# Compter les bibliothèques partagées
find /usr/lib -name "*.so*" | wc -l
```

Résultat typique : **3000-5000 bibliothèques !**

### Bibliothèques essentielles

| Bibliothèque | Rôle | Utilisée par |
|--------------|------|--------------|
| **libc.so.6** | Bibliothèque C standard | Presque tous les programmes |
| **libm.so.6** | Fonctions mathématiques | Applications scientifiques |
| **libpthread.so.0** | POSIX threads | Applications multithread |
| **libdl.so.2** | Chargement dynamique | Plugins, extensions |
| **libssl.so** | Cryptographie SSL/TLS | Navigateurs, curl, ssh |
| **libX11.so** | Serveur graphique X11 | Applications GUI Linux |
| **libGL.so** | OpenGL | Jeux, applications 3D |

### Gestionnaires de packages

Les bibliothèques dynamiques sont au cœur des systèmes de packages :

**Debian/Ubuntu (.deb) :**
```bash
# Installer une bibliothèque
apt install libssl1.1

# Voir quels programmes en dépendent
apt-cache rdepends libssl1.1
```

**RedHat/Fedora (.rpm) :**
```bash
# Installer une bibliothèque
dnf install openssl-libs

# Voir les dépendances
rpm -q --requires openssl-libs
```

---

## Les trois niveaux de nommage

Comme vu précédemment avec les bibliothèques statiques, les `.so` suivent une convention de nommage stricte, mais plus complexe :

### 1. Linker name (développement)

```
libmath.so
```

Utilisé lors de la compilation avec `-lmath`.

### 2. SONAME (runtime)

```
libmath.so.1
```

Version **majeure** seulement. Garantit la compatibilité binaire.

### 3. Real name (fichier physique)

```
libmath.so.1.2.3
```

Version **complète** : MAJOR.MINOR.PATCH

### Relations via liens symboliques

```bash
libmath.so → libmath.so.1 → libmath.so.1.2.3
    │            │                 │
    │            │                 └─ Fichier réel
    │            └─ SONAME (compatibilité runtime)
    └─ Linker name (compilation)
```

Cette structure sera expliquée en détail dans les sections suivantes.

---

## Différences techniques avec les bibliothèques statiques

| Aspect | Statique (.a) | Dynamique (.so) |
|--------|---------------|-----------------|
| **Format** | Archive (ar) | ELF shared object |
| **Compilation** | Normale | Nécessite `-fPIC` |
| **Linking** | Copie du code | Référence à la .so |
| **Symboles** | Tous inclus | Résolution au runtime |
| **Réutilisation** | Pas de partage | Partagée en mémoire |
| **Mise à jour** | Recompilation | Remplacement du fichier |
| **Chargement** | À la compilation | Au runtime |
| **Taille binaire** | Grande | Petite |
| **Dépendances** | Aucune | Fichier .so requis |

---

## Outils de base pour les `.so`

### Inspecter une bibliothèque

```bash
# Type de fichier
file libmath.so

# Symboles exportés
nm -D libmath.so

# SONAME intégré
readelf -d libmath.so | grep SONAME

# Dépendances
ldd libmath.so
```

### Inspecter un programme

```bash
# Bibliothèques requises
ldd programme

# Dépendances en détail
readelf -d programme | grep NEEDED

# Vérifier l'exécution
LD_DEBUG=libs ./programme 2>&1 | less
```

---

## Prérequis pour la suite

Avant de plonger dans les détails techniques des sections suivantes, assurez-vous de comprendre :

1. ✅ **La différence fondamentale** : statique = copie, dynamique = référence
2. ✅ **Le concept de partage mémoire** : plusieurs programmes, une seule copie
3. ✅ **Les trois noms** : linker name, SONAME, real name
4. ✅ **Le rôle du loader** : programme qui charge les bibliothèques
5. ✅ **Les avantages/inconvénients** : quand utiliser quel type

---

## Vue d'ensemble des sections suivantes

Dans les sections à venir, nous allons explorer :

### 14.2.1 - Compilation avec `-fPIC`
- Pourquoi le code doit être "position independent"
- Comment compiler correctement pour créer une `.so`
- Les détails techniques du PIC

### 14.2.2 - Versioning et SONAME
- Comment versionner vos bibliothèques
- Le mécanisme du SONAME en détail
- Gérer les mises à jour et la compatibilité

### 14.2.3 - Résolution de symboles au runtime
- Comment le loader trouve et charge les bibliothèques
- Le mécanisme PLT/GOT
- Lazy vs eager binding
- Variables d'environnement (`LD_LIBRARY_PATH`, etc.)

---

## Récapitulatif

Les bibliothèques dynamiques sont un pilier fondamental des systèmes d'exploitation modernes. Elles permettent :

### Caractéristiques essentielles

| Aspect | Détail |
|--------|--------|
| **Extension** | `.so` (Linux), `.dylib` (macOS), `.dll` (Windows) |
| **Convention** | `lib<nom>.so[.version]` |
| **Intégration** | Chargées au runtime, non copiées |
| **Partage** | Une copie en mémoire pour tous |
| **Mise à jour** | Remplacement sans recompilation |
| **Taille** | Exécutables plus petits |
| **Complexité** | Plus grande (dépendances) |

### Points clés

- ✅ Les bibliothèques dynamiques **économisent de la mémoire** en partageant le code
- ✅ Elles permettent des **mises à jour sans recompilation**
- ✅ Elles réduisent la **taille des exécutables**
- ✅ Elles permettent les **systèmes de plugins**
- ✅ Mais elles ajoutent de la **complexité** (dépendances)
- ✅ Le **loader dynamique** gère le chargement au runtime
- ✅ Le **SONAME** assure la compatibilité binaire
- ✅ La compilation nécessite **`-fPIC`** (Position Independent Code)

---

## Transition vers les détails techniques

Maintenant que vous comprenez **pourquoi** et **quand** utiliser les bibliothèques dynamiques, nous allons voir **comment** les créer et les utiliser en pratique.

La prochaine section commence par un concept fondamental : le **Position Independent Code (PIC)**, qui est la clé pour comprendre comment les bibliothèques dynamiques fonctionnent techniquement. 🚀

⏭️ [Compilation avec -fPIC](/14-bibliotheques/02.1-compilation-fpic.md)
