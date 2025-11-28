🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 31.9 Distribution de bibliothèques partagées

## Introduction

Jusqu'à présent, nous avons distribué des **applications complètes** (packages, AppImages, conteneurs). Mais que faire quand vous développez une **bibliothèque** destinée à être utilisée par d'autres développeurs ?

Dans cette section, nous allons explorer comment créer, versionner et distribuer des **bibliothèques partagées** (shared libraries, fichiers `.so` sur Linux).

### Qu'est-ce qu'une bibliothèque partagée ?

Une **bibliothèque partagée** est un fichier contenant du code compilé qui peut être :
- ✅ Utilisé par plusieurs programmes simultanément
- ✅ Chargé en mémoire une seule fois (partagé entre processus)
- ✅ Mis à jour indépendamment des applications qui l'utilisent

**Exemples de bibliothèques partagées courantes :**
- `libc.so.6` - Bibliothèque C standard
- `libcurl.so.4` - Client HTTP/FTP
- `libssl.so.3` - Cryptographie SSL/TLS
- `libpthread.so.0` - Support des threads POSIX

## Bibliothèques statiques vs partagées

### Différences fondamentales

| Aspect | Statique (.a) | Partagée (.so) |
|--------|---------------|----------------|
| **Linkage** | À la compilation | À l'exécution |
| **Taille binaire** | Grande (code inclus) | Petite (référence) |
| **Mémoire** | Dupliquée par processus | Partagée |
| **Mises à jour** | Recompilation nécessaire | Transparentes |
| **Dépendances** | Aucune au runtime | Doit trouver le .so |
| **Portabilité** | Excellente | Peut nécessiter installation |

### Illustration

**Avec bibliothèque statique :**

```
Programme A (10 MB)           Programme B (10 MB)
├── Code app (2 MB)          ├── Code app (2 MB)
└── libmath.a (8 MB) ← ─┐    └── libmath.a (8 MB) ← ─┐
                         │                            │
                         └────── Code dupliqué ──────┘

Mémoire totale : 20 MB
```

**Avec bibliothèque partagée :**

```
Programme A (2 MB)            Programme B (2 MB)
├── Code app (2 MB)          ├── Code app (2 MB)
└── Référence → libmath.so   └── Référence → libmath.so
                         ↓                      ↓
                    libmath.so (8 MB)
                    [Chargée une seule fois]

Mémoire totale : 12 MB
```

## Créer une bibliothèque partagée simple

### Structure du projet

```
libmath/
├── include/
│   └── mymath.h
├── src/
│   ├── add.c
│   ├── multiply.c
│   └── power.c
├── Makefile
└── README.md
```

### Fichier d'en-tête

**include/mymath.h**

```c
#ifndef MYMATH_H
#define MYMATH_H

// API publique de la bibliothèque
int add(int a, int b);
int multiply(int a, int b);
int power(int base, int exponent);

// Obtenir la version de la bibliothèque
const char* mymath_version(void);

#endif /* MYMATH_H */
```

### Implémentations

**src/add.c**

```c
int add(int a, int b) {
    return a + b;
}
```

**src/multiply.c**

```c
int multiply(int a, int b) {
    return a * b;
}
```

**src/power.c**

```c
int power(int base, int exponent) {
    int result = 1;
    for (int i = 0; i < exponent; i++) {
        result *= base;
    }
    return result;
}

const char* mymath_version(void) {
    return "1.0.0";
}
```

### Makefile pour bibliothèque partagée

**Makefile**

```makefile
# Configuration
LIB_NAME = mymath
VERSION = 1.0.0
MAJOR = 1
MINOR = 0
PATCH = 0

# Fichiers
SOURCES = src/add.c src/multiply.c src/power.c
OBJECTS = $(SOURCES:.c=.o)
HEADER = include/mymath.h

# Noms des bibliothèques
LIB_SO = lib$(LIB_NAME).so
LIB_SO_VERSION = $(LIB_SO).$(VERSION)
LIB_SO_MAJOR = $(LIB_SO).$(MAJOR)

# Compilateur et flags
CC = gcc
CFLAGS = -Wall -Werror -O2 -fPIC -Iinclude
LDFLAGS = -shared

# Répertoires d'installation
PREFIX ?= /usr/local
LIBDIR = $(PREFIX)/lib
INCLUDEDIR = $(PREFIX)/include

# Règle par défaut
all: $(LIB_SO_VERSION)

# Compilation des fichiers objets avec -fPIC
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Création de la bibliothèque partagée
$(LIB_SO_VERSION): $(OBJECTS)
	$(CC) $(LDFLAGS) -Wl,-soname,$(LIB_SO_MAJOR) -o $@ $(OBJECTS)
	ln -sf $(LIB_SO_VERSION) $(LIB_SO_MAJOR)
	ln -sf $(LIB_SO_MAJOR) $(LIB_SO)

# Installation
install: $(LIB_SO_VERSION)
	install -d $(DESTDIR)$(LIBDIR)
	install -d $(DESTDIR)$(INCLUDEDIR)
	install -m 755 $(LIB_SO_VERSION) $(DESTDIR)$(LIBDIR)/
	ln -sf $(LIB_SO_VERSION) $(DESTDIR)$(LIBDIR)/$(LIB_SO_MAJOR)
	ln -sf $(LIB_SO_MAJOR) $(DESTDIR)$(LIBDIR)/$(LIB_SO)
	install -m 644 $(HEADER) $(DESTDIR)$(INCLUDEDIR)/
	ldconfig -n $(DESTDIR)$(LIBDIR)

# Désinstallation
uninstall:
	rm -f $(DESTDIR)$(LIBDIR)/$(LIB_SO)*
	rm -f $(DESTDIR)$(INCLUDEDIR)/mymath.h
	ldconfig

# Nettoyage
clean:
	rm -f $(OBJECTS) $(LIB_SO)* *.o

.PHONY: all install uninstall clean
```

### Explication des flags importants

#### -fPIC (Position Independent Code)

```bash
# OBLIGATOIRE pour les bibliothèques partagées
gcc -fPIC -c source.c -o source.o
```

**Pourquoi ?** Le code PIC peut être chargé à n'importe quelle adresse mémoire, ce qui est essentiel pour les bibliothèques partagées.

#### -shared

```bash
# Créer une bibliothèque partagée
gcc -shared -o libmymath.so objects.o
```

**Rôle :** Indique au linker de créer une bibliothèque partagée et non un exécutable.

#### -Wl,-soname

```bash
gcc -shared -Wl,-soname,libmymath.so.1 -o libmymath.so.1.0.0 objects.o
```

**Rôle :** Définit le SONAME (voir section suivante).

### Build et test

```bash
# Compiler
make

# Résultat :
# libmymath.so.1.0.0  (fichier réel)
# libmymath.so.1      (symlink → libmymath.so.1.0.0)
# libmymath.so        (symlink → libmymath.so.1)

# Vérifier le SONAME
readelf -d libmymath.so.1.0.0 | grep SONAME
# 0x000000000000000e (SONAME)    Library soname: [libmymath.so.1]

# Lister les symboles exportés
nm -D libmymath.so.1.0.0
```

## SONAME : Versioning des bibliothèques

### Qu'est-ce que le SONAME ?

Le **SONAME** (Shared Object Name) est un mécanisme de versioning qui permet la compatibilité binaire entre différentes versions d'une bibliothèque.

### Schéma de versioning

```
libmymath.so.MAJOR.MINOR.PATCH

Exemple : libmymath.so.1.2.3
```

**Règles de versioning :**

| Changement | Version | Compatibilité | SONAME |
|------------|---------|---------------|--------|
| Correction de bug | PATCH (1.0.0 → 1.0.1) | ✅ Compatible | Inchangé (1) |
| Nouvelle fonctionnalité | MINOR (1.0.0 → 1.1.0) | ✅ Compatible | Inchangé (1) |
| Changement incompatible | MAJOR (1.0.0 → 2.0.0) | ❌ Incompatible | Change (2) |

### Structure des liens symboliques

```
libmymath.so           → libmymath.so.1         [Lien de développement]
libmymath.so.1         → libmymath.so.1.2.3     [SONAME]
libmymath.so.1.2.3                              [Fichier réel]
```

**Utilisation :**

- **libmymath.so** : Utilisé par le linker à la compilation (`-lmymath`)
- **libmymath.so.1** : SONAME, référencé dans les binaires
- **libmymath.so.1.2.3** : Fichier réel, la version spécifique installée

### Pourquoi c'est important ?

**Scénario :**

```bash
# Application compilée avec libmymath 1.0.0
$ ldd mon-app
    libmymath.so.1 => /usr/lib/libmymath.so.1.0.0

# Plus tard, mise à jour vers 1.2.3 (bug fixes)
$ sudo apt-get upgrade libmymath

# L'application fonctionne toujours !
$ ldd mon-app
    libmymath.so.1 => /usr/lib/libmymath.so.1.2.3

# Le SONAME (.1) est le même, donc compatibilité assurée
```

### Création avec SONAME

```bash
# Compiler avec SONAME
gcc -shared -Wl,-soname,libmymath.so.1 \
    -o libmymath.so.1.2.3 \
    add.o multiply.o power.o

# Créer les liens symboliques
ln -sf libmymath.so.1.2.3 libmymath.so.1
ln -sf libmymath.so.1 libmymath.so

# Vérifier
readelf -d libmymath.so.1.2.3 | grep SONAME
```

## Utiliser une bibliothèque partagée

### Programme exemple

**test_mymath.c**

```c
#include <stdio.h>
#include <mymath.h>

int main(void) {
    printf("mymath version: %s\n", mymath_version());
    printf("2 + 3 = %d\n", add(2, 3));
    printf("4 * 5 = %d\n", multiply(4, 5));
    printf("2^8 = %d\n", power(2, 8));
    return 0;
}
```

### Compilation avec la bibliothèque

```bash
# Méthode 1 : Spécifier le chemin de la bibliothèque
gcc test_mymath.c -o test_mymath -L. -lmymath -Iinclude

# Options :
# -L.        : Chercher les bibliothèques dans le répertoire courant
# -lmymath   : Lier avec libmymath.so
# -Iinclude  : Chercher les headers dans include/

# Exécution (nécessite LD_LIBRARY_PATH)
LD_LIBRARY_PATH=. ./test_mymath
```

### Le problème du chargeur dynamique

```bash
# Tentative d'exécution sans LD_LIBRARY_PATH
./test_mymath

# Erreur :
# error while loading shared libraries: libmymath.so.1:
# cannot open shared object file: No such file or directory
```

**Pourquoi ?** Le chargeur dynamique (`ld.so`) ne trouve pas la bibliothèque.

### Solutions au problème de chargement

#### Solution 1 : LD_LIBRARY_PATH (temporaire)

```bash
# Définir la variable d'environnement
export LD_LIBRARY_PATH=/path/to/lib:$LD_LIBRARY_PATH

# Exécuter
./test_mymath
```

**Inconvénient :** Temporaire, doit être redéfini à chaque session.

#### Solution 2 : Installation système

```bash
# Installer la bibliothèque
sudo make install

# Met les fichiers dans :
# /usr/local/lib/libmymath.so*
# /usr/local/include/mymath.h

# Mettre à jour le cache du chargeur
sudo ldconfig

# Maintenant ça fonctionne
./test_mymath
```

#### Solution 3 : RPATH (chemin embarqué)

```bash
# Compiler avec RPATH
gcc test_mymath.c -o test_mymath \
    -L. -lmymath -Iinclude \
    -Wl,-rpath,/path/to/lib

# Le chemin est embarqué dans le binaire
readelf -d test_mymath | grep RPATH
# 0x000000000000000f (RPATH)    Library rpath: [/path/to/lib]

# Fonctionne sans LD_LIBRARY_PATH
./test_mymath
```

#### Solution 4 : Ajouter au système

```bash
# Créer un fichier de configuration
sudo sh -c 'echo "/opt/mylibs" > /etc/ld.so.conf.d/mylibs.conf'

# Mettre à jour le cache
sudo ldconfig

# Vérifier
ldconfig -p | grep mymath
```

### Ordre de recherche des bibliothèques

Le chargeur dynamique cherche dans cet ordre :

1. **RPATH** (embarqué dans le binaire)
2. **LD_LIBRARY_PATH** (variable d'environnement)
3. **RUNPATH** (embarqué, priorité plus basse)
4. **Cache système** (`/etc/ld.so.cache`)
5. **Chemins par défaut** (`/lib`, `/usr/lib`)

```bash
# Voir où sont trouvées les bibliothèques
ldd ./test_mymath

# Résultat :
# libmymath.so.1 => /usr/local/lib/libmymath.so.1.2.3
# libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6
```

## pkg-config : Faciliter l'utilisation

### Qu'est-ce que pkg-config ?

**pkg-config** est un outil qui fournit des informations sur les bibliothèques installées :
- Chemins d'inclusion (`-I`)
- Chemins de bibliothèques (`-L`)
- Flags de linkage (`-l`)
- Version

### Créer un fichier .pc

**mymath.pc.in** (template)

```
prefix=@PREFIX@
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: MyMath
Description: Bibliothèque mathématique de démonstration
Version: @VERSION@
Requires:
Libs: -L${libdir} -lmymath
Cflags: -I${includedir}
```

**Générer mymath.pc :**

```bash
# Remplacer les variables
sed -e "s|@PREFIX@|/usr/local|g" \
    -e "s|@VERSION@|1.2.3|g" \
    mymath.pc.in > mymath.pc

# Installer
sudo mkdir -p /usr/local/lib/pkgconfig
sudo install -m 644 mymath.pc /usr/local/lib/pkgconfig/
```

**Ou dans le Makefile :**

```makefile
install: $(LIB_SO_VERSION)
	# ... installation de la lib ...

	# Créer et installer le fichier .pc
	sed -e "s|@PREFIX@|$(PREFIX)|g" \
	    -e "s|@VERSION@|$(VERSION)|g" \
	    mymath.pc.in > mymath.pc
	install -d $(DESTDIR)$(LIBDIR)/pkgconfig
	install -m 644 mymath.pc $(DESTDIR)$(LIBDIR)/pkgconfig/
```

### Utiliser pkg-config

```bash
# Vérifier la présence de la bibliothèque
pkg-config --exists mymath && echo "Trouvée"

# Obtenir la version
pkg-config --modversion mymath
# 1.2.3

# Obtenir les flags de compilation
pkg-config --cflags mymath
# -I/usr/local/include

# Obtenir les flags de linkage
pkg-config --libs mymath
# -L/usr/local/lib -lmymath

# Compiler facilement
gcc test_mymath.c -o test_mymath \
    $(pkg-config --cflags --libs mymath)
```

### Dans un Makefile

```makefile
# Utiliser pkg-config
CFLAGS += $(shell pkg-config --cflags mymath)
LDFLAGS += $(shell pkg-config --libs mymath)

test: test_mymath.c
	$(CC) $(CFLAGS) test_mymath.c -o test $(LDFLAGS)
```

### Dans CMake

```cmake
# Trouver la bibliothèque avec pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(MYMATH REQUIRED mymath)

# Utiliser les variables générées
include_directories(${MYMATH_INCLUDE_DIRS})
link_directories(${MYMATH_LIBRARY_DIRS})

add_executable(test test_mymath.c)
target_link_libraries(test ${MYMATH_LIBRARIES})
```

## Visibilité des symboles

### Contrôler les symboles exportés

Par défaut, **tous les symboles** sont exportés. Pour une meilleure encapsulation :

**mymath.h**

```c
#ifndef MYMATH_H
#define MYMATH_H

// Macro pour la visibilité
#if defined(_WIN32) || defined(__CYGWIN__)
  #define MYMATH_EXPORT __declspec(dllexport)
#else
  #if __GNUC__ >= 4
    #define MYMATH_EXPORT __attribute__((visibility("default")))
  #else
    #define MYMATH_EXPORT
  #endif
#endif

// API publique
MYMATH_EXPORT int add(int a, int b);
MYMATH_EXPORT int multiply(int a, int b);
MYMATH_EXPORT int power(int base, int exponent);
MYMATH_EXPORT const char* mymath_version(void);

#endif
```

**Compiler avec visibilité par défaut hidden :**

```bash
gcc -fPIC -fvisibility=hidden -c source.c
```

**Avantages :**
- ✅ Réduit la taille de la bibliothèque
- ✅ Améliore les performances (moins de symboles à résoudre)
- ✅ Évite les conflits de symboles
- ✅ Encapsulation propre (comme `static` mais pour les .so)

### Vérifier les symboles exportés

```bash
# Lister tous les symboles
nm -D libmymath.so

# Symboles visibles uniquement
nm -D libmymath.so | grep ' T '

# Avec readelf
readelf -s libmymath.so | grep FUNC
```

## Distribution de bibliothèques

### Méthode 1 : Package système (.deb)

**Structure pour package Debian :**

```
libmymath-1.2.3/
├── debian/
│   ├── control
│   ├── rules
│   ├── libmymath1.install        # Bibliothèque runtime
│   └── libmymath-dev.install     # Headers et .so de dev
├── include/
│   └── mymath.h
├── src/
└── Makefile
```

**debian/control**

```
Source: libmymath
Section: libs
Priority: optional

Package: libmymath1
Architecture: any
Depends: ${shlibs:Depends}, ${misc:Depends}
Description: Bibliothèque mathématique - runtime
 Bibliothèque partagée pour les fonctions mathématiques.

Package: libmymath-dev
Section: libdevel
Architecture: any
Depends: libmymath1 (= ${binary:Version}), ${misc:Depends}
Description: Bibliothèque mathématique - fichiers de développement
 Headers et fichiers nécessaires pour développer avec libmymath.
```

**debian/libmymath1.install**

```
usr/lib/*/libmymath.so.*
```

**debian/libmymath-dev.install**

```
usr/lib/*/libmymath.so
usr/include/*
usr/lib/*/pkgconfig/*
```

**Résultat :**
- `libmymath1_1.2.3_amd64.deb` - Pour utiliser la bibliothèque
- `libmymath-dev_1.2.3_amd64.deb` - Pour développer avec

### Méthode 2 : Package RPM

**libmymath.spec**

```spec
Name:           libmymath
Version:        1.2.3
Release:        1%{?dist}
Summary:        Bibliothèque mathématique

License:        MIT
URL:            https://github.com/user/libmymath
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  gcc make

%description
Bibliothèque partagée pour les fonctions mathématiques.

%package        devel
Summary:        Fichiers de développement pour %{name}
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description    devel
Headers et fichiers nécessaires pour développer avec libmymath.

%prep
%setup -q

%build
make %{?_smp_mflags}

%install
make install DESTDIR=%{buildroot} PREFIX=%{_prefix}

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%license LICENSE
%doc README.md
%{_libdir}/libmymath.so.*

%files devel
%{_libdir}/libmymath.so
%{_includedir}/mymath.h
%{_libdir}/pkgconfig/mymath.pc

%changelog
* Mon Nov 28 2024 Nom <email> - 1.2.3-1
- Release 1.2.3
```

### Méthode 3 : Installation depuis source

**Script d'installation standard :**

```bash
# Workflow classique
./configure --prefix=/usr/local
make
make check
sudo make install
sudo ldconfig
```

**Avec CMake :**

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(mymath VERSION 1.2.3 LANGUAGES C)

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# Sources
add_library(mymath src/add.c src/multiply.c src/power.c)

# Headers publics
target_include_directories(mymath PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Propriétés de version
set_target_properties(mymath PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER include/mymath.h
)

# Installation
include(GNUInstallDirs)

install(TARGETS mymath
    EXPORT mymathTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Générer le fichier .pc
configure_file(mymath.pc.in mymath.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/mymath.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# Export pour find_package()
install(EXPORT mymathTargets
    FILE mymathTargets.cmake
    NAMESPACE mymath::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mymath
)

# Config pour CMake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    mymathConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/mymathConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mymath
)
```

**Utilisation :**

```bash
mkdir build && cd build
cmake ..
make
sudo make install
sudo ldconfig
```

## Cross-compilation de bibliothèques

### Compiler pour ARM

```bash
# Compiler pour ARM64
aarch64-linux-gnu-gcc -fPIC -shared \
    -Wl,-soname,libmymath.so.1 \
    -o libmymath.so.1.2.3 \
    add.c multiply.c power.c

# Vérifier l'architecture
file libmymath.so.1.2.3
# ELF 64-bit LSB shared object, ARM aarch64
```

### Avec CMake et toolchain

```bash
# Utiliser un fichier toolchain
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-arm64.cmake ..
make

# Résultat : bibliothèque ARM64
```

## Compatibilité ABI (Application Binary Interface)

### Qu'est-ce que l'ABI ?

L'**ABI** définit comment les programmes interagissent au niveau binaire :
- Taille des types
- Alignement en mémoire
- Conventions d'appel
- Layout des structures

### Changements qui cassent l'ABI

**❌ Changements incompatibles (nécessitent MAJOR++) :**

```c
// Version 1.x
typedef struct {
    int x;
    int y;
} Point;

// Version 2.0 - INCOMPATIBLE !
typedef struct {
    int x;
    int y;
    int z;  // Nouveau champ !
} Point;
```

```c
// Version 1.x
int calculate(int a, int b);

// Version 2.0 - INCOMPATIBLE !
int calculate(int a, int b, int c);  // Nouveau paramètre !
```

**✅ Changements compatibles (MINOR++) :**

```c
// Ajout de nouvelles fonctions
int new_function(int x);

// Changement d'implémentation interne (pas de changement d'interface)
```

### Vérifier la compatibilité ABI

```bash
# Installer abi-compliance-checker
sudo apt-get install abi-compliance-checker

# Créer un descripteur XML
cat > libmymath.xml << 'EOF'
<version>1.0.0</version>
<headers>include/mymath.h</headers>
<libs>libmymath.so.1.0.0</libs>
EOF

# Comparer deux versions
abi-compliance-checker -lib mymath \
    -old libmymath-1.0.xml \
    -new libmymath-2.0.xml
```

## Débogage de bibliothèques partagées

### Problèmes courants

#### Erreur : "cannot open shared object file"

```bash
# Vérifier où le binaire cherche
ldd ./mon-programme

# Résultat :
# libmymath.so.1 => not found

# Solutions :
# 1. LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/path/to/lib

# 2. Ajouter au cache système
sudo sh -c 'echo "/path/to/lib" > /etc/ld.so.conf.d/mylib.conf'
sudo ldconfig

# 3. Compiler avec RPATH
gcc -Wl,-rpath,/path/to/lib ...
```

#### Erreur : "version not found"

```bash
# Le binaire demande une version spécifique
./mon-programme
# error: version `GLIBC_2.34' not found

# Vérifier les versions nécessaires
objdump -T mon-programme | grep GLIBC

# Vérifier les versions disponibles
objdump -T /lib/x86_64-linux-gnu/libc.so.6 | grep GLIBC
```

#### Conflits de symboles

```bash
# Deux bibliothèques définissent le même symbole
# Voir quel symbole est utilisé
LD_DEBUG=symbols ./mon-programme 2>&1 | grep symbol_name
```

### Outils de diagnostic

```bash
# Lister les bibliothèques dépendantes
ldd ./mon-programme

# Voir les symboles
nm -D libmymath.so

# Informations détaillées
readelf -d libmymath.so

# Où cherche le chargeur ?
ld --verbose | grep SEARCH_DIR
```

## Bonnes pratiques

### 1. Versioning sémantique

```
Version: MAJOR.MINOR.PATCH

Incrémenter :
- PATCH : Corrections de bugs (ABI compatible)
- MINOR : Nouvelles fonctionnalités (ABI compatible)
- MAJOR : Changements incompatibles (ABI cassée)
```

### 2. Toujours définir un SONAME

```bash
# ✓ Bon
gcc -shared -Wl,-soname,libmymath.so.1 -o libmymath.so.1.2.3 ...

# ✗ Mauvais
gcc -shared -o libmymath.so ...
```

### 3. Séparer runtime et développement

**Package runtime :**
- `libmymath1_1.2.3_amd64.deb`
- Contient : `libmymath.so.1.2.3`, `libmymath.so.1`

**Package développement :**
- `libmymath-dev_1.2.3_amd64.deb`
- Contient : `libmymath.so`, headers, `.pc`

### 4. Fournir un fichier pkg-config

```
# Facilite l'utilisation de la bibliothèque
pkg-config --cflags --libs mymath
```

### 5. Documenter l'API

```c
/**
 * @brief Additionne deux entiers
 * @param a Premier entier
 * @param b Deuxième entier
 * @return La somme de a et b
 */
int add(int a, int b);
```

### 6. Préfixer les symboles

```c
// ✓ Bon - évite les conflits
int mymath_add(int a, int b);
int mymath_multiply(int a, int b);

// ✗ Mauvais - risque de conflit
int add(int a, int b);
```

### 7. Tests de compatibilité

```bash
# Tester la bibliothèque avec différentes versions de clients
# Tester la bibliothèque sur différentes distributions
```

### 8. Documentation complète

```
README.md
├── Installation
├── Utilisation
├── API Reference
├── Exemples
├── Changelog
└── License
```

## Exemple complet : Projet production-ready

### Structure

```
libmymath/
├── CMakeLists.txt
├── include/
│   └── mymath/
│       └── mymath.h
├── src/
│   ├── add.c
│   ├── multiply.c
│   └── internal.h
├── tests/
│   └── test_mymath.c
├── examples/
│   └── example.c
├── docs/
│   └── Doxyfile
├── cmake/
│   ├── mymathConfig.cmake.in
│   └── toolchain-arm64.cmake
├── mymath.pc.in
├── README.md
├── LICENSE
└── CHANGELOG.md
```

### CMakeLists.txt production

```cmake
cmake_minimum_required(VERSION 3.15)
project(mymath VERSION 1.2.3 LANGUAGES C)

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_TESTING "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

# Standard C
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Sources
file(GLOB SOURCES src/*.c)
list(FILTER SOURCES EXCLUDE REGEX ".*internal.*")

# Bibliothèque
add_library(mymath ${SOURCES})

# Alias pour uniformité
add_library(mymath::mymath ALIAS mymath)

# Headers publics
target_include_directories(mymath PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Propriétés
set_target_properties(mymath PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER include/mymath/mymath.h
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

# Compilation flags
target_compile_options(mymath PRIVATE
    -Wall -Wextra -Werror
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Debug>:-g -O0>
)

# Installation
include(GNUInstallDirs)

install(TARGETS mymath
    EXPORT mymathTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mymath
)

# pkg-config
configure_file(mymath.pc.in mymath.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/mymath.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# CMake config
install(EXPORT mymathTargets
    FILE mymathTargets.cmake
    NAMESPACE mymath::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mymath
)

configure_file(cmake/mymathConfig.cmake.in mymathConfig.cmake @ONLY)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    mymathConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/mymathConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/mymathConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mymath
)

# Tests
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# Exemples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
```

## Résumé

**Distribution d'une bibliothèque partagée :**

1. **Compilation** : `-fPIC` et `-shared`
2. **Versioning** : MAJOR.MINOR.PATCH avec SONAME
3. **Installation** : `/usr/local/lib` ou `/usr/lib`
4. **Configuration** : `ldconfig`, fichier `.pc`
5. **Package** : Séparer runtime et dev
6. **Documentation** : API, exemples, changelog

**Commandes clés :**

```bash
# Compilation
gcc -fPIC -shared -Wl,-soname,libname.so.1 -o libname.so.1.0.0 sources.c

# Installation
sudo make install
sudo ldconfig

# Utilisation
pkg-config --cflags --libs libname
gcc app.c $(pkg-config --cflags --libs libname)

# Diagnostic
ldd ./app
nm -D libname.so
readelf -d libname.so
```

La distribution de bibliothèques partagées nécessite rigueur et attention au versioning pour assurer compatibilité et stabilité dans le temps.

---


⏭️ [Documentation et Maintenance](/32-documentation-maintenance/README.md)
