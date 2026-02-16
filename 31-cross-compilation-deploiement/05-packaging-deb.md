🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 31.5 Packaging Linux : .deb

## Introduction

Vous avez développé un programme en C, vous l'avez compilé, testé avec QEMU... et maintenant ? Comment le **distribuer** facilement à vos utilisateurs ?

Sur les systèmes Debian et dérivés (Ubuntu, Mint, Raspberry Pi OS, etc.), la méthode standard est de créer un **package .deb**. C'est le format utilisé par des outils comme `apt`, `apt-get` et `dpkg`.

### Qu'est-ce qu'un package .deb ?

Un fichier `.deb` est une **archive** qui contient :
- ✅ Votre programme compilé (binaires)
- ✅ Les fichiers de configuration
- ✅ La documentation
- ✅ Les métadonnées (nom, version, dépendances, etc.)
- ✅ Les scripts d'installation/désinstallation

### Pourquoi créer un package .deb ?

**Sans package :**
```bash
# L'utilisateur doit faire :
git clone https://github.com/user/projet.git  
cd projet  
make  
sudo make install  
# Et espérer que ça fonctionne...
```

**Avec package :**
```bash
# L'utilisateur fait simplement :
sudo apt install mon-programme
# Ou
sudo dpkg -i mon-programme_1.0.0_amd64.deb
```

**Avantages :**
- ✅ **Installation facile** en une commande
- ✅ **Gestion des dépendances** automatique
- ✅ **Désinstallation propre** sans laisser de traces
- ✅ **Mises à jour** facilitées
- ✅ **Distribution** via des dépôts APT
- ✅ **Professionnel** et rassurant pour les utilisateurs

## Anatomie d'un package .deb

### Structure d'un fichier .deb

Un fichier `.deb` est en réalité une **archive AR** contenant deux archives TAR :

```
mon-programme_1.0.0_amd64.deb
├── debian-binary          # Version du format (2.0)
├── control.tar.xz         # Métadonnées et scripts
│   ├── control            # Informations du package
│   ├── md5sums            # Checksums des fichiers
│   ├── postinst           # Script post-installation
│   ├── prerm              # Script pré-suppression
│   └── ...
└── data.tar.xz            # Contenu du package
    ├── usr/
    │   ├── bin/
    │   │   └── mon-programme
    │   └── share/
    │       ├── doc/
    │       └── man/
    └── etc/
        └── mon-programme/
```

### Nomenclature des packages

Format : `nom_version_architecture.deb`

**Exemples :**
- `mon-programme_1.0.0_amd64.deb` → version 1.0.0 pour x86_64
- `mon-programme_1.0.0_arm64.deb` → version 1.0.0 pour ARM 64 bits
- `mon-programme_1.0.0_armhf.deb` → version 1.0.0 pour ARM 32 bits
- `mon-programme_1.0.0_all.deb` → indépendant de l'architecture

**Architectures courantes :**
| Architecture Debian | Description |
|--------------------|-------------|
| `amd64` | x86_64 (Intel/AMD 64 bits) |
| `i386` | x86 32 bits |
| `arm64` | ARM 64 bits (aarch64) |
| `armhf` | ARM 32 bits hard float |
| `armel` | ARM 32 bits soft float |
| `all` | Indépendant de l'architecture (scripts, données) |

## Méthode 1 : Création manuelle (simple)

### Structure du projet

```
mon-projet/
├── mon-programme          # Votre binaire compilé
├── mon-programme.1        # Page de manuel (optionnel)
├── README.md
└── debian-package/        # Dossier de packaging
    ├── DEBIAN/
    │   └── control
    └── usr/
        ├── bin/
        │   └── mon-programme
        └── share/
            └── doc/
                └── mon-programme/
                    ├── README
                    └── copyright
```

### Étape 1 : Créer la structure

```bash
# Créer la structure du package
mkdir -p debian-package/DEBIAN  
mkdir -p debian-package/usr/bin  
mkdir -p debian-package/usr/share/doc/mon-programme  
mkdir -p debian-package/usr/share/man/man1  
```

### Étape 2 : Copier les fichiers

```bash
# Copier le binaire
cp mon-programme debian-package/usr/bin/

# Donner les permissions d'exécution
chmod 755 debian-package/usr/bin/mon-programme

# Copier la documentation
cp README.md debian-package/usr/share/doc/mon-programme/README
```

### Étape 3 : Créer le fichier control

**debian-package/DEBIAN/control**

```
Package: mon-programme  
Version: 1.0.0  
Section: utils  
Priority: optional  
Architecture: amd64  
Depends: libc6 (>= 2.31)  
Maintainer: Votre Nom <votre.email@example.com>  
Description: Description courte en une ligne  
 Description longue sur plusieurs lignes.
 Chaque ligne doit commencer par un espace.
 .
 Un paragraphe vide est représenté par un point seul.
Homepage: https://github.com/user/mon-programme
```

**Explication des champs :**

- **Package** : Nom du package (minuscules, pas d'espaces)
- **Version** : Version du logiciel
- **Section** : Catégorie (utils, devel, net, etc.)
- **Priority** : Importance (optional, required, important)
- **Architecture** : Architecture cible (amd64, arm64, all, etc.)
- **Depends** : Dépendances nécessaires
- **Maintainer** : Votre nom et email
- **Description** : Description courte puis longue (indentée)
- **Homepage** : Site web du projet (optionnel)

### Étape 4 : Créer le package

```bash
# Construire le package
dpkg-deb --build debian-package

# Renommer avec la bonne nomenclature
mv debian-package.deb mon-programme_1.0.0_amd64.deb

# Vérifier le package
dpkg-deb --info mon-programme_1.0.0_amd64.deb  
dpkg-deb --contents mon-programme_1.0.0_amd64.deb  
```

### Étape 5 : Installer et tester

```bash
# Installer le package
sudo dpkg -i mon-programme_1.0.0_amd64.deb

# Vérifier l'installation
which mon-programme  
mon-programme --version  

# Lister les fichiers installés
dpkg -L mon-programme

# Désinstaller
sudo dpkg -r mon-programme
```

## Le fichier control en détail

### Champs obligatoires

```
Package: mon-programme  
Version: 1.0.0  
Architecture: amd64  
Maintainer: Votre Nom <email@example.com>  
Description: Description courte  
```

### Champs recommandés

```
Section: utils  
Priority: optional  
Homepage: https://github.com/user/projet  
Depends: libc6 (>= 2.31), libssl3 (>= 3.0.0)  
```

### Champs optionnels utiles

```
Recommends: autre-paquet  
Suggests: paquet-optionnel  
Conflicts: ancien-paquet  
Replaces: ancien-paquet  
Provides: interface-virtuelle  
```

### Exemples de sections

| Section | Description |
|---------|-------------|
| `admin` | Outils d'administration système |
| `devel` | Outils de développement |
| `utils` | Utilitaires divers |
| `net` | Outils réseau |
| `web` | Applications web |
| `database` | Bases de données |
| `libs` | Bibliothèques |

### Gestion des dépendances

**Syntaxe :**

```
Depends: paquet1, paquet2 (>= 2.0), paquet3 (< 3.0) | paquet4
```

**Opérateurs :**
- `=` : Version exacte
- `>=` : Version minimale
- `<=` : Version maximale
- `<<` : Strictement inférieure
- `>>` : Strictement supérieure
- `|` : OU (alternative)

**Exemple complet :**

```
Package: mon-serveur-web  
Version: 2.1.0  
Architecture: amd64  
Depends: libc6 (>= 2.31),  
         libssl3 (>= 3.0.0),
         libcurl4 (>= 7.68.0)
Recommends: logrotate, systemd  
Suggests: mon-serveur-web-plugins  
Conflicts: ancien-serveur-web  
```

## Scripts de maintenance

Les packages peuvent inclure des **scripts** exécutés lors de l'installation/désinstallation :

### Types de scripts

| Script | Moment d'exécution |
|--------|-------------------|
| **preinst** | Avant l'installation |
| **postinst** | Après l'installation |
| **prerm** | Avant la suppression |
| **postrm** | Après la suppression |

### Exemple : postinst

**debian-package/DEBIAN/postinst**

```bash
#!/bin/bash
set -e

# Ce script est exécuté après l'installation

# Créer un utilisateur système
if ! id -u mon-programme-user &>/dev/null; then
    useradd -r -s /usr/sbin/nologin mon-programme-user
fi

# Créer les répertoires nécessaires
mkdir -p /var/log/mon-programme  
chown mon-programme-user:mon-programme-user /var/log/mon-programme  

# Créer les fichiers de configuration par défaut
if [ ! -f /etc/mon-programme/config.conf ]; then
    cat > /etc/mon-programme/config.conf << 'EOF'
# Configuration par défaut
port=8080  
log_level=info  
EOF  
fi  

# Recharger systemd si on a installé un service
if [ -d /run/systemd/system ]; then
    systemctl daemon-reload
fi

echo "Installation de mon-programme terminée avec succès!"

exit 0
```

```bash
# Rendre le script exécutable
chmod 755 debian-package/DEBIAN/postinst
```

### Exemple : prerm

**debian-package/DEBIAN/prerm**

```bash
#!/bin/bash
set -e

# Ce script est exécuté avant la suppression

# Arrêter le service s'il est actif
if systemctl is-active --quiet mon-programme.service; then
    systemctl stop mon-programme.service
fi

exit 0
```

### Exemple : postrm

**debian-package/DEBIAN/postrm**

```bash
#!/bin/bash
set -e

# Ce script est exécuté après la suppression

case "$1" in
    purge)
        # Suppression complète (avec --purge)
        # Supprimer les fichiers de configuration
        rm -rf /etc/mon-programme

        # Supprimer les logs
        rm -rf /var/log/mon-programme

        # Supprimer l'utilisateur
        if id -u mon-programme-user &>/dev/null; then
            userdel mon-programme-user
        fi
        ;;

    remove)
        # Suppression normale (garder la config)
        echo "Configuration conservée dans /etc/mon-programme"
        ;;

    *)
        ;;
esac

exit 0
```

## Package avec service systemd

Pour distribuer un daemon/service :

### Structure étendue

```
debian-package/
├── DEBIAN/
│   ├── control
│   ├── postinst
│   └── prerm
├── usr/
│   └── bin/
│       └── mon-daemon
└── etc/
    └── systemd/
        └── system/
            └── mon-daemon.service
```

### Fichier service systemd

**debian-package/etc/systemd/system/mon-daemon.service**

```ini
[Unit]
Description=Mon Daemon en C  
After=network.target  

[Service]
Type=simple  
User=mon-daemon-user  
Group=mon-daemon-user  
ExecStart=/usr/bin/mon-daemon  
Restart=on-failure  
RestartSec=5s  

# Sécurité
NoNewPrivileges=true  
PrivateTmp=true  
ProtectSystem=strict  
ProtectHome=true  

[Install]
WantedBy=multi-user.target
```

### Script postinst pour le service

```bash
#!/bin/bash
set -e

# Créer l'utilisateur
if ! id -u mon-daemon-user &>/dev/null; then
    useradd -r -s /usr/sbin/nologin mon-daemon-user
fi

# Recharger systemd
if [ -d /run/systemd/system ]; then
    systemctl daemon-reload

    # Activer le service (démarrage automatique)
    systemctl enable mon-daemon.service

    # Démarrer le service
    systemctl start mon-daemon.service
fi

exit 0
```

## Packaging pour plusieurs architectures

### Cross-compilation et packaging

**Script build-all-deb.sh**

```bash
#!/bin/bash

set -e

PROJECT_NAME="mon-programme"  
VERSION="1.0.0"  

# Architectures à compiler
declare -A ARCHS=(
    ["amd64"]="x86_64-linux-gnu-gcc"
    ["arm64"]="aarch64-linux-gnu-gcc"
    ["armhf"]="arm-linux-gnueabihf-gcc"
)

# Fonction de build
build_deb() {
    local arch=$1
    local compiler=$2

    echo "=== Building for $arch ==="

    # Compiler
    if [ "$arch" = "amd64" ]; then
        gcc src/main.c -o ${PROJECT_NAME} -O2 -Wall
    else
        $compiler src/main.c -o ${PROJECT_NAME} -O2 -Wall
    fi

    # Créer la structure du package
    local pkg_dir="debian-${arch}"
    rm -rf "$pkg_dir"

    mkdir -p "$pkg_dir/DEBIAN"
    mkdir -p "$pkg_dir/usr/bin"
    mkdir -p "$pkg_dir/usr/share/doc/${PROJECT_NAME}"

    # Copier les fichiers
    cp ${PROJECT_NAME} "$pkg_dir/usr/bin/"
    chmod 755 "$pkg_dir/usr/bin/${PROJECT_NAME}"
    cp README.md "$pkg_dir/usr/share/doc/${PROJECT_NAME}/"

    # Créer le fichier control
    cat > "$pkg_dir/DEBIAN/control" << EOF
Package: ${PROJECT_NAME}  
Version: ${VERSION}  
Section: utils  
Priority: optional  
Architecture: ${arch}  
Depends: libc6 (>= 2.31)  
Maintainer: Votre Nom <email@example.com>  
Description: Description courte  
 Description longue sur plusieurs lignes.
EOF

    # Construire le package
    local deb_file="${PROJECT_NAME}_${VERSION}_${arch}.deb"
    dpkg-deb --build "$pkg_dir" "$deb_file"

    echo "✓ Package créé : $deb_file"

    # Vérifier
    dpkg-deb --info "$deb_file"

    # Nettoyer
    rm -rf "$pkg_dir"
}

# Compiler pour toutes les architectures
for arch in "${!ARCHS[@]}"; do
    build_deb "$arch" "${ARCHS[$arch]}"
    echo ""
done

echo "=== Packages créés ==="  
ls -lh *.deb  
```

**Utilisation :**

```bash
chmod +x build-all-deb.sh
./build-all-deb.sh

# Résultat :
# mon-programme_1.0.0_amd64.deb
# mon-programme_1.0.0_arm64.deb
# mon-programme_1.0.0_armhf.deb
```

## Méthode 2 : Utiliser dh-make et debhelper

### Avantages de debhelper

**debhelper** est un ensemble d'outils qui automatisent la création de packages Debian selon les standards officiels.

**Avantages :**
- ✅ Respect des conventions Debian
- ✅ Génération automatique de scripts
- ✅ Support des builds complexes
- ✅ Intégration avec les dépôts officiels

### Installation

```bash
sudo apt-get install dh-make devscripts build-essential
```

### Initialisation d'un projet

```bash
# Structure du projet source
mkdir mon-programme-1.0.0  
cd mon-programme-1.0.0  

# Copier vos sources
cp /path/to/src/* .

# Créer une archive source
cd ..  
tar czf mon-programme_1.0.0.orig.tar.gz mon-programme-1.0.0/  
cd mon-programme-1.0.0  

# Initialiser le packaging Debian
dh_make -s --createorig

# Choisir 's' (single binary package)
# Remplir les informations demandées
```

### Structure générée

```
mon-programme-1.0.0/
├── debian/
│   ├── changelog          # Historique des versions
│   ├── control            # Métadonnées du package
│   ├── copyright          # Informations de licence
│   ├── rules              # Script de build
│   ├── source/
│   │   └── format
│   └── *.ex               # Templates à personnaliser
├── src/
│   └── main.c
├── Makefile
└── README.md
```

### Éditer debian/control

**debian/control**

```
Source: mon-programme  
Section: utils  
Priority: optional  
Maintainer: Votre Nom <email@example.com>  
Build-Depends: debhelper-compat (= 13), gcc  
Standards-Version: 4.6.0  
Homepage: https://github.com/user/mon-programme  

Package: mon-programme  
Architecture: any  
Depends: ${shlibs:Depends}, ${misc:Depends}  
Description: Description courte en une ligne  
 Description longue détaillée sur plusieurs lignes.
 .
 Fonctionnalités principales :
  - Fonctionnalité 1
  - Fonctionnalité 2
```

### Éditer debian/changelog

**debian/changelog**

```
mon-programme (1.0.0-1) unstable; urgency=medium

  * Initial release.
  * Ajout de la fonctionnalité X
  * Correction du bug Y

 -- Votre Nom <email@example.com>  Mon, 28 Nov 2024 10:00:00 +0100
```

**Format strict :**
- Première ligne : `paquet (version) distribution; urgency=niveau`
- Changements précédés de deux espaces et un astérisque
- Dernière ligne : ` -- Mainteneur <email>  Date`

### Éditer debian/rules

**debian/rules** (Makefile)

```makefile
#!/usr/bin/make -f

%:
	dh $@

override_dh_auto_build:
	# Compilation personnalisée
	$(CC) $(CFLAGS) src/main.c -o mon-programme

override_dh_auto_install:
	# Installation personnalisée
	install -D -m 755 mon-programme $(CURDIR)/debian/mon-programme/usr/bin/mon-programme
```

### Builder le package

```bash
# Builder le package
dpkg-buildpackage -us -uc -b

# Options :
# -us : ne pas signer le .dsc
# -uc : ne pas signer le .changes
# -b : build binaire seulement (pas de source)

# Le package est créé dans le répertoire parent
cd ..  
ls -l *.deb  
```

## Vérification et validation des packages

### Vérifier avec lintian

**lintian** est l'outil officiel de vérification des packages Debian :

```bash
# Installer lintian
sudo apt-get install lintian

# Vérifier un package
lintian mon-programme_1.0.0_amd64.deb

# Vérification approfondie
lintian -i -I --pedantic mon-programme_1.0.0_amd64.deb
```

**Types d'erreurs/warnings :**
- **E** : Erreur (doit être corrigée)
- **W** : Warning (devrait être corrigée)
- **I** : Info (bon à savoir)
- **P** : Pedantic (standards stricts)

### Inspecter un package

```bash
# Afficher les métadonnées
dpkg-deb --info mon-programme_1.0.0_amd64.deb

# Lister le contenu
dpkg-deb --contents mon-programme_1.0.0_amd64.deb

# Extraire le contenu
dpkg-deb --extract mon-programme_1.0.0_amd64.deb extracted/  
dpkg-deb --control mon-programme_1.0.0_amd64.deb extracted/DEBIAN  

# Vérifier l'architecture
dpkg-deb --field mon-programme_1.0.0_amd64.deb Architecture
```

### Tester l'installation

```bash
# Installation
sudo dpkg -i mon-programme_1.0.0_amd64.deb

# Vérifier le statut
dpkg -s mon-programme

# Lister les fichiers installés
dpkg -L mon-programme

# Chercher quel package contient un fichier
dpkg -S /usr/bin/mon-programme

# Simuler la suppression
dpkg -r --dry-run mon-programme

# Suppression réelle
sudo dpkg -r mon-programme

# Purge (supprime aussi la config)
sudo dpkg --purge mon-programme
```

## Distribution des packages

### Méthode 1 : Distribution directe

**GitHub Releases :**

```bash
# Créer une release sur GitHub
# Uploader les fichiers .deb

# Les utilisateurs peuvent installer avec :
wget https://github.com/user/projet/releases/download/v1.0.0/mon-programme_1.0.0_amd64.deb  
sudo dpkg -i mon-programme_1.0.0_amd64.deb  
sudo apt-get install -f  # Installer les dépendances manquantes  
```

### Méthode 2 : Dépôt APT personnel

#### Structure d'un dépôt

```
mon-depot/
├── dists/
│   └── stable/
│       └── main/
│           └── binary-amd64/
│               ├── Packages
│               └── Packages.gz
└── pool/
    └── main/
        └── m/
            └── mon-programme/
                ├── mon-programme_1.0.0_amd64.deb
                └── mon-programme_1.0.0_arm64.deb
```

#### Créer un dépôt avec reprepro

```bash
# Installer reprepro
sudo apt-get install reprepro

# Créer la structure
mkdir -p mon-depot/conf

# Configurer le dépôt
cat > mon-depot/conf/distributions << 'EOF'  
Origin: Mon Dépôt  
Label: mon-depot  
Codename: stable  
Architectures: amd64 arm64 armhf  
Components: main  
Description: Dépôt personnel  
EOF  

# Ajouter un package
cd mon-depot  
reprepro includedeb stable ../mon-programme_1.0.0_amd64.deb  

# Générer les métadonnées
reprepro export
```

#### Publier le dépôt

**Méthode simple : GitHub Pages**

```bash
# Dans le dépôt Git
git add .  
git commit -m "Add packages"  
git push  

# Activer GitHub Pages sur le dépôt
```

**Configuration utilisateur :**

```bash
# Ajouter le dépôt
echo "deb [trusted=yes] https://user.github.io/mon-depot stable main" | \
    sudo tee /etc/apt/sources.list.d/mon-depot.list

# Mettre à jour
sudo apt-get update

# Installer
sudo apt-get install mon-programme
```

### Méthode 3 : PPA (Ubuntu)

Pour distribuer sur Ubuntu via un PPA (Personal Package Archive) :

```bash
# 1. Créer un compte Launchpad : https://launchpad.net
# 2. Créer un PPA
# 3. Signer et uploader les packages

# Les utilisateurs pourront installer avec :
sudo add-apt-repository ppa:username/ppa-name  
sudo apt-get update  
sudo apt-get install mon-programme  
```

## Intégration CI/CD

### GitHub Actions

**.github/workflows/build-deb.yml**

```yaml
name: Build DEB Packages

on:
  push:
    tags:
      - 'v*'
  workflow_dispatch:

jobs:
  build-deb:
    runs-on: ubuntu-latest

    strategy:
      matrix:
        arch:
          - { name: amd64, compiler: gcc }
          - { name: arm64, compiler: aarch64-linux-gnu-gcc, cross: gcc-aarch64-linux-gnu }
          - { name: armhf, compiler: arm-linux-gnueabihf-gcc, cross: gcc-arm-linux-gnueabihf }

    steps:
    - uses: actions/checkout@v4

    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y dpkg-dev debhelper lintian
        if [ -n "${{ matrix.arch.cross }}" ]; then
          sudo apt-get install -y ${{ matrix.arch.cross }}
        fi

    - name: Get version
      id: version
      run: |
        VERSION=${GITHUB_REF#refs/tags/v}
        echo "version=$VERSION" >> $GITHUB_OUTPUT

    - name: Build binary
      run: |
        if [ "${{ matrix.arch.name }}" = "amd64" ]; then
          gcc src/main.c -o mon-programme -O2 -Wall
        else
          ${{ matrix.arch.compiler }} src/main.c -o mon-programme -O2 -Wall
        fi

    - name: Create package structure
      run: |
        mkdir -p debian-pkg/DEBIAN
        mkdir -p debian-pkg/usr/bin
        mkdir -p debian-pkg/usr/share/doc/mon-programme

        cp mon-programme debian-pkg/usr/bin/
        chmod 755 debian-pkg/usr/bin/mon-programme
        cp README.md debian-pkg/usr/share/doc/mon-programme/

        cat > debian-pkg/DEBIAN/control << EOF
        Package: mon-programme
        Version: ${{ steps.version.outputs.version }}
        Section: utils
        Priority: optional
        Architecture: ${{ matrix.arch.name }}
        Depends: libc6 (>= 2.31)
        Maintainer: Votre Nom <email@example.com>
        Description: Description de mon programme
         Description longue.
        EOF

    - name: Build DEB package
      run: |
        dpkg-deb --build debian-pkg
        mv debian-pkg.deb mon-programme_${{ steps.version.outputs.version }}_${{ matrix.arch.name }}.deb

    - name: Verify with lintian
      run: lintian mon-programme_${{ steps.version.outputs.version }}_${{ matrix.arch.name }}.deb || true

    - name: Upload artifact
      uses: actions/upload-artifact@v4
      with:
        name: deb-packages
        path: '*.deb'

    - name: Create Release
      if: startsWith(github.ref, 'refs/tags/')
      uses: softprops/action-gh-release@v2
      with:
        files: '*.deb'
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

### GitLab CI

**.gitlab-ci.yml**

```yaml
image: ubuntu:22.04

stages:
  - build
  - package
  - deploy

variables:
  PACKAGE_NAME: mon-programme
  VERSION: "1.0.0"

before_script:
  - apt-get update -qq
  - apt-get install -y -qq dpkg-dev debhelper lintian

build:amd64:
  stage: build
  script:
    - gcc src/main.c -o ${PACKAGE_NAME} -O2 -Wall
  artifacts:
    paths:
      - ${PACKAGE_NAME}
    expire_in: 1 hour

build:arm64:
  stage: build
  before_script:
    - apt-get update -qq
    - apt-get install -y -qq gcc-aarch64-linux-gnu
  script:
    - aarch64-linux-gnu-gcc src/main.c -o ${PACKAGE_NAME} -O2 -Wall
  artifacts:
    paths:
      - ${PACKAGE_NAME}
    expire_in: 1 hour

package:amd64:
  stage: package
  dependencies:
    - build:amd64
  script:
    - ./scripts/create-deb.sh amd64 ${VERSION}
  artifacts:
    paths:
      - "*.deb"

package:arm64:
  stage: package
  dependencies:
    - build:arm64
  script:
    - ./scripts/create-deb.sh arm64 ${VERSION}
  artifacts:
    paths:
      - "*.deb"

deploy:
  stage: deploy
  script:
    - echo "Upload to repository"
  only:
    - tags
```

## Bonnes pratiques

### 1. Versioning sémantique

Utilisez le versioning sémantique : `MAJOR.MINOR.PATCH`

```
1.0.0   → Première release stable
1.0.1   → Correction de bugs
1.1.0   → Nouvelles fonctionnalités (rétrocompatible)
2.0.0   → Changements majeurs (breaking changes)
```

### 2. Documentation complète

```
debian-package/usr/share/doc/mon-programme/
├── README
├── copyright          # OBLIGATOIRE (licence)
├── changelog.gz       # Historique des versions
└── examples/          # Exemples d'utilisation
```

### 3. Page de manuel

Fournissez une page de manuel :

```bash
# Créer une page de manuel
mkdir -p debian-package/usr/share/man/man1

# Format : man-pages
cat > debian-package/usr/share/man/man1/mon-programme.1 << 'EOF'
.TH MON-PROGRAMME 1 "November 2024" "Version 1.0.0" "User Commands"
.SH NAME
mon-programme \- Description courte
.SH SYNOPSIS
.B mon-programme
[\fIOPTIONS\fR]
.SH DESCRIPTION
Description complète du programme.
.SH OPTIONS
.TP
.BR \-h ", " \-\-help
Affiche l'aide
.TP
.BR \-v ", " \-\-version
Affiche la version
.SH EXAMPLES
.TP
mon-programme \-\-option  
Description de l'exemple  
.SH AUTHOR
Votre Nom <email@example.com>  
EOF  

# Compresser
gzip debian-package/usr/share/man/man1/mon-programme.1
```

### 4. Fichier copyright

**debian-package/usr/share/doc/mon-programme/copyright**

```
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/  
Upstream-Name: mon-programme  
Upstream-Contact: Votre Nom <email@example.com>  
Source: https://github.com/user/mon-programme  

Files: *  
Copyright: 2024 Votre Nom <email@example.com>  
License: MIT  
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction...
 [Texte complet de la licence]
```

### 5. Changelog

```bash
# Format du changelog
cat > changelog << 'EOF'  
mon-programme (1.0.1-1) stable; urgency=medium  

  * Correction du bug #123
  * Amélioration des performances

 -- Votre Nom <email@example.com>  Mon, 28 Nov 2024 14:30:00 +0100

mon-programme (1.0.0-1) stable; urgency=low

  * Release initiale

 -- Votre Nom <email@example.com>  Mon, 21 Nov 2024 10:00:00 +0100
EOF

# Compresser
gzip -9 changelog  
mv changelog.gz debian-package/usr/share/doc/mon-programme/  
```

### 6. Tests avant distribution

```bash
# 1. Vérifier avec lintian
lintian -i -I --pedantic mon-programme_1.0.0_amd64.deb

# 2. Tester l'installation
sudo dpkg -i mon-programme_1.0.0_amd64.deb

# 3. Tester le programme
mon-programme --version  
mon-programme --help  

# 4. Vérifier les fichiers installés
dpkg -L mon-programme

# 5. Tester la désinstallation
sudo dpkg -r mon-programme

# 6. Tester la purge
sudo dpkg -i mon-programme_1.0.0_amd64.deb  
sudo dpkg --purge mon-programme  
```

### 7. Conventions de nommage

```
# Nom du package : minuscules, tirets (pas d'underscores)
✓ mon-programme
✓ mon-super-outil
✗ Mon_Programme
✗ MonProgramme

# Fichiers binaires : minuscules
✓ /usr/bin/mon-programme
✗ /usr/bin/MonProgramme

# Fichiers de config : minuscules
✓ /etc/mon-programme/config.conf
```

## Dépannage

### Erreur : "dependency problems"

**Problème :** Dépendances manquantes

**Solution :**
```bash
# Installer le package et ses dépendances
sudo dpkg -i mon-programme.deb  
sudo apt-get install -f  # Résout les dépendances  
```

### Erreur : "package is for architecture 'amd64'"

**Problème :** Mauvaise architecture

**Solution :**
```bash
# Vérifier l'architecture du système
dpkg --print-architecture

# Télécharger la bonne version du package
```

### Erreur dans les scripts de maintenance

**Problème :** Script postinst échoue

**Debugging :**
```bash
# Activer le mode debug
sudo sh -x /var/lib/dpkg/info/mon-programme.postinst configure

# Voir les logs
journalctl -xe
```

## Résumé des commandes

```bash
# Création manuelle
mkdir -p debian-pkg/{DEBIAN,usr/bin}  
cat > debian-pkg/DEBIAN/control << EOF  
# Contenu du control
EOF  
dpkg-deb --build debian-pkg mon-programme_1.0.0_amd64.deb  

# Vérification
dpkg-deb --info mon-programme.deb  
dpkg-deb --contents mon-programme.deb  
lintian mon-programme.deb  

# Installation
sudo dpkg -i mon-programme.deb  
sudo apt-get install -f  # Dépendances  

# Gestion
dpkg -l mon-programme          # Statut  
dpkg -L mon-programme          # Fichiers  
dpkg -S /usr/bin/mon-programme # Recherche  

# Désinstallation
sudo dpkg -r mon-programme     # Garder config  
sudo dpkg --purge mon-programme # Tout supprimer  
```

## Conclusion

La création de packages .deb professionnalise votre distribution et facilite énormément la vie de vos utilisateurs.

**Récapitulatif :**

1. **Structure** : Créer l'arborescence DEBIAN + usr/
2. **Métadonnées** : Fichier control avec infos et dépendances
3. **Scripts** : postinst, prerm, etc. (optionnel)
4. **Build** : `dpkg-deb --build`
5. **Vérification** : lintian et tests
6. **Distribution** : GitHub Releases, dépôt APT, ou PPA

**Avantages pour vos utilisateurs :**
- ✅ Installation en une commande
- ✅ Gestion automatique des dépendances
- ✅ Désinstallation propre
- ✅ Mises à jour facilitées
- ✅ Intégration système complète

Dans la prochaine section, nous verrons le packaging RPM pour les systèmes Red Hat/CentOS/Fedora.

---


⏭️ [Packaging Linux : .rpm](/31-cross-compilation-deploiement/06-packaging-rpm.md)
